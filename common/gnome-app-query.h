/* gnome-app-query.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appquery lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appquery lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_QUERY_H__
#define __GNOME_APP_QUERY_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_QUERY            (gnome_app_query_get_type ())
#define GNOME_APP_QUERY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_QUERY, GnomeAppQuery))
#define GNOME_APP_QUERY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_QUERY, GnomeAppQueryClass))
#define GNOME_APP_IS_QUERY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_QUERY))
#define GNOME_APP_IS_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_QUERY))
#define GNOME_APP_QUERY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_QUERY, GnomeAppQueryClass))

typedef struct _GnomeAppQuery GnomeAppQuery;
typedef struct _GnomeAppQueryClass GnomeAppQueryClass;
typedef struct _GnomeAppQueryPrivate GnomeAppQueryPrivate;

struct _GnomeAppQuery
{
        GObject parent_instance;

        GnomeAppQueryPrivate   *priv;
};

struct _GnomeAppQueryClass
{
        GObjectClass parent_class;
};

#define SORTMODE_NEW		"new"
#define SORTMODE_ALPHA		"alpha"
#define SORTMODE_HIGH		"high"
#define SORTMODE_DOWN		"down"

GType                   gnome_app_query_get_type		(void);
GnomeAppQuery *         gnome_app_query_new			(void);
void			gnome_app_query_set_from_list		(GnomeAppQuery *query, gchar *prop, GList *list);

G_END_DECLS

#endif
