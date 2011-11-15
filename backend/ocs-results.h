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

#ifndef __OCS_RESULTS_H__
#define __OCS_RESULTS_H__

#include "open-results.h"

G_BEGIN_DECLS

#define TYPE_OCS_RESULTS         (ocs_results_get_type ())
#define OCS_RESULTS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OCS_RESULTS, OcsResults))
#define OCS_RESULTS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OCS_RESULTS, OcsResultsClass))
#define OCS_RESULTS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OCS_RESULTS, OcsResultsClass))
#define IS_OCS_RESULTS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OCS_RESULTS))
#define IS_OCS_RESULTS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OCS_RESULTS))

typedef struct _OcsResults OcsResults;
typedef struct _OcsResultsClass OcsResultsClass;
typedef struct _OcsResultsPrivate OcsResultsPrivate;

/**
 * OcsResults:
 *
 * The #OcsResults structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OcsResults
{
	/*< private >*/
	OpenResults parent_instance;

	OcsResultsPrivate *priv;
};

/**
 * OcsResultsClass:
 *
 * Class structure for #OcsResults.
 */
struct _OcsResultsClass
{
	OpenResultsClass parent_class;
};

GType		ocs_results_get_type		(void) G_GNUC_CONST;
OcsResults *	ocs_results_new			(void);
void		ocs_results_set_meta		(OpenResults *results, xmlNodePtr meta_node);
void		ocs_results_set_data		(OpenResults *results, GList *list);
const gchar *	ocs_results_get_meta		(OpenResults *results, const gchar *prop);
GList *		ocs_results_get_data		(OpenResults *results);
gboolean	ocs_results_get_status		(OpenResults *results);
gint		ocs_results_get_total_items	(OpenResults *results);

G_END_DECLS

#endif /* __OCS_RESULTS_H__ */
