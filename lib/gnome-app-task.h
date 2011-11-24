/* gnome-app-task.h - 

   Copyright 2011, Novell, Inc.

   The Gnome apptask lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome apptask lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_TASK_H__
#define __GNOME_APP_TASK_H__

#include <glib.h>
#include <glib-object.h>
#include "gnome-app-store.h"
#include "liboasyncworker/oasyncworker.h"
#include "liboasyncworker/oasyncworkertask.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_TASK            (gnome_app_task_get_type ())
#define GNOME_APP_TASK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_TASK, GnomeAppTask))
#define GNOME_APP_TASK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_TASK, GnomeAppTaskClass))
#define GNOME_APP_IS_TASK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_TASK))
#define GNOME_APP_IS_TASK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_TASK))
#define GNOME_APP_TASK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_TASK, GnomeAppTaskClass))

typedef struct _GnomeAppTask GnomeAppTask;
typedef struct _GnomeAppTaskClass GnomeAppTaskClass;
typedef struct _GnomeAppTaskPrivate GnomeAppTaskPrivate;

typedef gpointer   (*GnomeAppTaskFunc)  (gpointer userdata, gpointer func_result);

struct _GnomeAppTask
{
        GObject                 parent_instance;
	
	GnomeAppTaskPrivate	*priv;
};

struct _GnomeAppTaskClass
{
        GObjectClass parent_class;
};

GType			gnome_app_task_get_type			(void);
GnomeAppTask *		gnome_app_task_new (GnomeAppStore *store, gpointer userdata, const gchar *method, const gchar *function, ...);
OAsyncWorkerTask *	gnome_app_task_get_task			(GnomeAppTask *task);

G_END_DECLS

#endif
