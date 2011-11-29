/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: Liang chenye <liangchenye@gmail.com>

*/
#include <string.h>
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "gnome-app-store.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

struct _GnomeAppStoreUIPrivate
{
	GnomeAppInfoPage *info_page;
	GnomeAppFrameUI *frame_ui;
	GnomeAppStore *store;
};

G_DEFINE_TYPE (GnomeAppStoreUI, gnome_app_store_ui, CLUTTER_TYPE_STAGE)

static void
gnome_app_store_ui_init (GnomeAppStoreUI *ui)
{
	GnomeAppStoreUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_APP_TYPE_STORE_UI,
							 GnomeAppStoreUIPrivate);
	clutter_stage_set_title (CLUTTER_STAGE (ui), _("AppStore"));
	clutter_actor_set_size (CLUTTER_ACTOR (ui), 1000, 800);
        g_signal_connect (ui, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	priv->info_page = NULL;
	priv->store = gnome_app_store_get_default ();
	gnome_app_store_init_category (priv->store);
/*TODO: as we should add task every ui, it might be better to use this.. */
	priv->frame_ui = gnome_app_frame_ui_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->frame_ui));
}

static void
gnome_app_store_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_store_ui_parent_class)->dispose (object);
}

static void
gnome_app_store_ui_finalize (GObject *object)
{
	GnomeAppStoreUI *ui = GNOME_APP_STORE_UI (object);
	GnomeAppStoreUIPrivate *priv = ui->priv;

	if (priv->frame_ui)
		g_object_unref (priv->frame_ui);
	if (priv->info_page)
		g_object_unref (priv->info_page);

/*TODO: potencial issue: get_default may not be used again */
	if (priv->store)
		g_object_unref (priv->store);

	G_OBJECT_CLASS (gnome_app_store_ui_parent_class)->finalize (object);
}

static void
gnome_app_store_ui_class_init (GnomeAppStoreUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_store_ui_dispose;
	object_class->finalize = gnome_app_store_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppStoreUIPrivate));
}

GnomeAppStoreUI *
gnome_app_store_ui_new ()
{
        return g_object_new (GNOME_APP_TYPE_STORE_UI, NULL);
}

GnomeAppStoreUI *
gnome_app_store_ui_get_default ()
{
	static GnomeAppStoreUI *ui = NULL;
	if (!ui)
		ui = g_object_new (GNOME_APP_TYPE_STORE_UI, NULL);
	return ui;
}

//TODO: here, we can implement some animation when click between frame and page
void
gnome_app_store_ui_load_app_info (GnomeAppStoreUI *ui, OpenResult *info)
{
	GnomeAppStoreUIPrivate *priv = ui->priv;

	if (priv->info_page)
		g_object_unref (priv->info_page);
	priv->info_page = gnome_app_info_page_new_with_app (info);

#if 0
	ClutterAnimation *animation;

	animation =
	    clutter_actor_animate (priv->frame_ui, CLUTTER_EASE_IN_EXPO, 2000,
                           "x", new_x,
                           "y", new_y,
                           "width", new_width,
                           "height", new_height,
                           "color", &new_color,
                           "rotation-angle-z", new_angle,
                           "fixed::rotation-center-z", &vertex,
                           "fixed::reactive", FALSE,
                           NULL);
#endif
	clutter_actor_hide (CLUTTER_ACTOR (priv->frame_ui));
//TODO: i want to know, why the remove actor delete the object!? Should put it into memo..
//	clutter_container_remove_actor (CLUTTER_CONTAINER (ui), priv->frame_ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->info_page));
}

void
gnome_app_store_ui_load_frame_ui (GnomeAppStoreUI *ui)
{
	GnomeAppStoreUIPrivate *priv = ui->priv;

	if (priv->info_page) {
		clutter_container_remove_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->info_page));
		g_object_unref (priv->info_page);
		priv->info_page = NULL;
	}

	clutter_actor_show (CLUTTER_ACTOR (priv->frame_ui));
}
