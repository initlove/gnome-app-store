/* gnome-app-utils.c -

   Copyright 2011

   The Gnome appitem lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appitem lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/
#ifndef GMENU_I_KNOW_THIS_IS_UNSTABLE
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <gmenu-tree.h>
#ifdef HAVE_GNOME
#include <libsoup/soup-gnome.h>
#else
#include <libsoup/soup.h>
#endif
#include <libsoup/soup-address.h>
#include <libsoup/soup-auth-domain-basic.h>
#include <libsoup/soup-auth-domain-digest.h>
#include <libsoup/soup-message.h>
#include <libsoup/soup-server.h>
#include <stdio.h>
#include <string.h>
#include "gnome-app-utils.h"

#define server_debug TRUE

static GList *local_categories = NULL;

static void
gather_entries_recurse (GMenuTreeDirectory *trunk)
{
	GSList *contents;
	GSList *iter;
	contents = gmenu_tree_directory_get_contents (trunk);

	for (iter = contents; iter; iter = iter->next) {
		GMenuTreeItem *item = iter->data;
		GMenuTreeDirectory *dir;
		const gchar *name;

		switch (gmenu_tree_item_get_type (item)) {
			case GMENU_TREE_ITEM_DIRECTORY:
			        dir = GMENU_TREE_DIRECTORY (item);
				name = gmenu_tree_directory_get_name (dir);
				local_categories = g_list_prepend (local_categories, g_strdup (name));
				/*FIXME: if the return value of get_local_categories change to a tree on day.
				  we should enable this ...
			        gather_entries_recurse (dir);
				*/
			        break;
			default:
			        break;
		}
		gmenu_tree_item_unref (item);
	}
	g_slist_free (contents);
}

const GList *
gnome_app_get_local_categories ()
{
	if (local_categories)
		return local_categories;

	GMenuTree *apps_tree;
	GMenuTreeDirectory *trunk;

	apps_tree = gmenu_tree_lookup ("applications.menu", GMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
	trunk = gmenu_tree_get_root_directory (apps_tree);
	gather_entries_recurse (trunk);

	gmenu_tree_item_unref (trunk);
	gmenu_tree_unref (apps_tree);

	return (const GList *) local_categories;
}

SoupSession *
gnome_app_soup_session_new (gboolean sync, gchar *cafile)
{
	SoupSession *session;

	if (sync) {
		session = soup_session_sync_new_with_options (
			SOUP_SESSION_SSL_CA_FILE, cafile,
#ifdef HAVE_GNOME
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_GNOME_FEATURES_2_26,
#endif
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_DECODER,
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_COOKIE_JAR,
			SOUP_SESSION_USER_AGENT, "get ",
			SOUP_SESSION_ACCEPT_LANGUAGE_AUTO, TRUE,
			NULL);
	} else {
		session = soup_session_async_new_with_options (
			SOUP_SESSION_SSL_CA_FILE, cafile,
#ifdef HAVE_GNOME
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_GNOME_FEATURES_2_26,
#endif
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_DECODER,
			SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_COOKIE_JAR,
			SOUP_SESSION_USER_AGENT, "get ",
			SOUP_SESSION_ACCEPT_LANGUAGE_AUTO, TRUE,
			NULL);
	}

	return session;
}

SoupBuffer *
gnome_app_get_data_from_url (SoupSession *session, const char *url)
{
	const char *name;
	SoupMessage *msg;
	const char *header;
	const char *method;
	SoupBuffer *buf = NULL;

	printf ("Resolve: %s\n", url);

	method = SOUP_METHOD_GET;
	msg = soup_message_new (method, url);
	soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);

	soup_session_send_message (session, msg);

	name = soup_message_get_uri (msg)->path;

	if (server_debug) {
		SoupMessageHeadersIter iter;
		const char *hname, *value;
		char *path = soup_uri_to_string (soup_message_get_uri (msg), TRUE);

		printf ("%s %s HTTP/1.%d\n", method, path,
			soup_message_get_http_version (msg));
		g_free (path);
		soup_message_headers_iter_init (&iter, msg->request_headers);
		while (soup_message_headers_iter_next (&iter, &hname, &value))
			printf ("%s: %s\r\n", hname, value);
		printf ("\n");

		printf ("HTTP/1.%d %d %s\n",
			soup_message_get_http_version (msg),
			msg->status_code, msg->reason_phrase);
		soup_message_headers_iter_init (&iter, msg->response_headers);
		while (soup_message_headers_iter_next (&iter, &hname, &value))
			printf ("%s: %s\r\n", hname, value);
		printf ("\n");
	} else if (SOUP_STATUS_IS_TRANSPORT_ERROR (msg->status_code))
		printf ("%s: %d %s\n", name, msg->status_code, msg->reason_phrase);

	if (SOUP_STATUS_IS_REDIRECTION (msg->status_code)) {
		header = soup_message_headers_get_one (msg->response_headers,
			                               "Location");
		if (header) {
			SoupURI *uri;
			char *uri_string;

			if (!server_debug)
			        printf ("  -> %s\n", header);

			uri = soup_uri_new_with_base (soup_message_get_uri (msg), header);
			uri_string = soup_uri_to_string (uri, FALSE);
			buf = gnome_app_get_data_from_url (session, uri_string);
			g_free (uri_string);
			soup_uri_free (uri);
		}
	} else if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		buf = soup_message_body_get_chunk (msg->response_body, msg->response_body->length);
	}

	g_object_unref (msg);

	return buf;
}

