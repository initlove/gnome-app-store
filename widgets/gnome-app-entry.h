/*
   Copyright 2011, Novell, Inc.

   The Gnome appentry lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appentry lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_ENTRY_H__
#define __GNOME_APP_ENTRY_H__

#include <clutter/clutter.h>
#include "gnome-app-widget.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_ENTRY            (gnome_app_entry_get_type ())
#define GNOME_APP_ENTRY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_ENTRY, GnomeAppEntry))
#define GNOME_APP_ENTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_ENTRY, GnomeAppEntryClass))
#define GNOME_APP_IS_ENTRY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_ENTRY))
#define GNOME_APP_IS_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_ENTRY))
#define GNOME_APP_ENTRY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_ENTRY, GnomeAppEntryClass))

typedef struct _GnomeAppEntry GnomeAppEntry;
typedef struct _GnomeAppEntryClass GnomeAppEntryClass;
typedef struct _GnomeAppEntryPrivate GnomeAppEntryPrivate;

struct _GnomeAppEntry
{
	GnomeAppWidget parent_instance;
	GnomeAppEntryPrivate	*priv;
};

struct _GnomeAppEntryClass
{
        GnomeAppWidgetClass parent_class;
};

GType			gnome_app_entry_get_type	(void);
GnomeAppEntry *		gnome_app_entry_new		(void);
const gchar *		gnome_app_entry_get_text	(GnomeAppEntry *entry);
void			gnome_app_entry_set_text	(GnomeAppEntry *entry, gchar *text);

G_END_DECLS

#endif
