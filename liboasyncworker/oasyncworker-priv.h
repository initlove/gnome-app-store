#ifndef O_ASYNC_WORKER_PRIV_H
#define O_ASYNC_WORKER_PRIV_H

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

#include "oasyncworker.h"
#include "oasyncworkertask.h"

struct _OAsyncWorkerPrivate
{
	GStaticMutex lock;
	gboolean dispose_has_run;

	GThread *thread;	
	OAsyncWorkerTask *current;
	GList *tasks;
	gboolean pause, cont;
	guint idseed;
	
#ifdef HAVE_SCHED_H
	gboolean affinity_is_set;
	unsigned long *affinity_mask;
	gboolean scheduler_is_set;
	gpointer scheduler_param;
	int scheduler_policy
#endif

};

void 	_o_async_worker_sort_tasks 	(OAsyncWorker *queue);

#endif
