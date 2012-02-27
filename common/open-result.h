/* open-result.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appresult lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appresult lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __OPEN_RESULT_H__
#define __OPEN_RESULT_H__

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define TYPE_OPEN_RESULT		(open_result_get_type ())
#define OPEN_RESULT(obj)            	(G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OPEN_RESULT, OpenResult))
#define OPEN_RESULT_CLASS(klass)    	(G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_OPEN_RESULT, OpenResultClass))
#define IS_OPEN_RESULT(obj)        	 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OPEN_RESULT))
#define IS_OPEN_RESULT_CLASS(klass) 	(G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_OPEN_RESULT))
#define OPEN_RESULT_GET_CLASS(obj)  	(G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_OPEN_RESULT, OpenResultClass))

typedef struct _OpenResult OpenResult;
typedef struct _OpenResultClass OpenResultClass;
typedef struct _OpenResultPrivate OpenResultPrivate;

struct _OpenResult
{
        GObject                 parent_instance;
	
	OpenResultPrivate	*priv;
};

struct _OpenResultClass
{
        GObjectClass parent_class;

	/* virtual */
	const gchar	*(*get)			(OpenResult *result, const gchar *prop);
	const gchar 	*(*get_backend_type)	(OpenResult *result);
	gchar		**(*get_props)		(OpenResult *result);
	GList 		*(*get_child)		(OpenResult *result);
};

GType			open_result_get_type		(void);
OpenResult *		open_result_new			(void);
const gchar *		open_result_get			(OpenResult *result, const gchar *prop);
const gchar *		open_result_get_backend_type	(OpenResult *result);
gchar **		open_result_get_props		(OpenResult *result);
GList *			open_result_get_child		(OpenResult *result);
void			open_result_debug		(OpenResult *result);

G_END_DECLS

#endif
