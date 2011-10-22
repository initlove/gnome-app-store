#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-config.h"
#include "gnome-app-store.h"

#include "gnome-app-stage.h"
#include "gnome-app-info-ui.h"

#define TEST_SERVER FALSE
#define TEST_GAME FALSE

/* remove all the contents . */
int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	ClutterActor *scroll, *box;
	GnomeAppConfig *conf;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	clutter_main ();

	return EXIT_SUCCESS;
}
