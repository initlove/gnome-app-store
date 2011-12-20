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
#if 1
	ClutterActor *message;
	GnomeAppStore *store;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (store, clutter_threads_leave);

	message = CLUTTER_ACTOR (gnome_app_message_ui_new ());
	clutter_container_add (CLUTTER_CONTAINER (stage), message, NULL);
#else
	ClutterActor *page, *actor;
	ClutterScript *script;
	GError *error;

	script = clutter_script_new ();
	error = NULL;
	clutter_script_load_from_file (script, "/home/dliang/gnome-app-store/ui/message-info-page.json", &error);
//	clutter_script_load_from_file (script, "/home/dliang/gnome-app-store/ui/test.json", &error);
	if (error)
		printf ("error in load %s\n", error->message);
        clutter_script_get_objects (script, "message-info-page", &page, "sender_label", &actor, NULL);
	ClutterLayoutManager *layout;
	layout = clutter_box_get_layout_manager (page);
	gint col, row;
	clutter_layout_manager_child_get (layout, page, actor, "column", &col, "row", &row, NULL);
	printf ("col %d %d\n", col, row);
	clutter_layout_manager_child_set (layout, page, actor, "column", 2, "row", 1, NULL);
	clutter_container_add (CLUTTER_CONTAINER (stage), page, NULL);
#endif
	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
