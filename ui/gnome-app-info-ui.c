/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: Lance Wang <lzwang@suse.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>

#include "gnome-app-utils.h"
#include "gnome-app-info-ui.h"
#include "gnome-app-info.h"
#include "gnome-app-install.h"

struct _GnomeAppInfoUIPrivate
{
	GnomeAppInfo *info;
};

G_DEFINE_TYPE (GnomeAppInfoUI, gnome_app_info_ui, CLUTTER_TYPE_GROUP)

static void
gnome_app_info_ui_init (GnomeAppInfoUI *ui)
{
	GnomeAppInfoUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_APP_TYPE_INFO_UI,
							 GnomeAppInfoUIPrivate);
	priv->info = NULL;
}

static void
gnome_app_info_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_ui_parent_class)->dispose (object);
}

static void
gnome_app_info_ui_finalize (GObject *object)
{
	GnomeAppInfoUI *ui = GNOME_APP_INFO_UI (object);
	GnomeAppInfoUIPrivate *priv = ui->priv;

	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_info_ui_parent_class)->finalize (object);
}

static void
gnome_app_info_ui_class_init (GnomeAppInfoUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_info_ui_dispose;
	object_class->finalize = gnome_app_info_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoUIPrivate));
}

GnomeAppInfoUI *
gnome_app_info_ui_new (void)
{
	return g_object_new (GNOME_APP_TYPE_INFO_UI, NULL);
}

static gboolean
app_fullview_cb (GnomeAppInfo *info)
{
	gnome_app_info_debug (info);
}

static void
generate_app_ui (GnomeAppInfoUI *ui, GnomeAppInfo *info)
{
}

GnomeAppInfoUI *
gnome_app_info_ui_new_with_app (GnomeAppInfo *info)
{
	GnomeAppInfoUI *ui;

	g_return_val_if_fail (info != NULL, NULL);

	ui = g_object_new (GNOME_APP_TYPE_INFO_UI, NULL);
	ui->priv->info = g_object_ref (info);

	const gchar *icon_name;
	const gchar *app_name;
	const gchar *uri;
	GError *err = NULL;

        ClutterLayoutManager *layout;
        ClutterActor *box, *text;
        ClutterAction *action;
        ClutterActor *icon = NULL;

        layout = clutter_box_layout_new ();
        clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
        clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (layout), 6);

	app_name = gnome_app_info_get (info, "name");
        box = clutter_box_new (layout);
        text = clutter_text_new ();
        clutter_text_set_ellipsize (CLUTTER_TEXT (text), PANGO_ELLIPSIZE_END);
        clutter_text_set_text (CLUTTER_TEXT (text), app_name);
        clutter_actor_set_width (text, 64);
        clutter_container_add_actor (CLUTTER_CONTAINER (box), text);

	uri = gnome_app_info_get (info, "smallpreviewpic1");
        if (uri) {
		gchar *local_uri;
		local_uri = gnome_app_get_local_icon (uri);
                icon = clutter_texture_new_from_file (local_uri, NULL);
		g_free (local_uri);
        }

        if (icon) {
        	clutter_actor_set_width (icon, 64);
	        clutter_actor_set_height (icon, 64);
        	clutter_container_add_actor (CLUTTER_CONTAINER (box), icon);
	        clutter_actor_set_reactive (box, TRUE);
	} else {
	/* what to do? */
	}

	clutter_container_add_actor (CLUTTER_CONTAINER (ui), box);
        g_signal_connect_swapped (box, "button-press-event",
                            G_CALLBACK (app_fullview_cb), g_object_ref (info));

	return ui;
}
