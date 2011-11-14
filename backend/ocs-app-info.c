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
 * Lesser General Public License for more full.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Liang chenye <liangchenye@gmail.com>
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>

#include "ocs-app-info.h"

static gchar *ocs_app_props [] = {
	"id",
	"name",
	"version",
	"changed",
	"created",
	"typeid",
	"typename",
	"language",
	"personid",
	"profilepage",
	"downloads",
	"score",
	"description",
	"comments",
	"fans",
	"licensetype",
	"license",
	"preview1",
	"previewpic1",
	"smallpreviewpic1",
	"downloadway1",
	"downloadtype1",
	"downloadprice1",
	"downloadlink1",
	"downloadname1",
	"downloadsize1",
	"downloadgpgsignature1",
	"downloadgpgfingerprint1",
	"downloadtype2",
	"downloadprice2",
	"downloadlink2",
	"downloadname2",
	"downloadgpgsignature2",
	"downloadgpgfingerprint2",
	"downloadtype3",
	"downloadprice3",
	"downloadlink3",
	"downloadname3",
	"downloadgpgsignature3",
	"downloadgpgfingerprint3",
	"detailpage",
	NULL
};

struct _OcsAppInfoPrivate
{
	xmlNodePtr summary;
	/*TODO: full ?*/
	xmlNodePtr full;
};

G_DEFINE_TYPE (OcsAppInfo, ocs_app_info, GNOME_APP_TYPE_INFO)

static void
ocs_app_info_init (OcsAppInfo *info)
{
	OcsAppInfoPrivate *priv;

        
	info->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info,
						TYPE_OCS_APP_INFO,
						OcsAppInfoPrivate);
	priv->summary = NULL;
	priv->full = NULL;
}

static const gchar *
get_backend_type (GnomeAppInfo *app_info)
{
	return "ocs";
}

const gchar **
get_props (GnomeAppInfo *app_info)
{
	return (const gchar **)ocs_app_props;
}

static gboolean
is_summary (const gchar *prop)
{
	gint i;

	for (i = 0; ocs_app_props [i]; i++) {
		if (strcmp (prop, ocs_app_props [i]) == 0)
			return TRUE;
	}

	return FALSE;
}

static gboolean
is_full (const gchar *prop)
{
/*TODO: what is the difference between summary and full in ocs ? */
	return FALSE;
}

/*TODO: Better name?  to make it sounds like http request */
static void
get_full_info (OcsAppInfo *info)
{
}

static const gchar *
get_val (xmlNodePtr content, const gchar *prop)
{
	xmlNodePtr node;
	const char *val;

	val = NULL;
	for (node = content->xmlChildrenNode; node; node = node->next) {
		if (strcmp (node->name, prop) == 0) {
			val = (const char *) xmlNodeGetContent (node);
			break;
		}
	}

	return val;
}

const gchar *
get (GnomeAppInfo *ginfo, const gchar *prop)
{
	OcsAppInfo *info = OCS_APP_INFO (ginfo);
	xmlNodePtr node;
	const gchar *val;

	if (is_summary (prop)) {
		if (!info->priv->summary) {
			printf ("This app is empty!\n");
			return NULL;
		} else {
			val = get_val (info->priv->summary, prop);
			if (!val || strlen (val) < 1)
				return NULL;
		}
	} else if (is_full (prop)) {
		if (!info->priv->full) {
			get_full_info (info);
		}
		val = get_val (info->priv->full, prop);
	} else {
		printf ("Wrong prop name or prop was not support in this 'ocs backend.\n");
		printf ("Use gnome_app_info_get_props to list all the props\n");
		return NULL;
	}
}

static void
ocs_app_info_dispose (GObject *object)
{
	G_OBJECT_CLASS (ocs_app_info_parent_class)->dispose (object);
}

static void
ocs_app_info_finalize (GObject *object)
{
	OcsAppInfo *app_info = OCS_APP_INFO (object);
	OcsAppInfoPrivate *priv = app_info->priv;

	if (priv->summary)
		xmlFreeNode (priv->summary);
	if (priv->full)
		xmlFreeNode (priv->full);

	G_OBJECT_CLASS (ocs_app_info_parent_class)->finalize (object);
}

static void
ocs_app_info_class_init (OcsAppInfoClass *klass)
{
	GnomeAppInfoClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = ocs_app_info_dispose;
	object_class->finalize = ocs_app_info_finalize;

	parent_class = GNOME_APP_INFO_CLASS (klass);

	parent_class->get = get;
	parent_class->get_backend_type = get_backend_type;
	parent_class->get_props = get_props;

	g_type_class_add_private (object_class, sizeof (OcsAppInfoPrivate));
}

OcsAppInfo *
ocs_app_info_new ()
{
        return g_object_new (TYPE_OCS_APP_INFO, NULL);
}

void
ocs_app_info_set_summary (OcsAppInfo *info, xmlNodePtr summary)
{
	OcsAppInfoPrivate *priv = info->priv;

	if (priv->summary)
		xmlFreeNode (priv->summary);
	priv->summary = xmlCopyNode (summary, 1);
}

