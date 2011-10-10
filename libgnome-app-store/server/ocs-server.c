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

#include "ocs-server.h"
#include "gnome-app-config.h"

struct _OcsServerPrivate
{
	gchar *url;
};

enum {
        PROP_0,
        PROP_CONFIG,
};

G_DEFINE_TYPE (OcsServer, ocs_server, APP_TYPE_SERVER)

static void
ocs_server_init (OcsServer *server)
{
	OcsServerPrivate *priv;

	server->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (server,
						TYPE_OCS_SERVER,
						OcsServerPrivate);
}

static const gchar *
get_server_type (AppServer *server)
{
	return "ocs";
}
        
static const gchar *
get_server_name (AppServer *server)
{
}

static const gchar *
get_server_icon_name (AppServer *server)
{
}
        
static GList *
get_cid_list_by_group (AppServer *server, gchar *group)
{
}

static gchar *
get_cname_by_id (AppServer *server, gchar *category_id)
{
}

static GList *
get_appid_list_by_cid_list (AppServer *server, GList *categories)
{
}

static GnomeAppItem *
get_app_by_id (AppServer *server, gchar *app_id)
{
}

static gboolean
set_config (AppServer *server, GnomeAppConfig *config)
{
	return TRUE;
}

static void
ocs_server_class_init (OcsServerClass *klass)
{
	AppServerClass *parent_class;
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

//        object_class->dispose = gnome_app_item_dispose;
//        object_class->finalize = gnome_app_item_finalize;

	parent_class = APP_SERVER_CLASS (klass);

	parent_class->get_server_type = get_server_type;
	parent_class->get_server_name = get_server_name;
	parent_class->get_server_icon_name = get_server_icon_name;
	parent_class->get_cid_list_by_group = get_cid_list_by_group;
	parent_class->get_cname_by_id = get_cname_by_id;
	parent_class->get_appid_list_by_cid_list = get_appid_list_by_cid_list;
	parent_class->get_app_by_id = get_app_by_id;
	parent_class->set_config = set_config;

        g_type_class_add_private (object_class, sizeof (OcsServerPrivate));
}

