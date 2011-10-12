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

#ifndef __APP_SERVER_H__
#define __APP_SERVER_H__

#include "gnome-app-item.h"
#include "gnome-app-config.h"

G_BEGIN_DECLS

#define APP_TYPE_SERVER         (app_server_get_type ())
#define APP_SERVER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), APP_TYPE_SERVER, AppServer))
#define APP_SERVER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), APP_TYPE_SERVER, AppServerClass))
#define APP_SERVER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), APP_TYPE_SERVER, AppServerClass))
#define APP_IS_SERVER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), APP_TYPE_SERVER))
#define APP_IS_SERVER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), APP_TYPE_SERVER))

typedef struct _AppServer AppServer;
typedef struct _AppServerClass AppServerClass;
typedef struct _AppServerPrivate AppServerPrivate;

/**
 * AppServer:
 *
 * The #AppServer structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _AppServer
{
	/*< private >*/
	GObject parent_instance;
	AppServerPrivate *priv;
};

/**
 * AppServerClass:
 * @parent_class: The parent class.
 * @get_server_type: Virtual function for app_server_get_server_type().
 * @get_server_name: Virtual function for app_server_get_server_name().
 * @get_server_icon_name: Virtual function for app_server_get_server_icon_name().
 * @get_cid_list_by_group: Virtual function for app_server_get_cid_list_by_group().
 *		The name is the client side policy, find the matched category list,
 *		if the group == NULL, list all the available cids 
 * @get_cname_by_id: Virtual function for app_server_get_cname_by_id ().
 *		ocs server set the category id to make each category uniq.
 *		app stream server donnot need this, id equal to name in app steam server.
 * @get_appid_list_by_cid_list: Virtuahl function for app_server_get_appid_list_by_cid_list ().
 *		If the categories list is NULL, we list all the apps.
 *		Return the app id list.
 * @get_app_by_id: Virtual function for app_server_get_app_by_id ();
 * @set_config: Virtual function for app_server_set_config ();
 *		Each kind of server has its special way to load config, initial the server.
 * Class structure for #AppServer.
 */
struct _AppServerClass
{
	GObjectClass parent_class;

	/* pure virtual */
	const gchar *(*get_server_type) (AppServer        *server);
	const gchar *(*get_server_name) (AppServer        *server);
	const gchar *(*get_server_icon_name) (AppServer        *server);
	/*FIXME: the return value should better be a tree */
	GList	    *(*get_cid_list_by_group)  (AppServer *server,
						gchar *group);
	gchar	    *(*get_cname_by_id) (AppServer *server,
						gchar *category_id);
	GList	    *(*get_appid_list_by_cid_list) (AppServer *server,
						GList *categories);
	GnomeAppItem *(*get_app_by_id)		(AppServer *server,
						gchar *app_id);
	gboolean     (*set_config)		(AppServer *server,
						GnomeAppConfig *config);
};

GType        	app_server_get_type			(void) G_GNUC_CONST;
AppServer  *	app_server_new_from_config 		(GnomeAppConfig *config);
const gchar *	app_server_get_server_type		(AppServer	*server);
const gchar *	app_server_get_server_name		(AppServer	*server);
const gchar *	app_server_get_server_icon_name		(AppServer	*server);
GList *		app_server_get_cid_list_by_group	(AppServer	*server, gchar *group);
gchar *		app_server_get_cname_by_id		(AppServer	*server, gchar *cid);
GList *		app_server_get_appid_list_by_cid_list	(AppServer	*server, GList *cid_list);
GnomeAppItem *	app_server_get_app_by_id		(AppServer	*server, gchar *app_id);

GList        *	app_server_get_all (void);

G_END_DECLS

#endif /* __APP_SERVER_H__ */
