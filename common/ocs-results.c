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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ocs-result.h"
#include "ocs-results.h"

struct _OcsResultsPrivate
{
	xmlNodePtr meta;
	GList *data;
	time_t timestamps;
};

G_DEFINE_TYPE (OcsResults, ocs_results, TYPE_OPEN_RESULTS)

static xmlNodePtr
ocs_find_node (xmlDocPtr doc_ptr, gchar *node_name)
{
        xmlNodePtr root_node, data_node;

        root_node = xmlDocGetRootElement (doc_ptr);

        if (!root_node) {
                printf ("Cannot get the root node!\n");
                return NULL;
        }

        for (data_node = root_node->xmlChildrenNode; data_node; data_node = data_node->next) {
                if (strcmp (data_node->name, node_name) == 0)
                        return data_node;
        }
        return NULL;
}

static void
ocs_results_init (OcsResults *info)
{
	OcsResultsPrivate *priv;

        
	info->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info,
						TYPE_OCS_RESULTS,
						OcsResultsPrivate);
	priv->meta = NULL;
	priv->data = NULL;
	priv->timestamps = 0;
}

static void
ocs_results_dispose (GObject *object)
{
	G_OBJECT_CLASS (ocs_results_parent_class)->dispose (object);
}

static void
ocs_results_finalize (GObject *object)
{
	OcsResults *results = OCS_RESULTS (object);
	OcsResultsPrivate *priv = results->priv;

	if (priv->meta)
		xmlFreeNode (priv->meta);
	if (priv->data)
		g_list_free (priv->data);

	G_OBJECT_CLASS (ocs_results_parent_class)->finalize (object);
}

static void
ocs_results_class_init (OcsResultsClass *klass)
{
	OpenResultsClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = ocs_results_dispose;
	object_class->finalize = ocs_results_finalize;

	parent_class = OPEN_RESULTS_CLASS (klass);

	parent_class->get_meta = ocs_results_get_meta;
	parent_class->get_data = ocs_results_get_data;
	parent_class->get_timestamps = ocs_results_get_timestamps;
	parent_class->get_status = ocs_results_get_status;
	parent_class->get_total_items = ocs_results_get_total_items;

	g_type_class_add_private (object_class, sizeof (OcsResultsPrivate));
}

OcsResults *
ocs_results_new ()
{
	return g_object_new (TYPE_OCS_RESULTS, NULL);
}

xmlDocPtr
ocs_results_get_default_doc ()
{
        static xmlDocPtr doc_ptr = NULL;
        gchar *doc_file;

        if (!doc_ptr) {
/*TODO other dir */
                doc_file = "/home/novell/gnome-app-store/backend/ocs-spec/ocs-services-results.xml";
                doc_ptr = xmlParseFile (doc_file);
                if (!doc_ptr) {
                        g_critical ("Cannot find ocs-results doc in %s\n", doc_file);
                        exit (1);
                }
        }

        return doc_ptr;
}

OpenResults *
ocs_get_results (const gchar *ocs, gint len)
{
	g_return_val_if_fail (ocs, NULL);

	xmlDocPtr doc_ptr;
	OcsResults *results;
	GList *list;
	const gchar *list_node_name;
	xmlNodePtr meta_node, data_node;

	doc_ptr = xmlParseMemory (ocs, len);
	if (!doc_ptr) {
		doc_ptr = xmlRecoverMemory (ocs, len);
		if (!doc_ptr) {
			g_debug ("Cannot parse the document!\n");
			return NULL;
		} else {
			g_debug ("We recover the memory !\n");
		}
	}

	results = NULL;
	meta_node = ocs_find_node (doc_ptr, "meta");
	if (meta_node) {
		results = ocs_results_new ();
		ocs_results_set_meta (OPEN_RESULTS (results), meta_node);
	} else {
		g_debug ("Error in get meta node!\n");
		return NULL;
	}

	data_node = ocs_find_node (doc_ptr, "data");
	if (data_node) {
		list = ocs_result_list_new_with_node (data_node);
		ocs_results_set_data (OPEN_RESULTS (results), list);
	}

	results->priv->timestamps = time (NULL);

	return OPEN_RESULTS (results);
}

void
ocs_results_set_meta (OpenResults *open_results, xmlNodePtr meta_node)
{
	OcsResults *results;
	OcsResultsPrivate *priv;

	results = OCS_RESULTS (open_results);
	priv = results->priv;
	if (priv->meta)
		xmlFreeNode (priv->meta);
	priv->meta = xmlCopyNode (meta_node, 1);
}

void
ocs_results_set_data (OpenResults *open_results, GList *list)
{
	OcsResults *results;
	OcsResultsPrivate *priv;

	results = OCS_RESULTS (open_results);
	priv = results->priv;
	if (priv->data)
		g_list_free (priv->data);
	priv->data = g_list_copy (list);
}

const gchar *
ocs_results_get_meta (OpenResults *open_results, const gchar *prop)
{
	OcsResults *results;
	xmlNodePtr node;
	gchar *content;

	results = OCS_RESULTS (open_results);
	for (node = results->priv->meta->xmlChildrenNode; node; node = node->next) {
		if (strcmp (node->name, prop) == 0) {
			content = xmlNodeGetContent (node);
			if (content && content [0])
				return content;
		}
	}

	return NULL;
}

GList *
ocs_results_get_data (OpenResults *open_results)
{
	OcsResults *results;

	results = OCS_RESULTS (open_results);

	return results->priv->data;
}

time_t
ocs_results_get_timestamps (OpenResults *open_results)
{
	OcsResults *results;

	results = OCS_RESULTS (open_results);

	return results->priv->timestamps;
}

gboolean
ocs_results_get_status (OpenResults *open_results)
{
	g_return_val_if_fail (open_results, FALSE);

	gboolean val;
	const gchar *value;
	
	value = ocs_results_get_meta (open_results, "statuscode");
	if (value && (strcmp (value, "100") == 0))
		return TRUE;

	return FALSE;
}

gint
ocs_results_get_total_items (OpenResults *open_results)
{
	gboolean val;
	const gchar *value;
	
	value = ocs_results_get_meta (open_results, "totalitems");
	if (value)
		return atoi (value);

	return -1;
}
