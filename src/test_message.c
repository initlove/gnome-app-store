#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-message-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;

  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();
	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), "message test");
	clutter_actor_set_size (stage, 400, 600);
	clutter_actor_show_all (stage);

	ClutterActor *message;
	GnomeAppStore *store;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (store, clutter_threads_leave);

	message = CLUTTER_ACTOR (gnome_app_message_ui_new ());
	clutter_container_add (CLUTTER_CONTAINER (stage), message, NULL);

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
