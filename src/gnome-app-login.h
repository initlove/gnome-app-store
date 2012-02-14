/*
   Copyright 2011, Novell, Inc.

   The Gnome applogin lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome applogin lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_LOGIN_H__
#define __GNOME_APP_LOGIN_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_LOGIN            (gnome_app_login_get_type ())
#define GNOME_APP_LOGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_LOGIN, GnomeAppLogin))
#define GNOME_APP_LOGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_LOGIN, GnomeAppLoginClass))
#define GNOME_APP_IS_LOGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_LOGIN))
#define GNOME_APP_IS_LOGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_LOGIN))
#define GNOME_APP_LOGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_LOGIN, GnomeAppLoginClass))

typedef struct _GnomeAppLogin GnomeAppLogin;
typedef struct _GnomeAppLoginClass GnomeAppLoginClass;
typedef struct _GnomeAppLoginPrivate GnomeAppLoginPrivate;

struct _GnomeAppLogin
{
	GObject parent_instance;
	GnomeAppLoginPrivate	*priv;
};

struct _GnomeAppLoginClass
{
        GObjectClass parent_class;
	/* signal */
	void (*auth) (GnomeAppLogin *self);
};

GType			gnome_app_login_get_type              	(void);
GnomeAppLogin *		gnome_app_login_new			(void);
void			gnome_app_login_run	               	(GnomeAppLogin *login);

G_END_DECLS

#endif
