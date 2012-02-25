/*
   Copyright 2011, Novell, Inc.

   The Gnome appbutton lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appbutton lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_BUTTON_H__
#define __GNOME_APP_BUTTON_H__

#include <clutter/clutter.h>
#include "gnome-app-widget.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_BUTTON            (gnome_app_button_get_type ())
#define GNOME_APP_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_BUTTON, GnomeAppButton))
#define GNOME_APP_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_BUTTON, GnomeAppButtonClass))
#define GNOME_APP_IS_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_BUTTON))
#define GNOME_APP_IS_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_BUTTON))
#define GNOME_APP_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_BUTTON, GnomeAppButtonClass))

typedef struct _GnomeAppButton GnomeAppButton;
typedef struct _GnomeAppButtonClass GnomeAppButtonClass;
typedef struct _GnomeAppButtonPrivate GnomeAppButtonPrivate;

struct _GnomeAppButton
{
	GnomeAppWidget parent_instance;
	GnomeAppButtonPrivate	*priv;
};

struct _GnomeAppButtonClass
{
        GnomeAppWidgetClass parent_class;
};

GType			gnome_app_button_get_type		(void);
GnomeAppButton *	gnome_app_button_new			(void);
void			gnome_app_button_set_text		(GnomeAppButton *button, gchar *text);
gboolean		gnome_app_button_get_selected		(GnomeAppButton *button);
void			gnome_app_button_set_selected		(GnomeAppButton *button, gboolean selected);

G_END_DECLS

#endif
