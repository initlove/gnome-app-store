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
#include "gnome-app-config.h"
#include "gnome-app-server.h"

struct _GnomeAppServerPrivate
{
	gchar *server_uri;
};

G_DEFINE_TYPE (GnomeAppServer, gnome_app_server, G_TYPE_OBJECT)

static void
gnome_app_server_init (GnomeAppServer *server)
{
	GnomeAppServerPrivate *priv;

	server->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (server,
	                                                 GNOME_TYPE_APP_SERVER,
	                                                 GnomeAppServerPrivate);

	GnomeAppConfig *conf;
	conf = gnome_app_config_new ();
	priv->server_uri = gnome_app_config_get_server_uri (conf);
	g_object_unref (conf);
}

static void
gnome_app_server_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_server_parent_class)->dispose (object);
}

static void
gnome_app_server_finalize (GObject *object)
{
	GnomeAppServer *server = GNOME_APP_SERVER (object);
	GnomeAppServerPrivate *priv = server->priv;

	g_free (priv->server_uri);

	G_OBJECT_CLASS (gnome_app_server_parent_class)->finalize (object);
}

static void
gnome_app_server_class_init (GnomeAppServerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_server_dispose;
	object_class->finalize = gnome_app_server_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppServerPrivate));
}

GnomeAppServer *
gnome_app_server_new (void)
{
	return g_object_new (GNOME_TYPE_APP_SERVER, NULL);
}

gboolean
gnome_app_server_sync (GnomeAppServer *server)
{
	GnomeAppServerPrivate *priv = server->priv;

	/*FIXME:  works made here.... 
		put the data to CacheDir ..
	*/

	return TRUE;
}

