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

#include "ocs-app.h"
#include "ocs-backend.h"
#include "gnome-app-config.h"
#include "gnome-app-utils.h"

#if 0
#define backend_debug TRUE
#else
#define backend_debug FALSE
#endif

struct _OcsBackendPrivate
{
	gchar *server_uri;
	gchar *username;
	gchar *password;
	gboolean sync;
	gchar *cafile;
	SoupSession *session;

	GList *all_cid;
	GHashTable *cid_cname;
};

enum {
	PROP_0,
	PROP_CONFIG,
};

static SoupSession *current_session = NULL;

G_DEFINE_TYPE (OcsBackend, ocs_backend, APP_TYPE_BACKEND)

static void
ocs_backend_init (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	backend->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (backend,
						TYPE_OCS_BACKEND,
						OcsBackendPrivate);
	priv->all_cid = NULL;
	priv->cid_cname = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->server_uri = NULL;
	priv->username = NULL;
	priv->password = NULL;
	priv->sync = TRUE;
	priv->cafile = NULL;
	priv->session = NULL;
}

static const gchar *
get_backend_type (AppBackend *backend)
{
	return "ocs";
}
	
static const gchar *
get_backend_name (AppBackend *backend)
{
}

static const gchar *
get_backend_icon_name (AppBackend *backend)
{
}

/*FIXME: there must be such function in xml interface !*/
/*Search in the childnode of the rootnode*/
static xmlNodePtr
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
		
/*TODO: check the timestamp first, different api should have different refresh time.
 * If the current time  - create_time of the file longer than refresh time.
 * load from backend anyway!
*/
static gboolean
need_to_refresh (gchar *local_url)
{
	return FALSE;
}

static xmlDocPtr
get_doc_ptr (AppBackend *backend, gchar *url)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;

	GnomeAppConfig *config;
	gchar *md5;
	gchar *cache_dir;
	gchar *local_url;
	xmlDocPtr doc_ptr;
	gboolean refresh = TRUE;

	md5 = gnome_app_get_md5 (url);
        config = gnome_app_config_new ();
        cache_dir = gnome_app_config_get_cache_dir (config);
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
		SoupBuffer *buf;
		buf = gnome_app_get_data_from_url (priv->session, url);
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

	g_object_unref (config);
	g_free (md5);
	g_free (cache_dir);
	g_free (local_url);

	return doc_ptr;
}

static void
parse_all_cid (OcsBackend *ocs_backend, xmlNodePtr data_node)
{
	OcsBackendPrivate *priv = ocs_backend->priv;
        xmlNodePtr cate_node, id_node;
	gchar *id, *name;

	if (!data_node)
		return;

	for (cate_node = data_node->xmlChildrenNode; cate_node; cate_node = cate_node->next) {
		if (strcmp (cate_node->name, "category") == 0) {
			id = name = NULL;
			for (id_node = cate_node->xmlChildrenNode; id_node; id_node = id_node->next) {
				if (strcmp (id_node->name, "id") == 0) {
					id = xmlNodeGetContent (id_node);
					priv->all_cid = g_list_prepend (priv->all_cid, g_strdup (id));
				} else if (strcmp (id_node->name, "name") == 0) {
					name = xmlNodeGetContent (id_node);
				}
			}
			if (id) {
				/*Backend bug! */
				if (!name) {
					printf ("fatal error in backend !\n");
					name = id;
				}
				g_hash_table_insert (priv->cid_cname, g_strdup (id), g_strdup (name));
			}
		}
	}
	if (priv->all_cid)
		priv->all_cid = g_list_reverse (priv->all_cid);
}

static void
init_cid (AppBackend *backend)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
	const gchar *get_cate_string;
	gchar *url;
        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;

	if (priv->all_cid)
		return;

	get_cate_string = "/v1/content/categories";
	url = g_strdup_printf ("http://%s:%s@%s%s", priv->username, priv->password,
	                        priv->server_uri, get_cate_string);
        doc_ptr = get_doc_ptr (backend, url);
	if (doc_ptr) {
		data_node = find_node (doc_ptr, "data");
		parse_all_cid (ocs_backend, data_node);
        	xmlFreeDoc(doc_ptr);
	}
	
	g_free (url);
}

static gchar *
get_cname_by_id (AppBackend *backend, gchar *category_id)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
	gchar *cname;

	cname = g_hash_table_lookup (priv->cid_cname, category_id);
	if (cname) {
		return g_strdup (cname);
	} else {
		return NULL;
	}
}

static GList *
get_cid_list_by_group (AppBackend *backend, gchar *group)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
	GList *list = NULL, *l;
	gchar *id, *cname;

	if (!priv->all_cid)
		init_cid (backend);

	if (group == NULL) {
	/*FIXME: TODO: should make all_cid a struct with more category fileds */
		return g_list_copy (priv->all_cid);
	} else {
		for (l = priv->all_cid; l; l = l->next) {
			id = (gchar *) l->data;
			cname = get_cname_by_id (backend, id);
			if (gnome_app_category_match_group (cname, group))
				list = g_list_prepend (list, g_strdup (id));
			g_free (cname);
		}	
	}
	if (list)
		list = g_list_reverse (list);
	return list;
}

static GList *
parse_appid_list (OcsBackend *ocs_backend, xmlNodePtr data_node)
{
	OcsBackendPrivate *priv = ocs_backend->priv;
        xmlNodePtr apps_node, id_node;
	gchar *id;
	GList *list = NULL;

	for (apps_node = data_node->xmlChildrenNode; apps_node; apps_node = apps_node->next) {
		if (strcmp (apps_node->name, "content") == 0) {
			for (id_node = apps_node->xmlChildrenNode; id_node; id_node = id_node->next) {
				if (strcmp (id_node->name, "id") == 0) {
					id = xmlNodeGetContent (id_node);
					list = g_list_prepend (list, g_strdup (id));
				}
			}
		}
	}
	if (list)
		list = g_list_reverse (list);

	return list;
}

static GList *
get_apps_by_query (AppBackend *backend, GnomeAppQuery *query)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
	GString *request = NULL;
	gchar *sprop [] = {"categories", "search", "user", "external", 
			"distribution", "license", "sortmode", NULL};
	gchar *iprop [] = {"page", "pagesize", NULL};
	gchar *scontent;
	gint icontent;
	gint i;
	gboolean begin;

	request = g_string_new ("https://");
	g_string_append_printf (request, "%s:%s@%s/v1/content/data?", priv->username, priv->password, priv->server_uri);

	/*TODO: if the prop was not exist, solve it ! */
	begin = TRUE;
	for (i = 0; *(sprop + i); i++) {
		g_object_get (query, *(sprop + i), &scontent, NULL);
		if (scontent) {
			if (begin)
				begin = FALSE;
			else
				g_string_append_c (request, '&');
			g_string_append_printf (request, "%s=%s", *(sprop + i), scontent);
			g_free (scontent);
		}
	}

	for (i = 0; *(iprop + i); i++) {
		g_object_get (query, *(iprop + i), &icontent, NULL);
		if (begin)
			begin = FALSE;
		else
			g_string_append_c (request, '&');
		g_string_append_printf (request, "%s=%d", *(iprop + i), icontent);
	}

	GList *list = NULL;
        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;
printf ("request %s\n", request->str);
        doc_ptr = get_doc_ptr (backend, request->str);
	if (doc_ptr) {
		data_node = find_node (doc_ptr, "data");
		list = parse_appid_list (ocs_backend, data_node);
        	xmlFreeDoc(doc_ptr);
	}

	g_string_free (request, TRUE);

	return list;
}

/*FIXME: it is the 'summary' content, we could preserve this content first .. */
static GList *
get_appid_list_by_cid_list (AppBackend *backend, GList *cid_list)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
        const gchar *get_apps_with_id;
        gchar *url;
	gchar *cid;
	GList *l;
	GString *ids = NULL;
	
	const gchar *pagesize = "&pagesize=35";
        get_apps_with_id = "/v1/content/data?categories=";

	for (l = cid_list; l; l = l->next) {
		cid = (gchar *) l->data;
		if (!ids)
			ids = g_string_new (cid);
		else {
			g_string_append (ids, "x");
			g_string_append (ids, cid);
		}
	}
	if (!ids) {
	        url = g_strdup_printf ("http://%s:%s@%s", priv->username, priv->password,
                                priv->server_uri, pagesize);
	} else {
	        url = g_strdup_printf ("http://%s:%s@%s%s%s%s", priv->username, priv->password,
                                priv->server_uri, get_apps_with_id, ids->str, pagesize);
		g_string_free (ids, TRUE);
	}

        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;
	GList *list = NULL;

        doc_ptr = get_doc_ptr (backend, url);
	if (doc_ptr) {
		data_node = find_node (doc_ptr, "data");
		list = parse_appid_list (ocs_backend, data_node);
        	xmlFreeDoc(doc_ptr);
	}

	g_free (url);

	return list;
}

static GnomeAppInfo *
get_app_by_id (AppBackend *backend, gchar *app_id)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;
        const gchar *get_app_details_with_id;
        gchar *url;

        get_app_details_with_id = "/v1/content/data/";

        url = g_strdup_printf ("http://%s:%s@%s%s%s", priv->username, priv->password,
                                priv->server_uri, get_app_details_with_id, app_id);

        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;
	GnomeAppInfo *info = NULL;

        doc_ptr = get_doc_ptr (backend, url);
	if (doc_ptr) {
		data_node = find_node (doc_ptr, "data");
		/*parse_app is done in a seperate file */
		info = parse_app (ocs_backend, data_node);
        	xmlFreeDoc(doc_ptr);
	}

        g_free (url);

	return info;
}

static gboolean
set_config (AppBackend *backend, GnomeAppConfig *config)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;

	priv->server_uri = gnome_app_config_get_server_uri (config);
	/*FIXME: the following should be retrieved from config too*/
	priv->username = g_strdup ("initlove");
	priv->password = g_strdup ("novell123456");
	priv->sync = TRUE;
	priv->cafile = NULL;
	priv->session = gnome_app_soup_session_new (priv->sync, priv->cafile);

	current_session = priv->session;

	return TRUE;
}

static void
ocs_backend_dispose (GObject *object)
{
        G_OBJECT_CLASS (ocs_backend_parent_class)->dispose (object);
}

static void
ocs_backend_finalize (GObject *object)
{
        OcsBackend *backend = OCS_BACKEND (object);
        OcsBackendPrivate *priv = backend->priv;

        if (priv->cid_cname)
                g_hash_table_destroy (priv->cid_cname);
	if (priv->all_cid)
		g_list_free (priv->all_cid);
	if (priv->server_uri)
		g_free (priv->server_uri);
	if (priv->username)
		g_free (priv->username);
	if (priv->password)
		g_free (priv->password);
	if (priv->cafile)
		g_free (priv->cafile);
	if (priv->session)
		g_object_unref (priv->session);
	current_session = NULL;

        G_OBJECT_CLASS (ocs_backend_parent_class)->finalize (object);
}

static void
ocs_backend_class_init (OcsBackendClass *klass)
{
	AppBackendClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = ocs_backend_dispose;
	object_class->finalize = ocs_backend_finalize;

	parent_class = APP_BACKEND_CLASS (klass);

	parent_class->get_backend_type = get_backend_type;
	parent_class->get_backend_name = get_backend_name;
	parent_class->get_backend_icon_name = get_backend_icon_name;
	parent_class->get_cid_list_by_group = get_cid_list_by_group;
	parent_class->get_cname_by_id = get_cname_by_id;
	parent_class->get_appid_list_by_cid_list = get_appid_list_by_cid_list;
	parent_class->get_apps_by_query = get_apps_by_query;
	parent_class->get_app_by_id = get_app_by_id;
	parent_class->set_config = set_config;

	g_type_class_add_private (object_class, sizeof (OcsBackendPrivate));
}

SoupSession *
ocs_backend_get_current_session ()
{
	return current_session;
}
