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
#include "app-backend.h"
#include "ocs-backend.h"
/* FIXME: mask the app stream, also remove it from Makefile.am */
//#include "app-stream-backend.h"

G_DEFINE_ABSTRACT_TYPE (AppBackend, app_backend, G_TYPE_OBJECT);

static void
app_backend_init (AppBackend *client)
{
}

static void
app_backend_class_init (AppBackendClass *klass)
{
//  klass->get_backend_icon = app_backend_get_backend_icon_default;
}

const gchar *
app_backend_get_backend_type (AppBackend *backend)
{
	g_return_val_if_fail (APP_IS_BACKEND (backend), NULL);

	return APP_BACKEND_GET_CLASS (backend)->get_backend_type (backend) ;
}

const gchar *
app_backend_get_backend_name (AppBackend *backend)
{
	g_return_val_if_fail (APP_IS_BACKEND (backend), NULL);

	return APP_BACKEND_GET_CLASS (backend)->get_backend_name (backend) ;
}

const gchar *
app_backend_get_backend_icon_name (AppBackend *backend)
{
	g_return_val_if_fail (APP_IS_BACKEND (backend), NULL);

	return APP_BACKEND_GET_CLASS (backend)->get_backend_icon_name (backend) ;
}

OpenResults *
app_backend_get_results (AppBackend *backend, OpenRequest *request)
{
	g_return_val_if_fail (APP_IS_BACKEND (backend), NULL);

	return APP_BACKEND_GET_CLASS (backend)->get_results (backend, request);
}

gboolean
app_backend_set_config (AppBackend *backend, GnomeAppConfig *config)
{
	g_return_val_if_fail (APP_IS_BACKEND (backend), FALSE);

	return APP_BACKEND_GET_CLASS (backend)->set_config (backend, config);
}

AppBackend *
app_backend_new_from_config (GnomeAppConfig *config)
{
	g_return_val_if_fail (GNOME_APP_IS_CONFIG (config), NULL);
	
	AppBackend *backend;
	gchar *type;

	type = gnome_app_config_get_server_type (config);
	
	if (strcmp (type, "ocs") == 0)
		backend = g_object_new (TYPE_OCS_BACKEND, NULL);
#if 0
// MASK THE APP_STREAM 
	else if (strcmp (type, "app_stream") == 0)
		backend = g_object_new (TYPE_APP_STREAM_BACKEND, NULL);
#endif
	else
		return NULL;

	app_backend_set_config (backend, config);

	return backend;
}

