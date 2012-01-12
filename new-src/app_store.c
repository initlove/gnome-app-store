#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include "open-result.h"
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-info-page.h"
#include "gnome-app-login-user.h"
#include "gnome-app-search-entry.h"
#include "gnome-app-icon-view.h"
#include "gnome-app-application.h"


int 
main (gint argc, gchar **argv)
{
	GnomeAppApplication *app;
	gint status;

	app = gnome_app_application_new ();
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
