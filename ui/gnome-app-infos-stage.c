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
#include "open-services.h"
#include "open-request.h"
#include "gnome-app-store.h"
#include "gnome-app-info-icon.h"
#include "gnome-app-infos-stage.h"

struct _GnomeAppInfosStagePrivate
{
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

	AppRequest *request;
};

G_DEFINE_TYPE (GnomeAppInfosStage, gnome_app_infos_stage, CLUTTER_TYPE_GROUP)

static void
on_drag_end (ClutterDragAction   *action,
	           ClutterActor        *actor,
	           gfloat               event_x,
	           gfloat               event_y,
	           ClutterModifierType  modifiers,
		GnomeAppInfosStage *infos_stage)
{
	GnomeAppInfosStagePrivate *priv;
	gfloat viewport_x;
	gfloat offset_x;
	gint child_visible;

	priv = infos_stage->priv;
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
gnome_app_infos_stage_init (GnomeAppInfosStage *infos_stage)
{
	GnomeAppInfosStagePrivate *priv;

	infos_stage->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (infos_stage,
	                                                 GNOME_APP_TYPE_INFOS_STAGE,
	                                                 GnomeAppInfosStagePrivate);

	priv->app_actors = NULL;
	priv->count = 0;
	priv->rows = 5;	//FIXME: should be calculated
	priv->cols = 7; //FIXME: should be calculated
	priv->icon_width = 96;
	priv->icon_height = 96;
	priv->request = NULL;

	priv->viewport = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (infos_stage), priv->viewport);
	clutter_actor_set_anchor_point (CLUTTER_ACTOR (priv->viewport), -60, 20);
#if 0
	priv->action = clutter_drag_action_new ();
	clutter_actor_add_action (priv->viewport, priv->action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action),
	                                   CLUTTER_DRAG_X_AXIS);
	g_signal_connect (priv->action, "drag-end", G_CALLBACK (on_drag_end), infos_stage);
	clutter_actor_set_reactive (priv->viewport, TRUE);
#endif
	priv->layout = clutter_table_layout_new ();
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);

	priv->layout_box = clutter_box_new (priv->layout);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->viewport), priv->layout_box);

}

static void
gnome_app_infos_stage_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_infos_stage_parent_class)->dispose (object);
}

static void
gnome_app_infos_stage_finalize (GObject *object)
{
	GnomeAppInfosStage *infos_stage = GNOME_APP_INFOS_STAGE (object);
	GnomeAppInfosStagePrivate *priv = infos_stage->priv;

	if (priv->request)
		g_object_unref (priv->request);
//TODO: any other thing to finalize ?
	
	G_OBJECT_CLASS (gnome_app_infos_stage_parent_class)->finalize (object);
}

static void
gnome_app_infos_stage_class_init (GnomeAppInfosStageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_infos_stage_dispose;
	object_class->finalize = gnome_app_infos_stage_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfosStagePrivate));
}

GnomeAppInfosStage *
gnome_app_infos_stage_new (void)
{
	return g_object_new (GNOME_APP_TYPE_INFOS_STAGE, NULL);
}

void
gnome_app_infos_stage_clean (GnomeAppInfosStage *infos_stage)
{
	GnomeAppInfosStagePrivate *priv = infos_stage->priv;

	if (priv->count) {
		GList *l;
		for (l = priv->app_actors; l; l = l->next)
			clutter_actor_destroy (CLUTTER_ACTOR (l->data));
		priv->count = 0;
	}
}

gint
gnome_app_infos_stage_get_pagesize (GnomeAppInfosStage *infos_stage)
{
	return infos_stage->priv->rows * infos_stage->priv->cols;
}

void
gnome_app_infos_stage_add_actor (GnomeAppInfosStage *infos_stage, ClutterActor *actor)
{
	GnomeAppInfosStagePrivate *priv = infos_stage->priv;
	int col, row;

	priv->app_actors = g_list_prepend (priv->app_actors, actor);
	col = priv->count / priv->rows;
	row = priv->count % priv->rows;
	priv->count ++;

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->layout), actor, col, row);
}

void
gnome_app_infos_stage_load (GnomeAppInfosStage *infos_stage, GList *data)
{
	AppInfo *info;
	GnomeAppInfoIcon *info_icon;
	GList *l;

	gnome_app_infos_stage_clean (infos_stage);
	for (l = data; l; l = l->next) {
		info = APP_INFO (l->data);
		info_icon = gnome_app_info_icon_new_with_app (info);
		gnome_app_infos_stage_add_actor (infos_stage, CLUTTER_ACTOR (info_icon));
	}
}

