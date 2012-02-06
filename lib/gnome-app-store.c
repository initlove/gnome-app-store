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
#include <glib.h>
#include <glib/gthread.h>
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
	gchar *username;
	gchar *password;
	gchar *server_url;
        GHashTable *categories;

	GnomeAppProxy *proxy;
	RestProxy *rest_proxy;
	OAsyncWorker *queue;

	/*FIXME: Donnot use this at present */
	GList *appid_list;	/* should be reload every timestamp time */
	GHashTable *app_id;

};

enum {
	PROP_0,
	PROP_USERNAME,
	PROP_PASSWORD,
	PROP_LAST,
};

G_DEFINE_TYPE (GnomeAppStore, gnome_app_store, G_TYPE_OBJECT)

static void
set_username (GnomeAppStore *store, gchar *username)
{
	GnomeAppStorePrivate *priv;

	priv = store->priv;
	if (priv->username)
		g_free (priv->username);
	if (username)
		priv->username = g_strdup (username);
	else
		priv->username = NULL;
}

static void
set_password (GnomeAppStore *store, gchar *password)
{
	GnomeAppStorePrivate *priv;

	priv = store->priv;
	if (priv->password)
		g_free (priv->password);
	if (password)
		priv->password = g_strdup (password);
	else
		priv->password = NULL;
}

static void
set_rest_proxy (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;
	gchar *url;

	priv = store->priv;
	g_return_if_fail (priv->server_url);

	if (priv->username && priv->password)
		url = g_strdup_printf ("http://%s:%s@%s", priv->username, priv->password, priv->server_url);
	else
		url = g_strdup_printf ("http://%s", priv->server_url);

	priv->rest_proxy = rest_proxy_new (url, FALSE);

	g_free (url);
}

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
			GnomeAppTask *task;
			const gchar *pagesize = "35";	/*FIXME: */

			task = gnome_app_task_new (NULL, "GET", "/v1/content/data");
		        gnome_app_task_add_params (task,
						   "categories", ids [i]->str,
						   "pagesize", pagesize,
						   "page", "0",
						   NULL);
			gnome_app_task_set_priority (task, TASK_PRIORITY_PRELOAD);
			gnome_app_task_push (task);

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

	const gchar *username;
	const gchar *password;
	const gchar *server_url;

	server_url = open_app_config_get_server_uri (priv->config);
	if (!server_url) {
		g_critical ("Cannot get the server uri !\n");
		return;
	}

	priv->server_url = g_strdup (server_url);
	priv->username = NULL;
	priv->password = NULL;

	username = open_app_config_get_username (priv->config);
	password = open_app_config_get_password (priv->config);

	set_username (store, username);
	set_password (store, password);
	set_rest_proxy (store);
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
	if (priv->server_url)
		g_free (priv->server_url);
	if (priv->username)
		g_free (priv->username);
	if (priv->password)
		g_free (priv->password);
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
gnome_app_store_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppStore *store;        

	store = GNOME_APP_STORE (object);
			       
       	switch (prop_id) {
		case PROP_USERNAME:
			set_username (store, g_value_get_string (value));
			set_rest_proxy (store);
			break;
		case PROP_PASSWORD:
			set_password (store, g_value_get_string (value));
			set_rest_proxy (store);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;	      
	}
}

static void
gnome_app_store_get_property (GObject    *object,
		guint       prop_id,
		GValue     *value,
		GParamSpec *pspec)
{
	GnomeAppStore *store;

	store = GNOME_APP_STORE (object);

	switch (prop_id) {
		case PROP_USERNAME:
			g_value_set_string (value, store->priv->username);
			break;
		case PROP_PASSWORD:
			g_value_set_string (value, store->priv->password);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;	      
	}
}

static void
gnome_app_store_class_init (GnomeAppStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_store_dispose;
	object_class->finalize = gnome_app_store_finalize;
	object_class->set_property = gnome_app_store_set_property;
	object_class->get_property = gnome_app_store_get_property;
	
        klass->lock = NULL;
	klass->unlock = NULL;

        g_object_class_install_property (object_class,
			PROP_USERNAME,
                        g_param_spec_string ("username", 
				"username",
				"username",
			        NULL,
				G_PARAM_READWRITE));

        g_object_class_install_property (object_class,
			PROP_PASSWORD,
                        g_param_spec_string ("password", 
				"password",
				"password",
			        NULL,
				G_PARAM_READWRITE));

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
gnome_app_store_get_username (GnomeAppStore *store)
{
	return open_app_config_get_username (store->priv->config);
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

void
gnome_app_store_set_lock_function (GnomeAppStore *store, void (*callback))
{
	GnomeAppStoreClass *class;

	class = GNOME_APP_STORE_GET_CLASS (store);
	class->lock = callback;
}

void
gnome_app_store_set_unlock_function (GnomeAppStore *store, void (*callback))
{
	GnomeAppStoreClass *class;

	class = GNOME_APP_STORE_GET_CLASS (store);
	class->unlock = callback;
}

void
gnome_app_store_lock (GnomeAppStore *store)
{
	GnomeAppStoreClass *class;

	class = GNOME_APP_STORE_GET_CLASS (store);
	if (class->lock)
		class->lock ();
}

void
gnome_app_store_unlock (GnomeAppStore *store)
{
	GnomeAppStoreClass *class;

	class = GNOME_APP_STORE_GET_CLASS (store);
	if (class->unlock)
		class->unlock ();
}
