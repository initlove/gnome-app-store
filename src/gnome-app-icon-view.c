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

struct _GnomeAppIconViewPrivate
{
	ClutterActor *viewport;
	ClutterAction *action;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;

	GnomeAppApplication *app;
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
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppIconView, gnome_app_icon_view, CLUTTER_TYPE_GROUP)

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

	priv->app = NULL;
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
icon_view_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppIconView *icon_view;

	icon_view = GNOME_APP_ICON_VIEW (object);

	switch (prop_id)
	{
	}
}

static void
icon_view_get_property (GObject      *object,
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
icon_view_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->dispose (object);
}

static void
icon_view_finalize (GObject *object)
{
	GnomeAppIconView *icon_view = GNOME_APP_ICON_VIEW (object);
	GnomeAppIconViewPrivate *priv = icon_view->priv;

//TODO: any other thing to finalize ?

	if (priv->app)
		g_object_unref (priv->app);
	if (priv->task)
		g_object_unref (priv->task);

	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->finalize (object);
}

static void
gnome_app_icon_view_class_init (GnomeAppIconViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = icon_view_set_property;
	object_class->get_property = icon_view_get_property;
	object_class->dispose = icon_view_dispose;
	object_class->finalize = icon_view_finalize;

	g_type_class_add_private (object_class, sizeof (GnomeAppIconViewPrivate));
}

GnomeAppIconView *
gnome_app_icon_view_new_with_app (GnomeAppApplication *app)
{
	GnomeAppIconView *icon_view;

	icon_view = g_object_new (GNOME_APP_TYPE_ICON_VIEW, NULL);
	icon_view->priv->app = g_object_ref (app);

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
	GnomeAppIconViewPrivate *priv = icon_view->priv;
	int col, row;

	priv->app_actors = g_list_prepend (priv->app_actors, actor);
	col = priv->count / priv->rows;
	row = priv->count % priv->rows;
	priv->count ++;

	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->layout), actor, col, row);
}

static gboolean
on_info_icon_event (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	OpenResult *info;
	GnomeAppApplication *app;

	info = g_object_get_data (G_OBJECT (actor), "info");
	app = g_object_get_data (G_OBJECT (actor), "application");

	switch (event->type)
	{
		case CLUTTER_BUTTON_PRESS:
			gnome_app_application_load (app, UI_TYPE_INFO_PAGE, info);  
			break;
		case CLUTTER_ENTER:
			clutter_actor_set_scale (actor, 1.5, 1.5);
			break;
		case CLUTTER_LEAVE:
			clutter_actor_set_scale (actor, 1, 1);
			break;
	}

	return TRUE;
}

static ClutterActor *
info_icon_new_with_app (OpenResult *info, GnomeAppApplication *app)
{
	ClutterScript *script;
	ClutterActor *actor, *info_icon;
	gint i;

	script = gnome_app_script_new_from_file ("app-info-icon");
	if (!script)
		return NULL;

	clutter_script_get_objects (script, "info-icon", &info_icon, NULL);
	gchar *prop [] = {
		"name", "personid", "description",
		"score", "downloads", "comments",
		"smallpreviewpic1", "previewpic1",
		"license", NULL};
			
	const gchar *val;

	clutter_script_get_objects (script, "name", &actor, NULL);
	val = open_result_get (info, "name");
	clutter_text_set_text (CLUTTER_TEXT (actor), val);
	clutter_script_get_objects (script, "smallpreviewpic1", &actor, NULL);
	val = open_result_get (info, "smallpreviewpic1");
	gnome_app_set_icon (actor, val);

	g_object_set_data (G_OBJECT (actor), "info", info);
        g_object_set_data (G_OBJECT (actor), "application", app);
	gnome_app_button_binding (actor);
	g_signal_connect (actor, "event", G_CALLBACK (on_info_icon_event), info_icon);

	g_object_unref (script);

	return info_icon;
}

void
gnome_app_icon_view_set_with_data (GnomeAppIconView *icon_view, const GList *data)
{
	OpenResult *info;
	ClutterActor *info_icon;
	ClutterAction *action;
	GList *l;

	gnome_app_icon_view_clean (icon_view);
	for (l = (GList *)data; l; l = l->next) {
		info = OPEN_RESULT (l->data);
		info_icon = info_icon_new_with_app (info, icon_view->priv->app);
		gnome_app_icon_view_add_actor (icon_view, CLUTTER_ACTOR (info_icon));
	}
}
