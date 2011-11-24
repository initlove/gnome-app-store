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

#ifndef __OCS_RESULT_H__
#define __OCS_RESULT_H__

#include <libxml/tree.h>
#include "open-result.h"

G_BEGIN_DECLS

#define TYPE_OCS_RESULT         (ocs_result_get_type ())
#define OCS_RESULT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OCS_RESULT, OcsResult))
#define OCS_RESULT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OCS_RESULT, OcsResultClass))
#define OCS_RESULT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OCS_RESULT, OcsResultClass))
#define IS_OCS_RESULT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OCS_RESULT))
#define IS_OCS_RESULT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OCS_RESULT))

typedef struct _OcsResult OcsResult;
typedef struct _OcsResultClass OcsResultClass;
typedef struct _OcsResultPrivate OcsResultPrivate;

/**
 * OcsResult:
 *
 * The #OcsResult structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OcsResult
{
	/*< private >*/
	OpenResult parent_instance;

	OcsResultPrivate *priv;
};

/**
 * OcsResultClass:
 *
 * Class structure for #OcsResult.
 */
struct _OcsResultClass
{
	OpenResultClass parent_class;
};

GType		ocs_result_get_type		(void) G_GNUC_CONST;
OcsResult *	ocs_result_new_with_node	(xmlNodePtr node);

G_END_DECLS

#endif /* __OCS_RESULT_H__ */
