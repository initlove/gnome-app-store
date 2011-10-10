/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2011
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Liang chenye <liangchenye@gmail.com>
 */

#include "gnome-app-config.h"
#include "app-server.h"
#include "ocs-server.h"
#include "app-stream-server.h"

G_DEFINE_ABSTRACT_TYPE (AppServer, app_server, G_TYPE_OBJECT);

static void
app_server_init (AppServer *client)
{
}

static void
app_server_class_init (AppServerClass *klass)
{
//  klass->get_server_icon = app_server_get_server_icon_default;
}

const gchar *
app_server_get_server_type (AppServer *server)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_server_type (server) ;
}

const gchar *
app_server_get_server_name (AppServer *server)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_server_name (server) ;
}

const gchar *
app_server_get_server_icon_name (AppServer *server)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_server_icon_name (server) ;
}

GList *
app_server_get_cid_list_by_group (AppServer *server, gchar *group)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_cid_list_by_group (server, group);
}

gchar *
app_server_get_cname_by_id (AppServer *server, gchar *category_id)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_cname_by_id (server, category_id);
}

GList *
app_server_get_appid_list_by_cid_list (AppServer *server, GList *categories)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_appid_list_by_cid_list (server, categories);
}

GnomeAppItem *
app_server_get_app_by_id (AppServer *server, gchar *app_id)
{
	g_return_val_if_fail (APP_IS_SERVER (server), NULL);

	return APP_SERVER_GET_CLASS (server)->get_app_by_id (server, app_id);
}

gboolean
app_server_set_config (AppServer *server, GnomeAppConfig *config)
{
	g_return_val_if_fail (APP_IS_SERVER (server), FALSE);

	return APP_SERVER_GET_CLASS (server)->set_config (server, config);
}

AppServer *
app_server_new_from_config (GnomeAppConfig *config)
{
	g_return_val_if_fail (GNOME_APP_IS_CONFIG (config), NULL);
	
	AppServer *server;

/*FIXME: more check here */
	if (0)
		server = g_object_new (TYPE_OCS_SERVER, NULL);
	else
		server = g_object_new (TYPE_APP_STREAM_SERVER, NULL);

	app_server_set_config (server, config);

	return server;
}

GList *
app_server_get_all (void)
{
	return NULL;
}

