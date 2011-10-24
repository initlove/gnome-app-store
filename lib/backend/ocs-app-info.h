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

#ifndef __OCS_APP_INFO_H__
#define __OCS_APP_INFO_H__

#include "gnome-app-info.h"

G_BEGIN_DECLS

#define TYPE_OCS_APP_INFO         (ocs_app_info_get_type ())
#define OCS_APP_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_OCS_APP_INFO, OcsAppInfo))
#define OCS_APP_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_OCS_APP_INFO, OcsAppInfoClass))
#define OCS_APP_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_OCS_APP_INFO, OcsAppInfoClass))
#define IS_OCS_APP_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_OCS_APP_INFO))
#define IS_OCS_APP_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_OCS_APP_INFO))

typedef struct _OcsAppInfo OcsAppInfo;
typedef struct _OcsAppInfoClass OcsAppInfoClass;
typedef struct _OcsAppInfoPrivate OcsAppInfoPrivate;

/**
 * OcsAppInfo:
 *
 * The #OcsAppInfo structure contains only private data and should
 * only be accessed using the provided API.
 */
struct _OcsAppInfo
{
	/*< private >*/
	GnomeAppInfo parent_instance;

	OcsAppInfoPrivate *priv;
};

/**
 * OcsAppInfoClass:
 *
 * Class structure for #OcsAppInfo.
 */
struct _OcsAppInfoClass
{
	GnomeAppInfoClass parent_class;
};

GType		ocs_app_info_get_type		(void) G_GNUC_CONST;
OcsAppInfo *	ocs_app_info_new		(void);
void		ocs_app_info_set_summary	(OcsAppInfo *info, xmlNodePtr summary);

G_END_DECLS

#endif /* __OCS_APP_INFO_H__ */
