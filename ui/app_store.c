#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-info.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	GnomeAppStoreUI *store_ui;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;


#if 1
	stage = gnome_app_store_ui_get_default ();
#else
	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), _("AppStore"));
	clutter_actor_set_size (stage, 900, 660);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);
        const GnomeAppStore *store;
	ClutterActor *page;
	GnomeAppInfo *info;
	GnomeAppQuery *query;
	GList *list, *l;

        store = gnome_app_store_get_default ();
	query = gnome_app_query_new ();
	g_object_set (query, "page", "0", "pagesize", "5", NULL);
        list = gnome_app_store_get_apps_by_query ((GnomeAppStore *)store, query);

	l = list;
	info = l->data;
	gnome_app_info_debug (info);
	page = gnome_app_info_page_new_with_app (info);
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (page));
#endif
	clutter_actor_show_all (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
