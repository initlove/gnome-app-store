#include <config.h>
#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-application.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

static void
on_info_icon_clicked (ClutterActor *info_icon,
		OpenResult *info,
		gpointer userdata)
{
	printf ("1\n");
}

int
main (int argc, char *argv[])
{
	GnomeAppStore *store;
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

	gnome_app_auth_valid ();

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
