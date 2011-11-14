/* open-request.h - 

   Copyright 2011, Novell, Inc.

   The Gnome apprequest lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome apprequest lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __OPEN_REQUEST_H__
#define __OPEN_REQUEST_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_OPEN_REQUEST            (open_request_get_type ())
#define OPEN_REQUEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OPEN_REQUEST, OpenRequest))
#define OPEN_REQUEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_OPEN_REQUEST, OpenRequestClass))
#define IS_OPEN_REQUEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OPEN_REQUEST))
#define IS_OPEN_REQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_OPEN_REQUEST))
#define OPEN_REQUEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_OPEN_REQUEST, OpenRequestClass))

typedef struct _OpenRequest OpenRequest;
typedef struct _OpenRequestClass OpenRequestClass;
typedef struct _OpenRequestPrivate OpenRequestPrivate;

struct _OpenRequest
{
        GObject parent_instance;

        OpenRequestPrivate   *priv;
};

struct _OpenRequestClass
{
        GObjectClass parent_class;
};

/*We only provide a mechanism */

GType                   open_request_get_type		(void);
OpenRequest *         	open_request_new			(void);
void			open_request_set 			(OpenRequest *request, gchar *arg_name, gchar *arg_value);
const gchar *		open_request_get 			(OpenRequest *request, gchar *arg);

G_END_DECLS

#endif
