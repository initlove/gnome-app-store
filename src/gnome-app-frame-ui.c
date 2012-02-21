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
#include "gnome-app-account-ui.h"
#include "gnome-app-frame-ui.h"
#include "gnome-app-icon-view.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppFrameUIPrivate
{
        ClutterScript	*script;
	ClutterActor	*account;
	ClutterActor	*categories;
        GnomeAppIconView *icon_view;
	GnomeAppStore *store;

	gboolean	is_search_enabled;
	gboolean	is_search_hint_enabled;
	gint		pagesize;
	GnomeAppTask	*task;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppFrameUI, gnome_app_frame_ui, CLUTTER_TYPE_GROUP)

static void
frame_ui_load_results (GnomeAppFrameUI *ui, OpenResults *results)
{
	GnomeAppFrameUIPrivate *priv;
	ClutterActor *status;
	ClutterActor *prev;
	ClutterActor *next;
	ClutterActor *total_items;
	gchar *message;

	if (!results) {
		g_debug ("Error in getting results !\n");
		return;
	}
	priv = ui->priv;
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
		gnome_app_icon_view_run (priv->icon_view);
	} else {
		clutter_actor_hide (prev);
		clutter_actor_hide (next);
	}
}

static gpointer
task_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppFrameUI *ui;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	ui = GNOME_APP_FRAME_UI (userdata);
	frame_ui_load_results (ui, results);

	return NULL;
}

static void
frame_ui_set_default_data (GnomeAppFrameUI *ui)
{
	GnomeAppFrameUIPrivate *priv;
	GnomeAppTask *task;
	gchar *pagesize;

	priv = ui->priv;
	pagesize = g_strdup_printf ("%d", priv->pagesize);

        task = gnome_app_task_new (ui, "GET", "/v1/content/data");
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
		GnomeAppFrameUI *ui)
{
	GnomeAppFrameUIPrivate *priv;
	ClutterActor *search_icon;
	ClutterActor *search_hint;
	const gchar *search;

	priv = ui->priv;
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
		
	if (ui->priv->is_search_hint_enabled) {
		ui->priv->is_search_hint_enabled = FALSE;
		clutter_actor_hide (search_hint);
	}
}

G_MODULE_EXPORT void
on_search_entry_activate (ClutterActor *actor,
		GnomeAppFrameUI *ui)
{
	const gchar *search;

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (open_app_pattern_match ("blank", search, NULL))
		return;

	GnomeAppFrameUIPrivate *priv;
	GnomeAppTask *task;
	gchar *pagesize;

	priv = ui->priv;
	pagesize = g_strdup_printf ("%d", priv->pagesize);

	if (priv->task)
		g_object_unref (priv->task);
        task = gnome_app_task_new (ui, "GET", "/v1/content/data");
	/*We need to ref it right after task_new, as some task may finished fast cause of proxy */
	priv->task = g_object_ref (priv->task);
	gnome_app_task_add_params (task,
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
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;
	ClutterActor *search_hint;

	ui = GNOME_APP_FRAME_UI (data);
	priv = ui->priv;
				
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
on_category_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;
	GnomeAppTask *task;
	const gchar *name;
	const gchar *cids;
	gchar *pagesize;
	gchar *page;

	ui = GNOME_APP_FRAME_UI (data);
	priv = ui->priv;
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		pagesize = g_strdup_printf ("%d", priv->pagesize);
		name = (gchar *) g_object_get_data (G_OBJECT (actor), "category_name");
		cids = gnome_app_store_get_cids_by_name (priv->store, name);
printf ("click on %s cids %s\n", name, cids);
/*TODO where to final the task */
		if (priv->task)
			g_object_unref (priv->task);
        	task = gnome_app_task_new (ui, "GET", "/v1/content/data");
		priv->task = g_object_ref (task);
		gnome_app_task_add_params (task,
				"categories", cids,
				"pagesize", pagesize,
				"page", "0",
				NULL);
		gnome_app_task_set_callback (task, task_callback);
		gnome_app_task_push (task);

		g_free (pagesize);
		break;
	}

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_icon_enter (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;

	ui = GNOME_APP_FRAME_UI (data);
	clutter_actor_set_scale (actor, 1.5, 1.5);
	clutter_actor_move_by (actor, -8, -8);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_icon_leave (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;

	ui = GNOME_APP_FRAME_UI (data);
/*TODO: bad numbers, should have better animation */
	clutter_actor_set_scale (actor, 1, 1);
	clutter_actor_move_by (actor, 8, 8);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_icon_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;
	ClutterActor	*prev;
	ClutterActor	*next;
	ClutterActor	*search_icon;
	ClutterActor	*search_entry;

	ui = GNOME_APP_FRAME_UI (data);
	priv = ui->priv;

	clutter_script_get_objects (priv->script,
			"prev-icon", &prev,
			"next-icon", &next,
			"search-icon", &search_icon,
			"search-entry", &search_entry,
			NULL);
	if (actor == search_icon) {
		on_search_entry_activate (search_entry, ui);
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
create_category_list (GnomeAppFrameUI *ui)
{
	ClutterLayoutManager *layout;
	ClutterActor *layout_box, *actor;
	const GList *list;
	GList *l;
	gchar *name;
	gint col, row;

	layout = clutter_table_layout_new ();
        clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (layout), 20);
	layout_box = clutter_box_new (layout);

	col = row = 0;
	const gchar **categories;

	categories = open_app_get_default_categories ();
	for (categories; *categories; categories ++) {
		actor = clutter_text_new ();
		g_object_set_data (G_OBJECT (actor), "category_name", (gpointer) *categories);
		clutter_text_set_editable (CLUTTER_TEXT (actor), FALSE);
		name = _((gchar *)*categories);
		clutter_text_set_text (CLUTTER_TEXT (actor), name);
		clutter_actor_set_reactive (actor, TRUE);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (actor), col, row);
		row ++;
		g_signal_connect (actor, "event", G_CALLBACK (on_category_event), ui);
	}

	return layout_box;
}

static void
gnome_app_frame_ui_init (GnomeAppFrameUI *ui)
{
	GnomeAppFrameUIPrivate *priv;
	ClutterActor *main_ui;
	ClutterActor *account_group;
	ClutterActor *icon_view_group;
	ClutterActor *categories_group;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
	                                                 GNOME_APP_TYPE_FRAME_UI,
	                                                 GnomeAppFrameUIPrivate);
	priv->is_search_enabled = FALSE;
	priv->is_search_hint_enabled = TRUE;
	priv->task = NULL;

        priv->script = gnome_app_script_new_from_file ("app-frame-ui");
        if (!priv->script) {
		return ;
        }
        clutter_script_connect_signals (priv->script, ui);
	clutter_script_get_objects (priv->script, 
			"frame-ui", &main_ui,
			"account-group", &account_group,
			"categories", &categories_group,
			"icon-view", &icon_view_group,
			NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (main_ui));

	priv->store = gnome_app_store_get_default ();
	gnome_app_store_init_category (GNOME_APP_STORE (priv->store));

	priv->account = CLUTTER_ACTOR (gnome_app_account_ui_new (NULL));
	clutter_container_add_actor (CLUTTER_CONTAINER (account_group), priv->account);

	priv->categories = create_category_list (ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (categories_group), priv->categories);

	priv->icon_view = gnome_app_icon_view_new ();
	priv->pagesize = gnome_app_icon_view_get_pagesize (priv->icon_view);
	clutter_container_add_actor (CLUTTER_CONTAINER (icon_view_group), CLUTTER_ACTOR (priv->icon_view));

	frame_ui_set_default_data (ui);
}

static void
gnome_app_frame_ui_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppFrameUI *ui;        

	ui = GNOME_APP_FRAME_UI (object);
			        
	switch (prop_id)
	{
	}
}

static void
gnome_app_frame_ui_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppFrameUI *ui;        

	ui = GNOME_APP_FRAME_UI (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_frame_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->dispose (object);
}

static void
gnome_app_frame_ui_finalize (GObject *object)
{
	GnomeAppFrameUI *ui = GNOME_APP_FRAME_UI (object);
	GnomeAppFrameUIPrivate *priv = ui->priv;

	if (priv->icon_view)
		g_object_unref (priv->icon_view);
	if (priv->task)
		g_object_unref (priv->task);

	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->finalize (object);
}

static void
gnome_app_frame_ui_class_init (GnomeAppFrameUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_frame_ui_set_property;
	object_class->get_property = gnome_app_frame_ui_get_property;
	object_class->dispose = gnome_app_frame_ui_dispose;
	object_class->finalize = gnome_app_frame_ui_finalize;

	g_type_class_add_private (object_class, sizeof (GnomeAppFrameUIPrivate));
}

GnomeAppFrameUI *
gnome_app_frame_ui_new (void)
{
	GnomeAppFrameUI *ui;

	ui = g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);

	return ui;
}
