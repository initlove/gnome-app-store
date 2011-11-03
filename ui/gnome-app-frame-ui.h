/* libgnome-app-frame-ui.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appframe_ui lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appframe_ui lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Sframe_uite 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_FRAME_UI_H__
#define __GNOME_APP_FRAME_UI_H__

#include <clutter/clutter.h>
#include "gnome-app-infos-stage.h"
#include "gnome-app-info-page.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_FRAME_UI            (gnome_app_frame_ui_get_type ())
#define GNOME_APP_FRAME_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_FRAME_UI, GnomeAppFrameUI))
#define GNOME_APP_FRAME_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_FRAME_UI, GnomeAppFrameUIClass))
#define GNOME_APP_IS_FRAME_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_FRAME_UI))
#define GNOME_APP_IS_FRAME_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_FRAME_UI))
#define GNOME_APP_FRAME_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_FRAME_UI, GnomeAppFrameUIClass))

typedef struct _GnomeAppFrameUI GnomeAppFrameUI;
typedef struct _GnomeAppFrameUIClass GnomeAppFrameUIClass;
typedef struct _GnomeAppFrameUIPrivate GnomeAppFrameUIPrivate;

struct _GnomeAppFrameUI
{
	ClutterGroup parent_instance;

	ClutterActor *search_entry;
	ClutterActor *category_list;
	GnomeAppInfosStage *infos_stage;
	ClutterActor *full;
	GnomeAppFrameUIPrivate	*priv;
};

struct _GnomeAppFrameUIClass
{
        ClutterGroupClass parent_class;
};

GType  		        gnome_app_frame_ui_get_type		(void);
GnomeAppFrameUI *      	gnome_app_frame_ui_new			(void);
GnomeAppFrameUI *      	gnome_app_frame_ui_get_default		(void);
void			gnome_app_frame_ui_set_full_info_mode	(GnomeAppFrameUI *ui, GnomeAppInfoPage *page);

G_END_DECLS

#endif
