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

#ifndef __OPEN_OCS_RESULTS_H__
#define __OPEN_OCS_RESULTS_H__

#include "open-results.h"

G_BEGIN_DECLS

#define TYPE_OPEN_OCS_RESULTS         (open_ocs_results_get_type ())
#define OPEN_OCS_RESULTS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OPEN_OCS_RESULTS, OpenOcsResults))
#define OPEN_OCS_RESULTS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OPEN_OCS_RESULTS, OpenOcsResultsClass))
#define OPEN_OCS_RESULTS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OPEN_OCS_RESULTS, OpenOcsResultsClass))
#define IS_OPEN_OCS_RESULTS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OPEN_OCS_RESULTS))
#define IS_OPEN_OCS_RESULTS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OPEN_OCS_RESULTS))

typedef struct _OpenOcsResults OpenOcsResults;
typedef struct _OpenOcsResultsClass OpenOcsResultsClass;
typedef struct _OpenOcsResultsPrivate OpenOcsResultsPrivate;

/**
 * OpenOcsResults:
 *
 * The #OpenOcsResults structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OpenOcsResults
{
	/*< private >*/
	OpenResults parent_instance;

	OpenOcsResultsPrivate *priv;
};

/**
 * OpenOcsResultsClass:
 *
 * Class structure for #OpenOcsResults.
 */
struct _OpenOcsResultsClass
{
	OpenResultsClass parent_class;
};

GType		open_ocs_results_get_type		(void) G_GNUC_CONST;
OpenOcsResults *	open_ocs_results_new			(void);
void		open_ocs_results_set_meta		(OpenResults *results, xmlNodePtr meta_node);
const gchar *	open_ocs_results_get_meta		(OpenResults *results, const gchar *prop);
void		open_ocs_results_set_data		(OpenResults *results, GList *list);
GList *		open_ocs_results_get_data		(OpenResults *results);
OpenResults *	open_ocs_get_results			(const gchar *open_ocs, gint len);
time_t		open_ocs_results_get_timestamps	(OpenResults *results);
gboolean	open_ocs_results_get_status		(OpenResults *results);
gint		open_ocs_results_get_total_items	(OpenResults *results);

G_END_DECLS

#endif /* __OPEN_OCS_RESULTS_H__ */
