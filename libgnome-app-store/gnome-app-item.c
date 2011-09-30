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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <string.h>

#include <clutter/clutter.h>

#include "local-app.h"
#include "gnome-app-item.h"
#include "gnome-app-install.h"

struct _GnomeAppItemPrivate
{
	gint		tag;
	gchar *		id;
	gchar *		name;
	/* The way to install the pkg. */
	gchar *		pkgname;
	gchar *		icon;
	gchar *		summary;
	GSList *	categories;
	GSList *	mime_types;

	gboolean	is_default_icon;
};

G_DEFINE_TYPE (GnomeAppItem, gnome_app_item, G_TYPE_OBJECT)

typedef enum _APP_TAG {
	APP_INVALID,
	APP_LOCAL,
	APP_ONLINE
} APP_TAG;

typedef enum _PKG_KEY_WORDS {
        PKG_ID = 0,
        PKG_PKGNAME,
        PKG_NAME,
        PKG_SUMMARY,
        PKG_ICON,
        PKG_APPCATEGORIES,
        PKG_MIMETYPES,
        PKG_APPCATEGORY,
        PKG_MIMETYPE,
        PKG_LAST,
} PKG_KEY_WORDS;

static PKG_KEY_WORDS
get_type_from_name (gchar *name)
{
        gchar *key_words [] = {"id", "pkgname", "name", "summary", "icon",
                        "appcategories", "mimetypes", "appcategory", "mimetype", NULL};
        int i;

        for (i = 0; i < PKG_LAST; i++) {
                if (strcmp (name, key_words [i]) == 0) {
                        return i;
                }
        }

        return PKG_LAST;
}

static void
set_content (GnomeAppItem *item, xmlNodePtr app_node)
{
        GnomeAppItemPrivate *priv = item->priv;
        xmlNodePtr node, sub_node;
        PKG_KEY_WORDS type;

        for (node = app_node->xmlChildrenNode; node; node = node->next) {
                type = get_type_from_name ((gchar *)(node->name));
                switch (type) {
                        case PKG_ID:
				priv->id = g_strdup (xmlNodeGetContent (node));

				if (is_local_app (priv->id)) {
					priv->tag = APP_LOCAL;
					return;
				} else {
					priv->tag = APP_ONLINE;
				}
                                break;
                        case PKG_PKGNAME:
				priv->pkgname = g_strdup (xmlNodeGetContent (node));
                                break;
                        case PKG_NAME:
				priv->name = g_strdup (xmlNodeGetContent (node));
                                break;
                        case PKG_SUMMARY:
				priv->summary = g_strdup (xmlNodeGetContent (node));
                                break;
                        case PKG_ICON:
				priv->icon = g_strdup (xmlNodeGetContent (node));
                                break;
                        case PKG_APPCATEGORIES:
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_APPCATEGORY) {
                                        	priv->categories = g_slist_prepend (priv->categories, g_strdup (xmlNodeGetContent (sub_node)));
					}
                                }
                                break;
                        case PKG_MIMETYPES:
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_MIMETYPE)
                                        	priv->mime_types = g_slist_prepend (priv->mime_types, g_strdup (xmlNodeGetContent (sub_node)));
                                }
                                break;
                        default:
                                break;
                }
        }
}

static void
gnome_app_item_init (GnomeAppItem *item)
{
	GnomeAppItemPrivate *priv;

	item->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (item,
                                                   GNOME_TYPE_APP_ITEM,
                                                   GnomeAppItemPrivate);

	priv->tag = APP_INVALID;
	priv->id = NULL;
	priv->name = NULL;
	priv->pkgname = NULL;
	priv->icon = NULL;
	priv->summary = NULL;
	priv->categories = NULL;
	priv->mime_types = NULL;

	priv->is_default_icon = FALSE;
}

static void
gnome_app_item_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_item_parent_class)->dispose (object);
}

static void
gnome_app_item_finalize (GObject *object)
{
	GnomeAppItem *item = GNOME_APP_ITEM (object);
	GnomeAppItemPrivate *priv = item->priv;

	if (priv->id)
		g_free (priv->id);
	if (priv->name)
		g_free (priv->name);
	if (priv->pkgname)
		g_free (priv->pkgname);
	if (priv->icon)
		g_free (priv->icon);
	if (priv->summary)
		g_free (priv->summary);
	if (priv->categories) {
	/*FIXME: add */
	}
	if (priv->mime_types) {
	/*FIXME: add */
	}
	G_OBJECT_CLASS (gnome_app_item_parent_class)->finalize (object);
}

static void
gnome_app_item_class_init (GnomeAppItemClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_item_dispose;
	object_class->finalize = gnome_app_item_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppItemPrivate));
}

GnomeAppItem *
gnome_app_item_new (void)
{
	return g_object_new (GNOME_TYPE_APP_ITEM, NULL);
}

GnomeAppItem *
gnome_app_item_new_with_node (gpointer data)
{
	GnomeAppItem *item;
	xmlNodePtr node = (xmlNodePtr) data;

	item = g_object_new (GNOME_TYPE_APP_ITEM, NULL);
	set_content (item, node);

	if (item->priv->tag != APP_ONLINE) {
		g_object_unref (item);
		return NULL;
	}
		
	return item;
}

const gchar *
gnome_app_item_get_pkgname (GnomeAppItem *item)
{
        GnomeAppItemPrivate *priv = item->priv;

	return priv->pkgname;
}

const gchar *
gnome_app_item_get_name (GnomeAppItem *item)
{
        GnomeAppItemPrivate *priv = item->priv;

	return priv->name;
}

/**
 * gnome_app_item_get_icon
 *
 * Make up a decent icon for this application, 
 * FIXME: add more args, for it at the given size.
 *
 * Return value: (transfer none): A floating #ClutterActor
 */

/*FIXME: should clean the is default to the server side */
ClutterActor *
gnome_app_item_get_icon (GnomeAppItem *item)
{
	gchar *icon_name;
	const gchar *icon_uri;
	gchar *app_name;

	icon_name = item->priv->icon;
	if (icon_name) {
		gchar *p, *content;
		content = g_strdup (icon_name);
		p = strrchr (content, '.');
		if (p)
			*p = 0;
		icon_uri = get_icon_uri (content);
		g_free (content);
	} else {
		icon_uri = NULL;
	}

	app_name = item->priv->name;

	ClutterActor *actor, *box, *text;
	ClutterActor *icon = NULL; //*FIXME we have icon, but can not be recognized by clutter
	ClutterLayoutManager *layout;
	ClutterAction *action;

	layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (layout), 6);

	box = clutter_box_new (layout);
	text = clutter_text_new ();
	clutter_text_set_ellipsize (CLUTTER_TEXT (text), PANGO_ELLIPSIZE_END);
	clutter_text_set_text (CLUTTER_TEXT (text), app_name);
	clutter_actor_set_width (text, 64);
	clutter_container_add_actor (CLUTTER_CONTAINER (box), text);

	if (icon_uri) {
		icon = clutter_texture_new_from_file (icon_uri, NULL);
	}

	if (!icon) {
		item->priv->is_default_icon = TRUE;
		gchar *default_icon_uri;
		default_icon_uri = get_default_icon ();
		icon = clutter_texture_new_from_file (default_icon_uri, NULL);
		g_free (default_icon_uri);
	}
	clutter_actor_set_width (icon, 64);
	clutter_actor_set_height (icon, 64);
	clutter_container_add_actor (CLUTTER_CONTAINER (box), icon);
	clutter_actor_set_reactive (box, TRUE);

 	g_signal_connect_swapped (box, "button-press-event",
                            G_CALLBACK (gnome_app_install), item->priv->pkgname);

	return CLUTTER_ACTOR (box);
}

gboolean
gnome_app_item_is_default_icon (GnomeAppItem *item)
{
	return item->priv->is_default_icon;
}
