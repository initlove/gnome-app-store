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
#include "gnome-app-query.h"
#include "gnome-app-store.h"
#include "gnome-app-info.h"
#include "gnome-app-info-ui.h"
#include "gnome-app-stage.h"

struct _GnomeAppStagePrivate
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

	GnomeAppQuery *query;
};

G_DEFINE_TYPE (GnomeAppStage, gnome_app_stage, CLUTTER_TYPE_GROUP)

static void
on_drag_end (ClutterDragAction   *action,
	           ClutterActor        *actor,
	           gfloat               event_x,
	           gfloat               event_y,
	           ClutterModifierType  modifiers,
		GnomeAppStage *stage)
{
//TODO:
return ;

	GnomeAppStagePrivate *priv;
	gfloat viewport_x;
	gfloat offset_x;
	gint child_visible;

	priv = stage->priv;
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
gnome_app_stage_init (GnomeAppStage *stage)
{
	GnomeAppStagePrivate *priv;

	stage->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (stage,
	                                                 GNOME_APP_TYPE_STAGE,
	                                                 GnomeAppStagePrivate);

	priv->app_actors = NULL;
	priv->count = 0;
	priv->rows = 5;	//FIXME: should be calculated
	priv->cols = 7; //FIXME: should be calculated
	priv->icon_width = 96;
	priv->icon_height = 96;
	priv->query = NULL;

	priv->viewport = clutter_box_new (clutter_box_layout_new ());
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), priv->viewport);
	clutter_actor_set_anchor_point (CLUTTER_ACTOR (priv->viewport), -60, 20);
	priv->action = clutter_drag_action_new ();
	clutter_actor_add_action (priv->viewport, priv->action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (priv->action),
	                                   CLUTTER_DRAG_X_AXIS);
	g_signal_connect (priv->action, "drag-end", G_CALLBACK (on_drag_end), stage);
	clutter_actor_set_reactive (priv->viewport, TRUE);

	priv->layout = clutter_table_layout_new ();
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->layout), priv->icon_width * 0.3);

	priv->layout_box = clutter_box_new (priv->layout);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->viewport), priv->layout_box);

}

static void
gnome_app_stage_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_stage_parent_class)->dispose (object);
}

static void
gnome_app_stage_finalize (GObject *object)
{
	GnomeAppStage *stage = GNOME_APP_STAGE (object);
	GnomeAppStagePrivate *priv = stage->priv;

	if (priv->query)
		g_object_unref (priv->query);
//TODO: any other thing to finalize ?
	
	G_OBJECT_CLASS (gnome_app_stage_parent_class)->finalize (object);
}

static void
gnome_app_stage_class_init (GnomeAppStageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_stage_dispose;
	object_class->finalize = gnome_app_stage_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppStagePrivate));
}

GnomeAppStage *
gnome_app_stage_new (void)
{
	return g_object_new (GNOME_APP_TYPE_STAGE, NULL);
}

void
gnome_app_stage_clean (GnomeAppStage *stage)
{
	GnomeAppStagePrivate *priv = stage->priv;

	if (priv->count) {
		GList *l;
		for (l = priv->app_actors; l; l = l->next)
			clutter_actor_destroy (CLUTTER_ACTOR (l->data));
		priv->count = 0;
	}
}

void
gnome_app_stage_add_actor (GnomeAppStage *stage, ClutterActor *actor)
{
	GnomeAppStagePrivate *priv = stage->priv;
	int col, row;

	priv->app_actors = g_list_prepend (priv->app_actors, actor);
	col = priv->count / priv->rows;
	row = priv->count % priv->rows;
	priv->count ++;

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->layout), actor, col, row);
}

void
gnome_app_stage_add_actors (GnomeAppStage *stage, GList *actors)
{
	GList *l;

	for (l = actors; l; l = l->next) {
		gnome_app_stage_add_actor (stage, CLUTTER_ACTOR (l->data));
	}
}

static void
load_query (GnomeAppStage *stage)
{
        GList *list, *l;
        const GnomeAppStore *store;

	store = gnome_app_store_get_default (); 
	list = gnome_app_store_get_apps_by_query (store, stage->priv->query);

	if (list)
		gnome_app_stage_clean (stage);
	else
		return;

        for (l = list; l; l = l->next) {
               	GnomeAppInfo *info;
		GnomeAppInfoUI *info_ui;
                ClutterActor *box;
		info = gnome_app_store_get_app_by_id (store, (gchar *)l->data);
		info_ui = gnome_app_info_ui_new_with_app (info);
		box = gnome_app_info_ui_get_icon (info_ui);
		gnome_app_stage_add_actor (stage, box);
		g_object_unref (info_ui);
        }
}

void
gnome_app_stage_load_query (GnomeAppStage *stage, GnomeAppQuery *query)
{
	gint pagesize;
	gchar *val;

	if (stage->priv->query)
		g_object_unref (stage->priv->query);
	stage->priv->query = g_object_ref (query);

	pagesize = stage->priv->rows * stage->priv->cols;
	if (pagesize < 1) {
		printf ("fatal issus: rows and cols error!\n");
		pagesize = 1;
	}
	val = g_strdup_printf ("%d", pagesize);
	g_object_set (stage->priv->query, QUERY_PAGESIZE, val, NULL);
	g_object_set (stage->priv->query, QUERY_PAGE, "0", NULL); // no need in fact
	g_free (val);

	load_query (stage);
}

void
gnome_app_stage_page_change (GnomeAppStage *stage, gint change)
{
	gchar *val;
	gint page;

	if (stage->priv->query) {
		g_object_get (stage->priv->query, QUERY_PAGE, &val, NULL);
		if (val) {
			page = atoi (val);
			page += change;
			if (page < 0) {
				/* nothing happen */
			} else {
				g_free (val);
				val = g_strdup_printf ("%d", page);
				g_object_set (stage->priv->query, QUERY_PAGE, val, NULL);
				load_query (stage);
			}
			g_free (val);
		}
		
	}
}
