#ifndef O_ASYNC_WORKER_H
#define O_ASYNC_WORKER_H

/* OAsyncWorker
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

G_BEGIN_DECLS

#define O_ASYNC_WORKER_TYPE		(o_async_worker_get_type ())
#define O_ASYNC_WORKER(self)		(G_TYPE_CHECK_INSTANCE_CAST ((self), O_ASYNC_WORKER_TYPE, OAsyncWorker))
#define O_ASYNC_WORKER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), O_ASYNC_WORKER_TYPE, OAsyncWorkerClass))
#define G_IS_THREAD_QUEUE(self)		(G_TYPE_CHECK_INSTANCE_TYPE ((self), O_ASYNC_WORKER_TYPE))
#define G_IS_THREAD_QUEUE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), O_ASYNC_WORKER_TYPE))
#define O_ASYNC_WORKER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), O_ASYNC_WORKER_TYPE, OAsyncWorkerClass))

typedef struct _OAsyncWorker OAsyncWorker;
typedef struct _OAsyncWorkerClass OAsyncWorkerClass;
typedef struct _OAsyncWorkerPrivate OAsyncWorkerPrivate;

typedef struct _OAsyncWorkerTask OAsyncWorkerTask;

struct _OAsyncWorker
{
	GObject parent;
	OAsyncWorkerPrivate *priv;
};

struct _OAsyncWorkerClass
{
	GObjectClass parent;
	
	void			(*task_started)			(gpointer instance, gint arg1);	
	void			(*task_added)			(gpointer instance, gint arg1);
	void			(*task_finished)		(gpointer instance, gint arg1);
	void			(*task_removed)			(gpointer instance, gint arg1);
	void			(*task_cancelled)		(gpointer instance, gint arg1);
};



GType			o_async_worker_get_type        		(void);
OAsyncWorker*		o_async_worker_new              	(void);

gint			o_async_worker_add			(OAsyncWorker *queue, OAsyncWorkerTask *task);
void			o_async_worker_remove			(OAsyncWorker *queue, gint task_id, gboolean run_callback);
OAsyncWorkerTask*	o_async_worker_get_current		(OAsyncWorker *queue);
OAsyncWorkerTask*	o_async_worker_get_with_id		(OAsyncWorker *queue, gint task_id);
gint			o_async_worker_add_wait			(OAsyncWorker *queue, gint micros, gint priority);
void			o_async_worker_join			(OAsyncWorker *queue);


void 			o_async_worker_sched_setaffinity	(OAsyncWorker *queue, unsigned long *mask);
unsigned long*		o_async_worker_sched_getaffinity	(OAsyncWorker *queue);
void			o_async_worker_sched_setscheduler	(OAsyncWorker *queue, int policy, gpointer param);
int			o_async_worker_sched_getscheduler	(OAsyncWorker *queue);

G_END_DECLS

#endif
