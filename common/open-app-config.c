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
	gchar *data;
	gint len;
	FILE *fp;

	fp = fopen (file_url, "w");
	if (fp == NULL) {
		printf ("Cannot create key file!\n");
		return NULL;
	}

	key_file = g_key_file_new ();
	default_url = "api.opendesktop.org";
	default_type = "ocs";
	g_key_file_set_value (key_file, "Server", "uri", default_url);
	g_key_file_set_value (key_file, "Server", "type", default_type);
	data = g_key_file_to_data (key_file, &len, NULL);
	fwrite (data, 1, len, fp);
	fclose (fp);
	g_free (data);

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

        filename = g_build_filename (g_get_home_dir (), ".open-app-store", "open-app-store.conf", NULL);
	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
                g_free (filename);
                filename = g_build_filename (CONFIGDIR, "open-app-store.conf", NULL);
        }

	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
		gchar *dir, *cmd;

		dir = g_build_filename (g_get_home_dir (), ".open-app-store", NULL);
		if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
			g_mkdir (dir, 0755);
		}
		g_free (dir);

                g_free (filename);
                filename = g_build_filename (g_get_home_dir (), ".open-app-store", "open-app-store.conf", NULL);
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

/*TODO: make the cache to 
 ~/.cache 
*/
gchar *
open_app_config_get_cache_dir (OpenAppConfig *config)
{
	g_return_val_if_fail (config && OPEN_APP_IS_CONFIG (config), NULL);

        GError  *error = NULL;
	gchar *val;
	const gchar *type;

	type = open_app_config_get_server_type (config);
	val = g_key_file_get_value (config->priv->key_file, "Local", "CacheDir", &error);

	if (error) {
		g_error_free (error);
		if (type) {
			val = g_build_filename (g_get_home_dir (), ".open-app-store", "cache", type, NULL);
		} else {
			val = g_build_filename (g_get_home_dir (), ".open-app-store", "cache", "failsave", NULL);
		}
	} else
		val = g_strdup (val);
	
	if (!g_file_test (val, G_FILE_TEST_EXISTS)) {
		g_mkdir_with_parents (val, 0755);
	}

	return val;
}

const gchar *
open_app_config_get_server_uri (OpenAppConfig *config)
{
	g_return_val_if_fail (config->priv->key_file != NULL, NULL);

        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Server", "uri", &error);

	if (error) {
		g_warning ("Failed to load server uri %s", error->message);
		g_error_free (error);
		return NULL;
	}

	return (const gchar *) val;
}

const gchar *
open_app_config_get_server_type (OpenAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Server", "type", &error);

	if (error) {
		g_warning ("Failed to load server type %s", error->message);
		g_error_free (error);
		return NULL;
	}

	return (const gchar *) val;
}

const gchar *
open_app_config_get_username (OpenAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Server", "username", &error);

	if (error) {
		g_warning ("Failed to load server username %s", error->message);
		g_error_free (error);
		return NULL;
	}

	return (const gchar *) val;
}

const gchar *
open_app_config_get_password (OpenAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Server", "password", &error);

	if (error) {
		g_warning ("Failed to load server password %s", error->message);
		g_error_free (error);
		return NULL;
	}

	return (const gchar *) val;
}


