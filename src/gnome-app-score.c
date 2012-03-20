/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Sscorete 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <stdlib.h>
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-score.h"

struct _GnomeAppScorePrivate
{
	ClutterActor *star [5];
	gint score;
};

/* Properties */
enum
{
	PROP_0,
	PROP_SCORE,
	PROP_LAST,
};

G_DEFINE_TYPE (GnomeAppScore, gnome_app_score, CLUTTER_TYPE_GROUP)

static void
score_load (GnomeAppScore *score)
{
	GnomeAppScorePrivate *priv;
	gchar *filename;
	gint i, count;

	priv = score->priv;
	/* the score is between 0 -- 100 */
	count = priv->score / 20;
	for (i = 0; i < count; i++) {
		filename = open_app_get_pixmap_uri ("starred");
		clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->star [i]), filename, NULL);
		g_free (filename);
	}
	for (; i < 5; i++) {
		filename = open_app_get_pixmap_uri ("non-starred");
		clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->star [i]), filename, NULL);
		g_free (filename);
	}
}

static void
gnome_app_score_init (GnomeAppScore *score)
{
	GnomeAppScorePrivate *priv;
	ClutterActor *layout;
	gint i;

	score->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (score,
							 GNOME_APP_TYPE_SCORE,
							 GnomeAppScorePrivate);
	priv->score = 0;

	layout = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (score), layout);

	for (i = 0; i < 5; i++) {
		priv->star [i] = clutter_texture_new ();
		clutter_actor_set_size (priv->star [i], 15, 15);
	        clutter_container_add_actor (CLUTTER_CONTAINER (layout), priv->star [i]);
	}
}

static void
gnome_app_score_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_score_parent_class)->dispose (object);
}

static void
gnome_app_score_finalize (GObject *object)
{
	GnomeAppScore *score = GNOME_APP_SCORE (object);
	GnomeAppScorePrivate *priv = score->priv;

	G_OBJECT_CLASS (gnome_app_score_parent_class)->finalize (object);
}

void
gnome_app_score_set_property (GObject *object,
	 	guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppScore *score;
	GnomeAppScorePrivate *priv;
	const gchar *str;

	score = GNOME_APP_SCORE (object);
	priv = score->priv;
	switch (prop_id)
	{
		case PROP_SCORE:
            str = g_value_get_string (value);
            if (str)
			    priv->score = atoi (str);
			score_load (score);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_score_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppScore *score;
	GnomeAppScorePrivate *priv;
	gchar *str;

	score = GNOME_APP_SCORE (object);
	priv = score->priv;
	switch (prop_id)
	{
		case PROP_SCORE:
			str = g_strdup_printf ("%d", priv->score);
			g_value_set_string (value, str);
			g_free (str);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_score_class_init (GnomeAppScoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_score_set_property;
	object_class->get_property = gnome_app_score_get_property;
	object_class->dispose = gnome_app_score_dispose;
	object_class->finalize = gnome_app_score_finalize;
	
	g_object_class_install_property (object_class,
			PROP_SCORE,
			g_param_spec_string ("score",
				"score",
				"score",
				NULL,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppScorePrivate));
}

GnomeAppScore *
gnome_app_score_new (void)
{
	GnomeAppScore *score;

	score = g_object_new (GNOME_APP_TYPE_SCORE, NULL);

	return score;
}
