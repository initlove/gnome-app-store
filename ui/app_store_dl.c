#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-config.h"
#include "gnome-app-store.h"

#include "gnome-app-frame-ui.h"
#include "gnome-app-item-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	GnomeAppFrameUI *frame_ui;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), _("AppStore"));
	clutter_actor_set_size (stage, 800, 600);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	frame_ui = gnome_app_frame_ui_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (frame_ui));
	clutter_stage_set_key_focus (CLUTTER_STAGE (stage), frame_ui->search_entry);

        GnomeAppStore *store;
        GList *cid_list;
        GList *appid_list;
        GList *l, *app_l;

        store = gnome_app_store_new ();

        cid_list = gnome_app_store_get_cid_list_by_group (store, "Game");
        appid_list = gnome_app_store_get_appid_list_by_cid_list (store, cid_list);
        for (app_l = appid_list; app_l; app_l = app_l->next) {
        	GnomeAppItem *item;
	        GnomeAppItemUI *item_ui;
		ClutterActor *box;
                item = gnome_app_store_get_app_by_id (store, (gchar *)app_l->data);
                item_ui = gnome_app_item_ui_new_with_app (item);
                box = gnome_app_item_ui_get_icon (item_ui);
                gnome_app_stage_add_actor (frame_ui->stage, box);
                g_object_unref (item_ui);
        }

	clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
