/* 
   Copyright 2011, Novell, Inc.

   The Gnome appframe lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appframe lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Sframete 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/

#ifndef __GNOME_APP_FRAME_H__
#define __GNOME_APP_FRAME_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_FRAME            (gnome_app_frame_get_type ())
#define GNOME_APP_FRAME(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_FRAME, GnomeAppFrame))
#define GNOME_APP_FRAME_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_FRAME, GnomeAppFrameClass))
#define GNOME_APP_IS_FRAME(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_FRAME))
#define GNOME_APP_IS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_FRAME))
#define GNOME_APP_FRAME_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_FRAME, GnomeAppFrameClass))

typedef struct _GnomeAppFrame GnomeAppFrame;
typedef struct _GnomeAppFrameClass GnomeAppFrameClass;
typedef struct _GnomeAppFramePrivate GnomeAppFramePrivate;

struct _GnomeAppFrame
{
	ClutterGroup parent_instance;

	GnomeAppFramePrivate	*priv;
};

struct _GnomeAppFrameClass
{
        ClutterGroupClass parent_class;
};

GType  		        gnome_app_frame_get_type		(void);
GnomeAppFrame *      	gnome_app_frame_new			(void);

G_END_DECLS

#endif
