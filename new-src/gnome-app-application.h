/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome App Store is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_APPLICATION_H__
#define __GNOME_APP_APPLICATION_H__

#include <gtk/gtk.h>

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

typedef enum {
	APP_ICON_VIEW,
	APP_INFO_PAGE,
	APP_LAST
} APP_VIEW_TYPE;

struct _GnomeAppApplication
{
	GtkApplication parent_instance;
	GnomeAppApplicationPrivate	*priv;
};

struct _GnomeAppApplicationClass
{
        GtkApplicationClass parent_class;
};

GType			gnome_app_application_get_type          (void);
GnomeAppApplication *	gnome_app_application_new		(void);
void			gnome_app_application_load 		(GnomeAppApplication *app, APP_VIEW_TYPE type, gpointer userdata);

G_END_DECLS

#endif
