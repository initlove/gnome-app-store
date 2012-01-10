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
#include <stdlib.h>
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-score-ui.h"

struct _GnomeAppScoreUIPrivate
{
	gint score;
};

G_DEFINE_TYPE (GnomeAppScoreUI, gnome_app_score_ui, CLUTTER_TYPE_GROUP)

static void
score_ui_load (GnomeAppScoreUI *ui)
{
	ClutterActor *layout;
	ClutterActor *actor;
	gchar *filename;
	gint i, score;

	layout = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), layout);

	/* the score is between 0 -- 100 */
	score = ui->priv->score / 20;
	for (i = 0; i < score; i++) {
		filename = open_app_get_pixmap_uri ("starred");
		actor = clutter_texture_new_from_file (filename, NULL);
		/*TODO: the size issue will be done much later */
		clutter_actor_set_size (actor, 15, 15);
	        clutter_container_add_actor (CLUTTER_CONTAINER (layout), actor);
		g_free (filename);
	}
	for (; i < 5; i++) {
		filename = open_app_get_pixmap_uri ("non-starred");
		actor = clutter_texture_new_from_file (filename, NULL);
		clutter_actor_set_size (actor, 15, 15);
	        clutter_container_add_actor (CLUTTER_CONTAINER (layout), actor);
		g_free (filename);
	}
}

static void
gnome_app_score_ui_init (GnomeAppScoreUI *ui)
{
	GnomeAppScoreUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_APP_TYPE_SCORE_UI,
							 GnomeAppScoreUIPrivate);
	priv->score = 0;
}

static void
gnome_app_score_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_score_ui_parent_class)->dispose (object);
}

static void
gnome_app_score_ui_finalize (GObject *object)
{
	GnomeAppScoreUI *ui = GNOME_APP_SCORE_UI (object);
	GnomeAppScoreUIPrivate *priv = ui->priv;

	G_OBJECT_CLASS (gnome_app_score_ui_parent_class)->finalize (object);
}

static void
gnome_app_score_ui_class_init (GnomeAppScoreUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_score_ui_dispose;
	object_class->finalize = gnome_app_score_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppScoreUIPrivate));
}

GnomeAppScoreUI *
gnome_app_score_ui_new_with_score (const gchar *score)
{
	GnomeAppScoreUI *ui;

	ui = g_object_new (GNOME_APP_TYPE_SCORE_UI, NULL);
	if (score)
		ui->priv->score = atoi (score);
	score_ui_load (ui);

	return ui;
}
