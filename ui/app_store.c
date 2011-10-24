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

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

        stage = clutter_stage_new ();
        clutter_stage_set_title (CLUTTER_STAGE (stage), _("AppStore"));
        clutter_actor_set_size (stage, 900, 600);
        g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	gchar *img;
	GError *error = NULL;
	if (argc <1)
		img = "http://openDesktop.org/CONTENT/content-pre1/144591-1.png";
	else
		img = argv [1];
	actor = clutter_texture_new ();
	clutter_texture_set_from_file (actor, img, &error);
	clutter_actor_set_width (actor, 20);
	clutter_actor_set_height (actor, 20);
	if (error) {
		printf ("error msg :%s \n", error->message);
		g_error_free (error);
	}
        clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (actor));

        clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
