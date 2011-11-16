#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "open-services.h"
#include "gnome-app-store.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	GnomeAppStoreUI *store_ui;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	stage = CLUTTER_ACTOR (gnome_app_store_ui_get_default ());
	clutter_actor_show_all (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
