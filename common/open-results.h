/* open-results.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appresults lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appresults lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __OPEN_RESULTS_H__
#define __OPEN_RESULTS_H__

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define TYPE_OPEN_RESULTS		(open_results_get_type ())
#define OPEN_RESULTS(obj)            	(G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OPEN_RESULTS, OpenResults))
#define OPEN_RESULTS_CLASS(klass)    	(G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_OPEN_RESULTS, OpenResultsClass))
#define IS_OPEN_RESULTS(obj)        	 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OPEN_RESULTS))
#define IS_OPEN_RESULTS_CLASS(klass) 	(G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_OPEN_RESULTS))
#define OPEN_RESULTS_GET_CLASS(obj)  	(G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_OPEN_RESULTS, OpenResultsClass))

typedef struct _OpenResults OpenResults;
typedef struct _OpenResultsClass OpenResultsClass;
typedef struct _OpenResultsPrivate OpenResultsPrivate;

struct _OpenResults
{
        GObject                 parent_instance;
	
	OpenResultsPrivate	*priv;
};

struct _OpenResultsClass
{
        GObjectClass parent_class;

	/* virtual */
	const gchar	*(*get_meta)		(OpenResults *results, const gchar *prop);
	GList		*(*get_data)		(OpenResults *results);
	/*get status and get total items can be done by get_meta, but different spec may have diffent 'name' */
	gboolean	(*get_status)		(OpenResults *results);
	gint		(*get_total_items)	(OpenResults *results);
};

GType			open_results_get_type		(void);
OpenResults *		open_results_new		(void);
const gchar *		open_results_get_meta		(OpenResults *results, const gchar *prop);
GList *			open_results_get_data		(OpenResults *results);

gboolean		open_results_get_status		(OpenResults *results);
gint			open_results_get_total_items	(OpenResults *results);

G_END_DECLS

#endif
