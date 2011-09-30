/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
   Copyright 2011, Novell, Inc.

   The Gnome appitem lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appitem lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Lance Wang <lzwang@suse.com>
*/

#ifndef __GNOME_APP_ITEM_PAGE_H__
#define __GNOME_APP_ITEM_PAGE_H__

#include <glib.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_ITEM_PAGE_LOG_DOMAIN = "item page"

#define GNOME_TYPE_APP_ITEM_PAGE            (gnome_app_item_page_get_type ())
#define GNOME_APP_ITEM_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_TYPE_APP_ITEM_PAGE, GnomeAppItemPAGE))
#define GNOME_APP_ITEM_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_TYPE_APP_ITEM_PAGE, GnomeAppItemPageClass))
#define GNOME_IS_APP_ITEM_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_APP_ITEM_PAGE))
#define GNOME_IS_APP_ITEM_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_TYPE_APP_ITEM_PAGE))
#define GNOME_APP_ITEM_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_TYPE_APP_ITEM_PAGE, GnomeAppItemPageClass))

typedef struct _GnomeAppItemPage GnomeAppItemPage;
typedef struct _GnomeAppItemPageClass GnomeAppItemPageClass;
typedef struct _GnomeAppItemPagePrivate GnomeAppItemPagePrivate;

struct _GnomeAppItemPage
{
        GObject                 parent_instance;
	
	GnomeAppItemPagePrivate	*priv;
};

struct _GnomeAppItemPageClass
{
        GObjectClass parent_class;
};

GType  		          gnome_app_item_page_get_type		(void);
GnomeAppItemPage *        	  gnome_app_item_page_new			(void);


G_END_DECLS

#endif
