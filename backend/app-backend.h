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

#include "gnome-app-config.h"
#include "open-request.h"
#include "open-results.h"

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

struct _AppBackendClass
{
	GObjectClass parent_class;

	/* pure virtual */
	const gchar *(*get_backend_type) (AppBackend        *backend);
	const gchar *(*get_backend_name) (AppBackend        *backend);
	const gchar *(*get_backend_icon_name) (AppBackend        *backend);
	OpenResults *(*get_results)	 (AppBackend *backend,
					 OpenRequest *request);
	gboolean     (*set_config)		(AppBackend *backend,
						GnomeAppConfig *config);
};

GType        	app_backend_get_type			(void) G_GNUC_CONST;
AppBackend  *	app_backend_new_from_config 		(GnomeAppConfig *config);
const gchar *	app_backend_get_backend_type		(AppBackend	*backend);
const gchar *	app_backend_get_backend_name		(AppBackend	*backend);
const gchar *	app_backend_get_backend_icon_name	(AppBackend	*backend);
OpenResults *	app_backend_get_results			(AppBackend	*backend, OpenRequest *request);

G_END_DECLS

#endif /* __APP_BACKEND_H__ */
