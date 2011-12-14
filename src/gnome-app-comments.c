/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <clutter/clutter.h>
#include "open-result.h"
#include "gnome-app-store.h"
#include "gnome-app-task.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"

#define SPACE_WIDTH 40

struct _GnomeAppCommentsPrivate
{
	GnomeAppStore *store;

	ClutterActor *viewport;
	ClutterAction *action_x;
	ClutterAction *action_y;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;

	/*This should get from comment actor*/
	gint view_height;
	gint view_width;

	gchar *content;
	gchar *content2;
};

G_DEFINE_TYPE (GnomeAppComments, gnome_app_comments, CLUTTER_TYPE_GROUP)

static void
on_drag_end (ClutterDragAction   *action,
             ClutterActor        *actor,
             gfloat               event_x,
             gfloat               event_y,
             ClutterModifierType  modifiers,
	     gpointer		  userdata)
{
	GnomeAppComments *comments;
	gfloat x, y, width, height;

	comments = GNOME_APP_COMMENTS (userdata);
	x = clutter_actor_get_x (comments->priv->layout_box);
	y = clutter_actor_get_y (comments->priv->layout_box);
	width = clutter_actor_get_width (comments->priv->layout_box);
	height = clutter_actor_get_height (comments->priv->layout_box);

	if (x > 0) {
  		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"x", 0.0,
				NULL);
	}
	if (y > 0) {
  		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"y", 0.0,
				NULL);
	}
       	if (x < (comments->priv->view_width -width)) {
		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"x", comments->priv->view_width - width,
				NULL);
	}
       	if (y < (comments->priv->view_height - height)) {
		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"y", comments->priv->view_height - height,
				NULL);
	}

}

static void
gnome_app_comments_init (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv;

	comments->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (comments,
	                                                 GNOME_APP_TYPE_COMMENTS,
	                                                 GnomeAppCommentsPrivate);

	priv->layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->layout), TRUE);
	priv->layout_box = clutter_box_new (priv->layout);

	priv->action_x = clutter_drag_action_new ();
    	clutter_actor_add_action (priv->layout_box, priv->action_x);
   	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action_x),
					CLUTTER_DRAG_X_AXIS);
        g_signal_connect (priv->action_x, "drag-end", G_CALLBACK (on_drag_end), comments);

	priv->action_y = clutter_drag_action_new ();
    	clutter_actor_add_action (priv->layout_box, priv->action_y);
   	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action_y),
					CLUTTER_DRAG_Y_AXIS);
        g_signal_connect (priv->action_y, "drag-end", G_CALLBACK (on_drag_end), comments);

      	clutter_actor_set_reactive (priv->layout_box, TRUE);
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), priv->layout_box);

	priv->view_width = 300.0;
	priv->view_height = 600.0;
	priv->content = NULL;
	priv->content2 = NULL;
}

static void
gnome_app_comments_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_comments_parent_class)->dispose (object);
}

static void
gnome_app_comments_finalize (GObject *object)
{
	GnomeAppComments *comments = GNOME_APP_COMMENTS (object);
	GnomeAppCommentsPrivate *priv = comments->priv;

	if (priv->content)
		g_free (priv->content);
	if (priv->content2)
		g_free (priv->content2);

	G_OBJECT_CLASS (gnome_app_comments_parent_class)->finalize (object);
}

static void
gnome_app_comments_class_init (GnomeAppCommentsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_comments_dispose;
	object_class->finalize = gnome_app_comments_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppCommentsPrivate));
}

GnomeAppComments *
gnome_app_comments_new (void)
{
	return g_object_new (GNOME_APP_TYPE_COMMENTS, NULL);
}

static gpointer
set_comments_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppComments *ui_comments;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	ui_comments = GNOME_APP_COMMENTS (userdata);
	gnome_app_comments_load (ui_comments, results);

	return NULL;
}

GnomeAppComments *
gnome_app_comments_new_with_content (const gchar *content, const gchar *content2)
{
	GnomeAppComments *comments;
	GnomeAppTask *task;
	gchar *function;

	comments = g_object_new (GNOME_APP_TYPE_COMMENTS, NULL);
	if (content)
		comments->priv->content = g_strdup (content);
	if (content2)
		comments->priv->content2 = g_strdup (content2);

	
	function = g_strdup_printf ("/v1/comments/data/1/%s/0", content);
	task = gnome_app_task_new (comments, "GET", function);
	gnome_app_task_add_params (task,
			"pagesize", "10",
			"page", "0",
			NULL);
	gnome_app_task_set_callback (task, set_comments_callback);
	gnome_app_task_push (task);

	g_free (function);

	return comments;
}

void
gnome_app_comments_clean (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv = comments->priv;

}

void
gnome_app_comments_add_actor (GnomeAppComments *comments, ClutterActor *actor)
{
	GnomeAppCommentsPrivate *priv = comments->priv;
}

static void
add_child_comments (GnomeAppComments *comments,
		    OpenResult *comment, gint level)
{
	ClutterLayoutManager *layout;
	OpenResult *child_comment;
	GnomeAppComment *ui_comment;
	ClutterLayoutManager *child_layout;
	ClutterActor *child_box;
	ClutterActor *space_actor;
	GList *l;

	layout = comments->priv->layout;
	for (l = open_result_get_child (comment); l; l = l->next) {
		child_comment = l->data;
		ui_comment = gnome_app_comment_new_with_comment (child_comment);
		gnome_app_comment_set_content (ui_comment, comments->priv->content, comments->priv->content2);

		child_layout = clutter_box_layout_new ();
	        clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (child_layout), FALSE);
		child_box = clutter_box_new (child_layout);
		space_actor = clutter_text_new ();
		clutter_actor_set_width (space_actor, SPACE_WIDTH * level);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (child_layout), space_actor,
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (child_layout), CLUTTER_ACTOR (ui_comment),
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);

		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), child_box,
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);

		add_child_comments (comments, child_comment, level + 1);
	}
}

void
gnome_app_comments_load (GnomeAppComments *comments, OpenResults *results)
{
	OpenResult *comment;
	GnomeAppComment *ui_comment;
	GList *l;
	gint level;

	gnome_app_comments_clean (comments);

	if (!open_results_get_status (results)) {
		g_debug ("Fail to load comments %s.", open_results_get_meta (results, "message"));
		return;
	}
	level = 1;
	g_debug ("total item of this comments %d\n", open_results_get_total_items (results));

	for (l = open_results_get_data (results); l; l = l->next) {
		comment = l->data;
		ui_comment = gnome_app_comment_new_with_comment (comment);
		gnome_app_comment_set_content (ui_comment, comments->priv->content, comments->priv->content2);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (comments->priv->layout), CLUTTER_ACTOR (ui_comment),
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);
		add_child_comments (comments, comment, level);
	}
}

