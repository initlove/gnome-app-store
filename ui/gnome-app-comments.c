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
#include <math.h>
#include <clutter/clutter.h>
#include "st.h"
#include "gnome-app-store.h"
#include "gnome-app-info-icon.h"
#include "gnome-app-comments.h"

struct _GnomeAppCommentsPrivate
{
	GnomeAppStore *store;

	ClutterActor *viewport;
	ClutterAction *action;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;

	GList *app_actors;
	gint count;
	gint rows;
	gint cols;
	gint icon_width;
	gint icon_height;

};

G_DEFINE_TYPE (GnomeAppComments, gnome_app_comments, CLUTTER_TYPE_GROUP)

static void
on_drag_end (ClutterDragAction   *action,
	           ClutterActor        *actor,
	           gfloat               event_x,
	           gfloat               event_y,
	           ClutterModifierType  modifiers,
		GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv;
	gfloat viewport_x;
	gfloat offset_x;
	gint child_visible;

	priv = comments->priv;
	viewport_x = clutter_actor_get_x (priv->viewport);
	/* check if we're at the viewport edges */
	if (viewport_x > 0) {
		clutter_actor_animate (priv->viewport, CLUTTER_EASE_OUT_BOUNCE, 250,
	                           "x", 0.0,
	                           NULL);
		return;
	}

	if (viewport_x < (-1.0f * (priv->icon_width * (priv->cols - 1)))) {
		clutter_actor_animate (priv->viewport, CLUTTER_EASE_OUT_BOUNCE, 250,
	                           "x", (-1.0f * (priv->icon_width * (priv->cols - 1))),
	                           NULL);
		return;
	}

	/* animate the viewport to fully show the child once we pass
	 * a certain threshold with the dragging action
	 */
	offset_x = fabsf (viewport_x) / priv->icon_width + 0.5f;
	if (offset_x > (priv->icon_width * 0.33))
		child_visible = (int) offset_x + 1;
	else
		child_visible = (int) offset_x;

	/* sanity check on the children number */
	child_visible = CLAMP (child_visible, 0, priv->cols);

	clutter_actor_animate (priv->viewport, CLUTTER_EASE_OUT_QUAD, 250,
	                       "x", (-1.0f * priv->icon_width * child_visible),
	                       NULL);

}

static void
gnome_app_comments_init (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv;

	comments->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (comments,
	                                                 GNOME_APP_TYPE_COMMENTS,
	                                                 GnomeAppCommentsPrivate);

	priv->store = NULL;
	priv->app_actors = NULL;
	priv->count = 0;
	priv->rows = 5;	//FIXME: should be calculated
	priv->cols = 7; //FIXME: should be calculated
	priv->icon_width = 96;
	priv->icon_height = 96;

	priv->viewport = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (comments), priv->viewport);
	clutter_actor_set_anchor_point (CLUTTER_ACTOR (priv->viewport), -60, 20);
#if 0
	priv->action = clutter_drag_action_new ();
	clutter_actor_add_action (priv->viewport, priv->action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action),
	                                   CLUTTER_DRAG_X_AXIS);
	g_signal_connect (priv->action, "drag-end", G_CALLBACK (on_drag_end), comments);
	clutter_actor_set_reactive (priv->viewport, TRUE);
#endif
	priv->layout = clutter_table_layout_new ();
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);

	priv->layout_box = clutter_box_new (priv->layout);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->viewport), priv->layout_box);

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

//TODO: any other thing to finalize ?
	if (priv->store)
		g_object_unref (priv->store);

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

void
gnome_app_comments_clean (GnomeAppComments *comments)
{
	GnomeAppCommentsPrivate *priv = comments->priv;

	if (priv->count) {
		GList *l;
		for (l = priv->app_actors; l; l = l->next)
			clutter_actor_destroy (CLUTTER_ACTOR (l->data));
		priv->count = 0;
	}
}

gint
gnome_app_comments_get_pagesize (GnomeAppComments *comments)
{
	gint pagesize;
	
	pagesize = comments->priv->rows * comments->priv->cols;

	return pagesize;
}

void
gnome_app_comments_add_actor (GnomeAppComments *comments, ClutterActor *actor)
{
	GnomeAppCommentsPrivate *priv = comments->priv;
	int col, row;

	priv->app_actors = g_list_prepend (priv->app_actors, actor);
	col = priv->count / priv->rows;
	row = priv->count % priv->rows;
	priv->count ++;

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->layout), actor, col, row);
}

void
gnome_app_comments_load (GnomeAppComments *comments, const GList *data)
{
	OpenResult *info;
	GnomeAppInfoIcon *info_icon;
	GList *l;

	gnome_app_comments_clean (comments);
	for (l = (GList *)data; l; l = l->next) {
		info = OPEN_RESULT (l->data);
		info_icon = gnome_app_info_icon_new_with_app (comments->priv->store, info);
		gnome_app_comments_add_actor (comments, CLUTTER_ACTOR (info_icon));
	}
}

