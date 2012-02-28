/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <stdio.h>
#include <string.h>
#include <glib/gbase64.h>
#include "config.h"
#include "open-app-config.h"

/* TODO maybe we should make it an abstruct */
struct _OpenAppConfigPrivate
{
        GKeyFile *key_file;
};

G_DEFINE_TYPE (OpenAppConfig, open_app_config, G_TYPE_OBJECT)

static GKeyFile *
create_default_key_file (gchar *file_url)
{
	g_return_val_if_fail (file_url != NULL, NULL);

	GKeyFile *key_file;
	const gchar *default_url;
	const gchar *default_type;
	GError *error;
	gchar *content;
	gint len;

	key_file = g_key_file_new ();
	default_url = "api.opendesktop.org";
	default_type = "ocs";
	g_key_file_set_value (key_file, "Server", "uri", default_url);
	g_key_file_set_value (key_file, "Server", "type", default_type);
	content = g_key_file_to_data (key_file, &len, NULL);

	error = NULL;
	if (!g_file_set_contents (file_url, content, len, &error)) {
		g_error ("Error in save user file %s\n", error->message);
		g_error_free (error);
	}
	g_free (content);

	return key_file;	
}

static void
open_app_config_init (OpenAppConfig *config)
{
	OpenAppConfigPrivate *priv;
        GError  *error;
        gchar *filename;

	config->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (config,
	                                                 OPEN_APP_TYPE_CONFIG,
	                                                 OpenAppConfigPrivate);

        filename = g_build_filename (g_get_user_config_dir (), PACKAGE_NAME, "config", NULL);
	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
                g_free (filename);
                filename = g_build_filename (CONFIGDIR, "config", NULL);
        }

	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
		gchar *dir, *cmd;

        	dir = g_build_filename (g_get_user_config_dir (), PACKAGE_NAME, NULL);
		if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
			g_mkdir (dir, 0755);
		}
		g_free (dir);

                g_free (filename);
        	filename = g_build_filename (g_get_user_config_dir (), PACKAGE_NAME, "config", NULL);
		priv->key_file = create_default_key_file (filename);
        } else {
		priv->key_file = g_key_file_new ();
		error = NULL;
        	g_key_file_load_from_file (priv->key_file, filename, G_KEY_FILE_NONE, &error);

		if (error) {
			g_warning ("Failed to load keyfile %s  %s", filename, error->message);
			g_error_free (error);

			g_key_file_free (priv->key_file);
			priv->key_file = NULL;
		}
	}
	g_free (filename);
}

static void
open_app_config_dispose (GObject *object)
{
	G_OBJECT_CLASS (open_app_config_parent_class)->dispose (object);
}

static void
open_app_config_finalize (GObject *object)
{
	OpenAppConfig *config = OPEN_APP_CONFIG (object);
	OpenAppConfigPrivate *priv = config->priv;

	if (priv->key_file)
		g_key_file_free (priv->key_file);

	G_OBJECT_CLASS (open_app_config_parent_class)->finalize (object);
}

static void
open_app_config_class_init (OpenAppConfigClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = open_app_config_dispose;
	object_class->finalize = open_app_config_finalize;
	 
	g_type_class_add_private (object_class, sizeof (OpenAppConfigPrivate));
}

OpenAppConfig *
open_app_config_new (void)
{
	return g_object_new (OPEN_APP_TYPE_CONFIG, NULL);
}

gchar *
open_app_config_get_server_uri (OpenAppConfig *config)
{
	g_return_val_if_fail (config->priv->key_file != NULL, NULL);

	OpenAppConfigPrivate *priv;
        GError  *error;
	gchar *val;

	priv = config->priv;
	error = NULL;
	val = g_key_file_get_value (priv->key_file, "Server", "uri", &error);

	if (error) {
		g_warning ("Failed to load server uri %s", error->message);
		g_error_free (error);
		return NULL;
	}

	if (val && val [0])
		return val;
	else
		return NULL;
}

gchar *
open_app_config_get_server_type (OpenAppConfig *config)
{
	OpenAppConfigPrivate *priv;
        GError  *error;
	gchar *val;

	priv = config->priv;
	error = NULL;
	val = g_key_file_get_value (priv->key_file, "Server", "type", &error);

	if (error) {
		g_warning ("Failed to load server type %s", error->message);
		g_error_free (error);
		return NULL;
	}

	if (val && val [0])
		return val;
	else
		return NULL;
}

gchar *
open_app_config_get_username (OpenAppConfig *config)
{
	OpenAppConfigPrivate *priv;
        GError  *error;
	gchar *val;

	priv = config->priv;
	error = NULL;
	val = g_key_file_get_value (priv->key_file, "Server", "username", &error);
	if (error) {
		g_error_free (error);
		return NULL;
	}

	if (val && val [0])
		return val;
	else
		return NULL;
}

gchar *
open_app_config_get_password (OpenAppConfig *config)
{
	OpenAppConfigPrivate *priv;
        GError  *error;
	gchar *val;

	priv = config->priv;
	error = NULL;
	val = g_key_file_get_value (priv->key_file, "Server", "password", &error);
	if (error) {
		g_error_free (error);
		return NULL;
	}
	if (val && val [0]) {
		gchar *real;
		gint len;

		real = g_base64_decode (val, &len);
		g_free (val);
		return real;
	} else
		return NULL;
}

gboolean
open_app_config_save (OpenAppConfig *config, gchar *username, gchar *password)
{
	OpenAppConfigPrivate *priv;
	GError *error;
	gchar *filename;
	gchar *content;
	gint len;
	gboolean val;

	if (!username)
		return TRUE;
	priv = config->priv;
	g_key_file_set_value (priv->key_file, "Server", "username", username);
	if (password) {
		const gchar *warning;
		gchar *encode;

		encode = g_base64_encode (password, strlen (password));
		g_key_file_set_value (priv->key_file, "Server", "password", encode);
		g_free (encode);

		warning = "The password is base64 encoded, should not save it to local ";
		error = NULL;
		if (!g_key_file_set_comment (priv->key_file, "Server", "password", warning, &error)) {
			g_error ("Error in set comment %s\n", error->message);
			g_error_free (error);
			error = NULL;
		}
	}
	error = NULL;
	content = g_key_file_to_data (priv->key_file, &len, &error);
	if (error) {
		g_error ("Error in get data %s\n", error->message);
		g_error_free (error);
		return FALSE;
	}

        filename = g_build_filename (g_get_user_config_dir (), PACKAGE_NAME, "config", NULL);
	error = NULL;
	if (!g_file_set_contents (filename, content, len, &error)) {
		val = FALSE;
		g_error ("Error in save user file %s\n", error->message);
		g_error_free (error);
	} else {
		val = TRUE;
	}
	g_free (content);
	g_free (filename);

	return val;
}
