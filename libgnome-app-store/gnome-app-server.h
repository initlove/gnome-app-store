/* libgnome-app-server.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appserver lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appserver lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_SERVER_H__
#define __GNOME_APP_SERVER_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_TYPE_APP_SERVER            (gnome_app_server_get_type ())
#define GNOME_APP_SERVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_TYPE_APP_SERVER, GnomeAppServer))
#define GNOME_APP_SERVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_TYPE_APP_SERVER, GnomeAppServerClass))
#define GNOME_IS_APP_SERVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_APP_SERVER))
#define GNOME_IS_APP_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_TYPE_APP_SERVER))
#define GNOME_APP_SERVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_TYPE_APP_SERVER, GnomeAppServerClass))

typedef struct _GnomeAppServer GnomeAppServer;
typedef struct _GnomeAppServerClass GnomeAppServerClass;
typedef struct _GnomeAppServerPrivate GnomeAppServerPrivate;

struct _GnomeAppServer
{
	GObject parent_instance;
	
	GnomeAppServerPrivate	*priv;
};

struct _GnomeAppServerClass
{
        GObjectClass parent_class;
};

GType  		        gnome_app_server_get_type		(void);
GnomeAppServer *	gnome_app_server_new			(void);
gboolean		gnome_app_server_sync			(GnomeAppServer *server);


G_END_DECLS

#endif
