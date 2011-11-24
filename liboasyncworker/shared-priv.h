#ifndef G_SHARED_PRIV_H
#define G_SHARED_PRIV_H

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

#define O_ASYNC_WORKER_CANCEL_INVALID		g_warning ("You can only call this function from within the launcher function of an item\n")
#define O_ASYNC_WORKER_OBJECT_NOT_READY(o)	g_warning ("Object instance at %X not ready\n", (o))

#define IS_VALID_OBJECT(o)			((o) && ((o)->priv))
#define LOCK_OBJECT(o)				(IS_VALID_OBJECT(o)?g_static_mutex_lock (&(o)->priv->lock):O_ASYNC_WORKER_OBJECT_NOT_READY((o)))
#define TRYLOCK_OBJECT(o)			(IS_VALID_OBJECT(o)?g_static_mutex_trylock (&(o)->priv->lock):O_ASYNC_WORKER_OBJECT_NOT_READY((o)))
#define UNLOCK_OBJECT(o)			(IS_VALID_OBJECT(o)?g_static_mutex_unlock (&(o)->priv->lock):O_ASYNC_WORKER_OBJECT_NOT_READY((o)))
#define LOCK_FREE(o)				(IS_VALID_OBJECT(o)?g_static_mutex_free (&(o)->priv->lock):O_ASYNC_WORKER_OBJECT_NOT_READY((o)))
#define LOCK_INIT(o)				(IS_VALID_OBJECT(o)?g_static_mutex_init (&(o)->priv->lock):O_ASYNC_WORKER_OBJECT_NOT_READY((o)))

#endif
