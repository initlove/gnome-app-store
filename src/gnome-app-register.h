/*
   Copyright 2011, Novell, Inc.

   The Gnome appregister lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appregister lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_REGISTER_H__
#define __GNOME_APP_REGISTER_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_REGISTER            (gnome_app_register_get_type ())
#define GNOME_APP_REGISTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_REGISTER, GnomeAppRegister))
#define GNOME_APP_REGISTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_REGISTER, GnomeAppRegisterClass))
#define GNOME_APP_IS_REGISTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_REGISTER))
#define GNOME_APP_IS_REGISTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_REGISTER))
#define GNOME_APP_REGISTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_REGISTER, GnomeAppRegisterClass))

typedef struct _GnomeAppRegister GnomeAppRegister;
typedef struct _GnomeAppRegisterClass GnomeAppRegisterClass;
typedef struct _GnomeAppRegisterPrivate GnomeAppRegisterPrivate;

struct _GnomeAppRegister
{
	ClutterGroup parent_instance;
	GnomeAppRegisterPrivate	*priv;
};

struct _GnomeAppRegisterClass
{
        ClutterGroupClass parent_class;
	/* signal */
	void (*regist) (GnomeAppRegister *self);
};

GType			gnome_app_register_get_type              (void);
GnomeAppRegister *	gnome_app_register_new			(void);

G_END_DECLS

#endif
