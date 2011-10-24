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
	GnomeAppInfo *app;
	ClutterActor *icon;
};

G_DEFINE_TYPE (GnomeAppInfoUI, gnome_app_info_ui, G_TYPE_OBJECT)

static void
gnome_app_info_ui_init (GnomeAppInfoUI *ui)
{
	GnomeAppInfoUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_TYPE_APP_INFO_UI,
							 GnomeAppInfoUIPrivate);
	priv->app = NULL;
	priv->icon = NULL;
}

static void
gnome_app_info_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_ui_parent_class)->dispose (object);
}

static void
gnome_app_info_ui_finalize (GObject *object)
{
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
	return g_object_new (GNOME_APP_TYPE_INFO, NULL);
}

static void
free_ui_resources (GnomeAppInfoUI *ui)
{
	GnomeAppInfoUIPrivate *priv;

	if (priv->icon != NULL) {
		g_object_unref (priv->icon);
		priv->icon = NULL;
	}
}

gboolean
gnome_app_info_ui_set_app (GnomeAppInfoUI *ui,
			    GnomeAppInfo *app)
{
	GnomeAppInfoUIPrivate *priv;

	g_return_val_if_fail (ui != NULL, FALSE);
	g_return_val_if_fail (app != NULL, FALSE);
	priv = ui->priv;

	if (priv->app != NULL) {
		g_object_unref (priv->app);
	}
	g_object_ref (app);
	priv->app = app;
//	free_ui_resources (ui);

	return TRUE;
}

GnomeAppInfoUI *
gnome_app_info_ui_new_with_app (GnomeAppInfo *app)
{
	GnomeAppInfoUI *ui;

	g_return_val_if_fail (app != NULL, NULL);

	ui = g_object_new (GNOME_TYPE_APP_INFO_UI, NULL);
	gnome_app_info_ui_set_app (ui, app);

	return ui;
}

static inline void
set_icon (GnomeAppInfoUI *ui, ClutterActor *icon)
{
	if (ui->priv->icon != NULL) {
		g_object_unref (icon);
	}
	ui->priv->icon = g_object_ref (icon);
}

static inline ClutterActor *
get_icon (GnomeAppInfoUI *ui)
{
	return ui->priv->icon;
}

static gboolean
app_fullview_cb (ClutterActor *actor,
                  ClutterButtonEvent *event,
                  gpointer            data)
{
	GnomeAppInfo *info;

	info = (GnomeAppInfo *)data;
	gnome_app_info_debug (info);
}

static ClutterActor *
get_icon_from_app (GnomeAppInfo *info)
{
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
/*TODO: we should make a strong png widget, to load web icon, local icon, theme icon */
		gchar *local_uri;
		local_uri = gnome_app_get_local_icon (uri);
                icon = clutter_texture_new_from_file (local_uri, NULL);
		g_free (local_uri);
		/*FIXME: if the icon cannot be textureed, should we remove it? */
		if (icon == NULL) 
			printf ("but the icon is NULL\n");
        }

        if (icon) {
        	clutter_actor_set_width (icon, 64);
	        clutter_actor_set_height (icon, 64);
        	clutter_container_add_actor (CLUTTER_CONTAINER (box), icon);
	        clutter_actor_set_reactive (box, TRUE);
	} else {
	/* what to do? */
	}
        g_signal_connect_swapped (box, "button-press-event",
                            G_CALLBACK (app_fullview_cb), info);

	return box;
}

ClutterActor *
gnome_app_info_ui_get_icon (GnomeAppInfoUI *ui)
{
	ClutterActor *icon;

	g_return_val_if_fail (GNOME_IS_APP_INFO_UI(ui), NULL);

	if (ui->priv->app == NULL) {
		return NULL;
	}

	icon = get_icon (ui);
	if (icon) {
		return icon;
	}

	icon = get_icon_from_app (ui->priv->app);
	if (icon != NULL) {
		set_icon (ui, icon);
	}

	return icon;
}
