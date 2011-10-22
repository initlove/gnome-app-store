/* gnome-app-info.h - 

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

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_INFO_H__
#define __GNOME_APP_INFO_H__

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_INFO            (gnome_app_info_get_type ())
#define GNOME_APP_INFO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_INFO, GnomeAppInfo))
#define GNOME_APP_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_INFO, GnomeAppInfoClass))
#define GNOME_APP_IS_INFO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_INFO))
#define GNOME_APP_IS_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_INFO))
#define GNOME_APP_INFO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_INFO, GnomeAppInfoClass))

typedef struct _GnomeAppInfo GnomeAppInfo;
typedef struct _GnomeAppInfoClass GnomeAppInfoClass;
typedef struct _GnomeAppInfoPrivate GnomeAppInfoPrivate;

struct _GnomeAppInfo
{
        GObject                 parent_instance;
	
	GnomeAppInfoPrivate	*priv;
};

struct _GnomeAppInfoClass
{
        GObjectClass parent_class;

	glong	(*get_comment_counts)		(GnomeAppInfo *info);
	glong	(*get_download_counts)		(GnomeAppInfo *info);
	gint	(*get_score)			(GnomeAppInfo *info);
	gchar 	*(*get_local_icon_url) 		(GnomeAppInfo *info);
	gchar 	*(*get_local_screenshot_url) 	(GnomeAppInfo *info);
};

GType			gnome_app_info_get_type			(void);
GnomeAppInfo *		gnome_app_info_new			(void);

const gchar *		gnome_app_info_get_id			(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_name			(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_pkgname		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_summary		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_description		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_icon_name		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_screenshot		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_categories		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_license		(GnomeAppInfo *info);
glong			gnome_app_info_get_comment_counts	(GnomeAppInfo *info);
glong			gnome_app_info_get_download_counts	(GnomeAppInfo *info);
gint			gnome_app_info_get_score		(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_local_icon_url	(GnomeAppInfo *info);
const gchar *		gnome_app_info_get_local_screenshot_url	(GnomeAppInfo *info);
void			gnome_app_info_debug			(GnomeAppInfo *info);

G_END_DECLS

#endif
