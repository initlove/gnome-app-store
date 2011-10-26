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
	ClutterActor *actor;
	GnomeAppConfig *conf;
	ClutterScript *script;
	gchar *filename;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
		return 1;

	if (argc > 1)
		filename = argv [1];
	else
		filename = "/home/novell/gnome-app-store/ui/scripts/test-script.json";

	script = clutter_script_new ();
	clutter_script_load_from_file (script, filename, NULL);
	clutter_script_get_objects (script, "main-stage", &stage, NULL);

        clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
