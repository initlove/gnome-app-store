/* open-query.h - 

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

#ifndef __OPEN_QUERY_H__
#define __OPEN_QUERY_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_OPEN_QUERY            (open_query_get_type ())
#define OPEN_QUERY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OPEN_QUERY, OpenQuery))
#define OPEN_QUERY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_OPEN_QUERY, OpenQueryClass))
#define IS_OPEN_QUERY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OPEN_QUERY))
#define IS_OPEN_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_OPEN_QUERY))
#define OPEN_QUERY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_OPEN_QUERY, OpenQueryClass))

typedef struct _OpenQuery OpenQuery;
typedef struct _OpenQueryClass OpenQueryClass;
typedef struct _OpenQueryPrivate OpenQueryPrivate;

struct _OpenQuery
{
        GObject parent_instance;

        OpenQueryPrivate   *priv;
};

struct _OpenQueryClass
{
        GObjectClass parent_class;
};

/*We only provide a mechanism */

GType                   open_query_get_type		(void);
OpenQuery *         	open_query_new			(void);
void			open_query_set 			(OpenQuery *query, gchar *arg_name, gchar *arg_value);
const gchar *		open_query_get 			(OpenQuery *query, gchar *arg);

G_END_DECLS

#endif
