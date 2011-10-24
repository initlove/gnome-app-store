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

#ifndef __OCS_BACKEND_H__
#define __OCS_BACKEND_H__

#include <libsoup/soup-session.h>
#include "app-backend.h"

G_BEGIN_DECLS

#define TYPE_OCS_BACKEND         (ocs_backend_get_type ())
#define OCS_BACKEND(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OCS_BACKEND, OcsBackend))
#define OCS_BACKEND_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OCS_BACKEND, OcsBackendClass))
#define OCS_BACKEND_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OCS_BACKEND, OcsBackendClass))
#define IS_OCS_BACKEND(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OCS_BACKEND))
#define IS_OCS_BACKEND_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OCS_BACKEND))

typedef struct _OcsBackend OcsBackend;
typedef struct _OcsBackendClass OcsBackendClass;
typedef struct _OcsBackendPrivate OcsBackendPrivate;

/**
 * OcsBackend:
 *
 * The #OcsBackend structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OcsBackend
{
	/*< private >*/
	AppBackend parent_instance;
	OcsBackendPrivate *priv;
};

/**
 * OcsBackendClass:
 *
 * Class structure for #OcsBackend.
 */
struct _OcsBackendClass
{
	AppBackendClass parent_class;
};

GType			ocs_backend_get_type		(void) G_GNUC_CONST;
const char * 		ocs_backend_get_server_uri	(OcsBackend *backend);
const char *		ocs_backend_get_username	(OcsBackend *backend);
const char *		ocs_backend_get_password	(OcsBackend *backend);
const SoupSession * 	ocs_backend_get_session	(OcsBackend *backend);
const char *		ocs_backend_get_cache_dir	(OcsBackend *backend);

GList *			ocs_get_cid_list_by_group (OcsBackend *ocs_backend, gchar *group);

G_END_DECLS

#endif /* __OCS_BACKEND_H__ */
