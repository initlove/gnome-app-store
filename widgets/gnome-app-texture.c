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
#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <cogl/cogl.h>
#include <cogl-pango/cogl-pango.h>

#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-texture.h"

struct _GnomeAppTexturePrivate
{
	gchar *url;
	gint type;
	gint status;
	gint pos;

	/* col, row, is for mult-texture */
	gint col;
	gint row;

};

/* Properties */
enum
{
	PROP_0,
	PROP_URL,
	PROP_TEXTURE_TYPE,
	PROP_LAST
};

enum {
	NONE_TEXTURE,
	MULT_TEXTURE,	/* icons in one file */
	DIR_TEXTURE,	/* icons in one dir */
	GIF_TEXTURE	/* gif texture */
};

enum {
	TEXTURE_STATUS_INIT,
	TEXTURE_STATUS_START,
	TEXTURE_STATUS_STOP
};

G_DEFINE_TYPE (GnomeAppTexture, gnome_app_texture, CLUTTER_TYPE_TEXTURE)

static void
gnome_app_texture_init (GnomeAppTexture *texture)
{
	GnomeAppTexturePrivate *priv;

	texture->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (texture,
	                                                 GNOME_APP_TYPE_TEXTURE,
	                                                 GnomeAppTexturePrivate);

	priv->type = NONE_TEXTURE;
	priv->url = NULL;
	priv->status = TEXTURE_STATUS_INIT;
}

static void
gnome_app_texture_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppTexture *texture;
	GnomeAppTexturePrivate *priv;
	ClutterActor *actor;
	const gchar *str;

	texture = GNOME_APP_TEXTURE (object);
	priv = texture->priv;
	switch (prop_id)
	{
		case PROP_URL:
			if (priv->url)
				g_free (priv->url);
			priv->url = g_strdup (g_value_get_string (value));
			break;
		case PROP_TEXTURE_TYPE:
			str = g_value_get_string (value);
			if (strcmp (str, "dir") == 0) {
				priv->type = DIR_TEXTURE;
				priv->pos = 1;
			} else if (strcmp (str, "mult") == 0)
				priv->type = MULT_TEXTURE;
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_texture_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppTexture *texture;
	GnomeAppTexturePrivate *priv;
	ClutterActor *actor;

	texture = GNOME_APP_TEXTURE (object);
	priv = texture->priv;
	switch (prop_id)
	{
		case PROP_URL:
			g_value_set_string (value, priv->url);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_texture_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_texture_parent_class)->dispose (object);
}

static void
gnome_app_texture_finalize (GObject *object)
{
	GnomeAppTexture *texture = GNOME_APP_TEXTURE (object);
	GnomeAppTexturePrivate *priv = texture->priv;

	if (priv->url)
		g_free (priv->url);

	G_OBJECT_CLASS (gnome_app_texture_parent_class)->finalize (object);
}

static void
gnome_app_texture_class_init (GnomeAppTextureClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  
        object_class->set_property = gnome_app_texture_set_property;
	object_class->get_property = gnome_app_texture_get_property;
	object_class->dispose = gnome_app_texture_dispose;
	object_class->finalize = gnome_app_texture_finalize;

	g_object_class_install_property (object_class,
			PROP_URL,
			g_param_spec_string ("url",
				"File/Dir url",
				"File/Dir url",
				NULL,
				G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
			PROP_TEXTURE_TYPE,
			g_param_spec_string ("texture-type",
				"Texture Type",
				"Texture Type",
				NULL,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppTexturePrivate));
}

GnomeAppTexture *
gnome_app_texture_new ()
{
	GnomeAppTexture *texture;

	texture = g_object_new (GNOME_APP_TYPE_TEXTURE, NULL);

	return texture;
}

GnomeAppTexture *
gnome_app_dtexture_new_from_dir (gchar *dir)
{
	g_return_if_fail (dir);

	GnomeAppTexture *texture;
	GnomeAppTexturePrivate *priv;

        if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
		g_error ("Cannot find the spin dir %s\n", dir);
		return NULL;
	}

	texture = g_object_new (GNOME_APP_TYPE_TEXTURE, NULL);
	priv = texture->priv;
	priv->type = DIR_TEXTURE;
	priv->url = g_strdup (dir);

	return texture;
}

GnomeAppTexture *
gnome_app_mtexture_new_from_file (gchar *url, gint col, gint row)
{
	g_return_if_fail (url);

	GnomeAppTexture *texture;
	GnomeAppTexturePrivate *priv;
	GError *error;

	texture = g_object_new (GNOME_APP_TYPE_TEXTURE, NULL);
	priv = texture->priv;
	priv->type = MULT_TEXTURE;
	priv->col = col;
	priv->row = row;
	priv->url = g_strdup (url);

	return texture;
}

static gboolean
texture_timeout (GnomeAppTexture *texture)
{
	GnomeAppTexturePrivate *priv;
	gchar *filename;

	priv = texture->priv;

	if (priv->status == TEXTURE_STATUS_STOP)
		return FALSE;

	filename = g_strdup_printf ("%s/%d.png", priv->url, priv->pos);
	if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
		g_free (filename);
		priv->pos = 1;
		filename = g_strdup_printf ("%s/%d.png", priv->url, priv->pos);
	}
	clutter_texture_set_from_file (CLUTTER_TEXTURE (texture), filename, NULL);
	g_free (filename);
	priv->pos ++;

	return TRUE;
}

void
gnome_app_texture_start (GnomeAppTexture *texture)
{
	GnomeAppTexturePrivate *priv;

	priv = texture->priv;
	priv->status = TEXTURE_STATUS_START;
	g_timeout_add (50, (GSourceFunc) texture_timeout, texture);
	clutter_actor_show (CLUTTER_ACTOR (texture));
}

void
gnome_app_texture_stop (GnomeAppTexture *texture)
{
	GnomeAppTexturePrivate *priv;

	priv = texture->priv;
	if (priv->status = TEXTURE_STATUS_START)
		priv->status = TEXTURE_STATUS_STOP;
	clutter_actor_hide (CLUTTER_ACTOR (texture));
}
