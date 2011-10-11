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

#include "ocs-app.h"
#include "ocs-server.h"
#include "gnome-app-config.h"
#include "gnome-app-utils.h"

#if 0
#define server_debug TRUE
#else
#define server_debug FALSE
#endif

struct _OcsServerPrivate
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


G_DEFINE_TYPE (OcsServer, ocs_server, APP_TYPE_SERVER)

static void
ocs_server_init (OcsServer *server)
{
	OcsServerPrivate *priv;

	server->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (server,
						TYPE_OCS_SERVER,
						OcsServerPrivate);
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
get_server_type (AppServer *server)
{
	return "ocs";
}
	
static const gchar *
get_server_name (AppServer *server)
{
}

static const gchar *
get_server_icon_name (AppServer *server)
{
}

/*FIXME: there must be such function in xml interface !*/
static xmlNodePtr
get_data_node (xmlDocPtr doc_ptr)
{
	xmlNodePtr root_node, data_node;

	root_node = xmlDocGetRootElement (doc_ptr);

	if (!root_node) {
		printf ("Cannot get the root node!\n");
		return NULL;
	}

	for (data_node = root_node->xmlChildrenNode; data_node; data_node = data_node->next) {
		if (strcmp (data_node->name, "data") == 0)
			return data_node;
	}
	return NULL;
}
	
static void
parse_all_cid (OcsServer *ocs_server, xmlNodePtr data_node)
{
	OcsServerPrivate *priv = ocs_server->priv;
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
				/*Server bug! */
				if (!name) {
					printf ("fatal error in server !\n");
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
init_cid (AppServer *server)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;
	const gchar *get_cate_string;
	gchar *url;
	SoupBuffer *buf;
        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;

	if (priv->all_cid)
		return;

	get_cate_string = "/v1/content/categories";
	url = g_strdup_printf ("http://%s:%s@%s%s", priv->username, priv->password,
	                        priv->server_uri, get_cate_string);
	buf = gnome_app_get_data_from_url (priv->session, url);
        doc_ptr = xmlParseMemory (buf->data, buf->length);
	if (!doc_ptr) {
		printf ("Cannot parse the value!\n");
	} else {
		data_node = get_data_node (doc_ptr);
		parse_all_cid (ocs_server, data_node);
        	xmlFreeDoc(doc_ptr);
	}
	
	soup_buffer_free (buf);
	g_free (url);
}

static GList *
get_cid_list_by_group (AppServer *server, gchar *group)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;
	GList *list = NULL;

	if (!priv->all_cid)
		init_cid (server);

	if (group == NULL)
		return g_list_copy (priv->all_cid);
	else {
		/*FIXME: not done yet ! */
	}
	return list;
}

static gchar *
get_cname_by_id (AppServer *server, gchar *category_id)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;
	gchar *cname;

	cname = g_hash_table_lookup (priv->cid_cname, category_id);
	if (cname) {
		return g_strdup (cname);
	} else {
		return NULL;
	}
}


static GList *
parse_appid_list (OcsServer *ocs_server, xmlNodePtr data_node)
{
	OcsServerPrivate *priv = ocs_server->priv;
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

/*FIXME: it is the 'summary' content, we could preserve this content first .. */
static GList *
get_appid_list_by_cid_list (AppServer *server, GList *cid_list)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;
        const gchar *get_apps_with_id;
        gchar *url;
	gchar *cid;
	GList *l;
	GString *ids = NULL;
	
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
	if (!ids)
		return NULL;
        url = g_strdup_printf ("http://%s:%s@%s%s%s", priv->username, priv->password,
                                priv->server_uri, get_apps_with_id, ids->str);
	g_string_free (ids, TRUE);

        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;
	SoupBuffer *buf;
	GList *list = NULL;

	buf = gnome_app_get_data_from_url (priv->session, url);

        doc_ptr = xmlParseMemory (buf->data, buf->length);
	if (!doc_ptr) {
		printf ("Cannot parse the value!\n");
	} else {
		data_node = get_data_node (doc_ptr);
		list = parse_appid_list (ocs_server, data_node);
        	xmlFreeDoc(doc_ptr);
	}

	soup_buffer_free (buf);
	g_free (url);

	return list;
}

static GnomeAppItem *
get_app_by_id (AppServer *server, gchar *app_id)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;
        const gchar *get_app_details_with_id;
        gchar *url;

        get_app_details_with_id = "/v1/content/data/";

        url = g_strdup_printf ("http://%s:%s@%s%s%s", priv->username, priv->password,
                                priv->server_uri, get_app_details_with_id, app_id);

        xmlDocPtr doc_ptr;
        xmlNodePtr data_node;
	SoupBuffer *buf;
	GnomeAppItem *item = NULL;

	buf = gnome_app_get_data_from_url (priv->session, url);
        doc_ptr = xmlParseMemory (buf->data, buf->length);
	if (!doc_ptr) {
		printf ("Cannot parse the value!\n");
	} else {
		data_node = get_data_node (doc_ptr);
		/*parse_app is done in a seperate file */
		item = parse_app (ocs_server, data_node);
        	xmlFreeDoc(doc_ptr);
	}

	soup_buffer_free (buf);
        g_free (url);

	return item;
}

static gboolean
set_config (AppServer *server, GnomeAppConfig *config)
{
	OcsServer *ocs_server = OCS_SERVER (server);
	OcsServerPrivate *priv = ocs_server->priv;

	priv->server_uri = gnome_app_config_get_server_uri (config);
	/*FIXME: the following should be retrieved from config too*/
	priv->username = g_strdup ("initlove");
	priv->password = g_strdup ("novell123456");
	priv->sync = TRUE;
	priv->cafile = NULL;
	priv->session = gnome_app_soup_session_new (priv->sync, priv->cafile);

	return TRUE;
}

static void
ocs_server_dispose (GObject *object)
{
        G_OBJECT_CLASS (ocs_server_parent_class)->dispose (object);
}

static void
ocs_server_finalize (GObject *object)
{
        OcsServer *server = OCS_SERVER (object);
        OcsServerPrivate *priv = server->priv;

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

        G_OBJECT_CLASS (ocs_server_parent_class)->finalize (object);
}

static void
ocs_server_class_init (OcsServerClass *klass)
{
	AppServerClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = ocs_server_dispose;
	object_class->finalize = ocs_server_finalize;

	parent_class = APP_SERVER_CLASS (klass);

	parent_class->get_server_type = get_server_type;
	parent_class->get_server_name = get_server_name;
	parent_class->get_server_icon_name = get_server_icon_name;
	parent_class->get_cid_list_by_group = get_cid_list_by_group;
	parent_class->get_cname_by_id = get_cname_by_id;
	parent_class->get_appid_list_by_cid_list = get_appid_list_by_cid_list;
	parent_class->get_app_by_id = get_app_by_id;
	parent_class->set_config = set_config;

	g_type_class_add_private (object_class, sizeof (OcsServerPrivate));
}

