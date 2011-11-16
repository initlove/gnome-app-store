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

#include "open-app-utils.h"
#include "ocs-request.h"

/*FIXME: there must be such function in xml interface !*/
/*Search in the childnode of the rootnode*/
xmlNodePtr
ocs_find_node (xmlDocPtr doc_ptr, gchar *node_name)
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

xmlDocPtr
ocs_request_get_default_doc ()
{
	static xmlDocPtr doc_ptr = NULL;
	gchar *doc_file;

	if (!doc_ptr) {
/*TODO other dir */
/*TODO: make the ocs-request-xml validation check be in this time ! */
		doc_file = "/home/novell/gnome-app-store/backend/ocs-spec/ocs-services-request.xml";
		doc_ptr = xmlParseFile (doc_file);
		if (!doc_ptr) {
			g_critical ("Cannot find ocs-request doc in %s\n", doc_file);
			exit (1);
		}
	}

	return doc_ptr;
}

gchar *
ocs_request_get_value (const gchar *services, 
		const gchar *operation, 
		const gchar *group, 
		const gchar *key)
{
	g_return_val_if_fail (services && operation && group &&key, NULL);

	xmlDocPtr doc_ptr;
	xmlNodePtr root_node, services_node, operation_node, group_node, key_node;
	gchar *val;

	doc_ptr = ocs_request_get_default_doc ();
	root_node = ocs_find_node (doc_ptr, "services");

	for (services_node = root_node->xmlChildrenNode; services_node; services_node = services_node->next) {
                if (strcmp (services_node->name, services) == 0)
			break;
	}
	if (!services_node) {
		g_debug ("Cannot find the service %s!\n", services);
		return NULL;
	}

	for (operation_node = services_node->xmlChildrenNode; operation_node; operation_node = operation_node->next) {
		if (strcmp (operation_node->name, operation) == 0)
			break;
	}
	if (!operation_node) {
		g_debug ("Cannot find the operation %s in service %s!\n", operation, services);
		return NULL;
	}

	for (group_node = operation_node->xmlChildrenNode; group_node; group_node = group_node->next) {
		if (strcmp (group_node->name, group) == 0)
			break;
	}
	if (!group_node)
		return NULL;

	for (key_node = group_node->xmlChildrenNode; key_node; key_node = key_node->next) {
		if (strcmp (key_node->name, key) == 0)
			break;
	}
	if (!key_node)
		return NULL;

	val = xmlNodeGetContent (key_node);

	if (val && val [0])
		return g_strdup (val);
	else
		return NULL;
}

/*TODO: make a cache or something */
/*TODO: make the ocs-request-xml validation check be for use it */
GList *
ocs_request_get_keys (const gchar *services, 
		const gchar *operation, 
		const gchar *group)
{
	xmlDocPtr doc_ptr;
	xmlNodePtr root_node, services_node, operation_node, group_node, key_node;
	GList *list = NULL;

	doc_ptr = ocs_request_get_default_doc ();
	root_node = ocs_find_node (doc_ptr, "services");

	for (services_node = root_node->xmlChildrenNode; services_node; services_node = services_node->next) {
		if (services) {
	                if (strcmp (services_node->name, services) == 0)
				break;
		} else {
			list = g_list_prepend (list, g_strdup (services_node->name));
		}
	}
	if (!services_node) {
		list = g_list_reverse (list);
		return list;
	}

	for (operation_node = services_node->xmlChildrenNode; operation_node; operation_node = operation_node->next) {
		if (operation) {
			if (strcmp (operation_node->name, operation) == 0)
				break;
		} else {
			list = g_list_prepend (list, g_strdup (operation_node->name));
		}
	}
	if (!operation_node) {
		list = g_list_reverse (list);
		return list;
	}

	for (group_node = operation_node->xmlChildrenNode; group_node; group_node = group_node->next) {
		if (group_node) {
			if (strcmp (group_node->name, group) == 0)
				break;
		} else {
			list = g_list_prepend (list, g_strdup (group_node->name));
		}
	}
	if (!group_node) {
		list = g_list_reverse (list);
		return list;
	}
	
	for (key_node = group_node->xmlChildrenNode; key_node; key_node = key_node->next) {
		list = g_list_prepend (list, g_strdup (key_node->name));
	}
	if (list)
		list = g_list_reverse (list);

	return list;
}

gboolean
ocs_request_is_valid (OpenRequest *request)
{
	/*Check the services,
	  Check the Args,
	  Check the mandatory URL_Args and POST_Args
	*/
	const gchar *services;
	const gchar *operation;
	gchar *syntax;

	services = open_request_get (request, "services");
	if (!services || !services [0])
		return FALSE;

	operation = open_request_get (request, "operation");
	if (!operation || !operation [0])
		return FALSE;

	syntax = ocs_request_get_value (services, operation, "Summary", "Syntax");
	if (!syntax || !syntax [0]) {
		g_critical ("Cannot find syntax of %s - %s\n", services, operation);
		exit (1); 
	}

	gint i, len;
	gboolean syntax_valid;

	syntax_valid = TRUE;
	len = strlen (syntax);
	for (i = 0; i < len; i++)
		if (*(syntax + i) == '"')
			syntax_valid = !syntax_valid;
	if (!syntax_valid) {
		g_critical ("Fatal error in ocs syntax %s - %s : %s!\n", services, operation, syntax);
		exit (1);
	}
	g_free (syntax);

	const gchar *groups [] = {"Args", "URL_Args", "POST_Args", NULL};
	GList *keys, *l;
	gchar *key;
	const gchar *val;

	for (i = 0; groups [i]; i++) {
		keys = ocs_request_get_keys (services, operation, groups [i]);
		for (l = keys; l; l = l->next) {
			key = (gchar *) l->data;
			if (strcmp (groups [i], "Args") == 0) {
			} else {
				val = ocs_request_get_value (services, operation, groups [i], key);
				if (!val || (strcmp (val, "Mandatory") != 0))
					continue;
			}
			val = open_request_get (request, key);
			if (!val || !val [0]) {
				g_list_free (keys);
				return FALSE;
			}
		}
		g_list_free (keys);
	}

	return TRUE;
}

static void
ocs_request_hot_fix (OcsBackend *backend, OpenRequest *request)
{
	const gchar *category_name;

	category_name = open_request_get (request, "category_name");

	if (category_name && category_name [0]) {
		const gchar *categories;
		categories = ocs_get_categories_by_name (backend, category_name);
		if (categories) {
			open_request_set (request, "categories", categories);
		}
		
	}
}

gchar *
ocs_get_request_url (OcsBackend *backend, OpenRequest *request)
{
	g_return_val_if_fail (request && IS_OPEN_REQUEST (request), NULL);

	if (!ocs_request_is_valid (request))
		return NULL;

	ocs_request_hot_fix (backend, request);

	const gchar *services;
	const gchar *operation;
	GString *url;

	services = open_request_get (request, "services");
	operation = open_request_get (request, "operation");
        url = g_string_new ("https://");
        g_string_append_printf (url, "%s:%s@%s",
				ocs_backend_get_username (backend),
				ocs_backend_get_password (backend),
				ocs_backend_get_server_uri (backend));

	/* Syntax and Args */
	gchar *syntax;
	gchar *begin, *p;
	gchar *key;
	const gchar *value;

	syntax = ocs_request_get_value (services, operation, "Summary", "Syntax");
        for (begin = syntax;;) {
                if (!begin || !begin [0])
                        break;
                p = strchr (begin, '"');
                if (!p) {
                        g_string_append_printf (url, "%s", begin);
                        break;
                }   

                *p = 0;
                key = p + 1;
                p = strchr (key, '"');
                *p = 0;
                if (begin && begin [0])
                        g_string_append_printf (url, "%s", begin);
		value = open_request_get (request, key);
                g_string_append_printf (url, "%s", value);
                begin = p + 1;
        }
	g_free (syntax);

	/* URL_Args */
	gboolean first;
	GList *keys, *l;

	first = TRUE;
	keys = ocs_request_get_keys (services, operation, "URL_Args");
	for (l = keys; l; l = l->next) {
		key = (gchar *) l->data;
		value = open_request_get (request, key);
		if (!value || !value [0])
			 continue;
		if (first) {
			first = FALSE;
			g_string_append_c (url, '?');
		} else {
			g_string_append_c (url, '&');
		}
                g_string_append_printf (url, "%s=%s", key, value);
	}

	gchar *val;

	val = url->str;
	g_string_free (url, FALSE);

	g_debug ("Make the request %s\n", val);

	return val;
}

gboolean
ocs_cache_doc (OcsBackend *backend, xmlDocPtr doc_ptr, gchar *request_url)
{
	g_return_val_if_fail (doc_ptr && request_url, FALSE);

        const gchar *cache_dir;
        gchar *md5;
	gchar *local_dir;
        gchar *local_url;

        md5 = open_app_get_md5 ((const gchar *)request_url);
        cache_dir = ocs_backend_get_cache_dir (backend);
	local_dir = g_build_filename (cache_dir, "xml", NULL);
        if (!g_file_test (local_url, G_FILE_TEST_EXISTS)) {
		g_mkdir_with_parents (local_dir, 0755);
	}
        local_url = g_build_filename (cache_dir, "xml", md5, NULL);

	xmlSaveFile (local_url, doc_ptr);

        g_free (md5);
	g_free (local_dir);
        g_free (local_url);
}

xmlDocPtr
ocs_get_request_doc (OcsBackend *backend, gchar *request, gboolean need_refresh)
{
        const gchar *cache_dir;
        gchar *md5;
        gchar *local_url;
        xmlDocPtr doc_ptr;
        gboolean refresh;

	g_return_val_if_fail (request != NULL, NULL);

	refresh = need_refresh;

        md5 = open_app_get_md5 ((const gchar *)request);
        cache_dir = ocs_backend_get_cache_dir (backend);
        local_url = g_build_filename (cache_dir, "xml", md5, NULL);

        if (g_file_test (local_url, G_FILE_TEST_EXISTS)) {
		if (refresh) {
			g_unlink (local_url);
		} else {
			doc_ptr = xmlParseFile (local_url);
			if (!doc_ptr) {
				g_debug ("Cannot parse the saved xml? \n");
				g_unlink (local_url);
	                        refresh = TRUE;
        	        }
		}
        } else 
		refresh = TRUE;

        if (refresh) {
		SoupSession *session;
                SoupBuffer *buf;

		session = (SoupSession *) ocs_backend_get_session (backend);
                buf = open_app_get_data_by_request (session, request);
                doc_ptr = xmlParseMemory (buf->data, buf->length);
                if (!doc_ptr) {
                        printf ("Cannot parse the value!\n");
                }
                soup_buffer_free (buf);
        }
        g_free (md5);
        g_free (local_url);

        return doc_ptr;
}


