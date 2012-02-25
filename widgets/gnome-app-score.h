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

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_SCORE_H__
#define __GNOME_APP_SCORE_H__

#include <glib.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_SCORE            (gnome_app_score_get_type ())
#define GNOME_APP_SCORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_SCORE, GnomeAppScore))
#define GNOME_APP_SCORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_SCORE, GnomeAppScoreClass))
#define GNOME_APP_IS_SCORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_SCORE))
#define GNOME_APP_IS_SCORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_SCORE))
#define GNOME_APP_SCORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_SCORE, GnomeAppScoreClass))

typedef struct _GnomeAppScore GnomeAppScore;
typedef struct _GnomeAppScoreClass GnomeAppScoreClass;
typedef struct _GnomeAppScorePrivate GnomeAppScorePrivate;

struct _GnomeAppScore
{
        ClutterGroup            parent_instance;
	
	GnomeAppScorePrivate	*priv;
};

struct _GnomeAppScoreClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_score_get_type	(void);
GnomeAppScore *		gnome_app_score_new		(void);

G_END_DECLS

#endif
