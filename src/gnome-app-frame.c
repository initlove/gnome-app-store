/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <config.h>
#include <glib/gi18n.h>
#include <stdio.h>
#include <string.h>
#include <clutter/clutter.h>
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-store.h"
#include "gnome-app-task.h"
#include "gnome-app-frame.h"
#include "gnome-app-icon-view.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-widgets.h"

struct _GnomeAppFramePrivate
{
        ClutterScript	*script;
	ClutterActor	*categories;
	ClutterActor 	*spin;
	ClutterActor    *selected_button;
        GnomeAppIconView *icon_view;
	GnomeAppStore *store;

	gboolean	lock;
	gboolean	is_search_enabled;
	gboolean	is_search_hint_enabled;
	gint		pagesize;
	GnomeAppTask	*task;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LOCK_STATUS,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppFrame, gnome_app_frame, CLUTTER_TYPE_GROUP)

static gboolean
is_frame_locked (GnomeAppFrame *frame)
{
	GnomeAppFramePrivate *priv;

	priv = frame->priv;

	return priv->lock;
}

static void
gnome_app_frame_set_lock (GnomeAppFrame *frame, const gchar *str)
{
	g_return_if_fail (str);

	GnomeAppFramePrivate *priv;
	gboolean _lock;

	if (strcmp (str, "lock") == 0)
		_lock = TRUE;
	else if (strcmp (str, "unlock") == 0)
		_lock = FALSE;
	else
		return;

	priv = frame->priv;
	if (priv->lock == _lock) {
		g_critical ("You should not %s more than once!\tFrame\n", str);
		return;
	}
	priv->lock = _lock;
	if (priv->lock) {
		gnome_app_texture_start (GNOME_APP_TEXTURE (priv->spin)); 
		clutter_actor_hide (CLUTTER_ACTOR (priv->icon_view));
	} else {
		gnome_app_texture_stop (GNOME_APP_TEXTURE (priv->spin));
		clutter_actor_show (CLUTTER_ACTOR (priv->icon_view));
	}
}

static void
frame_load_results (GnomeAppFrame *frame, OpenResults *results)
{
	GnomeAppFramePrivate *priv;
	ClutterActor *status;
	ClutterActor *prev;
	ClutterActor *next;
	ClutterActor *total_items;
	gchar *message;

	if (!results) {
		g_debug ("Error in getting results !\n");
		return;
	}
	priv = frame->priv;
	clutter_script_get_objects (priv->script,
				"status", &status,
				"total-items", &total_items,
				"prev-icon", &prev,
				"next-icon", &next,
				NULL);
	if (open_results_get_status (results)) {
		clutter_actor_hide (status);
	} else {
		clutter_actor_hide (prev);
		clutter_actor_hide (next);
		message = g_strdup_printf ("Error: %s!", open_results_get_meta (results, "message"));
		clutter_text_set_text (CLUTTER_TEXT (status), message);

		g_free (message);
		return ;
	}

	gint total;
	gint cur_page;

	total = open_results_get_total_items (results);
	if (total > 0) {
		gchar *total_items_text;
		const gchar *val;

		total_items_text = g_strdup_printf (_("%d apps"), total);
		clutter_text_set_text (CLUTTER_TEXT (total_items), total_items_text);
		g_free (total_items_text);
        
		val = gnome_app_task_get_param_value (priv->task, "page");
		if (val) {
			cur_page = atoi (val);
		} else {
			g_debug ("Cannot find 'page' in task!\n");
			return;
		}

		if (cur_page > 0) {
			clutter_actor_show (prev);
		} else {
			clutter_actor_hide (prev);
		}
		if ((cur_page + 1) * priv->pagesize <= total) {
			clutter_actor_show (next);
		} else {
			clutter_actor_hide (next);
		}

		g_object_set (G_OBJECT (priv->icon_view), "results", results, NULL);
		clutter_actor_show (CLUTTER_ACTOR (priv->icon_view));
	} else {
		clutter_actor_hide (prev);
		clutter_actor_hide (next);
	}
}

static gpointer
task_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppFrame *frame;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	frame = GNOME_APP_FRAME (userdata);
	frame_load_results (frame, results);

	return NULL;
}

static void
frame_set_default_data (GnomeAppFrame *frame)
{
	GnomeAppFramePrivate *priv;
	GnomeAppTask *task;
	gchar *pagesize;

	priv = frame->priv;
	pagesize = g_strdup_printf ("%d", priv->pagesize);

        task = gnome_app_task_new (frame, "GET", "/v1/content/data");
	priv->task = g_object_ref (task);
	gnome_app_task_add_params (task,
				"sortmode", "new",
				"pagesize", pagesize,
				"page", "0",
				NULL);
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_push (task);

	g_free (pagesize);
}

G_MODULE_EXPORT void
on_search_entry_text_changed (ClutterActor *actor,
		GnomeAppFrame *frame)
{
	GnomeAppFramePrivate *priv;
	ClutterActor *search_icon;
	ClutterActor *search_hint;
	const gchar *search;

	priv = frame->priv;
	clutter_script_get_objects (priv->script,
			"search-icon", &search_icon,
			"search-hint", &search_hint,
			NULL);

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (open_app_pattern_match ("blank", search, NULL)) {
		if (clutter_actor_get_reactive (search_icon) == TRUE) {
			clutter_actor_set_opacity (search_icon, 100);
			clutter_actor_set_reactive (search_icon, FALSE);
		}
	} else {
		if (clutter_actor_get_reactive (search_icon) == FALSE) {
			clutter_actor_set_opacity (search_icon, 255);
			clutter_actor_set_reactive (search_icon, TRUE);
		}
	}
		
	if (frame->priv->is_search_hint_enabled) {
		frame->priv->is_search_hint_enabled = FALSE;
		clutter_actor_hide (search_hint);
	}
}

G_MODULE_EXPORT void
on_search_entry_activate (ClutterActor *actor,
		GnomeAppFrame *frame)
{
	const gchar *search;

	if (is_frame_locked (frame))
		return;

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (open_app_pattern_match ("blank", search, NULL))
		return;

	GnomeAppFramePrivate *priv;
	GnomeAppTask *task;
	gchar *pagesize;

	priv = frame->priv;
	pagesize = g_strdup_printf ("%d", priv->pagesize);

	if (priv->task)
		g_object_unref (priv->task);

        task = gnome_app_task_new (frame, "GET", "/v1/content/data");
	/*We need to ref it right after task_new, as some task may finished fast cause of proxy */
	priv->task = g_object_ref (priv->task);
	gnome_app_task_add_params (task,
				"sortmode", "new",
				"search", search,
				"pagesize", pagesize,
				"page", "0",
				NULL);
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_push (task);

	g_free (pagesize);
}

G_MODULE_EXPORT gboolean
on_search_entry_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrame *frame;
	GnomeAppFramePrivate *priv;
	ClutterActor *search_hint;

	frame = GNOME_APP_FRAME (data);
	priv = frame->priv;

	clutter_script_get_objects (priv->script,
			"search-hint", &search_hint,
			NULL);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		if (priv->is_search_hint_enabled) {
			clutter_actor_hide (search_hint);
			priv->is_search_hint_enabled = FALSE;
		}
		break;
	}

	return FALSE;
}

static gboolean
on_category_button_press (ClutterActor *actor,
   		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppFrame *frame;
	GnomeAppFramePrivate *priv;
	GnomeAppTask *task;
	const gchar *name;
	const gchar *cids;
	gchar *pagesize;
	gchar *page;

	frame = GNOME_APP_FRAME (data);
	priv = frame->priv;
	if (is_frame_locked (frame))
		return FALSE;

	if (!priv->selected_button) {
		priv->selected_button = actor;
	} else {
		gnome_app_button_set_selected (GNOME_APP_BUTTON (priv->selected_button), FALSE);
		priv->selected_button = actor;
	}
	pagesize = g_strdup_printf ("%d", priv->pagesize);
	name = (gchar *) g_object_get_data (G_OBJECT (actor), "category_name");
	cids = gnome_app_store_get_cids_by_name (priv->store, name);
printf ("click on %s cids %s\n", name, cids);
/*TODO where to final the task */
	if (priv->task)
		g_object_unref (priv->task);
        task = gnome_app_task_new (frame, "GET", "/v1/content/data");
	priv->task = g_object_ref (task);
	gnome_app_task_add_params (task,
			"sortmode", "new",
			"categories", cids,
			"pagesize", pagesize,
			"page", "0",
			NULL);
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_push (task);
	g_free (pagesize);

	return FALSE;
}

G_MODULE_EXPORT gboolean
on_icon_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrame *frame;
	GnomeAppFramePrivate *priv;
	ClutterActor	*prev;
	ClutterActor	*next;
	ClutterActor	*search_icon;
	ClutterActor	*search_entry;

	frame = GNOME_APP_FRAME (data);
	priv = frame->priv;

	/*TODO: if it was locked, and the user clicked again and again,
	 * it means we should make the task priority higher 
	 * 
	 */
	if (is_frame_locked (frame))
		return FALSE;

	clutter_script_get_objects (priv->script,
			"prev-icon", &prev,
			"next-icon", &next,
			"search-icon", &search_icon,
			"search-entry", &search_entry,
			NULL);
	if (actor == search_icon) {
		on_search_entry_activate (search_entry, frame);
	} else {
		GnomeAppTask *task;
		const gchar *val;
		gchar *page;
		int cur_page;

		val = gnome_app_task_get_param_value (priv->task, "page");
		if (val) {
			cur_page = atoi (val);
		} else {
			g_debug ("Cannot get 'page' in the task \n");
			return FALSE;
		}
		
		if (actor == prev) {
			cur_page --;
		} else if (actor == next) {
			cur_page ++;
		}
		task = gnome_app_task_copy (priv->task);
		g_object_unref (priv->task);
		priv->task = g_object_ref (task);
		page = g_strdup_printf ("%d", cur_page);
        	gnome_app_task_add_param (task, "page", page);
		gnome_app_task_push (task);
		g_free (page);
	}

	return TRUE;
}

static ClutterActor *
create_category_list (GnomeAppFrame *frame)
{
	ClutterLayoutManager *layout;
	ClutterActor *layout_box, *actor;
	const GList *list;
	const gchar **categories;
	GList *l;
	gchar *name;
	gint col, row;

	layout = clutter_table_layout_new ();
        clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (layout), 20);
	layout_box = clutter_box_new (layout);

	col = row = 0;
	categories = open_app_get_default_categories ();
	for (categories; *categories; categories ++) {
		actor = CLUTTER_ACTOR (gnome_app_button_new ());
		clutter_actor_set_reactive (actor, TRUE);
		g_object_set (G_OBJECT (actor), "button-mode", "select", NULL);
		g_object_set_data (G_OBJECT (actor), "category_name", (gpointer) *categories);
		name = _((gchar *)*categories);
		gnome_app_button_set_text (GNOME_APP_BUTTON (actor), name);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (actor), col, row);
		row ++;
		g_signal_connect (actor, "button-press-event", G_CALLBACK (on_category_button_press), frame);
	}

	return layout_box;
}

static void
gnome_app_frame_init (GnomeAppFrame *frame)
{
	GnomeAppFramePrivate *priv;
	ClutterActor *main_ui;
	ClutterActor *icon_view_group;
	ClutterActor *categories_group;
	ClutterActor *prev;
	ClutterActor *next;

	frame->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (frame,
	                                                 GNOME_APP_TYPE_FRAME,
	                                                 GnomeAppFramePrivate);
	priv->is_search_enabled = FALSE;
	priv->is_search_hint_enabled = TRUE;
	priv->task = NULL;
	priv->lock = FALSE;
	priv->selected_button = NULL;

        priv->script = gnome_app_script_new_from_file ("app-frame");
        if (!priv->script) {
		return ;
        }
        clutter_script_connect_signals (priv->script, frame);
	clutter_script_get_objects (priv->script, 
			"frame", &main_ui,
			"categories", &categories_group,
			"icon-view", &icon_view_group,
			"prev-icon", &prev,
			"next-icon", &next,
			NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (frame), CLUTTER_ACTOR (main_ui));

	priv->store = gnome_app_store_get_default ();
	gnome_app_store_init_category (GNOME_APP_STORE (priv->store));

	priv->categories = create_category_list (frame);
	clutter_container_add_actor (CLUTTER_CONTAINER (categories_group), priv->categories);

	priv->icon_view = gnome_app_icon_view_new ();
	priv->pagesize = gnome_app_icon_view_get_pagesize (priv->icon_view);
	clutter_container_add_actor (CLUTTER_CONTAINER (icon_view_group), CLUTTER_ACTOR (priv->icon_view));

	gchar *spin_dir;
	gfloat prev_x, prev_y;
	gfloat next_x, next_y;

	spin_dir = open_app_get_spin_dir ();
	priv->spin = CLUTTER_ACTOR (gnome_app_dtexture_new_from_dir (spin_dir));
	g_free (spin_dir);
	clutter_container_add_actor (CLUTTER_CONTAINER (icon_view_group), CLUTTER_ACTOR (priv->spin));
	clutter_actor_get_position (prev, &prev_x, &prev_y);
	clutter_actor_get_position (next, &next_x, &next_y);
	clutter_actor_set_position (priv->spin, (prev_x + next_x)/2, (prev_y + next_y)/2);

	frame_set_default_data (frame);
}

static void
gnome_app_frame_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppFrame *frame;
	const gchar *str;

	frame = GNOME_APP_FRAME (object);
			        
	switch (prop_id)
	{
		case PROP_LOCK_STATUS:
			str = g_value_get_string (value);
			if (!str)
				return;
			gnome_app_frame_set_lock (frame, str);
			break;
	}
}

static void
gnome_app_frame_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppFrame *frame;        

	frame = GNOME_APP_FRAME (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_frame_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_frame_parent_class)->dispose (object);
}

static void
gnome_app_frame_finalize (GObject *object)
{
	GnomeAppFrame *frame = GNOME_APP_FRAME (object);
	GnomeAppFramePrivate *priv = frame->priv;

	if (priv->icon_view)
		g_object_unref (priv->icon_view);
	if (priv->task)
		g_object_unref (priv->task);

	G_OBJECT_CLASS (gnome_app_frame_parent_class)->finalize (object);
}

static void
gnome_app_frame_class_init (GnomeAppFrameClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_frame_set_property;
	object_class->get_property = gnome_app_frame_get_property;
	object_class->dispose = gnome_app_frame_dispose;
	object_class->finalize = gnome_app_frame_finalize;

	g_object_class_install_property (object_class,
			PROP_LOCK_STATUS,		
			g_param_spec_string ("lock-status",
			"Lock Status",
			"Lock Status",
			NULL,
			G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppFramePrivate));
}

GnomeAppFrame *
gnome_app_frame_new (void)
{
	GnomeAppFrame *frame;

	frame = g_object_new (GNOME_APP_TYPE_FRAME, NULL);

	return frame;
}
