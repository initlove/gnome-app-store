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

#include "gnome-app-item.h"

struct _GnomeAppItemUIPrivate
{
	GnomeAppItem *app;
	ClutterActor *icon;
};

G_DEFINE_TYPE (GnomeAppItemUI, gnome_app_item_ui, G_TYPE_OBJECT)

static void
gnome_app_item_ui_init (GnomeAppItemUI *ui)
{
	GnomeAppItemUIPrivate *priv;

	item->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
							 GNOME_TYPE_APP_ITEM_UI,
							 GnomeAppItemUIPrivate);
}

static void
gnome_app_item_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_item_ui_parent_class)->dispose (object);
}

static void
gnome_app_item_ui_finalize (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_item_ui_parent_class)->finalize (object);
}

static void
gnome_app_item_ui_class_init (GnomeAppItemUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_item_ui_dispose;
	object_class->finalize = gnome_app_item_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppItemUIPrivate));
}

GnomeAppItemUI *
gnome_app_item_ui_new (void)
{
	return g_object_new (GNOME_TYPE_APP_ITEM, NULL);
}

static void
free_ui_resources (GnomeAppItemUI *ui)
{
	GnomeAppItemUIPrivate *priv;

	if (priv->icon != NULL) {
		g_object_unref (priv->icon);
		priv->icon = NULL;
	}
}

void
gnome_app_item_ui_set_app (GnomeAppItemUI *ui,
			    GnomeAppItem *app)
{
	GnomeAppItemUIPrivate *priv;

	g_return_val_if_fail (ui != NULL, FALSE);
	g_return_val_if_fail (app != NULL, FALSE);

	priv = ui->priv;

	if (priv->app != NULL) {
		g_object_unref (priv->app);
	}
	g_object_ref (app);
	priv->app = app;
	free_ui_resources (ui);
}

GnomeAppItemUI *
gnome_app_item_ui_new_with_app (GnomeAppItem *app)
{
	GnomeAppItemUI *ui;

	g_return_val_if_fail (app != NULL, NULL);

	ui = g_object_new (GNOME_TYPE_APP_ITEM_UI, NULL);
	gnome_app_item_ui_set_app (ui, app);
	return ui;
}

static inline void
set_icon (GnomeAppItemUI *ui, ClutterActor *icon)
{
	if (ui->priv->icon != NULL) {
		g_object_unref (icon);
	}
	ui->priv->icon = g_object_ref (icon);
}

static inline ClutterActor *
get_icon (GnomeAppItemUI *ui)
{
	return ui->priv->icon;
}

static ClutterActor *
get_icon_from_app (GnomeAppItem *app)
{
	const gchar *uri;

	ClutterActor *icon = NULL;

	GError *err = NULL;

	uri = gnome_app_item_get_icon_uri (app);
	if (uri == NULL) {
		g_log (GNOME_APP_ITEM_UI_LOG_DOMAIN,
		       G_LOG_LEVEL_WARNING,
		       "there is no icon");
		return NULL;
	}

	icon = clutter_texture_new_from_file (uri, &err);
	if (icon == NULL) {
		g_log (GNOME_APP_ITEM_UI_LOG_DOMAIN,
		       G_LOG_LEVEL_WARNING,
		       "failed to get icon %s: %s",
		       uri,
		       err->message);
		g_error_free (err);
		return NULL;
	}
	clutter_actor_set_width (icon, 64);
	clutter_actor_set_height (icon, 64);

	return CLUTTER_ACTOR (icon);
}

ClutterActor *
gnome_app_item_ui_get_icon (GnomeAppItemUI *ui)
{
	ClutterActor *icon;

	g_return_val_if_fail (GNOME_IS_APP_ITEM_UI(ui), NULL);

	if (ui->app == NULL) {
		return NULL;
	}

	icon = get_icon (ui);
	if (icon) {
		return icon;
	}

	icon = get_icon_from_app (ui->app);
	if (icon != NULL) {
		set_icon (ui, icon);
	}

	return icon;
}

