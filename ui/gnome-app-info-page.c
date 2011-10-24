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

GnomeAppInfoPage *
gnome_app_info_page_new (void)
{
	return g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
}

GnomeAppInfoPage *
gnome_app_info_page_new_with_app (GnomeAppInfo *info)
{
	GnomeAppInfoPage *page;

	g_return_val_if_fail (info != NULL, NULL);

	page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
	page->priv->info = g_object_ref (info);

	gchar *local_uri;
	const gchar *val;
	GError *err = NULL;

        ClutterLayoutManager *layout;
	ClutterActor *layout_box, *box;
        ClutterActor *name, *personid, *smallpreview, *preview;

	box = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (page), box);
	layout = clutter_table_layout_new ();
	layout_box = clutter_box_new (layout);
        clutter_container_add_actor (CLUTTER_CONTAINER (box), layout_box);
#if 1
	val = gnome_app_info_get (info, "name");
        name = clutter_text_new ();
        clutter_text_set_ellipsize (CLUTTER_TEXT (name), PANGO_ELLIPSIZE_END);
        clutter_text_set_text (CLUTTER_TEXT (name), val);
        clutter_actor_set_width (name, 64);

	val = gnome_app_info_get (info, "personid");
	personid = clutter_text_new ();
        clutter_text_set_ellipsize (CLUTTER_TEXT (personid), PANGO_ELLIPSIZE_END);
        clutter_text_set_text (CLUTTER_TEXT (personid), val);
        clutter_actor_set_width (personid, 64);

        val = gnome_app_info_get (info, "smallpreviewpic1");
 	local_uri= gnome_app_get_local_icon (val);
        smallpreview =  clutter_texture_new_from_file (local_uri, NULL);
	g_free (local_uri);

        val = gnome_app_info_get (info, "previewpic1");
 	local_uri= gnome_app_get_local_icon (val);
        preview =  clutter_texture_new_from_file (local_uri, NULL);
	g_free (local_uri);

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), name, 0, 0);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), personid, 0, 1);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), smallpreview, 1, 0);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), preview, 1, 1);
#endif

	return page;
}
