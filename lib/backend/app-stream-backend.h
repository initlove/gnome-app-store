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

#ifndef __APP_STREAM_BACKEND_H__
#define __APP_STREAM_BACKEND_H__

#include "app-backend.h"

G_BEGIN_DECLS

#define TYPE_APP_STREAM_BACKEND         (app_stream_backend_get_type ())
#define APP_STREAM_BACKEND(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_APP_STREAM_BACKEND, AppStreamBackend))
#define APP_STREAM_BACKEND_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_APP_STREAM_BACKEND, AppStreamBackendClass))
#define APP_STREAM_BACKEND_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_APP_STREAM_BACKEND, AppStreamBackendClass))
#define IS_APP_STREAM_BACKEND(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_APP_STREAM_BACKEND))
#define IS_APP_STREAM_BACKEND_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_APP_STREAM_BACKEND))

typedef struct _AppStreamBackend AppStreamBackend;
typedef struct _AppStreamBackendClass AppStreamBackendClass;
typedef struct _AppStreamBackendPrivate AppStreamBackendPrivate;

/**
 * AppStreamBackend:
 *
 * The #AppStreamBackend structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _AppStreamBackend
{
	/*< private >*/
	AppBackend parent_instance;
	AppStreamBackendPrivate *priv;
};

/**
 * AppStreamBackendClass:
 *
 * Class structure for #AppStreamBackend.
 */
struct _AppStreamBackendClass
{
	AppBackendClass parent_class;
};

GType        app_stream_backend_get_type                 (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __APP_STREAM_BACKEND_H__ */
