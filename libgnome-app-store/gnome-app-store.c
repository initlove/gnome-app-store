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


#include "local-app.h"
#include "gnome-app-item.h"
#include "gnome-app-config.h"
#include "gnome-app-store.h"

struct _GnomeAppStorePrivate
{
	GSList *		apps;
};

G_DEFINE_TYPE (GnomeAppStore, gnome_app_store, G_TYPE_OBJECT)

static void
load_apps (GnomeAppStore *store)
{
        xmlDocPtr doc_ptr;
        xmlNodePtr root_node, apps_node, app_node;
	gchar *file_uri;

	GnomeAppConfig *conf;
	conf = gnome_app_config_new ();

	file_uri = g_build_filename (gnome_app_config_get_cache_dir (conf), "appdata.xml", NULL);
	g_object_unref (conf);

        doc_ptr = xmlParseFile (file_uri);

        root_node = xmlDocGetRootElement (doc_ptr);

        for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
                if (strcmp (app_node->name, "application") == 0) {
			GnomeAppItem *item = gnome_app_item_new_with_node (app_node);
			if (item)
				store->priv->apps = g_slist_prepend (store->priv->apps, item);			
                }
        }

        xmlFreeDoc(doc_ptr);
	g_free (file_uri);
}

static void
gnome_app_store_init (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_TYPE_APP_STORE,
                                                   GnomeAppStorePrivate);

	priv->apps = NULL;


	load_apps (store);
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
/*FIXME: should free lots of things ... */

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
	return g_object_new (GNOME_TYPE_APP_STORE, NULL);
}

/**
 * gnome_app_store_get_apps
 *
 * get all the apps from the given store
 *
 * Return value: (transfer none): GSList 
 */

GSList *
gnome_app_store_get_apps (GnomeAppStore *store)
{
        GnomeAppStorePrivate *priv = store->priv;

	return priv->apps;
}

guint
gnome_app_store_get_counts (GnomeAppStore *store)
{
        GnomeAppStorePrivate *priv = store->priv;

	return g_slist_length (priv->apps);
}

/**
 * gnome_app_store_get_nth_app
 *
 * get the nth app from the given store
 *
 * Return value: (transfer none): GnomeAppItem * 
 */
GnomeAppItem *
gnome_app_store_get_nth_app (GnomeAppStore *store, guint pos)
{
        GnomeAppStorePrivate *priv = store->priv;
	GSList *l;
	guint i;

	for (i = 0, l = priv->apps; (i < pos) && l; i++, l = l->next) {
	}

	return (GnomeAppItem *) l->data;
}


