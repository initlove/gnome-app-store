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
#include <sys/stat.h>
#include "gnome-app-config.h"

struct _GnomeAppConfigPrivate
{
        GKeyFile *key_file;
};

G_DEFINE_TYPE (GnomeAppConfig, gnome_app_config, G_TYPE_OBJECT)

static void
gnome_app_config_init (GnomeAppConfig *config)
{
	GnomeAppConfigPrivate *priv;
        GError  *error;
        gchar *filename;
        struct stat statbuf;
        int r;

	config->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (config,
	                                                 GNOME_TYPE_APP_CONFIG,
	                                                 GnomeAppConfigPrivate);

        filename = g_build_filename (g_get_home_dir (), ".gnome-app-store", "gnome-app-store.conf", NULL);
        r = g_stat (filename, &statbuf);
        if (r < 0) {
                g_free (filename);
                filename = g_build_filename (CONFIGDIR, "gnome-app-store.conf", NULL);
        }

	priv->key_file = g_key_file_new ();
	error = NULL;
        g_key_file_load_from_file (priv->key_file, filename, G_KEY_FILE_NONE, &error);

	if (error) {
		g_warning ("Failed to load keyfile %s  %s", filename, error->message);
		g_error_free (error);

		g_key_file_free (priv->key_file);
		priv->key_file = NULL;
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
	return g_object_new (GNOME_TYPE_APP_CONFIG, NULL);
}

gchar *
gnome_app_config_get_cache_dir (GnomeAppConfig *config)
{
        GError  *error = NULL;
	gchar *val;

	val = g_key_file_get_value (config->priv->key_file, "Local", "CacheDir", &error);

	if (error) {
		g_error_free (error);
		val = g_build_filename (g_get_home_dir (), ".gnome-app-store", "cache", NULL);
	}
/*FIXME: this is just for test usage*/
	if (!g_file_test (val, G_FILE_TEST_EXISTS)) {
		g_free (val);
		val = g_build_filename (g_get_current_dir (), "../test-data", NULL);
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

	return val;
}
