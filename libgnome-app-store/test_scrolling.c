#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>

#include "gnome-app-config.h"
#include "gnome-app-store.h"
#include "gnome-app-stage.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	ClutterActor *scroll, *box;
	GnomeAppItem *item;
	gint i;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	GnomeAppConfig *conf;
	conf = gnome_app_config_new ();
	printf ("se %s, ca %s\n", gnome_app_config_get_server_uri (conf), gnome_app_config_get_cache_dir (conf));

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), "Scrolling");
	clutter_actor_set_size (stage, 800, 600);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	/* scroll: the group that contains the scrolling viewport; we set its
	 * size to be the same as one rectangle, position it in the middle of
	 * the stage and set it to clip its contents to the allocated size
	 */
	scroll = gnome_app_stage_new ();

	clutter_container_add_actor (CLUTTER_CONTAINER (stage), scroll);
	      
	GnomeAppStore *store;
	store = gnome_app_store_new ();
	guint  counts = gnome_app_store_get_counts (store);

printf ("counts %d\n", counts);

	for (i = 0; i < counts; i++)
	  {
		item = gnome_app_store_get_nth_app (store, i);
		box = gnome_app_item_get_icon (item);
		if (gnome_app_item_is_default_icon (item) == FALSE) {
			gnome_app_stage_add_actor (GNOME_APP_STAGE (scroll), box);
		}
	  }
	clutter_actor_show (stage);

	clutter_main ();

	return EXIT_SUCCESS;
}
