#ifndef O_ASYNC_WORKER_TASK_PRIV_H
#define O_ASYNC_WORKER_TASK_PRIV_H

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

#include "oasyncworker.h"
#include "oasyncworkertask.h"

enum {
	CANCEL_NOT_CANCELLED,
	CANCEL_NO_CALLBACK,
	CANCEL_DO_CALLBACK,
};

struct _OAsyncWorkerTaskPrivate
{
	GStaticMutex lock;
	gboolean dispose_has_run;

	OAsyncWorker *queue;
	gpointer arguments;
	OAsyncWorkerTaskFunc func;
	OAsyncWorkerTaskCallback callback;

	gboolean iwanttocancel;
	gboolean imincallback;

	gint priority;	
	gint id;
	gint cancelled;
	gboolean removed;

};

gint	_o_async_worker_task_get_id		(OAsyncWorkerTask *task);
void	_o_async_worker_task_set_id		(OAsyncWorkerTask *task, gint id);
void	_o_async_worker_try_cancel		(OAsyncWorker *queue, OAsyncWorkerTask *task, gboolean run_callback);


#define O_ASYNC_WORKER_TASK_GET_PRIVATE(o)	(G_TYPE_INSTANCE_GET_PRIVATE ((o), DCONF_TYPE_VALUE, OAsyncWorkerPrivate))

#endif
