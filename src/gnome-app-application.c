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
#include <string.h>
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-login.h"
#include "gnome-app-application.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

struct _GnomeAppApplicationPrivate
{
	GnomeAppStore *store;
	ClutterActor *stage;
	GnomeAppLogin *login;
	GnomeAppInfoPage *info_page;
	GnomeAppFrameUI *frame_ui;
//	GnomeAppActions *actions;
};

enum {
	PROP_0,
	PROP_APP_STORE,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppApplication, gnome_app_application, G_TYPE_OBJECT)

static void	gnome_app_application_run (GnomeAppLogin *login, GnomeAppApplication *app);

static void
application_load_app_info (GnomeAppApplication *app, OpenResult *info)
{
	GnomeAppApplicationPrivate *priv;
	ClutterActor *actions;

        priv = app->priv;
	gnome_app_info_page_set_with_data (priv->info_page, info);
#if 0
	actions = gnome_app_info_page_get_actions (priv->info_page);
	gnome_app_actions_set_with_data (app->actions, actions);
#endif
	clutter_actor_hide (CLUTTER_ACTOR (priv->frame_ui));
	clutter_actor_show (CLUTTER_ACTOR (priv->info_page));
}

static void
application_load_frame_ui (GnomeAppApplication *app)
{
	GnomeAppApplicationPrivate *priv;
	ClutterActor *actions;

	priv = app->priv;
	clutter_actor_show (priv->stage);
#if 0
	actions = gnome_app_frame_ui_get_actions (priv->frame_ui);
	gnome_app_actions_set_with_data (app->actions, actions);
#endif
	clutter_actor_hide (CLUTTER_ACTOR (priv->info_page));
	clutter_actor_show (CLUTTER_ACTOR (priv->frame_ui));
}

void
gnome_app_application_load (GnomeAppApplication *app, UI_TYPE type, gpointer userdata)
{
	switch (type) {
		case UI_TYPE_FRAME_UI:
			application_load_frame_ui (app);
			break;
		case UI_TYPE_INFO_PAGE:
			application_load_app_info (app, userdata);
			break;
	}
}

static void
gnome_app_application_init (GnomeAppApplication *app)
{
	GnomeAppApplicationPrivate *priv;

	app->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (app,
							 GNOME_APP_TYPE_APPLICATION,
							 GnomeAppApplicationPrivate);

	priv->store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (priv->store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (priv->store, clutter_threads_leave);

	priv->login = gnome_app_login_new ();
	g_signal_connect (priv->login, "auth", G_CALLBACK (gnome_app_application_run), app);

	priv->frame_ui = NULL;
	priv->info_page = NULL;
}

static void
gnome_app_application_run (GnomeAppLogin *login, GnomeAppApplication *app)
{
	GnomeAppApplicationPrivate *priv;

	priv = app->priv;

	gnome_app_store_init_category (priv->store);

	priv->stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (priv->stage), _("AppStore"));
	clutter_actor_set_size (CLUTTER_ACTOR (priv->stage), 1000, 800);
        g_signal_connect (priv->stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	priv->info_page = gnome_app_info_page_new_with_app (app);
	priv->frame_ui = gnome_app_frame_ui_new_with_app (app);
	  
	clutter_container_add (CLUTTER_CONTAINER (priv->stage), CLUTTER_ACTOR (priv->info_page), NULL);
	clutter_container_add (CLUTTER_CONTAINER (priv->stage), CLUTTER_ACTOR (priv->frame_ui), NULL);
//	priv->actions = gnome_app_actions_new_with_app (app);

	application_load_frame_ui (app);
	g_object_unref (priv->login);
	priv->login = NULL;
}

static void
gnome_app_application_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_application_parent_class)->dispose (object);
}

static void
gnome_app_application_finalize (GObject *object)
{
	GnomeAppApplication *app = GNOME_APP_APPLICATION (object);
	GnomeAppApplicationPrivate *priv = app->priv;

	if (priv->login)
		g_object_unref (priv->login);
	if (priv->frame_ui)
		g_object_unref (priv->frame_ui);
	if (priv->info_page)
		g_object_unref (priv->info_page);

/*TODO: potencial issue: get_default may not be used again */
	if (priv->store)
		g_object_unref (priv->store);

	G_OBJECT_CLASS (gnome_app_application_parent_class)->finalize (object);
}

static void
application_get_property (GObject    *object,
		guint       prop_id,
		GValue     *value,
		GParamSpec *pspec)
{
	GnomeAppApplication *self;

		
	self = GNOME_APP_APPLICATION (object);

	switch (prop_id) {
		case PROP_APP_STORE:
			g_value_set_object (value, self->priv->store);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_application_class_init (GnomeAppApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_application_dispose;
	object_class->finalize = gnome_app_application_finalize;
	
	object_class->get_property = application_get_property;

	g_object_class_install_property (object_class,
			PROP_APP_STORE,
			g_param_spec_object ("app-store",
				"app store",
				"app store",
				GNOME_APP_TYPE_STORE,
				G_PARAM_READABLE));

	g_type_class_add_private (object_class, sizeof (GnomeAppApplicationPrivate));
}

GnomeAppApplication *
gnome_app_application_new ()
{
        return g_object_new (GNOME_APP_TYPE_APPLICATION, NULL);
}

GnomeAppApplication *
gnome_app_application_get_default ()
{
	static GnomeAppApplication *app = NULL;

	if (!app)
		app = g_object_new (GNOME_APP_TYPE_APPLICATION, NULL);

	return app;
}
