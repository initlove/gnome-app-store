/* gnome-app-store.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appstore lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appstore lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_STORE_H__
#define __GNOME_APP_STORE_H__

#include <glib.h>
#include <glib-object.h>
/*FIXME: why should add common here? */
#include "common/gnome-app-item.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_STORE            (gnome_app_store_get_type ())
#define GNOME_APP_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_STORE, GnomeAppStore))
#define GNOME_APP_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_STORE, GnomeAppStoreClass))
#define GNOME_APP_IS_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_STORE))
#define GNOME_APP_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_STORE))
#define GNOME_APP_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_STORE, GnomeAppStoreClass))

typedef struct _GnomeAppStore GnomeAppStore;
typedef struct _GnomeAppStoreClass GnomeAppStoreClass;
typedef struct _GnomeAppStorePrivate GnomeAppStorePrivate;

struct _GnomeAppStore
{
        GObject                 parent_instance;
	
	GnomeAppStorePrivate	*priv;
};

struct _GnomeAppStoreClass
{
        GObjectClass parent_class;
};

GType			gnome_app_store_get_type			(void);
GnomeAppStore *		gnome_app_store_new				(void);
GList *			gnome_app_store_get_cid_list_by_group		(GnomeAppStore *store, gchar *group);
gchar *			gnome_app_store_get_cname_by_id 		(GnomeAppStore *store, gchar *cid);
GList *			gnome_app_store_get_appid_list_by_cid_list	(GnomeAppStore *store, GList *cid_list);
GnomeAppItem *		gnome_app_store_get_app_by_id			(GnomeAppStore *store, gchar *app_id);

G_END_DECLS

#endif
