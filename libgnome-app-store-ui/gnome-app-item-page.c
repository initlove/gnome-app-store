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
#include <clutter/clutter.h>

#include "gnome-app-item-page.h"

struct _GnomeAppItemPagePrivate
{
	GnomeAppItem   *app;
	GnomeAppItemUI *ui;
};

G_DEFINE_TYPE (GnomeAppItemPage, gnome_app_item_page, G_TYPE_OBJECT)

static void
gnome_app_item_page_init (GnomeAppItemPage *page)
{
	GnomeAppItemPagePrivate *priv;

	item->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (page,
							 GNOME_TYPE_APP_ITEM_PAGE,
							 GnomeAppItemPagePrivate);
}

static void
gnome_app_item_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_item_page_parent_class)->dispose (object);
}

static void
gnome_app_item_page_finalize (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_item_page_parent_class)->finalize (object);
}

static void
gnome_app_item_ui_class_init (GnomeAppItemUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_item_page_dispose;
	object_class->finalize = gnome_app_item_page_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppItemPagePrivate));
}

GnomeAppItemPage *
gnome_app_item_page_new (void)
{
	return g_object_new (GNOME_TYPE_APP_ITEM_PAGE, NULL);
}

void
gnome_app_item_page_set_app (GnomeAppItemPage *page, GnomeAppItem *app)
{
	GnomeAppItemPagePrivate *priv;

	g_return_val_if_fail (GNOME_APP_IS
}

GnomeAppItemPage *
gnome_app_item_page_new_with_app (GnomeAppItem *app)
{
	GObject *object;
	GnomeAppItemPage *page;

	page = (GnomeAppItemPage *)g_object_new (GNOME_TYPE_APP_ITEM_PAGE, NULL);
	gnome_app_item_page_set_app (page, app);
	return page;
}
