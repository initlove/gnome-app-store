/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: liangchenye <liangchenye@gmail.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-ui-score.h"

struct _GnomeAppUIScorePrivate
{
	gint score;
};

G_DEFINE_TYPE (GnomeAppUIScore, gnome_app_ui_score, CLUTTER_TYPE_GROUP)

static void
ui_score_load (GnomeAppUIScore *ui)
{
	ClutterActor *layout;
	ClutterActor *actor;

	layout = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), layout);

	gchar *filename;
	gint i, score;
	
	score = ui->priv->score / 20;
	for (i = 0; i < score; i++) {
		filename = open_app_get_ui_uri ("starred");
		actor = clutter_texture_new_from_file (filename, NULL);
	        clutter_container_add_actor (CLUTTER_CONTAINER (layout), actor);
		g_free (filename);
	}
	for (; i < 5; i++) {
		filename = open_app_get_ui_uri ("non-starred");
		actor = clutter_texture_new_from_file (filename, NULL);
	        clutter_container_add_actor (CLUTTER_CONTAINER (layout), actor);
		g_free (filename);
	}
}

static void
gnome_app_ui_score_init (GnomeAppUIScore *ui)
{
	GnomeAppUIScorePrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_APP_TYPE_UI_SCORE,
							 GnomeAppUIScorePrivate);
	priv->score = 0;
	ui_score_load (ui);
}

static void
gnome_app_ui_score_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_ui_score_parent_class)->dispose (object);
}

static void
gnome_app_ui_score_finalize (GObject *object)
{
	GnomeAppUIScore *ui = GNOME_APP_UI_SCORE (object);
	GnomeAppUIScorePrivate *priv = ui->priv;

	G_OBJECT_CLASS (gnome_app_ui_score_parent_class)->finalize (object);
}

static void
gnome_app_ui_score_class_init (GnomeAppUIScoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_ui_score_dispose;
	object_class->finalize = gnome_app_ui_score_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppUIScorePrivate));
}

GnomeAppUIScore *
gnome_app_ui_score_new (void)
{
	return g_object_new (GNOME_APP_TYPE_UI_SCORE, NULL);
}

void
gnome_app_ui_score_set_score (GnomeAppUIScore *ui, gchar *score)
{
	if (score)
		ui->priv->score = atoi (score);

	ui_score_load (ui);
}
