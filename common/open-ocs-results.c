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
 * Author: David Liang <dliang@novell.com>
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "open-ocs-result.h"
#include "open-ocs-results.h"

struct _OpenOcsResultsPrivate
{
	xmlNodePtr meta;
	GList *data;
	time_t timestamps;
};

G_DEFINE_TYPE (OpenOcsResults, open_ocs_results, TYPE_OPEN_RESULTS)

static xmlNodePtr
open_ocs_find_node (xmlDocPtr doc_ptr, gchar *node_name)
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
open_ocs_results_init (OpenOcsResults *info)
{
	OpenOcsResultsPrivate *priv;

        
	info->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info,
						TYPE_OPEN_OCS_RESULTS,
						OpenOcsResultsPrivate);
	priv->meta = NULL;
	priv->data = NULL;
	priv->timestamps = 0;
}

static void
open_ocs_results_dispose (GObject *object)
{
	G_OBJECT_CLASS (open_ocs_results_parent_class)->dispose (object);
}

static void
open_ocs_results_finalize (GObject *object)
{
	OpenOcsResults *results = OPEN_OCS_RESULTS (object);
	OpenOcsResultsPrivate *priv = results->priv;

	if (priv->meta)
		xmlFreeNode (priv->meta);
	if (priv->data)
		g_list_free (priv->data);

	G_OBJECT_CLASS (open_ocs_results_parent_class)->finalize (object);
}

static void
open_ocs_results_class_init (OpenOcsResultsClass *klass)
{
	OpenResultsClass *parent_class;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = open_ocs_results_dispose;
	object_class->finalize = open_ocs_results_finalize;

	parent_class = OPEN_RESULTS_CLASS (klass);

	parent_class->get_meta = open_ocs_results_get_meta;
	parent_class->get_data = open_ocs_results_get_data;
	parent_class->get_timestamps = open_ocs_results_get_timestamps;
	parent_class->get_status = open_ocs_results_get_status;
	parent_class->get_total_items = open_ocs_results_get_total_items;

	g_type_class_add_private (object_class, sizeof (OpenOcsResultsPrivate));
}

OpenOcsResults *
open_ocs_results_new ()
{
	return g_object_new (TYPE_OPEN_OCS_RESULTS, NULL);
}

xmlDocPtr
open_ocs_results_get_default_doc ()
{
        static xmlDocPtr doc_ptr = NULL;
        gchar *doc_file;

        if (!doc_ptr) {
/*TODO other dir */
                doc_file = "/home/novell/gnome-app-store/backend/open_ocs-spec/open_ocs-services-results.xml";
                doc_ptr = xmlParseFile (doc_file);
                if (!doc_ptr) {
                        g_critical ("Cannot find open_ocs-results doc in %s\n", doc_file);
                        exit (1);
                }
        }

        return doc_ptr;
}

OpenResults *
open_ocs_get_results (const gchar *open_ocs_json, gint len_json)
{
	g_return_val_if_fail (open_ocs_json, NULL);

	xmlDocPtr doc_ptr;
	OpenOcsResults *results;
	GList *list;
	const gchar *list_node_name;
	xmlNodePtr meta_node, data_node;

    gchar *open_ocs;
    gint len;
//TODO: HOTFIX
    open_ocs = json_to_xml (open_ocs_json, &len);

	doc_ptr = xmlParseMemory (open_ocs, len);
    g_free (open_ocs);//HOTFIX
	if (!doc_ptr) {
		doc_ptr = xmlRecoverMemory (open_ocs, len);
		if (!doc_ptr) {
			g_debug ("Cannot parse the document!\n");

			return NULL;
		}
	}
	results = NULL;
	meta_node = open_ocs_find_node (doc_ptr, "meta");
	if (meta_node) {
		results = open_ocs_results_new ();
		open_ocs_results_set_meta (OPEN_RESULTS (results), meta_node);
	} else {
		g_debug ("Error in get meta node!\n");
		xmlFreeDoc (doc_ptr);

		return NULL;
	}

	data_node = open_ocs_find_node (doc_ptr, "data");
	if (data_node) {
		list = open_ocs_result_list_new_with_node (data_node);
		open_ocs_results_set_data (OPEN_RESULTS (results), list);
	}

	results->priv->timestamps = time (NULL);

	xmlFreeDoc (doc_ptr);

	return OPEN_RESULTS (results);
}

void
open_ocs_results_set_meta (OpenResults *open_results, xmlNodePtr meta_node)
{
	OpenOcsResults *results;
	OpenOcsResultsPrivate *priv;

	results = OPEN_OCS_RESULTS (open_results);
	priv = results->priv;
	if (priv->meta)
		xmlFreeNode (priv->meta);
	priv->meta = xmlCopyNode (meta_node, 1);
}

void
open_ocs_results_set_data (OpenResults *open_results, GList *list)
{
	OpenOcsResults *results;
	OpenOcsResultsPrivate *priv;

	results = OPEN_OCS_RESULTS (open_results);
	priv = results->priv;
	if (priv->data)
		g_list_free (priv->data);
	priv->data = g_list_copy (list);
}

const gchar *
open_ocs_results_get_meta (OpenResults *open_results, const gchar *prop)
{
	OpenOcsResults *results;
	xmlNodePtr node;
	gchar *content;

	results = OPEN_OCS_RESULTS (open_results);
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
open_ocs_results_get_data (OpenResults *open_results)
{
	OpenOcsResults *results;

	results = OPEN_OCS_RESULTS (open_results);

	return results->priv->data;
}

time_t
open_ocs_results_get_timestamps (OpenResults *open_results)
{
	OpenOcsResults *results;

	results = OPEN_OCS_RESULTS (open_results);

	return results->priv->timestamps;
}

gboolean
open_ocs_results_get_status (OpenResults *open_results)
{
	g_return_val_if_fail (open_results, FALSE);

	gboolean val;
	const gchar *value;
	
	value = open_ocs_results_get_meta (open_results, "statuscode");
	if (value && (strcmp (value, "100") == 0))
		return TRUE;

	return FALSE;
}

gint
open_ocs_results_get_total_items (OpenResults *open_results)
{
	gboolean val;
	const gchar *value;
	
	value = open_ocs_results_get_meta (open_results, "totalitems");
	if (value)
		return atoi (value);

	return -1;
}
