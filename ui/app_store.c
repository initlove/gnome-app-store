#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "../libgnome-app-store/common/gnome-app-config.h"
#include "../libgnome-app-store/gnome-app-store.h"

#include "gnome-app-stage.h"
#include "gnome-app-item-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	ClutterActor *scroll, *box;
	GnomeAppConfig *conf;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	conf = gnome_app_config_new ();
	printf ("se %s, ca %s\n", gnome_app_config_get_server_uri (conf), gnome_app_config_get_cache_dir (conf));

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), _("Scrolling"));
	clutter_actor_set_size (stage, 800, 600);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	/* scroll: the group that contains the scrolling viewport; we set its
	 * size to be the same as one rectangle, position it in the middle of
	 * the stage and set it to clip its contents to the allocated size
	 */
	scroll = (ClutterActor *)gnome_app_stage_new ();

	clutter_container_add_actor (CLUTTER_CONTAINER (stage), scroll);
	      
	GnomeAppStore *store;
	GnomeAppItem *item;
	GnomeAppItemUI *item_ui;
	const GList *local_categories;
	const GList *cid_list;
	const GList *appid_list;
	GList *l, *app_l;

	store = gnome_app_store_new ();
        local_categories = (const GList *)gnome_app_get_local_categories ();
        for (l = (GList *)local_categories; l; l = l->next) {
		cid_list = gnome_app_store_get_cid_list_by_group (store, (gchar *)l->data);
		appid_list = gnome_app_store_get_appid_list_by_cid_list (store, cid_list);
		for (app_l = appid_list; app_l; app_l = app_l->next) {
			item = gnome_app_store_get_app_by_id (store, (gchar *)app_l->data);
			item_ui = gnome_app_item_ui_new_with_app (item);
			box = gnome_app_item_ui_get_icon (item_ui);
			gnome_app_stage_add_actor (GNOME_APP_STAGE (scroll), box);
			g_object_unref (item_ui);
		}
	}

	clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
