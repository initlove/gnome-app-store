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
#include <string.h>
#include "gnome-app-proxy.h"
#include "common/open-app-utils.h"
#include "common/open-app-config.h"
#include "common/open-result.h"
#include "common/open-results.h"

typedef struct _ProxyData {
	OpenResults *results;
	GnomeAppTask *task;
} ProxyData;

typedef enum {
	TYPE_FIRST,
	TYPE_CONTENT_GET,
	TYPE_FAN_GET,
	TYPE_FAN_IS_FAN,
	TYPE_FAN_ADD,
	TYPE_FAN_REMOVE,
	TYPE_COMMENTS_GET,
	TYPE_COMMENTS_ADD,
	TYPE_LAST,
} FUNC_TYPE;

struct _GnomeAppProxyPrivate
{
	GHashTable *cache;

	gboolean refresh_array [TYPE_LAST][TYPE_LAST];
	gchar *func_prefix [TYPE_LAST];
};

G_DEFINE_TYPE (GnomeAppProxy, gnome_app_proxy, G_TYPE_OBJECT)

static void
init_function (GnomeAppProxy *proxy)
{
	GnomeAppProxyPrivate *priv;
	gint i, j;

	priv = proxy->priv;

	for (i = TYPE_FIRST + 1; i < TYPE_LAST; i++)
		for (j = TYPE_FIRST + 1; j < TYPE_LAST; j++)
			priv->refresh_array [i][j] = FALSE;

	priv->refresh_array [TYPE_FAN_ADD][TYPE_CONTENT_GET] = TRUE;
	priv->refresh_array [TYPE_FAN_ADD][TYPE_FAN_GET] = TRUE;
	priv->refresh_array [TYPE_FAN_ADD][TYPE_FAN_IS_FAN] = TRUE;
	priv->refresh_array [TYPE_FAN_REMOVE][TYPE_CONTENT_GET] = TRUE;
	priv->refresh_array [TYPE_FAN_REMOVE][TYPE_FAN_GET] = TRUE;
	priv->refresh_array [TYPE_FAN_REMOVE][TYPE_FAN_IS_FAN] = TRUE;
	priv->refresh_array [TYPE_COMMENTS_ADD][TYPE_CONTENT_GET] = TRUE;
	priv->refresh_array [TYPE_COMMENTS_ADD][TYPE_COMMENTS_GET] = TRUE;

	priv->func_prefix [TYPE_FIRST] = NULL;
	priv->func_prefix [TYPE_CONTENT_GET] = "/v1/content/data/";
	priv->func_prefix [TYPE_FAN_GET] = "/v1/fan/data/";
	priv->func_prefix [TYPE_FAN_IS_FAN] = "/v1/fan/status/";
	priv->func_prefix [TYPE_FAN_ADD] = "/v1/fan/add/";
	priv->func_prefix [TYPE_FAN_REMOVE] = "/v1/fan/remove/";
	priv->func_prefix [TYPE_COMMENTS_GET] = "/v1/comments/data/";
	priv->func_prefix [TYPE_COMMENTS_ADD] = "/v1/comments/add";
	priv->func_prefix [TYPE_LAST] = NULL;
}

static ProxyData *
proxy_data_new (GnomeAppTask *task, OpenResults *results)
{
	ProxyData *data;

	data = g_new0 (ProxyData, 1);
	data->results = g_object_ref (results);
	data->task = g_object_ref (task);

	return data;
}

static void
proxy_data_unref (gpointer userdata)
{
	ProxyData *data;

	data = (ProxyData *) userdata;
	g_object_unref (data->results);
	g_object_unref (data->task);
	g_free (data);
}

static void
gnome_app_proxy_init (GnomeAppProxy *proxy)
{
	GnomeAppProxyPrivate *priv;

	proxy->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (proxy,
                                                   GNOME_APP_TYPE_PROXY,
                                                   GnomeAppProxyPrivate);
	priv->cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, proxy_data_unref);
	init_function (proxy);
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

static gboolean
refresh_by_function (gpointer key,
		gpointer value,
		gpointer user_data)
{
	const gchar *function;
	gchar *refresh_function;
	ProxyData *data;

	data = (ProxyData *) value;
	refresh_function = (gchar *) user_data;
	function = gnome_app_task_get_function (data->task);
printf ("refresh function is %s,  in cache %s\n", refresh_function, function);
	if (strcmp (function, refresh_function) == 0) {
#ifndef DEVEL_MODE
#define DEVEL_MODE
#endif
#ifdef DEVEL_MODE
		gchar *str;
		gchar *md5;
		gchar *filename;

		str = gnome_app_task_to_str (data->task);
		g_debug ("remove from the proxy %s\n", str);
		md5 = open_app_get_md5 (str);
		filename = g_build_filename (g_get_user_cache_dir (), "gnome-app-store", "xml", md5, NULL);
		g_unlink (filename);

		g_free (filename);
		g_free (md5);
		g_free (str);

#endif
		return TRUE;
	} else
		return FALSE;
}

static void
refresh_task (GnomeAppProxy *proxy, GnomeAppTask *task, gint cur_type, gint refresh_type)
{
	GnomeAppProxyPrivate *priv;
	const gchar *function;
	const gchar *type;
	const gchar *contentid;
	const gchar *contentid2;
	gchar *refresh_function;
	gchar *key;

	priv = proxy->priv;
	g_debug ("refresh_task %s %s\n", priv->func_prefix [cur_type], priv->func_prefix [refresh_type]);
	refresh_function = NULL;
	switch (cur_type) {
		case TYPE_FAN_ADD:
		case TYPE_FAN_REMOVE:
			function = gnome_app_task_get_function (task);
			contentid = function + strlen (priv->func_prefix [cur_type]);
			refresh_function = g_strdup_printf ("%s%s", priv->func_prefix [refresh_type], contentid);
			break;
		case TYPE_COMMENTS_ADD:
			if (refresh_type == TYPE_CONTENT_GET) {
				contentid = gnome_app_task_get_param_value (task, "content");
				refresh_function = g_strdup_printf ("%s%s", priv->func_prefix [refresh_type], contentid);
			} else if (refresh_type == TYPE_COMMENTS_GET) {
				/*TODO: in reply case, we may just remove the certain page, but now we remove all
				 * it maybe good, so donnot handle it yet 
				 */
				type = gnome_app_task_get_param_value (task, "type");
				contentid = gnome_app_task_get_param_value (task, "content");
				contentid2 = gnome_app_task_get_param_value (task, "content2");
				refresh_function = g_strdup_printf ("%s%s/%s/%s", priv->func_prefix [refresh_type],
							type, contentid, contentid2);
			}
			break;
		default:
			break;
	}
	if (refresh_function) {
		g_hash_table_foreach_remove (priv->cache, refresh_by_function, refresh_function);
		g_free (refresh_function);
	}
}

static gboolean
should_be_proxy (GnomeAppProxy *proxy, GnomeAppTask *task)
{
	GnomeAppProxyPrivate *priv;
	const gchar *method;
	const gchar *function;
	gint type, i;

	priv = proxy->priv;
	method = gnome_app_task_get_method (task);
	if (strcasecmp (method, "GET") == 0) {
		return TRUE;
	}

	/*If we POST, refresh the relevant cached data */
	function = gnome_app_task_get_function (task);
	for (i = TYPE_FIRST + 1; i < TYPE_LAST; i++)
		if (strncmp (function, priv->func_prefix [i], strlen (priv->func_prefix [i])) == 0) {
			type = i;
			break;
		}

	if (type == TYPE_LAST) {
		return FALSE;
	}
	for (i = TYPE_FIRST + 1; i < TYPE_LAST; i++) {
		if (priv->refresh_array [type][i]) {
			refresh_task (proxy, task, type, i);
		}
	}
		
	return FALSE;
}

void
gnome_app_proxy_add (GnomeAppProxy *proxy, GnomeAppTask *task, OpenResults *results)
{
/*TODO we should make the cache stronger. */
	const gchar *method;
	const gchar *function;
	gchar *key;
	ProxyData *data;

//	g_debug ("gnome_app_proxy_add!");

        if (ocs_results_get_status (results)) {
		if (!should_be_proxy (proxy, task))
			return;

		key = gnome_app_task_to_str (task);
	//	g_debug ("Add to proxy %s\n", key);
		data = proxy_data_new (task, results);
		g_hash_table_replace (proxy->priv->cache, key, data);
		                
		if (gnome_app_task_get_priority (task) >= TASK_PRIORITY_NORMAL)			
			gnome_app_proxy_predict (proxy, task);
	}

}

OpenResults *
gnome_app_proxy_find (GnomeAppProxy *proxy, GnomeAppTask *task)
{
	ProxyData *data;
	OpenResults *results;
	gchar *key;

/*TODO if the proxy was in cache, but not done yet, make priority higher */
	key = gnome_app_task_to_str (task);
	data = (ProxyData *) g_hash_table_lookup (proxy->priv->cache, key);
	g_free (key);
	if (data) {
		return data->results;
	} else
		return NULL;
}

/*TODO REMOVE */
static gpointer
proxy_task_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppProxy *proxy;
	GnomeAppTask *task;
	OpenResults *results;
printf ("proxy task callback\n");

	proxy = GNOME_APP_PROXY (userdata);
	results = OPEN_RESULTS (func_result);

        if (ocs_results_get_status (results)) {
	}

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

	g_debug ("Next page predict!");
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
		next_page = g_strdup_printf ("%d", page_number + 1);
	        next_task = gnome_app_task_new (proxy, "GET", gnome_app_task_get_function (task));
//		gnome_app_task_set_callback (next_task, proxy_task_callback);
        	rest_params_iter_init (&iter, params);
        	while (rest_params_iter_next (&iter, &name, &param)) {
			if (strcmp (name, "page") == 0) {
				gnome_app_task_add_param (next_task, "page", next_page);
			} else {
                		content = rest_param_get_content (param);
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
