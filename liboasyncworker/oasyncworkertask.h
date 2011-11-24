#ifndef O_ASYNC_WORKER_TASK_H
#define O_ASYNC_WORKER_TASK_H

/* OAsyncWorkerTask
 * Copyright (C) 2005, 2006 Philip Van Hoof <pvanhoof@gnome.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <glib.h>
#include <glib-object.h>

#include "oasyncworker.h"

G_BEGIN_DECLS

#define O_ASYNC_WORKER_TASK_TYPE			(o_async_worker_task_get_type ())
#define O_ASYNC_WORKER_TASK(self)			(G_TYPE_CHECK_INSTANCE_CAST ((self), O_ASYNC_WORKER_TASK_TYPE, OAsyncWorkerTask))
#define O_ASYNC_WORKER_TASK_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), O_ASYNC_WORKER_TASK_TYPE, OAsyncWorkerTaskClass))
#define G_IS_THREAD_QUEUE_TASK(self)			(G_TYPE_CHECK_INSTANCE_TYPE ((self), O_ASYNC_WORKER_TASK_TYPE))
#define G_IS_THREAD_QUEUE_TASK_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), O_ASYNC_WORKER_TASK_TYPE))
#define O_ASYNC_WORKER_TASK_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), O_ASYNC_WORKER_TASK_TYPE, OAsyncWorkerTaskClass))

typedef struct _OAsyncWorkerTaskPrivate OAsyncWorkerTaskPrivate;
typedef struct _OAsyncWorkerTaskClass OAsyncWorkerTaskClass;

struct _OAsyncWorkerTask
{
	GObject parent;
	OAsyncWorkerTaskPrivate *priv;
};

struct _OAsyncWorkerTaskClass
{
	GObjectClass parent;
};

typedef gpointer 		(*OAsyncWorkerTaskFunc) 		(OAsyncWorkerTask *task, gpointer arguments);
typedef void	 		(*OAsyncWorkerTaskCallback) 		(OAsyncWorkerTask *task, gpointer func_result);

GType				o_async_worker_task_get_type         	(void);
OAsyncWorkerTask*		o_async_worker_task_new              	(void);
OAsyncWorkerTask*		o_async_worker_task_new_with_arguments	(gpointer arguments, OAsyncWorkerTaskCallback callback);

void				o_async_worker_task_set_arguments	(OAsyncWorkerTask *task, gpointer arguments);
void				o_async_worker_task_set_func		(OAsyncWorkerTask *task, OAsyncWorkerTaskFunc func);
void				o_async_worker_task_set_callback	(OAsyncWorkerTask *task, OAsyncWorkerTaskCallback callback);
void				o_async_worker_task_set_priority	(OAsyncWorkerTask *task, gint priority);

gpointer			o_async_worker_task_get_arguments	(OAsyncWorkerTask *task);
OAsyncWorkerTaskFunc		o_async_worker_task_get_func		(OAsyncWorkerTask *task);
OAsyncWorkerTaskCallback	o_async_worker_task_get_callback	(OAsyncWorkerTask *task);
gint				o_async_worker_task_get_priority	(OAsyncWorkerTask *task);

gboolean			o_async_worker_task_is_cancelled	(OAsyncWorkerTask *task);

OAsyncWorker*			o_async_worker_task_get_queue		(OAsyncWorkerTask *task);


void				o_async_worker_task_cancel_point	(OAsyncWorkerTask *task, gboolean run_callback);

void				o_async_worker_task_request_cancel	(OAsyncWorkerTask *task);


G_END_DECLS

#endif
