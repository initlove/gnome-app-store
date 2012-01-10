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

   Author: liangchenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_SCORE_UI_H__
#define __GNOME_APP_SCORE_UI_H__

#include <glib.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_SCORE_UI            (gnome_app_score_ui_get_type ())
#define GNOME_APP_SCORE_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_SCORE_UI, GnomeAppScoreUI))
#define GNOME_APP_SCORE_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_SCORE_UI, GnomeAppScoreUIClass))
#define GNOME_APP_IS_SCORE_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_SCORE_UI))
#define GNOME_APP_IS_SCORE_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_SCORE_UI))
#define GNOME_APP_SCORE_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_SCORE_UI, GnomeAppScoreUIClass))

typedef struct _GnomeAppScoreUI GnomeAppScoreUI;
typedef struct _GnomeAppScoreUIClass GnomeAppScoreUIClass;
typedef struct _GnomeAppScoreUIPrivate GnomeAppScoreUIPrivate;

struct _GnomeAppScoreUI
{
        ClutterGroup            parent_instance;
	
	GnomeAppScoreUIPrivate	*priv;
};

struct _GnomeAppScoreUIClass
{
        ClutterGroupClass parent_class;
};

GType				gnome_app_score_ui_get_type		(void);
GnomeAppScoreUI *		gnome_app_score_ui_new_with_score	(const gchar *score);	

G_END_DECLS

#endif
