#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-application.h"
#include "gnome-app-info-icon.h"
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
	ClutterActor *stage;
	GnomeAppApplication *app;

  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();
	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);

#if 1
	app = gnome_app_application_new ();
	stage = CLUTTER_ACTOR (app);
	clutter_actor_show_all (stage);
#else
	GnomeAppInfoIcon *info_icon;
	        
	info_icon = g_object_new (GNOME_APP_TYPE_INFO_ICON, NULL);
	g_signal_connect (info_icon, "selected", G_CALLBACK (on_info_icon_clicked), NULL);
	g_object_unref (info_icon);
#endif

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
