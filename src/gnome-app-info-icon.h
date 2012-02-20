/* 
   Copyright 2011, Novell, Inc.

   The Gnome appinfo_icon lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfo_icon lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_INFO_ICON_H__
#define __GNOME_APP_INFO_ICON_H__

#include <clutter/clutter.h>
#include "open-result.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_INFO_ICON            (gnome_app_info_icon_get_type ())
#define GNOME_APP_INFO_ICON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_INFO_ICON, GnomeAppInfoIcon))
#define GNOME_APP_INFO_ICON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_INFO_ICON, GnomeAppInfoIconClass))
#define GNOME_APP_IS_INFO_ICON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_INFO_ICON))
#define GNOME_APP_IS_INFO_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_INFO_ICON))
#define GNOME_APP_INFO_ICON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_INFO_ICON, GnomeAppInfoIconClass))

typedef struct _GnomeAppInfoIcon GnomeAppInfoIcon;
typedef struct _GnomeAppInfoIconClass GnomeAppInfoIconClass;
typedef struct _GnomeAppInfoIconPrivate GnomeAppInfoIconPrivate;

struct _GnomeAppInfoIcon
{
	ClutterGroup parent_instance;
	GnomeAppInfoIconPrivate	*priv;
};

struct _GnomeAppInfoIconClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_info_icon_get_type              	(void);
GnomeAppInfoIcon *	gnome_app_info_icon_new_with_info	        (OpenResult *info);

G_END_DECLS

#endif
