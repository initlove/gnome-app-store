/*
   Copyright 2011, Novell, Inc.

   The Gnome appmessage_ui lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appmessage_ui lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_MESSAGE_UI_H__
#define __GNOME_APP_MESSAGE_UI_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_MESSAGE_UI            (gnome_app_message_ui_get_type ())
#define GNOME_APP_MESSAGE_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_MESSAGE_UI, GnomeAppMessageUI))
#define GNOME_APP_MESSAGE_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_MESSAGE_UI, GnomeAppMessageUIClass))
#define GNOME_APP_IS_MESSAGE_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_MESSAGE_UI))
#define GNOME_APP_IS_MESSAGE_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_MESSAGE_UI))
#define GNOME_APP_MESSAGE_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_MESSAGE_UI, GnomeAppMessageUIClass))

typedef struct _GnomeAppMessageUI GnomeAppMessageUI;
typedef struct _GnomeAppMessageUIClass GnomeAppMessageUIClass;
typedef struct _GnomeAppMessageUIPrivate GnomeAppMessageUIPrivate;

struct _GnomeAppMessageUI
{
	ClutterGroup parent_instance;
	GnomeAppMessageUIPrivate	*priv;
};

struct _GnomeAppMessageUIClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_message_ui_get_type              	(void);
GnomeAppMessageUI *	gnome_app_message_ui_new			(void);

G_END_DECLS

#endif
