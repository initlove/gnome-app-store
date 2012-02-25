/* open-app-utils.c -

   Copyright 2011

   The Gnome appitem lib is free software; you can redescribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appitem lib is descributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@gmail.com>
*/

#include <libsoup/soup.h>
#include <libsoup/soup-address.h>
#include <libsoup/soup-auth-domain-basic.h>
#include <libsoup/soup-auth-domain-digest.h>
#include <libsoup/soup-message.h>
#include <libsoup/soup-server.h>
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <glib/gi18n.h>
#include <glib/gregex.h>
#include "open-app-config.h"
#include "open-app-utils.h"

typedef struct _OpenAppRegex {
	gchar *name;
	gchar *pattern;
	GRegexMatchFlags flags;
	gchar *message;
} OpenAppRegex;

#if 0
#define SERVER_DEBUG
#endif

SoupSession *
open_app_soup_session_new (gboolean sync, gchar *cafile)
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

gchar *
open_app_get_md5 (const gchar *str)
{
	gchar *checksum;

	checksum = g_compute_checksum_for_data (G_CHECKSUM_MD5,
					(const guchar *) str,
					strlen (str));

	return checksum;
}

SoupBuffer *
open_app_get_data_by_request (SoupSession *session, const gchar *request)
{
	SoupMessage *msg;
	SoupBuffer *buf;
	const gchar *name;
	const gchar *header;
	const gchar *method;

	g_return_val_if_fail (request != NULL, NULL);

//	g_debug ("open_app_get_data_by_request: %s\n", request);

	buf = NULL;
	method = SOUP_METHOD_GET;
	msg = soup_message_new (method, request);
	soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);
	soup_session_send_message (session, msg);

	name = soup_message_get_uri (msg)->path;

	
	if (SOUP_STATUS_IS_TRANSPORT_ERROR (msg->status_code)) {
		g_debug ("%s: %d %s\n", name, msg->status_code, msg->reason_phrase);
	} else {
#ifdef SERVER_DEBUG
		SoupMessageHeadersIter iter;
		const gchar *hname, *value;
		gchar *path = soup_uri_to_string (soup_message_get_uri (msg), TRUE);

		g_debug ("%s %s HTTP/1.%d\n", method, path,
			soup_message_get_http_version (msg));
		g_free (path);
		soup_message_headers_iter_init (&iter, msg->request_headers);
		while (soup_message_headers_iter_next (&iter, &hname, &value))
			g_debug ("%s: %s\r\n", hname, value);
		g_debug ("\n");

		g_debug ("HTTP/1.%d %d %s\n",
			soup_message_get_http_version (msg),
			msg->status_code, msg->reason_phrase);
		soup_message_headers_iter_init (&iter, msg->response_headers);
		while (soup_message_headers_iter_next (&iter, &hname, &value))
			g_debug ("%s: %s\r\n", hname, value);
		g_debug ("\n");
#endif
	}

	if (SOUP_STATUS_IS_REDIRECTION (msg->status_code)) {
		header = soup_message_headers_get_one (msg->response_headers,
						       "Location");
		if (header) {
			SoupURI *request;
			gchar *request_string;

			g_debug ("   -> %s\n", header);

			request = soup_uri_new_with_base (soup_message_get_uri (msg), header);
			request_string = soup_uri_to_string (request, FALSE);
			buf = open_app_get_data_by_request (session, request_string);
			g_free (request_string);
			soup_uri_free (request);
		}
	} else if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		buf = soup_message_body_flatten (msg->response_body);
	}

	g_object_unref (msg);

	return buf;
}

static gboolean
download_file (const gchar *source, const gchar *dest)
{
	g_return_val_if_fail (source && dest, FALSE);

//	g_debug ("download_file %s to %s\n", source, dest);

	SoupSession *session;
	SoupBuffer *buf;
	gchar *cafile;
	gboolean sync;
	FILE *fp;

	buf = NULL;
	sync = TRUE;
	cafile = NULL;
/*TODO: should we have the static session? */
	session = open_app_soup_session_new (sync, cafile);

	gint retry;
	for (retry = 0; retry <3; retry ++) {
		buf = open_app_get_data_by_request (session, source);
		if (buf)
			break;
		g_debug ("retry %d\n", retry);
	}
	g_object_unref (session);
	if (!buf)
		return FALSE;

	fp = fopen (dest, "w");
	if (fp) {
		fwrite (buf->data, 1, buf->length, fp);
		fclose (fp);
	}

	soup_buffer_free (buf);

	return TRUE;
}

gchar *
open_app_get_local_icon (const gchar *uri, gboolean download)
{
	g_return_val_if_fail (uri != NULL, NULL);

	OpenAppConfig *config;
	gchar *md5;
	gchar *img_dir;
	gchar *local_uri;

	config = open_app_config_new ();
	md5 = open_app_get_md5 (uri);
	img_dir = g_build_filename (g_get_user_cache_dir (), PACKAGE_NAME, "img", NULL);
	local_uri = g_build_filename (img_dir, md5, NULL);

	if (!g_file_test (img_dir, G_FILE_TEST_EXISTS))
		g_mkdir_with_parents (img_dir, 0755);

	if (!g_file_test (local_uri, G_FILE_TEST_EXISTS)) {
		if (download) {
			if (!download_file (uri, (const gchar *)local_uri)) {
				g_free (local_uri);
				local_uri = NULL;
			}
		} else {
			g_free (local_uri);
			local_uri = NULL;
		}
	}

	g_object_unref (config);
	g_free (md5);
	g_free (img_dir);

	return local_uri;
}

/*TODO: FIXME: how to get from server ??!!! */
const gchar **
open_app_get_default_categories ()
{
/*FIXME: 'Other' is the last and special one */
	static const gchar *defaults[] = {
		N_("Accessories"), N_("Games"), N_("Graphics"), N_("Internet"), N_("Office"),
		N_("Programming"), N_("Sound & Video"), N_("System Tools"), N_("Universal Access"),
		N_("Other"), NULL
	};
	return defaults;
}

gchar *
open_app_get_spin_dir (void)
{
	gchar *dir;

	dir = g_build_filename (PIXMAPSDIR, "spin", NULL);

	return dir;
}

/*TODO: make a hash? return const gchar *? */
gchar *
open_app_get_pixmap_uri (const gchar *name)
{
	g_return_val_if_fail (name, NULL);

	gchar *uri;
	gchar *real_name;

	if (g_file_test (name, G_FILE_TEST_EXISTS))
		return g_strdup (name);

	if (strchr (name, '.')) {
		real_name = g_strdup (name);
	} else {
		real_name = g_strconcat (name, ".png", NULL);
	}
	uri = g_build_filename (PIXMAPSDIR, real_name, NULL);

	g_free (real_name);

	return uri;
}

gchar *
open_app_get_ui_uri (const gchar *name)
{
	g_return_val_if_fail (name, NULL);

	gchar *uri;
	gchar *real_name;

	if (strchr (name, '.')) {
		real_name = g_strdup (name);
	} else {
		real_name = g_strconcat (name, ".json", NULL);
	}
	uri = g_build_filename (g_get_user_data_dir (), PACKAGE, real_name, NULL);
	if (!g_file_test (uri, G_FILE_TEST_EXISTS)) {
		g_free (uri);
		uri = g_build_filename (UIDIR, real_name, NULL);
	}

	g_free (real_name);

	return uri;
}

gboolean
open_app_is_compatible_distribution (const gchar *distribution)
{
	GError *error;
	gchar *desc;
	gchar *release;
	gchar *output;
	gchar **infos;
	gchar *p;
	gint i;
	gboolean val;

	g_return_val_if_fail (distribution != NULL, TRUE);

	/*TODO: list all this ? */
	const gchar *special_types [] = {"Source", "Arch", "other", NULL};
	for (i = 0; special_types [i]; i++) {
		if (strcasestr (special_types [i], distribution)) {
			return TRUE;
		}
	}

	val = FALSE;
	desc = NULL;
	release = NULL;
	error = NULL;
	output = NULL;
	g_spawn_command_line_sync ("lsb-release -a",
			&output, NULL, NULL, &error);
	if (error) {
		g_debug ("Error in getting describution info %s.",
				error->message);
		g_error_free (error);
	} else {
		if (output) {
			infos = g_strsplit (output, "\n", -1);
			for (i = 0; infos [i]; i++) {
				p = strchr (infos [i], ':');
				if (!p)
					continue;
				if (strncmp (infos [i], "Description", strlen ("Description")) == 0) {
					desc = g_strstrip (g_strdup (p + 1));
				} else if (strncmp (infos [i], "Release", strlen ("Release")) == 0) {
					release = g_strstrip (g_strdup (p + 1));
				}
			}
			g_strfreev (infos);
			g_free (output);
		}
	}

	/*TODO: what about release? */
	if (strcasestr (desc, distribution)) {
		val = TRUE;
	}

	if (desc)
		g_free (desc);
	if (release)
		g_free (release);

	return val;
}

gboolean
is_blank_text (const gchar *text)
{
	if (!text)
		return TRUE;

 	gint i, len;
		
	len = strlen (text);
	for (i = 0; i < len; i++) {
		if (*(text + i) == '\t' || *(text + i) == ' ') {
			continue;
		} else
			return FALSE;
	}
		
	return TRUE;
}

gboolean
open_app_pattern_match (const gchar *pattern_name, const gchar *content, GError **error)
{

	OpenAppRegex data [] = {
		{"blank", "[\\s]+", G_REGEX_MATCH_ANCHORED, N_("It is not blank")},
		{"notblank", "[^\\s]+", G_REGEX_MATCH_PARTIAL, N_("Should not be blank")},
		{"email", "[a-zA-Z0-9._%-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}", G_REGEX_MATCH_ANCHORED, N_("Enter the valid email address")},
		{"password", "[a-zA-Z0-9]{8,}$", G_REGEX_MATCH_ANCHORED, N_("The password should be >=8 alphanumeric chars")},
		{NULL, NULL, 0, NULL},
	};
	gint i;

	if (!content || !content [0]) {
		g_set_error (error, G_REGEX_ERROR, G_REGEX_ERROR_MATCH,
			  _("The text should not be NULL"));
		return FALSE;
	}

	for (i = 0; data [i].name; i++) {
		if (strcasecmp (pattern_name, data [i].name) == 0) {
			if (g_regex_match_simple (data [i].pattern, content, G_REGEX_OPTIMIZE, data [i].flags)) {
				return TRUE;
			} else {
				g_set_error (error, G_REGEX_ERROR, G_REGEX_ERROR_MATCH,
					  _(data [i].message));
				return FALSE;
			}
		}
	}
			
	g_set_error (error, G_REGEX_ERROR, G_REGEX_ERROR_MATCH,
			  _("Cannot find the <%s> pattern\n"), pattern_name);
	return FALSE;
}
