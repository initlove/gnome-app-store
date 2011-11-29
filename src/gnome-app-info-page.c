/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author: Lance Wang <lzwang@suse.com>

*/
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-comment.h"
#include "gnome-app-store-ui.h"
#include "gnome-app-score-ui.h"
#include "gnome-app-info-page.h"

struct _GnomeAppInfoPagePrivate
{
	OpenResult *info;
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static void
gnome_app_info_page_init (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;

	page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (page,
							 GNOME_APP_TYPE_INFO_PAGE,
							 GnomeAppInfoPagePrivate);
	priv->info = NULL;
}

static void
gnome_app_info_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->dispose (object);
}

static void
gnome_app_info_page_finalize (GObject *object)
{
	GnomeAppInfoPage *page = GNOME_APP_INFO_PAGE (object);
	GnomeAppInfoPagePrivate *priv = page->priv;

	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->finalize (object);
}

static void
gnome_app_info_page_class_init (GnomeAppInfoPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_info_page_dispose;
	object_class->finalize = gnome_app_info_page_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPagePrivate));
}

static gboolean
on_info_page_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
        GnomeAppStoreUI *store_ui;

        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
                store_ui = gnome_app_store_ui_get_default ();
                gnome_app_store_ui_load_frame_ui (store_ui);

                break;

        }
        return TRUE;
}

static void
set_pic_callback (gpointer userdata, gpointer func_re)
{
        ClutterActor *actor;
        gchar *dest_url;

        actor = CLUTTER_ACTOR (userdata);
        dest_url = (gchar *) func_re;
/*TODO: why should use this thread? */
        clutter_threads_enter ();
        clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), dest_url, NULL);
        clutter_threads_leave ();
}

static void
set_comments_callback (gpointer userdata, gpointer func_result)
{
	ClutterActor *comment;
	ClutterActor *comment_group;
	OpenResults *results;
	OpenResult *result;
	GList *list, *l;

	results = OPEN_RESULTS (func_result);
	comment_group = CLUTTER_ACTOR (userdata);

	/* TODO: in fact, I don't know why the position is no longer in use, we should confirm to set it .. */
	/* Do it later */

	list = open_results_get_data (results);

        clutter_threads_enter ();
	for (l = list; l; l = l->next) {
		result = l->data;
//		open_result_debug (result);
		comment = gnome_app_comment_new_with_comment (result);
		clutter_container_add_actor (CLUTTER_CONTAINER (comment_group), comment);
break;
	}
        clutter_threads_leave ();
}

GnomeAppInfoPage *
gnome_app_info_page_new_with_app (OpenResult *info)
{
	GnomeAppInfoPage *page;

	g_return_val_if_fail (info != NULL, NULL);

	page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
	page->priv->info = g_object_ref (info);

	gchar *filename;
	GError *error;
	ClutterScript *script;
	ClutterActor *actor;
	gint i;

	filename = open_app_get_ui_uri ("app-info-page");
	error = NULL;

	script = clutter_script_new ();
	clutter_script_load_from_file (script, filename, &error);
	if (error) {
		printf ("error in load script %s\n", error->message);
		g_error_free (error);
	}
	g_free (filename);
	clutter_script_get_objects (script, "info-page", &page, NULL);
        
	gchar *prop [] = {
		"name", "personid", "description", 
		"score", "downloads", "comments",
		"smallpreviewpic1", "previewpic1", 
		"license", NULL};

	const gchar *val;
	gchar *local_uri;

	for (i = 0; prop [i]; i++) {
		clutter_script_get_objects (script, prop [i], &actor, NULL);
		if (!actor)
			continue;
		val = open_result_get (info, prop [i]);
		if (!val)
			continue;
		if (CLUTTER_IS_TEXTURE (actor)) {
	                GnomeAppTask *task;

        	        task = gnome_download_task_new (actor, val);
                	gnome_app_task_set_callback (task, set_pic_callback);
	                gnome_app_task_push (task);
		} else if (CLUTTER_IS_TEXT (actor)) {
			if ((strcmp (prop [i], "comments") == 0) || (strcmp (prop [i], "downloads") == 0)) {
				gchar *val_label;

				val_label = g_strdup_printf ("%s %s", val, prop [i]);
				clutter_text_set_text (CLUTTER_TEXT (actor), val_label);
				g_free (val_label);
			} else
				clutter_text_set_text (CLUTTER_TEXT (actor), val);
		} 
#if 0
/*FIXME: cannot use the user defined object? */
			else if (GNOME_APP_IS_UI_SCORE (actor)) {
			gnome_app_ui_score_set_score (actor, val);
		}
#endif
	}

	ClutterActor *score_actor;

	clutter_script_get_objects (script, "score", &actor, NULL);
	score_actor = gnome_app_score_ui_new_with_score (open_result_get (info, "score"));
	clutter_container_add_actor (CLUTTER_CONTAINER (actor), score_actor);

/*TODO: how many comments shoude merge to comments .. */
	gchar *comment_count;
	clutter_script_get_objects (script, "comments-details", &actor, NULL);
	if (actor) {
		GnomeAppTask *task;
		const gchar *id;
		gchar *function;

		id = open_result_get (info, "id");
		function = g_strdup_printf ("/v1/comments/data/1/%s/0", id);
		task = gnome_app_task_new (actor, "GET", function,
                                "pagesize", "10",
                                "page", "0",
                                NULL);
		gnome_app_task_set_callback (task, set_comments_callback);
		gnome_app_task_push (task);

		g_free (function);
	}

        g_signal_connect (page, "event", G_CALLBACK (on_info_page_event), NULL);

	return page;
}
