/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Sinfo_iconte 330,
Boston, MA 02111-1307, USA.

Author: Lance Wang <lzwang@suse.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>

#include "gnome-app-utils.h"
#include "gnome-app-info.h"
#include "gnome-app-info-icon.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

struct _GnomeAppInfoIconPrivate
{
	GnomeAppInfo *info;
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
app_fullview_cb (GnomeAppInfo *info)
{
	gnome_app_info_debug (info);

	ClutterActor *stage, *page;

	page = gnome_app_info_page_new_with_app (info);

	GnomeAppFrameUI *info_icon;
	info_icon = gnome_app_frame_ui_get_default ();
	gnome_app_frame_ui_set_full_info_mode (info_icon, page);

	return TRUE;
}

GnomeAppInfoIcon *
gnome_app_info_icon_new_with_app (GnomeAppInfo *info)
{
	GnomeAppInfoIcon *info_icon;

	g_return_val_if_fail (info != NULL, NULL);

	info_icon = g_object_new (GNOME_APP_TYPE_INFO_ICON, NULL);
	info_icon->priv->info = g_object_ref (info);

        const gchar *filename;
	GError *error;
        ClutterScript *script;
        ClutterActor *actor;
        gint i;

	error = NULL;
        filename = "/home/novell/gnome-app-store/ui/scripts/app-info-icon.json";

        script = clutter_script_new ();
        clutter_script_load_from_file (script, filename, &error);
	if (error) {
		printf ("error in load script %s!\n", error->message);
		g_error_free (error);
	}
        clutter_script_get_objects (script, "info-icon", &info_icon, NULL);

        gchar *prop [] = {
                "name", "personid", "description",
                "score", "downloads", "comments",
                "smallpreviewpic1", "previewpic1",
                "license", NULL};

        const gchar *val;
        gchar *local_uri;

        for (i = 0; prop [i]; i++) {
                clutter_script_get_objects (script, prop [i], &actor, NULL);
                if (!actor)
                        continue;
                val = gnome_app_info_get (info, prop [i]);
                if (CLUTTER_IS_TEXTURE (actor)) {
                        local_uri = gnome_app_get_local_icon (val);
                        clutter_texture_set_from_file (actor, local_uri, NULL);
                        g_free (local_uri);
                } else if (CLUTTER_IS_TEXT (actor)) {
                        clutter_text_set_text (actor, val);
                }
#if 0
/*FIXME: cannot use the user defined object? */
                        else if (GNOME_APP_IS_UI_SCORE (actor)) {
                        gnome_app_ui_score_set_score (actor, val);
                }
#endif
        }

	return info_icon;
}
