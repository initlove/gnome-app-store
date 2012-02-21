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

//#include <libsoup/soup-session.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>

#include "open-services.h"
#include "open-app-config.h"
#include "open-app-utils.h"

#include "ocs-results.h"
#include "ocs-result.h"
#include "ocs-request.h"
#include "ocs-backend.h"

struct _OcsBackendPrivate
{
	gchar *server_uri;
	gchar *username;
	gchar *password;
	gboolean sync;
	gchar *cafile;
	gchar *cache_dir;
	SoupSession *session;

	GHashTable *categories;
};

G_DEFINE_TYPE (OcsBackend, ocs_backend, APP_TYPE_BACKEND)

static gboolean
category_name_match (const gchar *name, const gchar *category)
{
	g_return_val_if_fail (name && category, FALSE);

	if (strcasestr (name, category))
		return TRUE;
	else
		return FALSE;
}

static void
setup_category (OcsBackend *ocs_backend, GList *results_data)
{
	OcsBackendPrivate *priv;
	const gchar **default_categories;
	GString *ids [100];	/*FIXME: */
	gchar *categories [100];
	int i;
	GList *list, *l;
	OpenResult *result;
	const gchar *id;
	const gchar *name;
	gboolean match;
	gint other;

	priv = ocs_backend->priv;
	default_categories = open_app_get_default_categories ();
	for (i = 0; default_categories [i]; i++) {
		ids [i] = NULL;
		categories [i] = g_strdup (default_categories [i]);
		if (strlen (categories [i]) > 3)
			*(categories [i] + 3) = 0;
	}
	ids [i] = NULL;
	categories [i] = NULL;
	other = i - 1;	/*FIXME: other is the last one */

	for (l = results_data; l; l = l->next) {
		result = OPEN_RESULT (l->data);
		id = open_result_get (result, "id");
		name = open_result_get (result, "name");
		if (!id || !id [0] || !name || !name [0])
			continue;
		match = FALSE;

		for (i = 0; categories [i]; i++) {
			if (category_name_match (name, (const gchar *)categories [i])) {
				if (match == FALSE)
					match = TRUE;
				if (ids [i] == NULL) {
					ids [i] = g_string_new (id);
				} else {
			                g_string_append_c (ids [i], 'x');
        		                g_string_append (ids [i], id);
				}
			}
		}
		if (!match) {
			if (ids [other] == NULL) {
				ids [other] = g_string_new (id);
			} else {
			        g_string_append_c (ids [other], 'x');
        		        g_string_append (ids [other], id);
			}
		}
	}

	for (i = 0; default_categories [i]; i++) {
		if (ids [i]) {
			g_hash_table_insert (priv->categories, g_strdup (default_categories [i]), ids [i]->str);
			g_string_free (ids [i], FALSE);
		}
	}
}

static void
init_category (OcsBackend *ocs_backend)
{
	OpenResults *results;
	AppRequest *request;
        gchar *url;
        xmlDocPtr doc_ptr;
	gboolean refresh;

	refresh = FALSE;
	request = app_request_new ();
	app_request_set (request, "operation", "categories");
	url = ocs_get_request_url (ocs_backend, request);
        doc_ptr = ocs_get_request_doc (ocs_backend, url, refresh);
        if (doc_ptr) {
		results = ocs_get_results (request, doc_ptr);
		if (!refresh && ocs_results_get_status (results)) {
			GList *list;

			list = ocs_results_get_data (results);
			setup_category (ocs_backend, list);
			ocs_cache_doc (ocs_backend, doc_ptr, url);
		}
		xmlFreeDoc(doc_ptr);
	}

        g_object_unref (request);
	g_free (url);
}

const gchar *
ocs_get_categories_by_name (OcsBackend *backend, const gchar *category_name)
{
	g_return_val_if_fail (category_name, NULL);

	OcsBackendPrivate *priv;
	const gchar *val;

	priv = backend->priv;
	val = (const gchar *) g_hash_table_lookup (priv->categories, category_name);
/*TODO: if the category group is empty, we set the ids  to -1,
	in this case, the return value will be empty!
*/
	if (!val || !val [0])
		val = "-1";
	
	return val;
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
	priv->categories = NULL;
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

static OpenResults *
get_results (AppBackend *backend, OpenRequest *request)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;

	gchar *url;
	xmlDocPtr doc_ptr;
	OpenResults *results;
	gboolean refresh;

	results = NULL;
	refresh = FALSE;		/*TODO: make refresh mechanism */
	url = ocs_get_request_url (ocs_backend, request);
	doc_ptr = ocs_get_request_doc (ocs_backend, url, refresh);
	if (doc_ptr) {
		results = ocs_get_results (request, doc_ptr);
		/*TODO: if need to refresh, most time, it means, no need to cache */
		if (!refresh && ocs_results_get_status (results))
			ocs_cache_doc (ocs_backend, doc_ptr, url);
		xmlFreeDoc(doc_ptr);
	}

	g_free (url);

	return results;
}

/*TODO: FIXME: if set config for more than once, do the free thing.. */
static gboolean
set_config (AppBackend *backend, OpenAppConfig *config)
{
	OcsBackend *ocs_backend = OCS_BACKEND (backend);
	OcsBackendPrivate *priv = ocs_backend->priv;

	priv->server_uri = g_strdup (open_app_config_get_server_uri (config));
	/*FIXME: the following should be retrieved from config too*/
	priv->username = g_strdup (open_app_config_get_username (config));
	priv->password = g_strdup (open_app_config_get_password (config));
	priv->sync = TRUE;
	priv->cafile = NULL;
//	priv->session = open_app_soup_session_new (priv->sync, priv->cafile);
	priv->cache_dir = open_app_config_get_cache_dir (config);
	priv->categories = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	init_category (ocs_backend);

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
	if (priv->categories)
		g_hash_table_destroy (priv->categories);

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
	parent_class->get_results = get_results;
	parent_class->set_config = set_config;

	g_type_class_add_private (object_class, sizeof (OcsBackendPrivate));
}

const gchar *
ocs_backend_get_server_uri (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	priv = backend->priv;

	return (const gchar *) priv->server_uri;
}

const gchar *
ocs_backend_get_username (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	priv = backend->priv;

	return (const gchar *) priv->username;
}

const gchar *
ocs_backend_get_password (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	priv = backend->priv;

	return (const gchar *) priv->password;
}

const SoupSession *
ocs_backend_get_session (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	priv = backend->priv;

	return (const SoupSession *) priv->session;
}

const gchar *
ocs_backend_get_cache_dir (OcsBackend *backend)
{
	OcsBackendPrivate *priv;

	priv = backend->priv;

	return (const gchar *) priv->cache_dir;
}
