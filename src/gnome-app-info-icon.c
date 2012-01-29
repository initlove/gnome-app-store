/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Sinfo_iconte 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@suse.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-info-icon.h"
#include "gnome-app-info-page.h"
#include "gnome-app-application.h"

struct _GnomeAppInfoIconPrivate
{
	GnomeAppApplication *app;
	OpenResult *info;
};

/* Properties */
enum
{
	PROP_0,
	PROP_APPLICATION,
	PROP_INFO,
	PROP_LAST
};

enum
{
	CLICKED,
	LAST_SIGNAL
};

static guint info_icon_signals [LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppInfoIcon, gnome_app_info_icon, CLUTTER_TYPE_GROUP)

static void
gnome_app_info_icon_init (GnomeAppInfoIcon *info_icon)
{
	GnomeAppInfoIconPrivate *priv;

	info_icon->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info_icon,
							 GNOME_APP_TYPE_INFO_ICON,
							 GnomeAppInfoIconPrivate);
	priv->app = NULL;
	priv->info = NULL;
}

static void
info_icon_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoIcon *info_icon;

	info_icon = GNOME_APP_INFO_ICON (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			info_icon->priv->app = g_value_get_object (value);
			break;
	}
}

static void
info_icon_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoIcon *info_icon;      

	info_icon = GNOME_APP_INFO_ICON (object);

	switch (prop_id)
	{
		case PROP_INFO:
			g_value_set_object (value, info_icon->priv->info);
			break;
	}
}

static void
info_icon_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_icon_parent_class)->dispose (object);
}

static void
info_icon_finalize (GObject *object)
{
	GnomeAppInfoIcon *info_icon = GNOME_APP_INFO_ICON (object);
	GnomeAppInfoIconPrivate *priv = info_icon->priv;

	if (priv->app)
		g_object_unref (priv->app);
	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_info_icon_parent_class)->finalize (object);
}

static void
gnome_app_info_icon_class_init (GnomeAppInfoIconClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = info_icon_set_property;
	object_class->get_property = info_icon_get_property;
	object_class->dispose = info_icon_dispose;
	object_class->finalize = info_icon_finalize;

        g_object_class_install_property (object_class,
			PROP_APPLICATION,
	  		g_param_spec_object ("application",
		  		"application",
				"The application which handle the whole app ui",
				GNOME_APP_TYPE_APPLICATION,
				G_PARAM_READWRITE));

        g_object_class_install_property (object_class,
			PROP_INFO,
	  		g_param_spec_object ("info",
		  		"info",
				"The app info of the info icon",
				TYPE_OPEN_RESULT,
				G_PARAM_READABLE));
#if 0
	info_icon_signals [CLICKED] =
		g_signal_new (g_intern_static_string ("selected"),
				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (GnomeAppInfoIconClass, selected),
				NULL, NULL,
				g_cclosure_marshal_VOID__OBJECT,
				G_TYPE_NONE, 1,
				TYPE_OPEN_RESULT);
#endif
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoIconPrivate));
}

static gboolean
on_info_icon_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoIcon *info_icon;
	OpenResult *info;
	ClutterActor *page, *stage;

	info_icon = GNOME_APP_INFO_ICON (data);
printf ("1\n");
if (info_icon)
	printf ("get\n");
	info = info_icon->priv->info;
printf ("1\n");
	switch (event->type)
	{
	case CLUTTER_BUTTON_PRESS:
		gnome_app_application_load (info_icon->priv->app, UI_TYPE_INFO_PAGE, info);
//		g_signal_emit (G_OBJECT (actor), info_icon_signals [CLICKED], 0);
		break;
	case CLUTTER_ENTER:
		clutter_actor_set_scale (actor, 1.5, 1.5);
		break;
	case CLUTTER_LEAVE:
		clutter_actor_set_scale (actor, 1, 1);
		break;
	}
	return TRUE;
}

GnomeAppInfoIcon *
gnome_app_info_icon_new_with_app (OpenResult *info)
{
	GnomeAppInfoIcon *info_icon;

	g_return_val_if_fail (info != NULL, NULL);

	info_icon = g_object_new (GNOME_APP_TYPE_INFO_ICON, NULL);
	info_icon->priv->info = g_object_ref (info);

        gchar *filename;
	GError *error;
        ClutterScript *script;
        ClutterActor *actor;
        gint i;

	error = NULL;
        filename = open_app_get_ui_uri ("app-info-icon");

        script = clutter_script_new ();
        clutter_script_load_from_file (script, filename, &error);
	if (error) {
		printf ("error in load script %s!\n", error->message);
		g_error_free (error);
	}
	g_free (filename);
        clutter_script_get_objects (script, "info-icon", &info_icon, NULL);

        gchar *prop [] = {
                "name", "personid", "description",
                "score", "downloads", "comments",
                "smallpreviewpic1", "previewpic1",
                "license", NULL};

        const gchar *val;

	clutter_script_get_objects (script, "name", &actor, NULL);
	val = open_result_get (info, "name");
	clutter_text_set_text (CLUTTER_TEXT (actor), val);

	clutter_script_get_objects (script, "smallpreviewpic1", &actor, NULL);
	val = open_result_get (info, "smallpreviewpic1");
	
/*TODO when final the task */
	if (val) {
		gnome_app_ui_set_icon (actor, val);
	} else {
//TODO
	}

	g_signal_connect (actor, "event", G_CALLBACK (on_info_icon_event), info_icon);

	return info_icon;
}
