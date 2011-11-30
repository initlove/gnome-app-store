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
#include <rest/rest-proxy.h>
#include <stdio.h>
#include <string.h>

#include "gnome-app-proxy.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "liboasyncworker/oasyncworker.h"
#include "liboasyncworker/oasyncworkertask.h"
#include "common/open-app-utils.h"
#include "common/open-app-config.h"
#include "common/open-result.h"
#include "common/open-results.h"

struct _GnomeAppStorePrivate
{
	OpenAppConfig *config;
	gchar	*url;
        GHashTable *categories;

	GnomeAppProxy *proxy;
	RestProxy *rest_proxy;
	OAsyncWorker *queue;

	/*FIXME: Donnot use this at present */
	GList *appid_list;	/* should be reload every timestamp time */
	GHashTable *app_id;
};

G_DEFINE_TYPE (GnomeAppStore, gnome_app_store, G_TYPE_OBJECT)

void
gnome_app_store_add_task (GnomeAppStore *store, GnomeAppTask *task)
{
        o_async_worker_add (store->priv->queue, gnome_app_task_get_task (task));
}

RestProxy *
gnome_app_store_get_rest_proxy (GnomeAppStore *store)
{
	return store->priv->rest_proxy;
}

GnomeAppProxy *
gnome_app_store_get_proxy (GnomeAppStore *store)
{
	return store->priv->proxy;
}

static void
setup_category (GnomeAppStore *store, GList *results_data)
{
        const gchar **default_categories;
        GString *ids [100];     /*FIXME: */
        gchar *categories [100];
        int i;
        GList *list, *l;
        OpenResult *result;
        const gchar *id;
        const gchar *name;
        gboolean match;
        gint other;

        default_categories = open_app_get_default_categories ();
        for (i = 0; default_categories [i]; i++) {
                ids [i] = NULL;
                categories [i] = g_strdup (default_categories [i]);
                if (strlen (categories [i]) > 3)
                        *(categories [i] + 3) = 0;
        }
        ids [i] = NULL;
        categories [i] = NULL;
        other = i - 1;  /*FIXME: other is the last one */

        for (l = results_data; l; l = l->next) {
                result = OPEN_RESULT (l->data);
                id = open_result_get (result, "id");
                name = open_result_get (result, "name");
                if (!id || !id [0] || !name || !name [0])
                        continue;
                match = FALSE;

                for (i = 0; categories [i]; i++) {
		        if (strcasestr (name, categories [i])) {
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
                        g_hash_table_insert (store->priv->categories, g_strdup (default_categories [i]), ids [i]->str);
                        g_string_free (ids [i], FALSE);
                }
        }

}

static gpointer
category_task_callback (gpointer userdata, gpointer func_result)
{
    	GnomeAppStore *store;
        OpenResults *results;
	GList *list;

        store = GNOME_APP_STORE (userdata);

        results = OPEN_RESULTS (func_result);
	if (ocs_results_get_status (results)) {
		list = open_results_get_data (results);
		setup_category (store, list);
	}

	return NULL;
}

static void
gnome_app_store_init (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_APP_TYPE_STORE,
                                                   GnomeAppStorePrivate);

	priv->appid_list = NULL;
	priv->app_id = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
	priv->config = open_app_config_new ();
        priv->categories = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->proxy = gnome_app_proxy_new ();

	const gchar *user;
	const gchar *password;
	const gchar *server_url;

	server_url = open_app_config_get_server_uri (priv->config);
	if (!server_url) {
		g_critical ("Cannot get the server uri !\n");
		return;
	}
	user = open_app_config_get_username (priv->config);
	password = open_app_config_get_password (priv->config);
	if (user && password)
		priv->url = g_strdup_printf ("http://%s:%s@%s", user, password, server_url);
	else
		priv->url = g_strdup_printf ("http://%s", server_url);

	priv->rest_proxy = rest_proxy_new (priv->url, FALSE);
	priv->queue = o_async_worker_new ();
//FIXME: ?        o_async_worker_join (queue);
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
	if (priv->url)
		g_free (priv->url);
        if (priv->categories)
                g_hash_table_destroy (priv->categories);
	if (priv->rest_proxy)
		g_object_unref (priv->rest_proxy);
	if (priv->proxy)
		g_object_unref (priv->proxy);
	if (priv->queue)
		g_object_unref (priv->queue);

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

GnomeAppStore *
gnome_app_store_get_default (void)
{
	static GnomeAppStore *store = NULL;

	if (!store)
		store = g_object_new (GNOME_APP_TYPE_STORE, NULL);

	return store;
}

const gchar *
gnome_app_store_get_url (GnomeAppStore *store)
{
	return (const gchar *) store->priv->url;
}

const gchar *
gnome_app_store_get_cids_by_name (GnomeAppStore *store, const gchar *category_name)
{
        g_return_val_if_fail (category_name, "-1");

        const gchar *val;

        val = (const gchar *) g_hash_table_lookup (store->priv->categories, category_name);
/*TODO: if the category group is empty, we set the ids  to -1,
        in this case, the return value will be empty!
*/
        if (!val || !val [0])
                val = "-1";

        return val;
}

void
gnome_app_store_init_category (GnomeAppStore *store)
{
	GnomeAppTask *task;

	task = gnome_app_task_new (store, "GET", "/v1/content/categories");
        gnome_app_task_set_callback (task, category_task_callback);
        gnome_app_task_push (task);
}

