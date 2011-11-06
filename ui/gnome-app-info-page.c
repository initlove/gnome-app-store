/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author: Lance Wang <lzwang@suse.com>

*/
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>

#include "gnome-app-utils.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-info-page.h"
#include "gnome-app-info.h"
#include "gnome-app-install.h"

struct _GnomeAppInfoPagePrivate
{
	GnomeAppInfo *info;
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static void
gnome_app_info_page_init (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;

	page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (page,
							 GNOME_APP_TYPE_INFO_PAGE,
							 GnomeAppInfoPagePrivate);
	priv->info = NULL;
}

static void
gnome_app_info_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->dispose (object);
}

static void
gnome_app_info_page_finalize (GObject *object)
{
	GnomeAppInfoPage *page = GNOME_APP_INFO_PAGE (object);
	GnomeAppInfoPagePrivate *priv = page->priv;

	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->finalize (object);
}

static void
gnome_app_info_page_class_init (GnomeAppInfoPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_info_page_dispose;
	object_class->finalize = gnome_app_info_page_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPagePrivate));
}

static gboolean
on_info_page_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
        GnomeAppStoreUI *store_ui;

        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
                store_ui = gnome_app_store_ui_get_default ();
                gnome_app_store_ui_load_frame_ui (store_ui);

                break;

        }
        return TRUE;
}

GnomeAppInfoPage *
gnome_app_info_page_new_with_app (GnomeAppInfo *info)
{
	GnomeAppInfoPage *page;

	g_return_val_if_fail (info != NULL, NULL);

	page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
	page->priv->info = g_object_ref (info);

	const gchar *filename;
	GError *error;
	ClutterScript *script;
	ClutterActor *actor;
	gint i;

	filename = "/home/novell/gnome-app-store/ui/scripts/app-info-page.json";
	error = NULL;

	script = clutter_script_new ();
	clutter_script_load_from_file (script, filename, &error);
	if (error) {
		printf ("error in load script %s\n", error->message);
		g_error_free (error);
	}
	clutter_script_get_objects (script, "info-page", &page, NULL);
        
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
/*FIXME: tmp for no network debug */
if (!local_uri)
local_uri = g_strdup ("/home/novell/.gnome-app-store/cache/ocs/img/fe6835c189e4331dd6bce5b07537d44a");
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

	gchar *scores [] = { "score-1", "score-2", "score-3", "score-4", "score-5", NULL};
	gint app_score = atoi (gnome_app_info_get (info, "score")) / 20;
	for (i = 0; scores [i]; i++) {
		clutter_script_get_objects (script, scores [i], &actor, NULL);
		if (!actor)
			continue;
		if (i < app_score)
			clutter_texture_set_from_file (actor, "/home/novell/gnome-app-store/pixmaps/starred.png", NULL);
		else
			clutter_texture_set_from_file (actor, "/home/novell/gnome-app-store/pixmaps/non-starred.png", NULL);
	}

        g_signal_connect (page, "event", G_CALLBACK (on_info_page_event), NULL);

	return page;
}
