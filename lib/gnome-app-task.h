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
#include <rest/rest-proxy.h>

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

typedef enum {
	TASK_PRIORITY_INVALID = 0,
	TASK_PRIORITY_LOW = 10,
	TASK_PRIORITY_PREDICT = 20,
	TASK_PRIORITY_PRELOAD = 30,
	TASK_PRIORITY_NORMAL = 40,
	TASK_PRIORITY_HIGH = 80,
	TASK_PRIORITY_INSANE = 200,
} TaskPriority;

GType			gnome_app_task_get_type			(void);
GnomeAppTask *		gnome_app_task_new 			(gpointer userdata, const gchar *method, const gchar *function);
GnomeAppTask *		gnome_download_task_new 		(gpointer userdata, const gchar *url);
void			gnome_app_task_add_param 		(GnomeAppTask *task, const gchar *param, const gchar *value);
void			gnome_app_task_add_params 		(GnomeAppTask *task, ...);
void			gnome_app_task_set_userdata		(GnomeAppTask *task, gpointer userdata);
void			gnome_app_task_set_callback		(GnomeAppTask *task, GnomeAppTaskFunc callback);
void			gnome_app_task_set_priority		(GnomeAppTask *task, TaskPriority priority);
void			gnome_app_task_preload			(GnomeAppTask *task);
void			gnome_app_task_push			(GnomeAppTask *task);

gchar *			gnome_app_task_to_str			(GnomeAppTask *task);
const gchar *		gnome_app_task_get_param_value		(GnomeAppTask *task, const gchar *param);
const gchar *		gnome_app_task_get_method		(GnomeAppTask *task);
const gchar *		gnome_app_task_get_function		(GnomeAppTask *task);
TaskPriority		gnome_app_task_get_priority		(GnomeAppTask *task);
OAsyncWorkerTask *	gnome_app_task_get_task			(GnomeAppTask *task);
RestProxyCall *		gnome_app_task_get_call			(GnomeAppTask *task);

G_END_DECLS

#endif
