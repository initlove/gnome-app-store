/*
   Copyright 2011, Novell, Inc.

   The Gnome appwidget lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appwidget lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_WIDGET_H__
#define __GNOME_APP_WIDGET_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_WIDGET            (gnome_app_widget_get_type ())
#define GNOME_APP_WIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_WIDGET, GnomeAppWidget))
#define GNOME_APP_WIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_WIDGET, GnomeAppWidgetClass))
#define GNOME_APP_IS_WIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_WIDGET))
#define GNOME_APP_IS_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_WIDGET))
#define GNOME_APP_WIDGET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_WIDGET, GnomeAppWidgetClass))

typedef struct _GnomeAppWidget GnomeAppWidget;
typedef struct _GnomeAppWidgetClass GnomeAppWidgetClass;
typedef struct _GnomeAppWidgetPrivate GnomeAppWidgetPrivate;

struct _GnomeAppWidget
{
	ClutterGroup parent_instance;
	GnomeAppWidgetPrivate	*priv;
};

struct _GnomeAppWidgetClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_widget_get_type	(void);
GnomeAppWidget *	gnome_app_widget_new		(void);
gboolean		gnome_app_widget_get_entered	(GnomeAppWidget *widget);

G_END_DECLS

#endif
