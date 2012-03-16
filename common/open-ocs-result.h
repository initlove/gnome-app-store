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
 * Author: David Liang <dliang@novell.com>
 */

#ifndef __OPEN_OCS_RESULT_H__
#define __OPEN_OCS_RESULT_H__

#include <libxml/tree.h>
#include "open-result.h"

G_BEGIN_DECLS

#define TYPE_OPEN_OCS_RESULT         (open_ocs_result_get_type ())
#define OPEN_OCS_RESULT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OPEN_OCS_RESULT, OpenOcsResult))
#define OPEN_OCS_RESULT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OPEN_OCS_RESULT, OpenOcsResultClass))
#define OPEN_OCS_RESULT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OPEN_OCS_RESULT, OpenOcsResultClass))
#define IS_OPEN_OCS_RESULT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OPEN_OCS_RESULT))
#define IS_OPEN_OCS_RESULT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OPEN_OCS_RESULT))

typedef struct _OpenOcsResult OpenOcsResult;
typedef struct _OpenOcsResultClass OpenOcsResultClass;
typedef struct _OpenOcsResultPrivate OpenOcsResultPrivate;

/**
 * OpenOcsResult:
 *
 * The #OpenOcsResult structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OpenOcsResult
{
	/*< private >*/
	OpenResult parent_instance;

	OpenOcsResultPrivate *priv;
};

/**
 * OpenOcsResultClass:
 *
 * Class structure for #OpenOcsResult.
 */
struct _OpenOcsResultClass
{
	OpenResultClass parent_class;
};

GType		open_ocs_result_get_type		(void) G_GNUC_CONST;
OpenOcsResult *	open_ocs_result_new_with_node		(xmlNodePtr node);
GList *		open_ocs_result_list_new_with_node	(xmlNodePtr node);

G_END_DECLS

#endif /* __OPEN_OCS_RESULT_H__ */
