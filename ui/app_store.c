#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	GnomeAppStoreUI *store_ui;

  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();

	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);

	store_ui = gnome_app_store_ui_get_default ();
	stage = CLUTTER_ACTOR (store_ui);
	clutter_actor_show_all (stage);

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
