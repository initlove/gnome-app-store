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

#ifndef __APP_BACKEND_H__
#define __APP_BACKEND_H__

#include "gnome-app-info.h"
#include "gnome-app-config.h"
#include "gnome-app-query.h"

G_BEGIN_DECLS

#define APP_TYPE_BACKEND         (app_backend_get_type ())
#define APP_BACKEND(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), APP_TYPE_BACKEND, AppBackend))
#define APP_BACKEND_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), APP_TYPE_BACKEND, AppBackendClass))
#define APP_BACKEND_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), APP_TYPE_BACKEND, AppBackendClass))
#define APP_IS_BACKEND(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), APP_TYPE_BACKEND))
#define APP_IS_BACKEND_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), APP_TYPE_BACKEND))

typedef struct _AppBackend AppBackend;
typedef struct _AppBackendClass AppBackendClass;
typedef struct _AppBackendPrivate AppBackendPrivate;

/**
 * AppBackend:
 *
 * The #AppBackend structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _AppBackend
{
	/*< private >*/
	GObject parent_instance;
	AppBackendPrivate *priv;
};

/**
 * AppBackendClass:
 * @parent_class: The parent class.
 * @get_backend_type: Virtual function for app_backend_get_backend_type().
 * @get_backend_name: Virtual function for app_backend_get_backend_name().
 * @get_backend_icon_name: Virtual function for app_backend_get_backend_icon_name().
 * @get_cid_list_by_group: Virtual function for app_backend_get_cid_list_by_group().
 *		The name is the client side policy, find the matched category list,
 *		if the group == NULL, list all the available cids 
 * @get_cname_by_id: Virtual function for app_backend_get_cname_by_id ().
 *		ocs backend set the category id to make each category uniq.
 *		app stream backend donnot need this, id equal to name in app steam backend.
 * @get_appid_list_by_cid_list: Virtuahl function for app_backend_get_appid_list_by_cid_list ().
 *		If the categories list is NULL, we list all the apps.
 *		Return the app id list.
 * @get_app_by_id: Virtual function for app_backend_get_app_by_id ();
 * @set_config: Virtual function for app_backend_set_config ();
 *		Each kind of backend has its special way to load config, initial the backend.
 * Class structure for #AppBackend.
 */
struct _AppBackendClass
{
	GObjectClass parent_class;

	/* pure virtual */
	const gchar *(*get_backend_type) (AppBackend        *backend);
	const gchar *(*get_backend_name) (AppBackend        *backend);
	const gchar *(*get_backend_icon_name) (AppBackend        *backend);
	/*FIXME: the return value should better be a tree */
	GList	    *(*get_cid_list_by_group)  (AppBackend *backend,
						gchar *group);
	gchar	    *(*get_cname_by_id) (AppBackend *backend,
						gchar *category_id);
	GList	    *(*get_appid_list_by_cid_list) (AppBackend *backend,
						GList *categories);
	GList	    *(*get_apps_by_query)	 (AppBackend *backend,
						GnomeAppQuery *query);
	GnomeAppInfo *(*get_app_by_id)		(AppBackend *backend,
						gchar *app_id);
	gboolean     (*set_config)		(AppBackend *backend,
						GnomeAppConfig *config);
};

GType        	app_backend_get_type			(void) G_GNUC_CONST;
AppBackend  *	app_backend_new_from_config 		(GnomeAppConfig *config);
const gchar *	app_backend_get_backend_type		(AppBackend	*backend);
const gchar *	app_backend_get_backend_name		(AppBackend	*backend);
const gchar *	app_backend_get_backend_icon_name	(AppBackend	*backend);
GList *		app_backend_get_cid_list_by_group	(AppBackend	*backend, gchar *group);
gchar *		app_backend_get_cname_by_id		(AppBackend	*backend, gchar *cid);
GList *		app_backend_get_appid_list_by_cid_list	(AppBackend	*backend, GList *cid_list);
GList *		app_backend_get_apps_by_query		(AppBackend	*backend, GnomeAppQuery *query);
GnomeAppInfo *	app_backend_get_app_by_id		(AppBackend	*backend, gchar *app_id);

GList        *	app_backend_get_all (void);
G_END_DECLS

#endif /* __APP_BACKEND_H__ */
