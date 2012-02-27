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
#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-stage.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-info-icon.h"

struct _GnomeAppInfoIconPrivate
{
	ClutterScript *script;
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

	priv->script = NULL;
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

	if (priv->script)
		g_object_unref (priv->script);
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

G_MODULE_EXPORT gboolean
on_info_icon_event (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      userdata)
{
	GnomeAppInfoIcon *info_icon;
	GnomeAppInfoIconPrivate *priv;
	GnomeAppStage *app_stage;
	OpenResult *info;

	info_icon = GNOME_APP_INFO_ICON (userdata);
	priv = info_icon->priv;
	switch (event->type)
	{
		case CLUTTER_BUTTON_PRESS:
			app_stage = gnome_app_stage_get_default ();
			gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_DEFAULT, "GnomeAppInfoPage", "info", priv->info, NULL);
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
gnome_app_info_icon_new_with_info (OpenResult *info)
{
	GnomeAppInfoIcon *info_icon;
	GnomeAppInfoIconPrivate *priv;
	ClutterActor *name;
	ClutterActor *pic;

	info_icon = g_object_new (GNOME_APP_TYPE_INFO_ICON, NULL);
	priv = info_icon->priv;
	priv->script = gnome_app_script_new_from_file ("app-info-icon");
	if (!priv->script) {
		g_object_unref (info_icon);
		return NULL;
	}

	priv->info = g_object_ref (info);
	clutter_script_connect_signals (priv->script, info_icon);
	clutter_script_get_objects (priv->script, 
			"info-icon", &info_icon,
			"name", &name, 
			"smallpreviewpic1", &pic,
			NULL);

	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	gnome_app_set_icon (pic, open_result_get (info, "smallpreviewpic1"));

	return info_icon;
}
