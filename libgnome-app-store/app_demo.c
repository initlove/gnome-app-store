#include "gnome-app-store.h"
#include "gnome-app-item.h"
#include <clutter/clutter.h>
#include "local-app.h"
int
main (int argc, char **argv)
{
	GnomeAppStore *store;
	GSList *list, *l;

	g_thread_init (NULL);

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
		return 1;
	ClutterActor *stage, *box;

#if 0
	debug_local_apps ();
	return 0;
#endif

	store = gnome_app_store_new ();

	list = gnome_app_store_get_apps (store);

	printf ("\n\n%d\n", gnome_app_store_get_counts (store));

	GnomeAppItem *item;

	item = gnome_app_store_get_nth_app (store, 3);
	gchar *name = gnome_app_item_get_name (item);
	printf ("name %s\n", name);
#if 0
	printf ("%s %s\n", gnome_app_store_get_nth_app (store, 2), gnome_app_store_get_nth_app (store, 3));

	stage = clutter_stage_get_default ();

	int i, is_default; 
	const gchar *id;
	for (i = 0; i < 20; i++) {
		id = gnome_app_store_get_nth_app (store, i);
		box = gnome_app_store_get_icon_from_id (store, id);
		clutter_container_add_actor (CLUTTER_CONTAINER (stage), box);
	}
	clutter_actor_show_all (stage);
	clutter_main ();
#endif

	return 0;
} 
