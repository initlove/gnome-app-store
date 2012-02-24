/*
   Copyright 2011, Novell, Inc.

   The Gnome appcheck_box lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appcheck_box lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_CHECK_BOX_H__
#define __GNOME_APP_CHECK_BOX_H__

#include <clutter/clutter.h>
#include "gnome-app-widget.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_CHECK_BOX            (gnome_app_check_box_get_type ())
#define GNOME_APP_CHECK_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_CHECK_BOX, GnomeAppCheckBox))
#define GNOME_APP_CHECK_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_CHECK_BOX, GnomeAppCheckBoxClass))
#define GNOME_APP_IS_CHECK_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_CHECK_BOX))
#define GNOME_APP_IS_CHECK_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_CHECK_BOX))
#define GNOME_APP_CHECK_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_CHECK_BOX, GnomeAppCheckBoxClass))

typedef struct _GnomeAppCheckBox GnomeAppCheckBox;
typedef struct _GnomeAppCheckBoxClass GnomeAppCheckBoxClass;
typedef struct _GnomeAppCheckBoxPrivate GnomeAppCheckBoxPrivate;

struct _GnomeAppCheckBox
{
	GnomeAppWidget parent_instance;
	GnomeAppCheckBoxPrivate	*priv;
};

struct _GnomeAppCheckBoxClass
{
        GnomeAppWidgetClass parent_class;
};

GType			gnome_app_check_box_get_type		(void);
GnomeAppCheckBox *	gnome_app_check_box_new			(void);
gboolean		gnome_app_check_box_get_selected	(GnomeAppCheckBox *check_box);
void			gnome_app_check_box_set_selected	(GnomeAppCheckBox *check_box, gboolean selected);
G_END_DECLS

#endif
