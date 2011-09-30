/* gnome-app-config.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appconfig lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appconfig lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_CONFIG_H__
#define __GNOME_APP_CONFIG_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GNOME_TYPE_APP_CONFIG            (gnome_app_config_get_type ())
#define GNOME_APP_CONFIG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_TYPE_APP_CONFIG, GnomeAppConfig))
#define GNOME_APP_CONFIG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_TYPE_APP_CONFIG, GnomeAppConfigClass))
#define GNOME_IS_APP_CONFIG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_APP_CONFIG))
#define GNOME_IS_APP_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_TYPE_APP_CONFIG))
#define GNOME_APP_CONFIG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_TYPE_APP_CONFIG, GnomeAppConfigClass))

typedef struct _GnomeAppConfig GnomeAppConfig;
typedef struct _GnomeAppConfigClass GnomeAppConfigClass;
typedef struct _GnomeAppConfigPrivate GnomeAppConfigPrivate;

struct _GnomeAppConfig
{
        GObject parent_instance;

        GnomeAppConfigPrivate   *priv;
};

struct _GnomeAppConfigClass
{
        GObjectClass parent_class;
};

GType                   gnome_app_config_get_type               (void);
GnomeAppConfig *        gnome_app_config_new                    (void);

gchar   *		gnome_app_config_get_server_uri		(GnomeAppConfig *conf);
gchar	*		gnome_app_config_get_cache_dir		(GnomeAppConfig *conf);

G_END_DECLS

#endif
