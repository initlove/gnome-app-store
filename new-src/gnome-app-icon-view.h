/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appicon_view lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_ICON_VIEW_H__
#define __GNOME_APP_ICON_VIEW_H__

#include <gtk/gtk.h>
#include "gnome-app-task.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_ICON_VIEW            (gnome_app_icon_view_get_type ())
#define GNOME_APP_ICON_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_ICON_VIEW, GnomeAppIconView))
#define GNOME_APP_ICON_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_ICON_VIEW, GnomeAppIconViewClass))
#define GNOME_APP_IS_ICON_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_ICON_VIEW))
#define GNOME_APP_IS_ICON_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_ICON_VIEW))
#define GNOME_APP_ICON_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_ICON_VIEW, GnomeAppIconViewClass))

typedef struct _GnomeAppIconView GnomeAppIconView;
typedef struct _GnomeAppIconViewClass GnomeAppIconViewClass;
typedef struct _GnomeAppIconViewPrivate GnomeAppIconViewPrivate;

struct _GnomeAppIconView
{
	GtkIconView parent_instance;
	GnomeAppIconViewPrivate	*priv;
};

struct _GnomeAppIconViewClass
{
        GtkIconViewClass parent_class;
};

GnomeAppIconView *	gnome_app_icon_view_new ();
void			gnome_app_icon_view_set_with_task (GnomeAppIconView *app_iconview, GnomeAppTask *task);

G_END_DECLS

#endif
