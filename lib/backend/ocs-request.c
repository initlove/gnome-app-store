/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2011
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Liang chenye <liangchenye@gmail.com>
 */
#include <libsoup/soup-session.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>

#include "gnome-app-utils.h"
#include "ocs-request.h"

/*FIXME: there must be such function in xml interface !*/
/*Search in the childnode of the rootnode*/
xmlNodePtr
find_node (xmlDocPtr doc_ptr, gchar *node_name)
{
        xmlNodePtr root_node, data_node;

        root_node = xmlDocGetRootElement (doc_ptr);

        if (!root_node) {
                printf ("Cannot get the root node!\n");
                return NULL;
        }

        for (data_node = root_node->xmlChildrenNode; data_node; data_node = data_node->next) {
                if (strcmp (data_node->name, node_name) == 0)
                        return data_node;
        }
        return NULL;
}

gchar *
make_request_by_appid (OcsBackend *backend, gchar *id)
{
        gchar *url;

        url = g_strdup_printf ("http://%s:%s@%s%s/v1/content/data/%s", 
				ocs_backend_get_username (backend),
				ocs_backend_get_password (backend),
				ocs_backend_get_server_uri (backend),
                                id);

	return url;
}

static gchar *
get_ids_by_list (GList *list, gchar sep)
{
        GList *l;
        GString *ids = NULL;
        gchar *str;

        for (l = list; l; l = l->next) {
                str = (gchar *) l->data;
                if (!str)
                        continue;
                if (!ids) {
                        ids = g_string_new (str);
                } else {
                        g_string_append_c (ids, sep);
                        g_string_append (ids, str);
                }
        }

        if (ids) {
                str = ids->str;
                g_string_free (ids, FALSE);
        } else {
                str = NULL;
        }

        return str;
}

gchar *
make_request_by_query (OcsBackend *backend, GnomeAppQuery *query)
{
	GString *request;
        GList *list;
        gint prop_id;
        gchar *prop_value;
        gboolean begin;
	gchar *val;

        request = g_string_new ("https://");
        g_string_append_printf (request, "%s:%s@%s/v1/content/data?",
				ocs_backend_get_username (backend),
				ocs_backend_get_password (backend),
				ocs_backend_get_server_uri (backend));

        /*TODO: if the prop was not exist, solve it ! */
        begin = TRUE;
        for (prop_id = PROP_QUERY_GROUP; prop_id < PROP_QUERY_LAST; prop_id ++) {
                g_object_get (query, query_units [prop_id].name, &prop_value, NULL);
                if (prop_value) {
                        if (begin)
                                begin = FALSE;
                        else
                                g_string_append_c (request, '&');
                        if (prop_id == PROP_QUERY_GROUP) {
                                g_free (prop_value);
                                list = ocs_get_cid_list_by_group (backend, prop_value);
                                prop_value = get_ids_by_list (list, 'x');
                                g_list_free (list);
                        	g_string_append_printf (request, "%s=%s", "categories", prop_value);
                        } else
 	                       g_string_append_printf (request, "%s=%s", query_units [prop_id].name, prop_value);
                        g_free (prop_value);
                }
        }
	val = request->str;
	g_string_free (request, FALSE);

	return val;
}

/*TODO: check the timestamp first, different api should have different refresh time.
 * If the current time  - create_time of the file longer than refresh time.
 * load from backend anyway!
*/
static gboolean
need_to_refresh (gchar *local_url)
{
        return FALSE;
}

static gboolean
check_data_valid (xmlDocPtr doc_ptr)
{
        xmlNodePtr meta_node, status_node;
        gchar *content;
        gint status_code;
        gchar *message;

        meta_node = find_node (doc_ptr, "meta");
        if (!meta_node) {
                return FALSE;
        } else {
                for (status_node = meta_node->xmlChildrenNode; status_node; status_node = status_node->next) {
                        content = xmlNodeGetContent (status_node);
                        if (!content || strlen (content) < 1)
                                continue;
                        if (strcmp (status_node->name, "statuscode") == 0) {
                                status_code = atoi (content);
                        } else if (strcmp (status_node->name, "message") == 0) {
                                message = content;
                        }
                }
        }
        if (status_code == 100) {
                return TRUE;
        } else {
                printf ("Get the error msg from backend:\n\t%s\n", message);
                return FALSE;
        }
}

xmlDocPtr
ocs_get_request_doc (OcsBackend *backend, gchar *request)
{
        gchar *md5;
        const gchar *cache_dir;
        gchar *local_url;
        xmlDocPtr doc_ptr;
        gboolean refresh = TRUE;

        md5 = gnome_app_get_md5 ((const gchar *)request);
        cache_dir = ocs_backend_get_cache_dir (backend);
        local_url = g_build_filename (cache_dir, "xml", md5, NULL);

        if (g_file_test (local_url, G_FILE_TEST_EXISTS)) {
                if (need_to_refresh (local_url)) {
                        refresh = TRUE;
                } else {
                        doc_ptr = xmlParseFile (local_url);
                        if (!doc_ptr) {
                                g_unlink (local_url);
                                refresh = TRUE;
                        } else if (!check_data_valid (doc_ptr)) {
                                xmlFreeDoc(doc_ptr);
                                g_unlink (local_url);
                                doc_ptr = NULL;
                                refresh = TRUE;
                        } else {
                                refresh = FALSE;
                        }
                }
        }
        if (refresh) {
		SoupSession *session;
                SoupBuffer *buf;

		session = (SoupSession *) ocs_backend_get_session (backend);
                buf = gnome_app_get_data_by_request (session, request);
                doc_ptr = xmlParseMemory (buf->data, buf->length);
                if (!doc_ptr) {
                        printf ("Cannot parse the value!\n");
                } else if (!check_data_valid (doc_ptr)) {
                        xmlFreeDoc(doc_ptr);
                        doc_ptr = NULL;
                } else {
                        FILE *fp;
                        gchar *xml_dir;

                        xml_dir =  g_build_filename (cache_dir, "xml", NULL);
                        if (!g_file_test (xml_dir, G_FILE_TEST_EXISTS))
                                g_mkdir_with_parents (xml_dir, 0755);
                        g_free (xml_dir);

                        fp = fopen (local_url, "w");
                        if (!fp) {
                                /*TODO: popup more error, cause it is local disk issue */
                                printf ("Cannot save the xml file to disk %s!\n", local_url);
                        } else {
                                fwrite (buf->data, 1, buf->length, fp);
                                fclose (fp);
                        }
                }
                soup_buffer_free (buf);
        }
        g_free (md5);
        g_free (local_url);

        return doc_ptr;
}


