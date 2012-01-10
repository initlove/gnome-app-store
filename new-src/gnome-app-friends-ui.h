/*
   Copyright 2011, Novell, Inc.

   The Gnome appfriends_ui lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appfriends_ui lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_FRIENDS_UI_H__
#define __GNOME_APP_FRIENDS_UI_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_FRIENDS_UI            (gnome_app_friends_ui_get_type ())
#define GNOME_APP_FRIENDS_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_FRIENDS_UI, GnomeAppFriendsUI))
#define GNOME_APP_FRIENDS_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_FRIENDS_UI, GnomeAppFriendsUIClass))
#define GNOME_APP_IS_FRIENDS_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_FRIENDS_UI))
#define GNOME_APP_IS_FRIENDS_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_FRIENDS_UI))
#define GNOME_APP_FRIENDS_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_FRIENDS_UI, GnomeAppFriendsUIClass))

typedef struct _GnomeAppFriendsUI GnomeAppFriendsUI;
typedef struct _GnomeAppFriendsUIClass GnomeAppFriendsUIClass;
typedef struct _GnomeAppFriendsUIPrivate GnomeAppFriendsUIPrivate;

struct _GnomeAppFriendsUI
{
	ClutterGroup parent_instance;
	GnomeAppFriendsUIPrivate	*priv;
};

struct _GnomeAppFriendsUIClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_friends_ui_get_type              	(void);
GnomeAppFriendsUI *	gnome_app_friends_ui_new			(const gchar *personid);

G_END_DECLS

#endif
