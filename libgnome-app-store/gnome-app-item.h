/* libgnome-app-item.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appitem lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appitem lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_ITEM_H__
#define __GNOME_APP_ITEM_H__

#include <libxml/tree.h>
#include <glib.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_TYPE_APP_ITEM            (gnome_app_item_get_type ())
#define GNOME_APP_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_TYPE_APP_ITEM, GnomeAppItem))
#define GNOME_APP_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_TYPE_APP_ITEM, GnomeAppItemClass))
#define GNOME_IS_APP_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_APP_ITEM))
#define GNOME_IS_APP_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_TYPE_APP_ITEM))
#define GNOME_APP_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_TYPE_APP_ITEM, GnomeAppItemClass))

typedef struct _GnomeAppItem GnomeAppItem;
typedef struct _GnomeAppItemClass GnomeAppItemClass;
typedef struct _GnomeAppItemPrivate GnomeAppItemPrivate;

struct _GnomeAppItem
{
        GObject                 parent_instance;
	
	GnomeAppItemPrivate	*priv;
};

struct _GnomeAppItemClass
{
        GObjectClass parent_class;
};

GType  		          gnome_app_item_get_type		(void);
GnomeAppItem *        	  gnome_app_item_new			(void);
GnomeAppItem *        	  gnome_app_item_new_with_node		(gpointer node);

const gchar *		  gnome_app_item_get_name		(GnomeAppItem *item);
const gchar *		  gnome_app_item_get_pkgname		(GnomeAppItem *item);
ClutterActor *		  gnome_app_item_get_icon		(GnomeAppItem *item);
gboolean		  gnome_app_item_is_default_icon	(GnomeAppItem *item);

G_END_DECLS

#endif
