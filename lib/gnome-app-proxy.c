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
#include <stdio.h>
#include "gnome-app-proxy.h"
#include "common/open-app-utils.h"
#include "common/open-app-config.h"
#include "common/open-result.h"
#include "common/open-results.h"

struct _GnomeAppProxyPrivate
{
	GHashTable *cache;
};

G_DEFINE_TYPE (GnomeAppProxy, gnome_app_proxy, G_TYPE_OBJECT)

static void
gnome_app_proxy_init (GnomeAppProxy *proxy)
{
	GnomeAppProxyPrivate *priv;

	proxy->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (proxy,
                                                   GNOME_APP_TYPE_PROXY,
                                                   GnomeAppProxyPrivate);
	priv->cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
}

static void
gnome_app_proxy_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_proxy_parent_class)->dispose (object);
}

static void
gnome_app_proxy_finalize (GObject *object)
{
	GnomeAppProxy *proxy = GNOME_APP_PROXY (object);
	GnomeAppProxyPrivate *priv = proxy->priv;

	if (priv->cache)
	        g_hash_table_destroy (priv->cache);

	G_OBJECT_CLASS (gnome_app_proxy_parent_class)->finalize (object);
}

static void
gnome_app_proxy_class_init (GnomeAppProxyClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_proxy_dispose;
	object_class->finalize = gnome_app_proxy_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppProxyPrivate));
}

GnomeAppProxy *
gnome_app_proxy_new ()
{
        GnomeAppProxy *proxy;

	proxy = g_object_new (GNOME_APP_TYPE_PROXY, NULL);

        return proxy;
}

void
gnome_app_proxy_add (GnomeAppProxy *proxy, gchar *str, OpenResults *results)
{
/*TODO we should make the cache stronger. */
	if (g_hash_table_lookup (proxy->priv->cache, str))
		g_hash_table_replace (proxy->priv->cache, g_strdup (str), g_object_ref (results));
	else
		g_hash_table_insert (proxy->priv->cache, g_strdup (str), g_object_ref (results));

}

OpenResults *
gnome_app_proxy_find (GnomeAppProxy *proxy, gchar *str)
{
	OpenResults *results;
/*TODO if the proxy was in cache, but not done yet, make priority higher */
	results = (OpenResults *) g_hash_table_lookup (proxy->priv->cache, str);

	return results;
}

static gpointer
proxy_task_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppProxy *proxy;

	proxy = GNOME_APP_PROXY (userdata);

	return NULL;
}

static GnomeAppTask *
next_page_predict (GnomeAppProxy *proxy, GnomeAppTask *task)
{
	GnomeAppTask *next_task;
	RestProxyCall *call;
        RestParams *params;
        RestParamsIter iter;
        RestParam *param;
	const gchar *name;
	const gchar *content;

	if (strcasecmp (gnome_app_task_get_method (task), "GET") != 0) {
		g_critical ("No reason to predict next page in POST task!");
		return NULL;
	}

	call = gnome_app_task_get_call (task);
	if (!call)
		return NULL;

        params = rest_proxy_call_get_params (call);
        rest_params_iter_init (&iter, params);
	content = NULL;
        while (rest_params_iter_next (&iter, &name, &param)) {
		if (strcmp (name, "page") == 0) {
                	content = rest_param_get_content (param);
			break;
		}
        }
	if (content) {
		gint page_number;
		gchar *next_page;

		page_number = atoi (content);
		next_page = g_strdup_printf ("%d", page_number++);

	        next_task = gnome_app_task_new (proxy, "GET", gnome_app_task_get_function (task));
		gnome_app_task_set_callback (next_task, proxy_task_callback);
        	rest_params_iter_init (&iter, params);
        	while (rest_params_iter_next (&iter, &name, &param)) {
			if (strcmp (name, "page") == 0) {
				gnome_app_task_add_param (next_task, "page", next_page);
			} else {
                		gnome_app_task_add_param (next_task, name, content);
			}
		}
		gnome_app_task_set_priority (next_task, TASK_PRIORITY_PREDICT);
		gnome_app_task_push (next_task);

		g_free (next_page);
	} else {
		return NULL;
	}
}

/*TODO: VIP, should do lots of works here.... */
void
gnome_app_proxy_predict (GnomeAppProxy *proxy, GnomeAppTask *task)
{
	next_page_predict (proxy, task);
}
