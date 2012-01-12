/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome applogin_user lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_LOGIN_USER_H__
#define __GNOME_APP_LOGIN_USER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_LOGIN_USER            (gnome_app_login_user_get_type ())
#define GNOME_APP_LOGIN_USER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_LOGIN_USER, GnomeAppLoginUser))
#define GNOME_APP_LOGIN_USER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_LOGIN_USER, GnomeAppLoginUserClass))
#define GNOME_APP_IS_LOGIN_USER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_LOGIN_USER))
#define GNOME_APP_IS_LOGIN_USER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_LOGIN_USER))
#define GNOME_APP_LOGIN_USER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_LOGIN_USER, GnomeAppLoginUserClass))

typedef struct _GnomeAppLoginUser GnomeAppLoginUser;
typedef struct _GnomeAppLoginUserClass GnomeAppLoginUserClass;
typedef struct _GnomeAppLoginUserPrivate GnomeAppLoginUserPrivate;

struct _GnomeAppLoginUser
{
	GtkBox parent_instance;
	GnomeAppLoginUserPrivate	*priv;
};

struct _GnomeAppLoginUserClass
{
        GtkBoxClass parent_class;
};

GType			gnome_app_login_user_get_type              	(void);
GnomeAppLoginUser *	gnome_app_login_user_new			(void);

G_END_DECLS

#endif
