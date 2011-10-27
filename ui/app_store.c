#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-config.h"
#include "gnome-app-store.h"
#include "gnome-app-ui-score.h"
#include "gnome-app-stage.h"
#include "gnome-app-info-ui.h"

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

	script = clutter_script_new ();
	clutter_script_load_from_file (script, filename, NULL);
	clutter_script_get_objects (script, "main-stage", &stage, NULL);

	gchar *prop [] = {
		"name", "personid", "description", 
		"score",
		"smallpreviewpic1", "previewpic1", 
		"license", NULL};

	const gchar *val;
	gchar *local_uri;

//	gnome_app_info_debug (info);
	for (i = 0; prop [i]; i++) {
		clutter_script_get_objects (script, prop [i], &actor, NULL);
		if (!actor)
			continue;
		val = gnome_app_info_get (info, prop [i]);
		if (CLUTTER_IS_TEXTURE (actor)) {
			local_uri = gnome_app_get_local_icon (val);
			clutter_texture_set_from_file (actor, local_uri, NULL);
			g_free (local_uri);
		} else if (CLUTTER_IS_TEXT (actor)) {
			clutter_text_set_text (actor, val);
		} 
#if 0
/*FIXME: cannot use the user defined object? */
			else if (GNOME_APP_IS_UI_SCORE (actor)) {
			gnome_app_ui_score_set_score (actor, val);
		}
#endif
	}

	gchar *scores [] = { "score-1", "score-2", "score-3", "score-4", "score-5", NULL};
	gint app_score = atoi (gnome_app_info_get (info, "score")) / 20;
	for (i = 0; scores [i]; i++) {
		clutter_script_get_objects (script, scores [i], &actor, NULL);
		if (!actor)
			continue;
		if (i < app_score)
			clutter_texture_set_from_file (actor, "/home/novell/gnome-app-store/pixmaps/starred.png", NULL);
		else
			clutter_texture_set_from_file (actor, "/home/novell/gnome-app-store/pixmaps/non-starred.png", NULL);
	}
        clutter_actor_show (stage);
	clutter_main ();

	return EXIT_SUCCESS;
}
