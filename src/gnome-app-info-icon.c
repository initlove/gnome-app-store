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
#include "gnome-app-store.h"
#include "gnome-app-task.h"
#include "gnome-app-info-icon.h"
#include "gnome-app-info-page.h"
#include "gnome-app-store-ui.h"

struct _GnomeAppInfoIconPrivate
{
	OpenResult *info;
};

G_DEFINE_TYPE (GnomeAppInfoIcon, gnome_app_info_icon, CLUTTER_TYPE_GROUP)

static void
gnome_app_info_icon_init (GnomeAppInfoIcon *info_icon)
{
	GnomeAppInfoIconPrivate *priv;

	info_icon->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info_icon,
							 GNOME_APP_TYPE_INFO_ICON,
							 GnomeAppInfoIconPrivate);
	priv->info = NULL;
}

static void
gnome_app_info_icon_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_icon_parent_class)->dispose (object);
}

static void
gnome_app_info_icon_finalize (GObject *object)
{
	GnomeAppInfoIcon *info_icon = GNOME_APP_INFO_ICON (object);
	GnomeAppInfoIconPrivate *priv = info_icon->priv;

	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_info_icon_parent_class)->finalize (object);
}

static void
gnome_app_info_icon_class_init (GnomeAppInfoIconClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_info_icon_dispose;
	object_class->finalize = gnome_app_info_icon_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoIconPrivate));
}

static gboolean
on_info_icon_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	OpenResult *info;
	GnomeAppStoreUI *store_ui;
	ClutterActor *page, *stage;

	info = OPEN_RESULT (data);
	switch (event->type)
	{
	case CLUTTER_BUTTON_PRESS:
		store_ui = gnome_app_store_ui_get_default ();
		gnome_app_store_ui_load_app_info (store_ui, info);
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

static void
set_pic_callback (gpointer userdata, gpointer func_re)
{
	ClutterActor *actor;
	gchar *dest_url;

	actor = CLUTTER_ACTOR (userdata);
	dest_url = (gchar *) func_re;
/*TODO: why should use this thread? */
	clutter_threads_enter ();
	clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), dest_url, NULL);
	clutter_threads_leave ();
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
		GnomeAppStore *store;
		GnomeAppTask *task;

		store = gnome_app_store_get_default ();
		task = gnome_download_task_new (actor, val);
		gnome_app_task_set_callback (task, set_pic_callback);
		gnome_app_store_add_task (store, task);
	} else {
//TODO
	}

	g_signal_connect (actor, "event", G_CALLBACK (on_info_icon_event), info);

	return info_icon;
}
