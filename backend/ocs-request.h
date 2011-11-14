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

#ifndef __OCS_REQUEST_H__
#define __OCS_REQUEST_H__

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "open-request.h"
#include "ocs-backend.h"

G_BEGIN_DECLS

/*FIXME: make request a gobject ? */
/* make the cache better */

xmlNodePtr	ocs_find_node			(xmlDocPtr doc_ptr, gchar *node_name);
gchar *		ocs_get_request_url	 	(OcsBackend *backend, OpenRequest *request);
xmlDocPtr	ocs_get_request_doc		(OcsBackend *backend, gchar *request);
gboolean	ocs_request_is_valid		(OpenRequest *request);
GList *		ocs_request_get_keys		(const gchar *services,	const gchar *operation, const gchar *group);
gchar *		ocs_request_get_value		(const gchar *services,	const gchar *operation, const gchar *group, const gchar *key);


G_END_DECLS

#endif /* __OCS_REQUEST_H__ */
