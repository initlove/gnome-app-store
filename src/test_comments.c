#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>
#include "st.h"
#include "gnome-app-store.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;

  	g_type_init ();
	g_thread_init (NULL);
	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;
	clutter_threads_init ();
	GMainLoop *loop;
	loop = g_main_loop_new (NULL, FALSE);

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), "comment test");
	clutter_actor_set_size (stage, 400, 600);
	clutter_actor_show_all (stage);

	ClutterActor *comments;
	GList *data;
	GnomeAppStore *store;
	gfloat height;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (store, clutter_threads_leave);

	comments = CLUTTER_ACTOR (gnome_app_comments_new_with_content ("94391", NULL));
//	clutter_actor_set_size (comments, 300, 500);
	height = clutter_actor_get_height (comments);
printf ("height %f vs %f\n", height, clutter_actor_get_height (stage));
	ClutterActor *box;
	ClutterLayoutManager *layout;

	layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (layout, TRUE);
	box = clutter_box_new (layout);
	ClutterActor *text;
	gchar *str;
	gint i;
	for (i = 0; i < 10; i++) {
		text = clutter_text_new ();
		str = g_strdup_printf ("label %d", i);
		clutter_text_set_text (text, str);
		g_free (str);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), text,
				FALSE, /*expand*/
				FALSE, /*x-fill*/
				FALSE, /*y-fill*/
				CLUTTER_BOX_ALIGNMENT_START,
				CLUTTER_BOX_ALIGNMENT_START);
	}

	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), comments,
				FALSE, /*expand*/
				FALSE, /*x-fill*/
				FALSE, /*y-fill*/
				CLUTTER_BOX_ALIGNMENT_START,
				CLUTTER_BOX_ALIGNMENT_START);

	clutter_container_add (CLUTTER_CONTAINER (stage), box, NULL);

	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
