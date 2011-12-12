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
#include <rest/rest-proxy.h>
#include "gnome-app-task.h"
#include "gnome-app-proxy.h"
#include "gnome-app-store.h"
#include "liboasyncworker/oasyncworker.h"
#include "liboasyncworker/oasyncworkertask.h"
#include "common/open-app-utils.h"
#include "common/open-app-config.h"
#include "common/open-result.h"
#include "common/open-results.h"

struct _GnomeAppTaskPrivate
{
	gchar *url;
        RestProxyCall *call;
	OAsyncWorkerTask *async;

	GnomeAppTaskFunc callback;
	gpointer userdata;
};

G_DEFINE_TYPE (GnomeAppTask, gnome_app_task, G_TYPE_OBJECT)

/*FIXME: */
extern RestProxy * gnome_app_store_get_rest_proxy (GnomeAppStore *store);
extern GnomeAppProxy *gnome_app_store_get_proxy (GnomeAppStore *store);
extern void	gnome_app_store_add_task (GnomeAppStore *store, GnomeAppTask *task);

static void
gnome_app_task_init (GnomeAppTask *task)
{
	GnomeAppTaskPrivate *priv;

	task->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (task,
                                                   GNOME_APP_TYPE_TASK,
                                                   GnomeAppTaskPrivate);
	priv->url = NULL;
	priv->call = NULL;
	priv->async = NULL;
	priv->callback = NULL;
	priv->userdata = NULL;
}

static void
gnome_app_task_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_task_parent_class)->dispose (object);
}

static void
gnome_app_task_finalize (GObject *object)
{
	GnomeAppTask *task = GNOME_APP_TASK (object);
	GnomeAppTaskPrivate *priv = task->priv;

	if (priv->url)
		g_free (priv->url);
	if (priv->async)
	        g_object_unref (priv->async);

	G_OBJECT_CLASS (gnome_app_task_parent_class)->finalize (object);
}

static void
gnome_app_task_class_init (GnomeAppTaskClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_task_dispose;
	object_class->finalize = gnome_app_task_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppTaskPrivate));
}

static gpointer
async_download_func (OAsyncWorkerTask *task, gpointer arguments)
{
	GnomeAppTask *app_task;
	GError *error = NULL;

	app_task = GNOME_APP_TASK (arguments);

	gchar *dest_url;
        dest_url = open_app_get_local_icon (app_task->priv->url, TRUE);

	return dest_url;
}

GnomeAppTask *
gnome_download_task_new (gpointer userdata, const gchar *url)
{
        GnomeAppTask *task;

	task = g_object_new (GNOME_APP_TYPE_TASK, NULL);
        task->priv->url = g_strdup (url);
	task->priv->userdata = userdata;
	task->priv->async = o_async_worker_task_new ();

        o_async_worker_task_set_arguments (task->priv->async, task);
        o_async_worker_task_set_func (task->priv->async, async_download_func);
        o_async_worker_task_set_priority (task->priv->async, TASK_PRIORITY_NORMAL);

	return task;
}

static gpointer
async_func (OAsyncWorkerTask *oasync_task, gpointer arguments)
{
	GnomeAppTask *app_task;
	GError *error = NULL;

	app_task = GNOME_APP_TASK (arguments);

	const gchar *method;
	OpenResults *results;

	method = rest_proxy_call_get_method (app_task->priv->call);
	if (strcasecmp (method, "GET") == 0) {
		GnomeAppStore *store;
		GnomeAppProxy *proxy;

		store = gnome_app_store_get_default ();
		proxy = gnome_app_store_get_proxy (store);
		results = gnome_app_proxy_find (proxy, app_task);
		if (results) {
			gnome_app_proxy_predict (proxy, app_task);
			return results;
		}
	}

	rest_proxy_call_sync (app_task->priv->call, &error);
	if (error) {
		g_debug ("error in get task %s\n", error->message);
		g_error_free (error);
		return NULL;
	}

        const gchar *payload;
        goffset len;

        payload = rest_proxy_call_get_payload (app_task->priv->call);
        len = rest_proxy_call_get_payload_length (app_task->priv->call);
        results = (OpenResults *) ocs_get_results (payload, len);

	GnomeAppStore *store;
	GnomeAppProxy *proxy;

	store = gnome_app_store_get_default ();
	proxy = gnome_app_store_get_proxy (store);
	gnome_app_proxy_add (proxy, app_task, results);

	return results;
}

static void
task_callback (OAsyncWorkerTask *oasync_task, gpointer func_result)
{
	GnomeAppTask *app_task;

	app_task = o_async_worker_task_get_arguments (oasync_task); 
	  
	if (app_task->priv->callback) {
		gnome_app_store_lock (gnome_app_store_get_default ());
		app_task->priv->callback (app_task->priv->userdata, func_result);
		gnome_app_store_unlock (gnome_app_store_get_default ());
	}
//TODO when to unref it ? 
//	g_object_unref (func_result);
}

GnomeAppTask *
gnome_app_task_new (gpointer userdata, const gchar *method, const gchar *function)
{
        g_return_val_if_fail (function, NULL);

	GnomeAppStore *store;
        GnomeAppTask *task;
	RestProxy *proxy;

	task = g_object_new (GNOME_APP_TYPE_TASK, NULL);
	store = gnome_app_store_get_default ();
        proxy = gnome_app_store_get_rest_proxy (store);
        task->priv->call = rest_proxy_new_call ((RestProxy *)proxy);
        rest_proxy_call_set_function (task->priv->call, function);
	rest_proxy_call_set_method (task->priv->call, method);
	task->priv->userdata = userdata;
	task->priv->async = o_async_worker_task_new ();
        o_async_worker_task_set_func (task->priv->async, async_func);
        o_async_worker_task_set_priority (task->priv->async, TASK_PRIORITY_NORMAL);

	return task;
}

void            
gnome_app_task_add_param (GnomeAppTask *task, const gchar *param, const gchar *value)
{
	rest_proxy_call_add_param (task->priv->call, param, value);
}

void            
gnome_app_task_add_params (GnomeAppTask *task, ...)
{
        va_list params;
	const gchar *param, *value;

        va_start (params, task);
        while ((param = va_arg (params, const gchar *)) != NULL) {
                value = va_arg (params, const gchar *);
                rest_proxy_call_add_param (task->priv->call, param, value);
        }
        va_end (params);
}

void
gnome_app_task_set_callback (GnomeAppTask *task, GnomeAppTaskFunc callback)
{
	task->priv->callback = callback;
        o_async_worker_task_set_callback (task->priv->async, task_callback);
}

void
gnome_app_task_set_priority (GnomeAppTask *task, TaskPriority priority)
{
        o_async_worker_task_set_priority (task->priv->async, priority);
}

void
gnome_app_task_push (GnomeAppTask *task)
{
	gchar *str;
	str = gnome_app_task_to_str (task);
	g_debug ("gnome_app_task_push %s", str);
	g_free (str);

	const gchar *method;
	OpenResults *results;

	if (!task->priv->url) {
		method = rest_proxy_call_get_method (task->priv->call);
		if (strcasecmp (method, "GET") == 0) {
			GnomeAppStore *store;
			GnomeAppProxy *proxy;

			store = gnome_app_store_get_default ();
			proxy = gnome_app_store_get_proxy (store);
			results = gnome_app_proxy_find (proxy, task);
			if (results) {
				g_debug ("we use the cached data");
				if (task->priv->callback)
					task->priv->callback (task->priv->userdata, results);
				else 
					g_debug ("Cannot find the callback ?");
				return;
			}
		}
	} else {
		gchar *img_local_cache;

		img_local_cache = open_app_get_local_icon (task->priv->url, FALSE);
		if (img_local_cache) {
			if (task->priv->callback) {
				task->priv->callback (task->priv->userdata, img_local_cache);
				g_free (img_local_cache);
				return;
			}
			g_free (img_local_cache);
		}
	}

        o_async_worker_task_set_arguments (task->priv->async, task);
	gnome_app_store_add_task (gnome_app_store_get_default (), task);
}

gchar *
gnome_app_task_to_str (GnomeAppTask *task)
{
	GString *task_str;
	gchar *str;
	const gchar *name;
        const gchar *content;
	RestParams *params;
	RestParamsIter iter;
        RestParam *param;
	gboolean first = TRUE;

	/*Download task */
	if (task->priv->url)
		return g_strdup (task->priv->url);

	task_str = g_string_new (rest_proxy_call_get_function (task->priv->call));
	params = rest_proxy_call_get_params (task->priv->call);
	rest_params_iter_init (&iter, params);
	while (rest_params_iter_next (&iter, &name, &param)) {
		content = rest_param_get_content (param);
		if (first) {
			g_string_append_c (task_str, '?');
			first = FALSE;
		} else 
			g_string_append_c (task_str, '&');
		g_string_append_printf (task_str, "%s=%s", name, content);
	}
	str = task_str->str;
	g_string_free (task_str, FALSE);

	return str;
}

const gchar *
gnome_app_task_get_method (GnomeAppTask *task)
{
	g_return_val_if_fail (task && task->priv->call, NULL);

	return rest_proxy_call_get_method (task->priv->call);
}

const gchar *
gnome_app_task_get_function (GnomeAppTask *task)
{
	g_return_val_if_fail (task && task->priv->call, NULL);

	return rest_proxy_call_get_function (task->priv->call);
}

TaskPriority
gnome_app_task_get_priority (GnomeAppTask *task)
{
	g_return_val_if_fail (task && task->priv->async, TASK_PRIORITY_INVALID);

        o_async_worker_task_get_priority (task->priv->async);
}

OAsyncWorkerTask *
gnome_app_task_get_task (GnomeAppTask *task)
{
	return task->priv->async;
}

RestProxyCall *
gnome_app_task_get_call (GnomeAppTask *task)
{
	return task->priv->call;
}
