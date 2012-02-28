#include <config.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "gnome-app-account.h"
#include "gnome-app-store.h"
#include "gnome-app-stage.h"

static void
gnome_app_type_regist_all ()
{
	gnome_app_widgets_init ();
	gnome_app_icon_view_get_type ();
	gnome_app_account_get_type ();
	gnome_app_login_get_type ();
	gnome_app_register_get_type ();
	gnome_app_frame_get_type ();
	gnome_app_info_page_get_type ();
}

int
main (int argc, char *argv[])
{
	GnomeAppStore *store;
	ClutterActor *stage;
	GMainLoop *loop;

	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	setlocale (LC_ALL, "");

  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();
	loop = g_main_loop_new (NULL, FALSE);

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (store, clutter_threads_leave);

	gnome_app_type_regist_all ();

	gnome_app_stage_get_default ();

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
