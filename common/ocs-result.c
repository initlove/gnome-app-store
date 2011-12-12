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

#include "ocs-result.h"

struct _OcsResultPrivate
{
	xmlNodePtr data;
};

G_DEFINE_TYPE (OcsResult, ocs_result, TYPE_OPEN_RESULT)

static void
ocs_result_init (OcsResult *info)
{
	OcsResultPrivate *priv;

        
	info->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info,
						TYPE_OCS_RESULT,
						OcsResultPrivate);
	priv->data = NULL;
}

static const gchar *
get_backend_type (OpenResult *result)
{
	return "ocs";
}

gchar **
get_props (OpenResult *result)
{
	OcsResult *ocs_result;
        GPtrArray  *props;
	xmlNodePtr node;
	gchar *name;

        props = g_ptr_array_new ();

	ocs_result = OCS_RESULT (result);
	for (node = ocs_result->priv->data->xmlChildrenNode; node; node = node->next) {
		name = (gchar *)node->name;
		if (name && name [0])
			g_ptr_array_add (props, g_strdup (name));
        }

        g_ptr_array_add (props, NULL);

        return (gchar **) g_ptr_array_free (props, FALSE);
}

static const gchar *
get (OpenResult *result, const gchar *prop)
{
	OcsResult *ocs_result;
	xmlNodePtr node;
	const char *val;

	ocs_result = OCS_RESULT (result);
	val = NULL;

	for (node = ocs_result->priv->data->xmlChildrenNode; node; node = node->next) {
		if (node->type == XML_TEXT_NODE)
			continue;
		if (strcmp (node->name, prop) == 0) {
			val = (const char *) xmlNodeGetContent (node);
			break;
		}
	}

	return val;
}

static GList *
get_child (OpenResult *result)
{
	OcsResult *ocs_result;
	xmlNodePtr node;	
	const gchar *val;
	const gchar *content;
	GList *list;

	val = NULL;
	ocs_result = OCS_RESULT (result);

	for (node = ocs_result->priv->data->xmlChildrenNode; node; node = node->next) {
		if (node->type == XML_TEXT_NODE)
			continue;
		/*TODO: lazy hack, assume 'childcount' comes before 'children' */
		if (strcmp (node->name, "childcount") == 0) {
		        content = xmlNodeGetContent (node);
			if (strcmp (content, "0") == 0) {
				return NULL;
			}
		}
		if (strcmp (node->name, "children") == 0) {
			list = ocs_result_list_new_with_node (node);

			return list;
		}
	}

	return NULL;
}

static void
ocs_result_dispose (GObject *object)
{
	G_OBJECT_CLASS (ocs_result_parent_class)->dispose (object);
}

static void
ocs_result_finalize (GObject *object)
{
	OcsResult *result = OCS_RESULT (object);
	OcsResultPrivate *priv = result->priv;

	if (priv->data)
		xmlFreeNode (priv->data);

	G_OBJECT_CLASS (ocs_result_parent_class)->finalize (object);
}

static void
ocs_result_class_init (OcsResultClass *klass)
{
	OpenResultClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = ocs_result_dispose;
	object_class->finalize = ocs_result_finalize;

	parent_class = OPEN_RESULT_CLASS (klass);

	parent_class->get = get;
	parent_class->get_backend_type = get_backend_type;
	parent_class->get_props = get_props;
	parent_class->get_child = get_child;

	g_type_class_add_private (object_class, sizeof (OcsResultPrivate));
}

OcsResult *
ocs_result_new_with_node (xmlNodePtr node)
{
	g_return_val_if_fail (node, NULL);

	OcsResult *result;
	OcsResultPrivate *priv;

	result = g_object_new (TYPE_OCS_RESULT, NULL);
	priv = result->priv;
	priv->data = xmlCopyNode (node, 1);

	return result;
}

GList *
ocs_result_list_new_with_node (xmlNodePtr data_node)
{
        xmlNodePtr node;
        OcsResult *result;
        GList *list = NULL;
        gchar *name;

        for (node = data_node->xmlChildrenNode; node; node = node->next) {
                if (node->type == XML_TEXT_NODE)
	                continue;
	        name = (gchar *)node->name;
	        result = ocs_result_new_with_node (node);
	        list = g_list_prepend (list, result);
	}
	if (list)
	        list = g_list_reverse (list);

	return list;
}
