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
#include "gnome-app-config.h"

struct _GnomeAppConfigPrivate
{
        GKeyFile *key_file;
};

G_DEFINE_TYPE (GnomeAppConfig, gnome_app_config, G_TYPE_OBJECT)

static GKeyFile *
create_default_key_file (gchar *file_url)
{
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
gnome_app_config_init (GnomeAppConfig *config)
{
	GnomeAppConfigPrivate *priv;
        GError  *error;
        gchar *filename;

	config->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (config,
	                                                 GNOME_APP_TYPE_CONFIG,
	                                                 GnomeAppConfigPrivate);

        filename = g_build_filename (g_get_home_dir (), ".gnome-app-store", "gnome-app-store.conf", NULL);
	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
                g_free (filename);
                filename = g_build_filename (CONFIGDIR, "gnome-app-store.conf", NULL);
        }

	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
		gchar *dir, *cmd;

		dir = g_build_filename (g_get_home_dir (), ".gnome-app-store", NULL);
		if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
			g_mkdir (dir, 0755);
		}
		g_free (dir);

                g_free (filename);
                filename = g_build_filename (g_get_home_dir (), ".gnome-app-store", "gnome-app-store.conf", NULL);
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
gnome_app_config_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_config_parent_class)->dispose (object);
}

static void
gnome_app_config_finalize (GObject *object)
{
	GnomeAppConfig *config = GNOME_APP_CONFIG (object);
	GnomeAppConfigPrivate *priv = config->priv;

	if (priv->key_file)
		g_key_file_free (priv->key_file);

	G_OBJECT_CLASS (gnome_app_config_parent_class)->finalize (object);
}

static void
gnome_app_config_class_init (GnomeAppConfigClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_config_dispose;
	object_class->finalize = gnome_app_config_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppConfigPrivate));
}

GnomeAppConfig *
gnome_app_config_new (void)
{
	return g_object_new (GNOME_APP_TYPE_CONFIG, NULL);
}

gchar *
gnome_app_config_get_cache_dir (GnomeAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;
	gchar *type;

	type = gnome_app_config_get_server_type (config);
	val = g_key_file_get_value (config->priv->key_file, "Local", "CacheDir", &error);

	if (error) {
		g_error_free (error);
		/*FIXME: might do better? */
		if (strcmp (type, "ocs") == 0)
			val = g_build_filename (g_get_home_dir (), ".gnome-app-store", "cache", type, NULL);
		else
			val = g_build_filename (g_get_home_dir (), ".gnome-app-store", "cache", NULL);
	} else
		val = g_strdup (val);
	
	if (!g_file_test (val, G_FILE_TEST_EXISTS)) {
		g_mkdir_with_parents (val, 0755);
	}

	return val;
}

gchar *
gnome_app_config_get_server_uri (GnomeAppConfig *config)
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

	return g_strdup (val);
}

gchar *
gnome_app_config_get_server_type (GnomeAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Server", "type", &error);

	if (error) {
		g_warning ("Failed to load server type %s", error->message);
		g_error_free (error);
		return NULL;
	}

	return g_strdup (val);
}

