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
#include <glib/gdir.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>
#include "gnome-app-item.h"
#include "gnome-app-utils.h"
#include "app-stream-backend.h"
#include "gnome-app-config.h"

struct _AppStreamBackendPrivate
{
	gchar *server_uri;
	gchar *cache_dir;
	GList *apps;
};

enum {
        PROP_0,
        PROP_CONFIG,
};

/*FIXME: better way to do it ? */
static GHashTable *icon_store = NULL;

G_DEFINE_TYPE (AppStreamBackend, app_stream_backend, APP_TYPE_BACKEND)

static void download_meta_data (GnomeAppConfig *config);

typedef enum _APP_STREAM_KEY_WORDS {
        APP_STREAM_ID = 0,
        APP_STREAM_PKGNAME,
        APP_STREAM_NAME,
        APP_STREAM_SUMMARY,
        APP_STREAM_ICON,
        APP_STREAM_APPCATEGORIES,
        APP_STREAM_MIMETYPES,
        APP_STREAM_APPCATEGORY,
        APP_STREAM_MIMETYPE,
        APP_STREAM_LAST,
} APP_STREAM_KEY_WORDS;

static APP_STREAM_KEY_WORDS
get_type_from_name (gchar *name)
{
        gchar *key_words [] = {"id", "pkgname", "name", "summary", "icon",
                        "appcategories", "mimetypes", "appcategory", "mimetype", NULL};
        int i;

        for (i = 0; i < APP_STREAM_LAST; i++) {
                if (strcmp (name, key_words [i]) == 0) {
                        return i;
                }
        }

        return APP_STREAM_LAST;
}

static gchar *
get_local_icon_url (GnomeAppItem *item)
{
	const gchar *icon_name;
	gchar *url = NULL;

	icon_name = gnome_app_item_get_icon_name (item);

        if (icon_name) {
                gchar *p, *content;
                content = g_strdup (icon_name);
                p = strrchr (content, '.');
                if (p)
                        *p = 0;
                url = g_hash_table_lookup (icon_store, content);
                g_free (content);
	}

	if (!url)
		url = g_hash_table_lookup (icon_store, "default");

	return g_strdup (url);
}

static void
set_item_callbacks (GnomeAppItem *item)
{
	GnomeAppItemClass *class = GNOME_APP_ITEM_GET_CLASS (item);

	class->get_local_icon_url = get_local_icon_url;
	/* if we did not set other callback 
	   it means we did not support it
	class->get_local_screenshot_url =
	class->get_comment_counts =
	class->get_rate =
	*/
}

static GnomeAppItem *
load_app (xmlNodePtr app_node)
{
        xmlNodePtr node, sub_node;
        APP_STREAM_KEY_WORDS type;
	gchar *p;
	GString *str = NULL;
        GnomeAppItem *item = NULL;

        for (node = app_node->xmlChildrenNode; node; node = node->next) {
                type = get_type_from_name ((gchar *)(node->name));
                switch (type) {
                        case APP_STREAM_ID:
				/* Should we just move this to the begin of load_app? cause it was a backend bug ! */
				item = gnome_app_item_new ();
				set_item_callbacks (item);
                        case APP_STREAM_PKGNAME:
                        case APP_STREAM_NAME:
                        case APP_STREAM_SUMMARY:
                        case APP_STREAM_ICON:
				g_object_set (G_OBJECT (item), (gchar *)node->name, xmlNodeGetContent (node), NULL);
                                break;
                        case APP_STREAM_APPCATEGORIES:
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
					p = xmlNodeGetContent (sub_node);
					if (!p || strlen (p) < 1)
						continue;
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == APP_STREAM_APPCATEGORY) {
						if (str == NULL) {
							str = g_string_new (p);
							g_string_append (str, ";");
						} else {
							g_string_append (str, p);
							g_string_append (str, ";");
						}
					}
                                }
				if (str) {
					g_object_set (G_OBJECT (item), "categories", str->str, NULL);
					g_string_free (str, TRUE);
					str = NULL;
				}
                                break;
                        case APP_STREAM_MIMETYPES:
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
					p = xmlNodeGetContent (sub_node);
					if (!p || strlen (p) < 1)
						continue;
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == APP_STREAM_MIMETYPE) {
						if (str == NULL) {
							str = g_string_new (p);
							g_string_append (str, ";");
						} else {
							g_string_append (str, p);
							g_string_append (str, ";");
						}
					}
                                }
				if (str) {
					g_object_set (G_OBJECT (item), "mimetypes", str->str, NULL);
					g_string_free (str, TRUE);
					str = NULL;
				}
                                break;
                        default:
                                break;
                }
        }

	return item;
}

static void
load_apps (AppStreamBackend *backend, gchar *appdata_uri)
{
        xmlDocPtr doc_ptr;
        xmlNodePtr root_node, app_node;

	doc_ptr = xmlParseFile (appdata_uri);

        root_node = xmlDocGetRootElement (doc_ptr);

        for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
                if (strcmp (app_node->name, "application") == 0) {
                        GnomeAppItem *item = load_app (app_node);
                        if (item)
                                backend->priv->apps = g_list_prepend (backend->priv->apps, item);
                }
        }

        xmlFreeDoc(doc_ptr);
}

static void
load_icons (AppStreamBackend *backend, gchar *icon_dir)
{
	GDir *dir;

        dir = g_dir_open (icon_dir, 0, NULL);

        if (!dir) {
                printf ("cannot open dir\n");
                g_free (icon_dir);
                return;
        }

        const gchar *basename;
        gchar *filename;
        gchar *p, *content;

        while ((basename = g_dir_read_name (dir)) != NULL) {
                filename = g_build_filename (icon_dir, basename, NULL);
                content = g_strdup (basename);
                p = strrchr (content, '.');
                if (p)
                        *p = 0;

                g_hash_table_insert (icon_store, content, filename);
        }
        g_dir_close (dir);

}

static void
app_stream_backend_init (AppStreamBackend *backend)
{
	AppStreamBackendPrivate *priv;
	gchar *appdata_uri;

	backend->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (backend,
						TYPE_APP_STREAM_BACKEND,
						AppStreamBackendPrivate);
	backend->priv->server_uri = NULL;
	backend->priv->cache_dir = NULL;
	priv->apps = NULL;
	icon_store = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

static const gchar *
get_backend_type (AppBackend *backend)
{
	return "app_stream";
}
        
static const gchar *
get_backend_name (AppBackend *backend)
{
        AppStreamBackend *app_stream_backend = APP_STREAM_BACKEND (backend);

	return app_stream_backend->priv->server_uri;
}

static const gchar *
get_backend_icon_name (AppBackend *backend)
{
}

static GList *
get_cid_list_by_group (AppBackend *backend, gchar *group)
{
	GList *list = NULL;

	if (group)
		list = g_list_prepend (list, g_strdup (group));
	else
		list = g_list_copy ((GList *)gnome_app_get_local_categories ());

	return list;
}

static gchar *
get_cname_by_id (AppBackend *backend, gchar *category_id)
{
	/* id == name */
	return g_strdup (category_id);
}

gboolean
item_match_categories (GnomeAppItem *item, GList *categories)
{
	GList *l;
	gchar *str;
	gchar *category;
	str = (gchar *)gnome_app_item_get_categories (item);

	if (!str)
		return FALSE;

	for (l = categories; l; l = l->next) {
		category = (gchar *) l->data;

		if (!gnome_app_category_match_group (str, category))
			return FALSE;
	}

	return TRUE;
}

static GList *
get_appid_list_by_cid_list (AppBackend *backend, GList *categories)
{
        AppStreamBackend *app_stream_backend = APP_STREAM_BACKEND (backend);

	GList *list, *l;
	GnomeAppItem *item;
	const gchar *app_id;

	list = NULL;
	for (l = app_stream_backend->priv->apps; l; l = l->next) {
		item = GNOME_APP_ITEM (l->data);
		if (item_match_categories (item, categories)) {
			app_id = gnome_app_item_get_id (item);
			list = g_list_prepend (list, g_strdup (app_id));
		}
	}

	return list;
}

static GnomeAppItem *
get_app_by_id (AppBackend *backend, gchar *id)
{
        AppStreamBackend *app_stream_backend = APP_STREAM_BACKEND (backend);

	GnomeAppItem *item;
	const gchar *app_id;
	GList *l;

	for (l = app_stream_backend->priv->apps; l; l = l->next) {
		item = GNOME_APP_ITEM (l->data);
		app_id = gnome_app_item_get_id (item);
		if (strcmp (app_id, id) == 0)
			return g_object_ref (item);
	}

	return NULL;
}

static gboolean
set_config (AppBackend *backend, GnomeAppConfig *conf)
{
        AppStreamBackend *app_stream_backend = APP_STREAM_BACKEND (backend);
	gchar *appdata_uri;
	gchar *icon_dir;

	app_stream_backend->priv->server_uri = gnome_app_config_get_server_uri (conf);
	app_stream_backend->priv->cache_dir = gnome_app_config_get_cache_dir (conf);
	download_meta_data (conf);

	appdata_uri = g_build_filename (app_stream_backend->priv->cache_dir, "appdata.xml", NULL);
	icon_dir = g_build_filename (app_stream_backend->priv->cache_dir, "icons", NULL);
	/*FIXME: tmp */
	if (!g_file_test (appdata_uri, G_FILE_TEST_EXISTS)) {
		printf ("The thin backend is not done, might not be done forever..!");
		printf ("the tmp solution is, you should  copy the all the data in test-data dir to %s\n", app_stream_backend->priv->cache_dir);
		exit (1);
	}
	load_apps (app_stream_backend, appdata_uri);
	load_icons (app_stream_backend, icon_dir);

	g_free (appdata_uri);
	g_free (icon_dir);
	return TRUE;
}

static void
app_stream_backend_dispose (GObject *object)
{
        G_OBJECT_CLASS (app_stream_backend_parent_class)->dispose (object);
}

static void
app_stream_backend_finalize (GObject *object)
{
        AppStreamBackend *backend = APP_STREAM_BACKEND (object);
        AppStreamBackendPrivate *priv = backend->priv;

	if (priv->server_uri)
		g_free (priv->server_uri);
	if (priv->cache_dir)
		g_free (priv->cache_dir);
	if (priv->apps)
		g_list_free (priv->apps);

	if (icon_store) {
		g_hash_table_destroy (icon_store);
		icon_store = NULL;
	}

        G_OBJECT_CLASS (app_stream_backend_parent_class)->finalize (object);
}

static void
app_stream_backend_class_init (AppStreamBackendClass *klass)
{
	AppBackendClass *parent_class;
        GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);
	object_class->dispose = app_stream_backend_dispose;
	object_class->finalize = app_stream_backend_finalize;

	parent_class = APP_BACKEND_CLASS (klass);
	parent_class->get_backend_type = get_backend_type;
	parent_class->get_backend_name = get_backend_name;
	parent_class->get_backend_icon_name = get_backend_icon_name;
	parent_class->get_cid_list_by_group = get_cid_list_by_group;
	parent_class->get_cname_by_id = get_cname_by_id;
	parent_class->get_appid_list_by_cid_list = get_appid_list_by_cid_list;
	parent_class->get_app_by_id = get_app_by_id;
	parent_class->set_config = set_config;

        g_type_class_add_private (object_class, sizeof (AppStreamBackendPrivate));
}

static void download_meta_data (GnomeAppConfig *config)
{
	/*FIXME: thin backend: now suppose it was downloaded */
}


