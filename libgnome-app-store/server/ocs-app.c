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
#include <libsoup/soup-session.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gnome-app-utils.h"
#include "gnome-app-item.h"
#include "gnome-app-config.h"
#include "ocs-app.h"

typedef enum _OCS_KEY_WORDS {
        OCS_ID = 0,
        OCS_NAME,
	OCS_VERSION,
	OCS_TYPEID,
	OCS_TYPENAME,
	OCS_LANGUAGE,
	OCS_PERSONID,
	OCS_PROFILEPAGE,
	OCS_CREATED_TIME,
	OCS_CHANGED_TIME,
	OCS_DOWNLOADS,
	OCS_SCORE,
	OCS_DESCRIPTION,
        OCS_SUMMARY,
	OCS_FEEDBACKURL,
	OCS_CHANGELOG,
	OCS_HOMEPAGE,
	OCS_HOMEPAGETYPE,
	OCS_LICENSETYPE,
	OCS_LICENSE,
	OCS_DONATIONPAGE,
	OCS_COMMENTS,
	OCS_COMMENTSPAGE,
	OCS_FANS,
	OCS_FANSPAGE,
	OCS_KNOWLEDGEBASEENTRIES,
	OCS_KNOWLEDGETBASEPAGE,
	OCS_DEPEND,
	OCS_PREVIEW1,
	OCS_PREVIEWPIC1,
	OCS_SMALLPREVIEWPIC1,
	OCS_DETAILPAGE,
	OCS_DOWNLOADTYPE1,
	OCS_DOWNLOADPRICE1,
	OCS_DOWNLOADLINK1,
	OCS_DOWNLOADNAME1,
	OCS_DOWNLOADGPGFINGERPRINT1,
	OCS_DOWNLOADGPGSIGNATURE1,
	OCS_DOWNLOADPACKAGENAME1,
	OCS_DOWNLOADREPOSITORY1,
	OCS_DOWNLOADSIZE1,
        OCS_LAST,
} OCS_KEY_WORDS;

static OCS_KEY_WORDS
get_type_from_name (gchar *name)
{
	gchar *key_words [] = {"id", "name", "version", "typeid", "typename", 
				"language", "personid", "profilepage", "created", "changed",
				"downloads", "score", "description", "summary", "feedbackurl",
				"changelog", "homepage", "homepagetype", "licensetype", "license",
				"donationpage", "comments", "commentspage", "fans", "fanspage",
				"knowledgebaseentries", "knowledgebasepage", "depend", 
				"preview1", "previewpic1", "smallpreviewpic1", "detailpage",
				"downloadtype1", "downloadprice1", "downloadlink1", "downloadname1",
				"downloadgpgfingerprint1", "downloadgpgsignature1", "downloadpackagename1",
				"downloadrepository1", "downloadsize1", NULL};
	int i;
	
	for (i = 0; i < OCS_LAST; i++) {
		if (strcmp (name, key_words [i]) == 0) {
			return i;
		}
	}

	return OCS_LAST;
}

static void
download_file (const gchar *source, const gchar *dest)
{
	SoupSession *session;
	gchar *cafile;
	gboolean sync;
	gchar *data;
	gint len;
	FILE *fp;

	sync = TRUE;
	cafile = NULL;
	session = gnome_app_soup_session_new (sync, cafile);
	data = gnome_app_get_data_from_url (session, source, &len);

	fp = fopen (dest, "w");
	if (fp) {
		fwrite (data, 1, len, fp);
		fclose (fp);
	}

	g_free (data);
	g_object_unref (session);
}

static gchar *
get_md5 (const gchar *url)
{
	gchar *checksum;

	checksum = g_compute_checksum_for_data (G_CHECKSUM_MD5,
                                      (const guchar *) url,
                                      strlen (url));

	return checksum;
}

static gchar *
get_local_url (const gchar *url)
{
	GnomeAppConfig *config;
	gchar *cache_dir;
	gchar *md5;
	gchar *local_url;
/* FIXME: maybe we should use some functions like gnome_app_config_get_default () */ 
	config = gnome_app_config_new ();
	cache_dir = gnome_app_config_get_cache_dir (config);
	md5 = get_md5 (url);
	local_url = g_build_filename (cache_dir, md5, NULL);
	if (!g_file_test (local_url, G_FILE_TEST_EXISTS)) {
		download_file (url, (const gchar *)local_url);
	}
	g_free (md5);
	g_free (cache_dir);
	g_object_unref (config);

	return local_url;
}

static gchar *
get_local_icon_url (GnomeAppItem *item)
{
	const gchar *icon;
	gchar *local_url;

	icon = gnome_app_item_get_icon_name (item);
	local_url = get_local_url (icon);

	return local_url;
}

static gchar *
get_local_screenshot_url (GnomeAppItem *item)
{
	const gchar *screenshot;
	gchar *local_url;

	screenshot = gnome_app_item_get_screenshot (item);
	local_url = get_local_url (screenshot);

	return local_url;
}

/*FIXME: it is 'full' content, the summary one could be used at the first glance */
GnomeAppItem *
parse_app (OcsServer *ocs_server, xmlNodePtr data_node)
{
	OcsServerPrivate *priv = ocs_server->priv;
        xmlNodePtr app_node, node;
        GnomeAppItemClass *class;
	GnomeAppItem *item = NULL;
	OCS_KEY_WORDS type;
	gchar *content;
	gchar *str;
	glong counts;
	gint score;

	for (app_node = data_node->xmlChildrenNode; app_node; app_node = app_node->next) {
		if (strcmp (app_node->name, "content") == 0) {
			for (node = node->xmlChildrenNode; node; node = node->next) {
				type = get_type_from_name ((gchar *)node->name);
				content = xmlNodeGetContent (node);
				switch (type) {
        				case OCS_ID:
						item = gnome_app_item_new ();
						g_object_set (G_OBJECT (item), (gchar *)node->name, content, NULL);
						break;
					case OCS_NAME:
						g_object_set (G_OBJECT (item), (gchar *)node->name, content, NULL);
						break;
					case OCS_VERSION:
						/*not implement*/
						break;
					case OCS_TYPEID:
						/*not implement*/
						break;
					case OCS_TYPENAME:
						/*FIXME: not sure if there were more than two OCS_TYPENAME, if so, we should add it */
						str = g_strdup_printf ("%s;", content);
						g_object_set (G_OBJECT (item), "categories", str, NULL);
						g_free (str);
						break;
					case OCS_LANGUAGE:
					case OCS_PERSONID:
					case OCS_PROFILEPAGE:
					case OCS_CREATED_TIME:
					case OCS_CHANGED_TIME:
						/*not implement*/
						break;
					case OCS_DOWNLOADS:
						counts = atol (content);
						g_object_set (G_OBJECT (item), "download-counts", counts, NULL);
						break;
					case OCS_SCORE:
						score = atoi (content);
						g_object_set (G_OBJECT (item), "score", score, NULL);
						break;
					case OCS_DESCRIPTION:
						g_object_set (G_OBJECT (item), (gchar *)node->name, content, NULL);
						break;
					case OCS_SUMMARY:
						g_object_set (G_OBJECT (item), (gchar *)node->name, content, NULL);
						break;
					case OCS_FEEDBACKURL:
					case OCS_CHANGELOG:
					case OCS_HOMEPAGE:
					case OCS_HOMEPAGETYPE:
					case OCS_LICENSETYPE:
						/*not implement*/
						break;
					case OCS_LICENSE:
						g_object_set (G_OBJECT (item), (gchar *)node->name, content, NULL);
						break;
					case OCS_DONATIONPAGE:
						/*not implement*/
						break;
					case OCS_COMMENTS:
						counts = atol (content);
						g_object_set (G_OBJECT (item), "comment-counts", counts, NULL);
						break;
					case OCS_COMMENTSPAGE:
						break;
					case OCS_FANS:
					case OCS_FANSPAGE:
					case OCS_KNOWLEDGEBASEENTRIES:
					case OCS_KNOWLEDGETBASEPAGE:
					case OCS_DEPEND:
						/*not implement*/
						break;
					case OCS_PREVIEW1:
						/*not implement*/
						break;
					case OCS_PREVIEWPIC1:
						g_object_set (G_OBJECT (item), "screenshot", content, NULL);
						class = GNOME_APP_ITEM_GET_CLASS (item);
						class->get_local_screenshot_url = get_local_screenshot_url;
						break;
					case OCS_SMALLPREVIEWPIC1:
						g_object_set (G_OBJECT (item), "icon", content, NULL);
						class = GNOME_APP_ITEM_GET_CLASS (item);
						class->get_local_icon_url = get_local_icon_url;
						break;
					case OCS_DETAILPAGE:
					case OCS_DOWNLOADTYPE1:
					case OCS_DOWNLOADPRICE1:
					case OCS_DOWNLOADLINK1:
					case OCS_DOWNLOADNAME1:
					case OCS_DOWNLOADGPGFINGERPRINT1:
					case OCS_DOWNLOADGPGSIGNATURE1:
					case OCS_DOWNLOADPACKAGENAME1:
					case OCS_DOWNLOADREPOSITORY1:
					case OCS_DOWNLOADSIZE1:
						/*not implement*/
						break;
					default:
						break;
				}
			}
		}
	}

	return item;
}

