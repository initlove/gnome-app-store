/*
   Copyright 2011, Novell, Inc.

   The Gnome apptexture lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome apptexture lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_TEXTURE_H__
#define __GNOME_APP_TEXTURE_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_APP_TYPE_TEXTURE            (gnome_app_texture_get_type ())
#define GNOME_APP_TEXTURE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_TEXTURE, GnomeAppTexture))
#define GNOME_APP_TEXTURE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_TEXTURE, GnomeAppTextureClass))
#define GNOME_APP_IS_TEXTURE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_TEXTURE))
#define GNOME_APP_IS_TEXTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_TEXTURE))
#define GNOME_APP_TEXTURE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_TEXTURE, GnomeAppTextureClass))

typedef struct _GnomeAppTexture GnomeAppTexture;
typedef struct _GnomeAppTextureClass GnomeAppTextureClass;
typedef struct _GnomeAppTexturePrivate GnomeAppTexturePrivate;

struct _GnomeAppTexture
{
	ClutterTexture parent_instance;
	GnomeAppTexturePrivate	*priv;
};

struct _GnomeAppTextureClass
{
        ClutterTextureClass parent_class;
};

GType			gnome_app_texture_get_type              (void);
GnomeAppTexture *	gnome_app_texture_new			(void);
GnomeAppTexture *	gnome_app_dtexture_new_from_file	(gchar *dir);
GnomeAppTexture *	gnome_app_mtexture_new_from_file	(gchar *filename, gint col, gint row);

G_END_DECLS

#endif
