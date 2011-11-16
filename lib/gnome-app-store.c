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
#include "common/open-app-config.h"
#include "common/open-request.h"
#include "common/open-results.h"

struct _GnomeAppStorePrivate
{
	AppBackend *backend;
	OpenAppConfig *config;

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

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_APP_TYPE_STORE,
                                                   GnomeAppStorePrivate);

	priv->appid_list = NULL;
	priv->app_id = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
	priv->app_timestamp = -1;	/* FIXME: not implement yet */
	priv->config = open_app_config_new ();
	priv->backend = app_backend_new_from_config (priv->config);
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
	if (priv->config)
		g_object_unref (priv->config);

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

OpenResults *
gnome_app_store_get_results (GnomeAppStore *store, OpenRequest *request)
{
	g_return_val_if_fail (store && GNOME_APP_IS_STORE (store), NULL);
	g_return_val_if_fail (request && IS_OPEN_REQUEST (request), NULL);

	return app_backend_get_results (store->priv->backend, request);
}
