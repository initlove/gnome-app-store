#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <glib/gi18n.h>

#include "gnome-app-store.h"
#include "gnome-app-task.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"


static gpointer
set_comments_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppComments *app_comments;
        ClutterActor *comment;
        ClutterActor *comment_group;
        OpenResults *results;
        OpenResult *result;
        GList *list, *l;

        results = OPEN_RESULTS (func_result);
	app_comments = GNOME_APP_COMMENTS (userdata);
	gnome_app_comments_load (app_comments, results);
}

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
	GnomeAppTask *task;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, clutter_threads_enter);
	gnome_app_store_set_unlock_function (store, clutter_threads_leave);

	comments = gnome_app_comments_new_with_content ("94391", NULL);
	clutter_container_add (CLUTTER_CONTAINER (stage), comments, NULL);

	task = gnome_app_task_new (comments, "GET", "/v1/comments/data/1/94391/0");
	gnome_app_task_add_params (task,
			"pagesize", "10",
			"page", "0",
			NULL);
				
	gnome_app_task_set_callback (task, set_comments_callback);
	gnome_app_task_push (task);


	g_main_loop_run (loop);
  	g_main_loop_unref (loop);

	return EXIT_SUCCESS;
}
