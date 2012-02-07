/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
   Copyright 2011, Novell, Inc.

   The Gnome appinfo lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfo lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_APPLICATION_H__
#define __GNOME_APP_APPLICATION_H__

#include <glib.h>
#include <clutter/clutter.h>

#include "open-result.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_APPLICATION            (gnome_app_application_get_type ())
#define GNOME_APP_APPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_APPLICATION, GnomeAppApplication))
#define GNOME_APP_APPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_APPLICATION, GnomeAppApplicationClass))
#define GNOME_APP_IS_APPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_APPLICATION))
#define GNOME_APP_IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_APPLICATION))
#define GNOME_APP_APPLICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_APPLICATION, GnomeAppApplicationClass))

typedef struct _GnomeAppApplication GnomeAppApplication;
typedef struct _GnomeAppApplicationClass GnomeAppApplicationClass;
typedef struct _GnomeAppApplicationPrivate GnomeAppApplicationPrivate;

struct _GnomeAppApplication
{
        ClutterStage           parent_instance;
	
	GnomeAppApplicationPrivate	*priv;
};

struct _GnomeAppApplicationClass
{
        ClutterStageClass parent_class;
};

typedef enum {
	UI_TYPE_FRAME_UI,
	UI_TYPE_INFO_PAGE,
	UI_LAST
} UI_TYPE;

GType				gnome_app_application_get_type		(void);
GnomeAppApplication *		gnome_app_application_get_default	(void);
GnomeAppApplication *		gnome_app_application_new		(void);
void				gnome_app_application_run		(void);
void				gnome_app_application_load		(GnomeAppApplication *ui, UI_TYPE type, gpointer userdata);
void				gnome_app_application_load_app_info	(GnomeAppApplication *ui, OpenResult *info);
void				gnome_app_application_load_frame_ui	(GnomeAppApplication *ui);

G_END_DECLS

#endif
