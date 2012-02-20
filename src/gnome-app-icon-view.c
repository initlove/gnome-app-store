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
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-application.h"
#include "gnome-app-icon-view.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-info-icon.h"

struct _GnomeAppIconViewPrivate
{
	ClutterScript *script;
	ClutterActor *viewport;
	ClutterAction *action;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;

	GnomeAppTask *task;
	GList *app_actors;
	gint count;
	gint rows;
	gint cols;
	gint icon_width;
	gint icon_height;

};

/* Properties */
enum
{
	PROP_0,
	PROP_RESULTS,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppIconView, gnome_app_icon_view, CLUTTER_TYPE_GROUP)

static void	gnome_app_icon_view_set_with_data (GnomeAppIconView *icon_view, OpenResults *results);

static void
on_drag_end (ClutterDragAction   *action,
	           ClutterActor        *actor,
	           gfloat               event_x,
	           gfloat               event_y,
	           ClutterModifierType  modifiers,
		GnomeAppIconView *icon_view)
{
	GnomeAppIconViewPrivate *priv;
	gfloat viewport_x;
	gfloat offset_x;
	gint child_visible;

	priv = icon_view->priv;
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
gnome_app_icon_view_init (GnomeAppIconView *icon_view)
{
	GnomeAppIconViewPrivate *priv;

	icon_view->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (icon_view,
	                                                 GNOME_APP_TYPE_ICON_VIEW,
	                                                 GnomeAppIconViewPrivate);

	priv->script = NULL;
	priv->task = NULL;
	priv->app_actors = NULL;
	priv->count = 0;
	priv->rows = 5;	//FIXME: should be calculated
	priv->cols = 7; //FIXME: should be calculated
	priv->icon_width = 96;
	priv->icon_height = 96;

	priv->viewport = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (icon_view), priv->viewport);

	clutter_actor_set_anchor_point (CLUTTER_ACTOR (priv->viewport), -60, 20);
#if 0
	priv->action = clutter_drag_action_new ();
	clutter_actor_add_action (priv->viewport, priv->action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action),
	                                   CLUTTER_DRAG_X_AXIS);
	g_signal_connect (priv->action, "drag-end", G_CALLBACK (on_drag_end), icon_view);
	clutter_actor_set_reactive (priv->viewport, TRUE);
#endif
	priv->layout = clutter_table_layout_new ();
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);

	priv->layout_box = clutter_box_new (priv->layout);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->viewport), priv->layout_box);
}

static void
gnome_app_icon_view_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppIconView *icon_view;

	icon_view = GNOME_APP_ICON_VIEW (object);
	switch (prop_id)
	{
		case PROP_RESULTS:
			gnome_app_icon_view_set_with_data (icon_view, g_value_get_object (value));
			break;
		default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_icon_view_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppIconView *icon_view;

	icon_view = GNOME_APP_ICON_VIEW (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_icon_view_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->dispose (object);
}

static void
gnome_app_icon_view_finalize (GObject *object)
{
	GnomeAppIconView *icon_view = GNOME_APP_ICON_VIEW (object);
	GnomeAppIconViewPrivate *priv = icon_view->priv;

	if (priv->script)
		g_object_unref (priv->script);
	if (priv->task)
		g_object_unref (priv->task);

	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->finalize (object);
}

static void
gnome_app_icon_view_class_init (GnomeAppIconViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_icon_view_set_property;
	object_class->get_property = gnome_app_icon_view_get_property;
	object_class->dispose = gnome_app_icon_view_dispose;
	object_class->finalize = gnome_app_icon_view_finalize;

	g_object_class_install_property (object_class,
			PROP_RESULTS,
			g_param_spec_object ("results",
				"results of return data",
				"results of return data",
				G_TYPE_OBJECT,
				G_PARAM_WRITABLE));

	g_type_class_add_private (object_class, sizeof (GnomeAppIconViewPrivate));
}

GnomeAppIconView *
gnome_app_icon_view_new (void)
{
	GnomeAppIconView *icon_view;

	icon_view = g_object_new (GNOME_APP_TYPE_ICON_VIEW, NULL);
//	clutter_actor_hide (CLUTTER_ACTOR (icon_view));

	return icon_view;
}

static void
gnome_app_icon_view_clean (GnomeAppIconView *icon_view)
{
	GnomeAppIconViewPrivate *priv = icon_view->priv;

	if (priv->count) {
		GList *l;
		for (l = priv->app_actors; l; l = l->next)
			clutter_actor_destroy (CLUTTER_ACTOR (l->data));
		priv->count = 0;
	}
}

gint
gnome_app_icon_view_get_pagesize (GnomeAppIconView *icon_view)
{
	gint pagesize;
	
	pagesize = icon_view->priv->rows * icon_view->priv->cols;

	return pagesize;
}

static void
gnome_app_icon_view_add_actor (GnomeAppIconView *icon_view, ClutterActor *actor)
{
	GnomeAppIconViewPrivate *priv;
	int col, row;

       	priv = icon_view->priv;
	priv->app_actors = g_list_prepend (priv->app_actors, actor);
	col = priv->count / priv->rows;
	row = priv->count % priv->rows;
	priv->count ++;

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->layout), actor, col, row);
}

static void
gnome_app_icon_view_set_with_data (GnomeAppIconView *icon_view, OpenResults *results)
{
	OpenResult *info;
	GnomeAppInfoIcon *info_icon;
	const GList *data;
	GList *l;

	data = open_results_get_data (results);
	gnome_app_icon_view_clean (icon_view);
	for (l = (GList *)data; l; l = l->next) {
		info = OPEN_RESULT (l->data);
		info_icon = gnome_app_info_icon_new_with_info (info);
		gnome_app_icon_view_add_actor (icon_view, CLUTTER_ACTOR (info_icon));
	}
}

void
gnome_app_icon_view_run (GnomeAppIconView *icon_view)
{
	clutter_actor_show (CLUTTER_ACTOR (icon_view));
}
