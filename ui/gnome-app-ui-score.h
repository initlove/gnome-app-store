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

#ifndef __GNOME_APP_UI_SCORE_H__
#define __GNOME_APP_UI_SCORE_H__

#include <glib.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_UI_SCORE            (gnome_app_ui_score_get_type ())
#define GNOME_APP_UI_SCORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_UI_SCORE, GnomeAppUIScore))
#define GNOME_APP_UI_SCORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_UI_SCORE, GnomeAppUIScoreClass))
#define GNOME_APP_IS_UI_SCORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_UI_SCORE))
#define GNOME_APP_IS_UI_SCORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_UI_SCORE))
#define GNOME_APP_UI_SCORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_UI_SCORE, GnomeAppUIScoreClass))

typedef struct _GnomeAppUIScore GnomeAppUIScore;
typedef struct _GnomeAppUIScoreClass GnomeAppUIScoreClass;
typedef struct _GnomeAppUIScorePrivate GnomeAppUIScorePrivate;

struct _GnomeAppUIScore
{
        ClutterGroup            parent_instance;
	
	GnomeAppUIScorePrivate	*priv;
};

struct _GnomeAppUIScoreClass
{
        ClutterGroupClass parent_class;
};

GType				gnome_app_ui_score_get_type		(void);
GnomeAppUIScore *		gnome_app_ui_score_new			(void);
void				gnome_app_ui_score_set_score		(GnomeAppUIScore *ui, gchar *score);

G_END_DECLS

#endif
