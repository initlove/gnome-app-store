/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <glib/gdir.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#include <clutter/clutter.h>

#include "gnome-app-store.h"
#include "backend/app-backend.h"
#include "common/gnome-app-info.h"
#include "common/gnome-app-config.h"
#include "common/gnome-app-query.h"

struct _GnomeAppStorePrivate
{
	AppBackend *backend;
	
	/*FIXME: Donnot use this at present */
	GList *appid_list;	/* should be reload every timestamp time */
	GHashTable *app_id;
	gint app_timestamp;	/*FIXME: 1. should category have timestamp too?
					 2. should appid_list and app_id have their own timestamp?
					 3. should each app have its own timestamp?  add to the GnomeAppInfo ?
				*/
};

G_DEFINE_TYPE (GnomeAppStore, gnome_app_store, G_TYPE_OBJECT)

static void
gnome_app_store_init (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;
	GnomeAppConfig *config;

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_APP_TYPE_STORE,
                                                   GnomeAppStorePrivate);

	priv->appid_list = NULL;
	priv->app_id = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
	priv->app_timestamp = -1;	/* FIXME: not implement yet */
	config = gnome_app_config_new ();
	priv->backend = app_backend_new_from_config (config);
	g_object_unref (config);
}

static void
gnome_app_store_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_store_parent_class)->dispose (object);
}

static void
gnome_app_store_finalize (GObject *object)
{
	GnomeAppStore *store = GNOME_APP_STORE (object);
	GnomeAppStorePrivate *priv = store->priv;

	if (priv->appid_list)
		g_list_free (priv->appid_list);
	if (priv->app_id)
		g_hash_table_destroy (priv->app_id);

	G_OBJECT_CLASS (gnome_app_store_parent_class)->finalize (object);
}

static void
gnome_app_store_class_init (GnomeAppStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_store_dispose;
	object_class->finalize = gnome_app_store_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppStorePrivate));
}

GnomeAppStore *
gnome_app_store_new (void)
{
	return g_object_new (GNOME_APP_TYPE_STORE, NULL);
}

static gboolean
app_need_reload (GnomeAppStore *store, gchar *app_id)
{
	/*FIXME: not implement */
	/* timestamp */
	return FALSE;
}

static void
app_timestamp_mark (GnomeAppStore *store, GnomeAppInfo *info)
{
	/*FIXME: not implement */
}

GnomeAppInfo *
gnome_app_store_get_app_by_id (GnomeAppStore *store, gchar *app_id)
{
/*FIXME: make a cache? */
#if 0
	GnomeAppInfo *info;

	info = g_hash_table_lookup (store->priv->app_id, app_id);
	if (!info) {
		info = app_backend_get_app_by_id (store->priv->backend, app_id);
		app_timestamp_mark (store, info);
		g_hash_table_insert (store->priv->app_id, g_strdup (app_id), info);
	} else if (app_need_reload (store, app_id)){
		info = app_backend_get_app_by_id (store->priv->backend, app_id);
		g_hash_table_replace (store->priv->app_id, g_strdup (app_id), info);
	}

	return g_object_ref (info);
#endif
}

GList *
gnome_app_store_get_apps_by_query (GnomeAppStore *store, GnomeAppQuery *query)
{
	GList *list;

	list = app_backend_get_apps_by_query (store->priv->backend, query);

	return list;
}

const GnomeAppStore *
gnome_app_store_get_default ()
{
	static const GnomeAppStore *default_store = NULL;

	if (!default_store)
		default_store = gnome_app_store_new ();

	return default_store;
}


