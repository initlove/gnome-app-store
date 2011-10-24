/* gnome-app-utils.h -

   Copyright 2011

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

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_UTILS_H__
#define __GNOME_APP_UTILS_H__

#include <glib.h>
#ifdef HAVE_GNOME
#include <libsoup/soup-gnome.h>
#else
#include <libsoup/soup.h>
#endif

G_BEGIN_DECLS

const GList *	gnome_app_get_local_categories ();
gchar *		gnome_app_get_md5 (const gchar *str);
gboolean	gnome_app_category_match_group (gchar *cname, gchar *group);
SoupBuffer *	gnome_app_get_data_by_request (SoupSession *session, const gchar *request);
SoupSession *	gnome_app_soup_session_new (gboolean sync, gchar *cafile);
gchar *		gnome_app_get_local_icon (const gchar *uri);

G_END_DECLS

#endif
