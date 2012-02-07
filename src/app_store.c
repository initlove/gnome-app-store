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
  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();
	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);

	gnome_app_auth ();

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
