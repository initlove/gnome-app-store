/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
   Copyright 2011, Novell, Inc.

   The Gnome appinfo lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfo lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Lance Wang <lzwang@suse.com>
*/

#ifndef __GNOME_APP_INFO_PAGE_H__
#define __GNOME_APP_INFO_PAGE_H__

#include <glib.h>
#include <clutter/clutter.h>

#include "gnome-app-info.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_INFO_PAGE            (gnome_app_info_page_get_type ())
#define GNOME_APP_INFO_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_INFO_PAGE, GnomeAppInfoPage))
#define GNOME_APP_INFO_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_INFO_PAGE, GnomeAppInfoPageClass))
#define GNOME_APP_IS_INFO_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_INFO_PAGE))
#define GNOME_APP_IS_INFO_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_INFO_PAGE))
#define GNOME_APP_INFO_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_INFO_PAGE, GnomeAppInfoPageClass))

typedef struct _GnomeAppInfoPage GnomeAppInfoPage;
typedef struct _GnomeAppInfoPageClass GnomeAppInfoPageClass;
typedef struct _GnomeAppInfoPagePrivate GnomeAppInfoPagePrivate;

struct _GnomeAppInfoPage
{
        ClutterGroup            parent_instance;
	
	GnomeAppInfoPagePrivate	*priv;
};

struct _GnomeAppInfoPageClass
{
        ClutterGroupClass parent_class;
};

GType				gnome_app_info_page_get_type		(void);
GnomeAppInfoPage *		gnome_app_info_page_new			(void);
GnomeAppInfoPage *		gnome_app_info_page_new_with_app 	(GnomeAppInfo *app);

G_END_DECLS

#endif
