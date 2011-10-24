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

#include "ocs-app-info.h"
#include "ocs-request.h"
#include "ocs-backend.h"
#include "gnome-app-config.h"
#include "gnome-app-utils.h"

struct _OcsBackendPrivate
{
	gchar *server_uri;
	gchar *username;
	gchar *password;
	gboolean sync;
	gchar *cafile;
	gchar *cache_dir;
	SoupSession *session;

	GList *cids;
	GHashTable *cid_name;

};

G_DEFINE_TYPE (OcsBackend, ocs_backend, APP_TYPE_BACKEND)

static void
parse_cids (OcsBackend *ocs_backend, xmlNodePtr data_node)
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
                                        priv->cids = g_list_prepend (priv->cids, g_strdup (id));
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
                                g_hash_table_insert (priv->cid_name, g_strdup (id), g_strdup (name));
                        }
                }
        }
}

static void
init_cids (OcsBackend *ocs_backend)
{
        OcsBackendPrivate *priv = ocs_backend->priv;
        gchar *request;
        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;

        request = g_strdup_printf ("http://%s:%s@%s/v1/content/categories",
				priv->username,
				priv->password,
				priv->server_uri);
        doc_ptr = ocs_get_request_doc (ocs_backend, request);
        if (doc_ptr) {
                data_node = find_node (doc_ptr, "data");
                parse_cids (ocs_backend, data_node);
                xmlFreeDoc(doc_ptr);
        }

        g_free (request);
}

static gchar *
get_cname_by_id (OcsBackend *backend, gchar *category_id)
{
        OcsBackendPrivate *priv = backend->priv;
        gchar *cname;

        cname = g_hash_table_lookup (priv->cid_name, category_id);
        if (cname) {
                return g_strdup (cname);
        } else {
                return NULL;
        }
}

GList *
ocs_get_cid_list_by_group (OcsBackend *backend, gchar *group)
{
        OcsBackendPrivate *priv = backend->priv;
        GList *list = NULL, *l;
        gchar *id, *cname;

        if (group == NULL) {
        /*FIXME: TODO: should make cids a struct with more category fileds */
                return g_list_copy (priv->cids);
        } else {
                for (l = priv->cids; l; l = l->next) {
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

static void
ocs_backend_init (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	backend->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (backend,
						TYPE_OCS_BACKEND,
						OcsBackendPrivate);
	priv->server_uri = NULL;
	priv->username = NULL;
	priv->password = NULL;
	priv->sync = TRUE;
	priv->cafile = NULL;
	priv->session = NULL;
	priv->cache_dir = NULL;
	priv->cids = NULL;
	priv->cid_name = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
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

static GList *
parse_apps (OcsBackend *ocs_backend, xmlNodePtr data_node)
{
	OcsBackendPrivate *priv = ocs_backend->priv;
	xmlNodePtr apps_node, id_node;
	gchar *id;
	GList *list = NULL;

	OcsAppInfo *info;

	for (apps_node = data_node->xmlChildrenNode; apps_node; apps_node = apps_node->next) {
		if (strcmp (apps_node->name, "content") == 0) {
			info = ocs_app_info_new ();
			ocs_app_info_set_summary (info, apps_node);
			list = g_list_prepend (list, info);
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

	gchar *request;
	xmlDocPtr doc_ptr;
	xmlNodePtr data_node;
	GList *list;

	request = make_request_by_query (ocs_backend, query);
	doc_ptr = ocs_get_request_doc (ocs_backend, request);

	if (doc_ptr) {
		data_node = find_node (doc_ptr, "data");
		list = parse_apps (ocs_backend, data_node);
		xmlFreeDoc(doc_ptr);
	} else
		list = NULL;

	g_free (request);

	return list;
}

/*TODO: FIXME: if set config for more than once, do the free thing.. */
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
	priv->cache_dir = gnome_app_config_get_cache_dir (config);

	init_cids (ocs_backend);

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
	if (priv->cache_dir)
		g_free (priv->cache_dir);
	if (priv->cids)
		g_list_free (priv->cids);
	if (priv->cid_name)
		g_hash_table_destroy (priv->cid_name);

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
	parent_class->get_apps_by_query = get_apps_by_query;
	parent_class->set_config = set_config;

	g_type_class_add_private (object_class, sizeof (OcsBackendPrivate));
}

const gchar *
ocs_backend_get_server_uri (OcsBackend *backend)
{
	return (const gchar *) backend->priv->server_uri;
}

const gchar *
ocs_backend_get_username (OcsBackend *backend)
{
	return (const gchar *) backend->priv->username;
}

const gchar *
ocs_backend_get_password (OcsBackend *backend)
{
	return (const gchar *) backend->priv->password;
}

const SoupSession *
ocs_backend_get_session (OcsBackend *backend)
{
	return (const SoupSession *) backend->priv->session;
}

const gchar *
ocs_backend_get_cache_dir (OcsBackend *backend)
{
	return (const gchar *) backend->priv->cache_dir;
}
