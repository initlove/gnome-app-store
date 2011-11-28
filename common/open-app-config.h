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

#ifndef __OPEN_APP_CONFIG_H__
#define __OPEN_APP_CONFIG_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OPEN_APP_TYPE_CONFIG            (open_app_config_get_type ())
#define OPEN_APP_CONFIG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OPEN_APP_TYPE_CONFIG, OpenAppConfig))
#define OPEN_APP_CONFIG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  OPEN_APP_TYPE_CONFIG, OpenAppConfigClass))
#define OPEN_APP_IS_CONFIG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OPEN_APP_TYPE_CONFIG))
#define OPEN_APP_IS_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  OPEN_APP_TYPE_CONFIG))
#define OPEN_APP_CONFIG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  OPEN_APP_TYPE_CONFIG, OpenAppConfigClass))

typedef struct _OpenAppConfig OpenAppConfig;
typedef struct _OpenAppConfigClass OpenAppConfigClass;
typedef struct _OpenAppConfigPrivate OpenAppConfigPrivate;

struct _OpenAppConfig
{
        GObject parent_instance;

        OpenAppConfigPrivate   *priv;
};

struct _OpenAppConfigClass
{
        GObjectClass parent_class;
};

GType                   open_app_config_get_type               (void);
OpenAppConfig *         open_app_config_new                    (void);

const gchar *		open_app_config_get_server_type 	(OpenAppConfig *config);
const gchar *		open_app_config_get_server_uri		(OpenAppConfig *conf);
const gchar *		open_app_config_get_username		(OpenAppConfig *conf);
const gchar *		open_app_config_get_password		(OpenAppConfig *conf);

G_END_DECLS

#endif
