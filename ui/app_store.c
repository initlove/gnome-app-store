#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-config.h"
#include "gnome-app-store.h"
#include "gnome-app-stage.h"
#include "gnome-app-info-icon.h"

#define TEST_SERVER FALSE
#define TEST_GAME FALSE

static ClutterScript *script = NULL;
static guint merge_id = 0;

static gboolean
blue_button_press (ClutterActor       *actor,
                   ClutterButtonEvent *event,
                   gpointer            data)
{
  g_print ("[*] Pressed '%s'\n", clutter_get_script_id (G_OBJECT (actor)));
  g_print ("[*] Unmerging objects with merge id: %d\n", merge_id);

  clutter_script_unmerge_objects (script, merge_id);

  return TRUE;
}

static gboolean
red_button_press (ClutterActor *actor,
                  ClutterButtonEvent *event,
                  gpointer            data)
{
  GObject *timeline;

  g_print ("[*] Pressed '%s'\n", clutter_get_script_id (G_OBJECT (actor)));

  timeline = clutter_script_get_object (script, "main-timeline");
  g_assert (CLUTTER_IS_TIMELINE (timeline));

  if (!clutter_timeline_is_playing (CLUTTER_TIMELINE (timeline)))
    clutter_timeline_start (CLUTTER_TIMELINE (timeline));
  else
    clutter_timeline_pause (CLUTTER_TIMELINE (timeline));

  return TRUE;
}

/* remove all the contents . */
int
main (int argc, char *argv[])
{
	gchar *filename;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
		return 1;

	if (argc > 1)
		filename = argv [1];
	else
		filename = "/home/novell/gnome-app-store/ui/scripts/page-info.json";

	ClutterActor *stage, *actor;
	GnomeAppInfo *info;
        GnomeAppQuery *query;
	GnomeAppStore *store;
        GList *l, *list;
	gint i;

        g_type_init ();

	store = gnome_app_store_get_default ();
        query = gnome_app_query_new ();
        g_object_set (query, QUERY_PAGESIZE, "35", QUERY_PAGE, "0", NULL);
        list = gnome_app_store_get_apps_by_query (store, query);
	l = list;
	l = l->next;
	info = l->data;

	ClutterActor *real_stage = clutter_stage_new ();
        clutter_stage_set_title (CLUTTER_STAGE (real_stage), _("AppStore"));
        clutter_actor_set_size (real_stage, 900, 600);
        g_signal_connect (real_stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

	ClutterActor *page = gnome_app_info_page_new_with_app (info);
	
        clutter_actor_show (real_stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
