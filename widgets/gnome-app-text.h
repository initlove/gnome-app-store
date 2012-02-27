/*
   Copyright 2011, Novell, Inc.

   The Gnome apptext lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome apptext lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_TEXT_H__
#define __GNOME_APP_TEXT_H__

#include <clutter/clutter.h>
#include "gnome-app-widget.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_TEXT            (gnome_app_text_get_type ())
#define GNOME_APP_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_TEXT, GnomeAppText))
#define GNOME_APP_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_TEXT, GnomeAppTextClass))
#define GNOME_APP_IS_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_TEXT))
#define GNOME_APP_IS_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_TEXT))
#define GNOME_APP_TEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_TEXT, GnomeAppTextClass))

typedef struct _GnomeAppText GnomeAppText;
typedef struct _GnomeAppTextClass GnomeAppTextClass;
typedef struct _GnomeAppTextPrivate GnomeAppTextPrivate;

struct _GnomeAppText
{
	GnomeAppWidget parent_instance;
	GnomeAppTextPrivate	*priv;
};

struct _GnomeAppTextClass
{
        GnomeAppWidgetClass parent_class;
};

GType			gnome_app_text_get_type	(void);
GnomeAppText *		gnome_app_text_new	(void);
const gchar *		gnome_app_text_get_text	(GnomeAppText *text);
void			gnome_app_text_set_text	(GnomeAppText *text, gchar *str);

G_END_DECLS

#endif
