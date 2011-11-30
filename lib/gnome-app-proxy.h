/* gnome-app-proxy.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appproxy lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appproxy lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_PROXY_H__
#define __GNOME_APP_PROXY_H__

#include <glib.h>
#include <glib-object.h>

#include "open-results.h"
#include "gnome-app-task.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_PROXY            (gnome_app_proxy_get_type ())
#define GNOME_APP_PROXY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_PROXY, GnomeAppProxy))
#define GNOME_APP_PROXY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_PROXY, GnomeAppProxyClass))
#define GNOME_APP_IS_PROXY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_PROXY))
#define GNOME_APP_IS_PROXY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_PROXY))
#define GNOME_APP_PROXY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_PROXY, GnomeAppProxyClass))

typedef struct _GnomeAppProxy GnomeAppProxy;
typedef struct _GnomeAppProxyClass GnomeAppProxyClass;
typedef struct _GnomeAppProxyPrivate GnomeAppProxyPrivate;

struct _GnomeAppProxy
{
        GObject                 parent_instance;
	
	GnomeAppProxyPrivate	*priv;
};

struct _GnomeAppProxyClass
{
        GObjectClass parent_class;
};

GType			gnome_app_proxy_get_type			(void);
GnomeAppProxy *		gnome_app_proxy_new 				(void);
OpenResults *		gnome_app_proxy_find				(GnomeAppProxy *proxy, gchar *str);
void			gnome_app_proxy_add				(GnomeAppProxy *proxy, gchar *str, OpenResults *result);
void			gnome_app_proxy_predict				(GnomeAppProxy *proxy, GnomeAppTask *task);

G_END_DECLS

#endif
