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

#ifndef __GNOME_APP_STORE_UI_H__
#define __GNOME_APP_STORE_UI_H__

#include <glib.h>
#include <clutter/clutter.h>

#include "open-services.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_STORE_UI            (gnome_app_store_ui_get_type ())
#define GNOME_APP_STORE_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_STORE_UI, GnomeAppStoreUI))
#define GNOME_APP_STORE_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_STORE_UI, GnomeAppStoreUIClass))
#define GNOME_APP_IS_STORE_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_STORE_UI))
#define GNOME_APP_IS_STORE_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_STORE_UI))
#define GNOME_APP_STORE_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_STORE_UI, GnomeAppStoreUIClass))

typedef struct _GnomeAppStoreUI GnomeAppStoreUI;
typedef struct _GnomeAppStoreUIClass GnomeAppStoreUIClass;
typedef struct _GnomeAppStoreUIPrivate GnomeAppStoreUIPrivate;

struct _GnomeAppStoreUI
{
        ClutterStage           parent_instance;
	
	GnomeAppStoreUIPrivate	*priv;
};

struct _GnomeAppStoreUIClass
{
        ClutterStageClass parent_class;
};

GType				gnome_app_store_ui_get_type		(void);
GnomeAppStoreUI *		gnome_app_store_ui_get_default		(void);
GnomeAppStoreUI *		gnome_app_store_ui_new			(void);
void				gnome_app_store_ui_load_app_info	(GnomeAppStoreUI *ui, AppInfo *info);
void				gnome_app_store_ui_load_frame_ui	(GnomeAppStoreUI *ui);

G_END_DECLS

#endif
