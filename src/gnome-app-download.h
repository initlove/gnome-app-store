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

   Author:	David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_DOWNLOAD_H__
#define __GNOME_APP_DOWNLOAD_H__

#include <glib.h>
#include "open-app-utils.h"
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_DOWNLOAD            (gnome_app_download_get_type ())
#define GNOME_APP_DOWNLOAD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_DOWNLOAD, GnomeAppDownload))
#define GNOME_APP_DOWNLOAD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_DOWNLOAD, GnomeAppDownloadClass))
#define GNOME_APP_IS_DOWNLOAD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_DOWNLOAD))
#define GNOME_APP_IS_DOWNLOAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_DOWNLOAD))
#define GNOME_APP_DOWNLOAD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_DOWNLOAD, GnomeAppDownloadClass))

typedef struct _GnomeAppDownload GnomeAppDownload;
typedef struct _GnomeAppDownloadClass GnomeAppDownloadClass;
typedef struct _GnomeAppDownloadPrivate GnomeAppDownloadPrivate;

struct _GnomeAppDownload
{
        ClutterGroup            parent_instance;
	
	GnomeAppDownloadPrivate	*priv;
};

struct _GnomeAppDownloadClass
{
        ClutterGroupClass parent_class;
};

GType				gnome_app_download_get_type		(void);
GnomeAppDownload *		gnome_app_download_new			(void);
GnomeAppDownload *		gnome_app_download_new_with_info	(OpenResult *info);

G_END_DECLS

#endif
