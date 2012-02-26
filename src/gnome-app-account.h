/*
   Copyright 2011, Novell, Inc.

   The Gnome appaccount lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appaccount lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_ACCOUNT_H__
#define __GNOME_APP_ACCOUNT_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_ACCOUNT            (gnome_app_account_get_type ())
#define GNOME_APP_ACCOUNT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_ACCOUNT, GnomeAppAccount))
#define GNOME_APP_ACCOUNT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_ACCOUNT, GnomeAppAccountClass))
#define GNOME_APP_IS_ACCOUNT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_ACCOUNT))
#define GNOME_APP_IS_ACCOUNT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_ACCOUNT))
#define GNOME_APP_ACCOUNT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_ACCOUNT, GnomeAppAccountClass))

typedef struct _GnomeAppAccount GnomeAppAccount;
typedef struct _GnomeAppAccountClass GnomeAppAccountClass;
typedef struct _GnomeAppAccountPrivate GnomeAppAccountPrivate;

struct _GnomeAppAccount
{
	ClutterGroup parent_instance;
	GnomeAppAccountPrivate	*priv;
};

struct _GnomeAppAccountClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_account_get_type              (void);
GnomeAppAccount *	gnome_app_account_new			(void);

G_END_DECLS

#endif
