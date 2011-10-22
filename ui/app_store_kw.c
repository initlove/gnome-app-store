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

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	ClutterActor *scroll, *box;
	GnomeAppConfig *conf;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	conf = gnome_app_config_new ();

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), _("Scrolling"));
	clutter_actor_set_size (stage, 800, 600);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	/* scroll: the group that contains the scrolling viewport; we set its
	 * size to be the same as one rectangle, position it in the middle of
	 * the stage and set it to clip its contents to the allocated size
	 */
	scroll = (ClutterActor *)gnome_app_stage_new ();

	clutter_container_add_actor (CLUTTER_CONTAINER (stage), scroll);
	      
	GnomeAppStore *store;
	GnomeAppInfo *info;
	GnomeAppInfoUI *info_ui;
/*TODO
	remove all the content 
*/
	clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
