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
gnome_app_comments_init (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv;
	gchar *dir;

	comments->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (comments,
	                                                 GNOME_APP_TYPE_COMMENTS,
	                                                 GnomeAppCommentsPrivate);
	clutter_actor_set_clip_to_allocation (CLUTTER_ACTOR (comments), TRUE);
	priv->layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->layout), TRUE);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (priv->layout), 10);
	priv->layout_box = clutter_box_new (priv->layout);
  
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), CLUTTER_ACTOR (priv->layout_box));

	priv->view_width = 350.0;
	priv->view_height = 600.0;
	priv->content = NULL;
	priv->content2 = NULL;
	priv->task = NULL;
	priv->lock = FALSE;
	priv->spin = CLUTTER_ACTOR (gnome_app_texture_new ());
	dir = open_app_get_spin_dir ();
	g_object_set (G_OBJECT (priv->spin), "texture-type", "spin", "url", dir, NULL);
	g_free (dir);
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), CLUTTER_ACTOR (priv->spin));
	clutter_actor_set_position (priv->spin, 60, 50);
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
	/*TODO: this is just hack, as thread problem not totally solved */
	if (priv->lock)
		gnome_app_texture_stop (GNOME_APP_TEXTURE (priv->spin));

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


static void
proxy_call_async_cb (RestProxyCall *call,
        const GError  *error,
        GObject       *weak_object,
        gpointer       userdata)
{
	GnomeAppComments *app_comments;
	OpenResults *results;
    const gchar *payload;
    goffset len;

	app_comments = GNOME_APP_COMMENTS (userdata);
    payload = rest_proxy_call_get_payload (call);
    len = rest_proxy_call_get_payload_length (call);

    results = (OpenResults *) open_ocs_get_results (payload, len);
    if (!open_results_get_status (results)) {
        g_debug ("Fail get comments: %s\n", open_results_get_meta (results, "message"));
    } else {
        clutter_threads_enter ();
	    gnome_app_comments_load (app_comments, results);
        clutter_threads_leave ();
    }
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
	
    RestProxy *proxy;
    RestProxyCall *call;
    proxy = gnome_app_get_proxy ();
    call = rest_proxy_new_call (proxy);
     
    rest_proxy_call_add_params (call,
            "type", "1",
            "content", content,
            "pagesize", "10",
            "page", "0",
            NULL);
    rest_proxy_call_set_function (call, "comments/get");
    rest_proxy_call_async (call,
            proxy_call_async_cb,
            NULL,
            comments,
            NULL);            

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
	GnomeAppComment *app_comment;
	GnomeAppCommentsPrivate *priv;
	ClutterLayoutManager *layout;
	ClutterLayoutManager *child_layout;
	ClutterActor *child_box;
	ClutterActor *space_actor;
	GList *l;

	priv = comments->priv;
	layout = priv->layout;
	for (l = open_result_get_child (comment, "children"); l; l = l->next) {
		child_comment = l->data;
		app_comment = gnome_app_comment_new_with_comment (child_comment);
		gnome_app_comment_set_content (app_comment, priv->content, priv->content2);
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
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (child_layout), CLUTTER_ACTOR (app_comment),
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);

		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), CLUTTER_ACTOR (child_box),
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);

		add_child_comments (comments, child_comment, level + 1);

		g_signal_connect (G_OBJECT (app_comment), "refresh", G_CALLBACK (on_comment_refresh_cb), comments);
	}
}

void
gnome_app_comments_load (GnomeAppComments *comments, OpenResults *results)
{
	GnomeAppCommentsPrivate *priv;;
	OpenResult *comment;
	GnomeAppComment *app_comment;
	GList *l;
	gint level;

	gnome_app_comments_clean (comments);

	if (!open_results_get_status (results)) {
		g_debug ("Fail to load comments %s.", open_results_get_meta (results, "message"));
		return;
	}
	level = 1;

	priv = comments->priv;
	for (l = open_results_get_data (results); l; l = l->next) {
		comment = l->data;
		app_comment = gnome_app_comment_new_with_comment (comment);
		gnome_app_comment_set_content (app_comment, priv->content, priv->content2);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->layout), CLUTTER_ACTOR (app_comment),
					     FALSE,  /* expand */
		                             FALSE, /* x-fill */
				    	     FALSE, /* y-fill */
					     CLUTTER_BOX_ALIGNMENT_START,
					     CLUTTER_BOX_ALIGNMENT_START);
		add_child_comments (comments, comment, level);
		g_signal_connect (G_OBJECT (app_comment), "refresh", G_CALLBACK (on_comment_refresh_cb), comments);
	}
}

