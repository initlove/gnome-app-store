/* gnome-app-ui-utils.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appui_utils lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appui_utils lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_UI_UTILS_H__
#define __GNOME_APP_UI_UTILS_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

void gnome_app_ui_set_icon (ClutterActor *actor, const gchar *uri);

G_END_DECLS

#endif
