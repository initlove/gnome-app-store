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
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-texture.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"

#define SPACE_WIDTH 40

struct _GnomeAppCommentsPrivate
{
	ClutterActor *viewport;
	ClutterAction *action_x;
	ClutterAction *action_y;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	ClutterActor *spin;

	/*This should get from comment actor*/
	gint view_height;
	gint view_width;

	gchar *content;
	gchar *content2;

	gboolean lock;
	GnomeAppTask *task;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LOCK_STATUS,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppComments, gnome_app_comments, CLUTTER_TYPE_GROUP)

static void
gnome_app_comments_set_lock (GnomeAppComments *comments, const gchar *str)
{
	g_return_if_fail (str);

	GnomeAppCommentsPrivate *priv;
	gboolean _lock;

	if (strcmp (str, "lock") == 0)
		_lock = TRUE;
	else if (strcmp (str, "unlock") == 0)
		_lock = FALSE;
	else
		return;

	priv = comments->priv;
	if (priv->lock == _lock) {
		g_critical ("You should not %s more than once!\tFrameUI\n", str);
		return;
	}
	priv->lock = _lock;
	if (priv->lock) {
		gnome_app_texture_start (GNOME_APP_TEXTURE (priv->spin));
		clutter_actor_hide (CLUTTER_ACTOR (priv->layout_box));
	} else {
		gnome_app_texture_stop (GNOME_APP_TEXTURE (priv->spin));
		clutter_actor_show (CLUTTER_ACTOR (priv->layout_box));
	}
}

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

	if (y > 0) {
  		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"y", 0.0,
				NULL);
	}
       	if (y < (comments->priv->view_height - height)) {
		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"y", comments->priv->view_height - height,
				NULL);
	}
#if 0
	if (x > 0) {
  		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"x", 0.0,
				NULL);
	}
       	if (x < (comments->priv->view_width -width)) {
		clutter_actor_animate (comments->priv->layout_box, CLUTTER_EASE_OUT_BOUNCE, 250,
				"x", comments->priv->view_width - width,
				NULL);
	}
#endif
}

static void
gnome_app_comments_init (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv;

	comments->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (comments,
	                                                 GNOME_APP_TYPE_COMMENTS,
	                                                 GnomeAppCommentsPrivate);
	clutter_actor_set_clip_to_allocation (CLUTTER_ACTOR (comments), TRUE);
	priv->layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->layout), TRUE);
	priv->layout_box = clutter_box_new (priv->layout);
#if 0
	priv->action_y = clutter_drag_action_new ();
    	clutter_actor_add_action (priv->layout_box, priv->action_y);
      	clutter_actor_set_reactive (priv->layout_box, TRUE);
   	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action_y),
					CLUTTER_DRAG_Y_AXIS);
        g_signal_connect (priv->action_y, "drag-end", G_CALLBACK (on_drag_end), comments);
#endif
  
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), priv->layout_box);

	priv->view_width = 350.0;
	priv->view_height = 600.0;
	priv->content = NULL;
	priv->content2 = NULL;
	priv->task = NULL;
	priv->lock = FALSE;
	priv->spin = CLUTTER_ACTOR (gnome_app_texture_new ());
	g_object_set (G_OBJECT (priv->spin), "texture-type", "spin", NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), CLUTTER_ACTOR (priv->spin));
	clutter_actor_set_position (priv->spin, priv->view_width/2, 50);
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

	if (priv->task)
		g_object_unref (priv->task);
	if (priv->content)
		g_free (priv->content);
	if (priv->content2)
		g_free (priv->content2);

	G_OBJECT_CLASS (gnome_app_comments_parent_class)->finalize (object);
}

static void
gnome_app_comments_set_property (GObject      *object,
		  		guint         prop_id,
				const GValue *value,
				GParamSpec   *pspec)
{
	GnomeAppComments *comments;
	const gchar *str;

	comments = GNOME_APP_COMMENTS (object);
	switch (prop_id)
	{
		case PROP_LOCK_STATUS:
			str = g_value_get_string (value);
			if (!str)
				return;
			gnome_app_comments_set_lock (comments, str);
			break;
	}
}

static void
gnome_app_comments_get_property (GObject      *object,
		                guint         prop_id,
		                GValue       *value,
		                GParamSpec   *pspec)
{
        GnomeAppComments *comments;

	comments = GNOME_APP_COMMENTS (object);
	switch (prop_id)
	{
			
	}
}

static void
gnome_app_comments_class_init (GnomeAppCommentsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_comments_set_property;
	object_class->get_property = gnome_app_comments_get_property;
	object_class->dispose = gnome_app_comments_dispose;
	object_class->finalize = gnome_app_comments_finalize;
	 
	g_object_class_install_property (object_class,
			PROP_LOCK_STATUS,
			g_param_spec_string ("lock-status",
				"Lock Status",
				"Lock Status",
				NULL,
				G_PARAM_READWRITE));

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
	GnomeAppComments *app_comments;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	app_comments = GNOME_APP_COMMENTS (userdata);
	gnome_app_comments_load (app_comments, results);

	return NULL;
}

GnomeAppComments *
gnome_app_comments_new_with_content (const gchar *content, const gchar *content2)
{
	GnomeAppComments *comments;
	GnomeAppCommentsPrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	comments = g_object_new (GNOME_APP_TYPE_COMMENTS, NULL);
	priv = comments->priv;
	if (content)
		priv->content = g_strdup (content);
	if (content2)
		priv->content2 = g_strdup (content2);

	
	function = g_strdup_printf ("/v1/comments/data/1/%s/0", content);
	task = gnome_app_task_new (comments, "GET", function);
	priv->task = g_object_ref (task);
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
	GList *list;
	GnomeAppCommentsPrivate *priv;
       
	priv = comments->priv;
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (priv->layout_box)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (priv->layout_box), CLUTTER_ACTOR (list->data));
}

void
gnome_app_comments_add_actor (GnomeAppComments *comments, ClutterActor *actor)
{
	GnomeAppCommentsPrivate *priv = comments->priv;
}

static void
on_comment_refresh_cb (GnomeAppComment *comment, gpointer userdata)
{
	printf ("signal caught\n");
	GnomeAppComments *comments;
	GnomeAppCommentsPrivate *priv;
	GnomeAppTask *task;

	comments = GNOME_APP_COMMENTS (userdata);
	priv = comments->priv;
	task = g_object_ref (priv->task);
	gnome_app_task_push (task);
}

static void
add_child_comments (GnomeAppComments *comments,
		    OpenResult *comment, gint level)
{
	OpenResult *child_comment;
	GnomeAppComment *ui_comment;
	GnomeAppCommentsPrivate *priv;
	ClutterLayoutManager *layout;
	ClutterLayoutManager *child_layout;
	ClutterActor *child_box;
	ClutterActor *space_actor;
	GList *l;

	priv = comments->priv;
	layout = priv->layout;
	for (l = open_result_get_child (comment); l; l = l->next) {
		child_comment = l->data;
		ui_comment = gnome_app_comment_new_with_comment (child_comment);
		gnome_app_comment_set_content (ui_comment, priv->content, priv->content2);
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

		g_signal_connect (G_OBJECT (ui_comment), "refresh", G_CALLBACK (on_comment_refresh_cb), comments);
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
		g_signal_connect (G_OBJECT (ui_comment), "refresh", G_CALLBACK (on_comment_refresh_cb), comments);
	}
}

