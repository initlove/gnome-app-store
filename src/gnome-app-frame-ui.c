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
#include "gnome-app-application.h"
#include "gnome-app-frame-ui.h"
#include "gnome-app-icon-view.h"

struct _GnomeAppFrameUIPrivate
{
	ClutterGroup	*ui_group;
	ClutterGroup	*account_group;
	ClutterActor	*account;
	ClutterActor	*search_icon;
	ClutterActor	*search_entry;
	ClutterActor 	*search_hint;
	gboolean	is_search_enabled;
	gboolean	is_search_hint_enabled;
	ClutterGroup 	*icon_view_group;
	ClutterGroup	*categories_group;
	ClutterActor	*categories;
	ClutterActor	*prev;
	ClutterActor	*next;
	ClutterActor	*status;
	ClutterActor	*total_items;
        GnomeAppIconView *icon_view;
        ClutterScript	*script;

	GnomeAppApplication *app;
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
	gchar *message;

	if (!results) {
		g_debug ("Error in getting results !\n");
		return;
	}
	if (open_results_get_status (results)) {
		clutter_actor_hide (ui->priv->status);
	} else {
		clutter_actor_hide (ui->priv->prev);
		clutter_actor_hide (ui->priv->next);
		message = g_strdup_printf ("Error: %s!", open_results_get_meta (results, "message"));
		clutter_text_set_text (CLUTTER_TEXT (ui->priv->status), message);

		g_free (message);
		return ;
	}

	GnomeAppFrameUIPrivate *priv;
	const GList *list;
	gint total_items;
	gint cur_page;

	priv = ui->priv;
	total_items = open_results_get_total_items (results);
	if (total_items > 0) {
		gchar *total_items_text;
		const gchar *val;

		total_items_text = g_strdup_printf (_("%d apps"), total_items);
		clutter_text_set_text (CLUTTER_TEXT (priv->total_items), total_items_text);
		g_free (total_items_text);
        
		val = gnome_app_task_get_param_value (priv->task, "page");
		if (val) {
			cur_page = atoi (val);
		} else {
			g_debug ("Cannot find 'page' in task!\n");
			return;
		}

		if (cur_page > 0) {
			clutter_actor_show (priv->prev);
		} else {
			clutter_actor_hide (priv->prev);
		}
		if ((cur_page + 1) * priv->pagesize <= total_items) {
			clutter_actor_show (priv->next);
		} else {
			clutter_actor_hide (priv->next);
		}

		list = open_results_get_data (results);
		gnome_app_icon_view_set_with_data (priv->icon_view, list);
	} else {
		clutter_actor_hide (priv->prev);
		clutter_actor_hide (priv->next);
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

static GnomeAppFrameUI *
gnome_app_frame_ui_get_default (void)
{
	static GnomeAppFrameUI *ui = NULL;

	if (!ui) {
		ui = g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);
	}

	return ui;
}

static void
on_search_entry_text_changed (ClutterActor *actor,
		GnomeAppFrameUI *ui)
{
	const gchar *search;

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (is_blank_text (search)) {
		if (clutter_actor_get_reactive (ui->priv->search_icon) == TRUE) {
			clutter_actor_set_opacity (ui->priv->search_icon, 100);
			clutter_actor_set_reactive (ui->priv->search_icon, FALSE);
		}
	} else {
		if (clutter_actor_get_reactive (ui->priv->search_icon) == FALSE) {
			clutter_actor_set_opacity (ui->priv->search_icon, 255);
			clutter_actor_set_reactive (ui->priv->search_icon, TRUE);
		}
	}
		
	if (ui->priv->is_search_hint_enabled) {
		ui->priv->is_search_hint_enabled = FALSE;
		clutter_actor_hide (ui->priv->search_hint);
	}
}

static void
on_search_entry_activate (ClutterActor *actor,
		GnomeAppFrameUI *ui)
{
	const gchar *search;

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (is_blank_text (search))
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

static gboolean
on_search_entry_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;

	ui = GNOME_APP_FRAME_UI (data);
	priv = ui->priv;
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		if (priv->is_search_hint_enabled) {
			clutter_actor_hide (priv->search_hint);
			priv->is_search_hint_enabled = FALSE;
		}
		break;
	}

	return FALSE;
}

static gboolean
on_search_icon_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;

	ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		on_search_entry_activate (priv->search_entry, ui);
		break;
	}

	return TRUE;
}

static gboolean
on_category_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;
	GnomeAppStore *store;
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
	        name = clutter_text_get_text (CLUTTER_TEXT (actor));
printf ("click on %s\n", name);
		g_object_get (priv->app, "app-store", &store, NULL);
		cids = gnome_app_store_get_cids_by_name (store, name);
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

static gboolean
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

static gboolean
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

static gboolean
on_icon_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;

	ui = GNOME_APP_FRAME_UI (data);
	priv = ui->priv;

	if (actor == priv->search_icon) {
		on_search_entry_activate (priv->search_entry, ui);
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
		
		if (actor == priv->prev) {
			cur_page --;
		} else if (actor == priv->next) {
			cur_page ++;
		}
        
		page = g_strdup_printf ("%d", cur_page);

		task = gnome_app_task_copy (priv->task);
		g_object_unref (priv->task);
		priv->task = g_object_ref (task);
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
		name = (gchar *)*categories;
		actor = clutter_text_new ();
		clutter_text_set_editable (CLUTTER_TEXT (actor), FALSE);
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

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
	                                                 GNOME_APP_TYPE_FRAME_UI,
	                                                 GnomeAppFrameUIPrivate);
	priv->is_search_enabled = FALSE;
	priv->is_search_hint_enabled = TRUE;
	priv->app = NULL;

        gchar *filename;
	GError *error;
        gint i;

        filename = open_app_get_ui_uri ("frame-ui");

        priv->script = clutter_script_new ();
	error = NULL;
        clutter_script_load_from_file (priv->script, filename, &error);
        if (error) {
                printf ("error in load script %s!\n", error->message);
                g_error_free (error);
        }
	g_free (filename);
	clutter_script_get_objects (priv->script, "frame-ui", &priv->ui_group,
					"account-group", &priv->account_group,
					"search-icon", &priv->search_icon,
					"search-hint", &priv->search_hint,
					"search-entry", &priv->search_entry, 
					"icon-view", &priv->icon_view_group,
					"categories", &priv->categories_group,
					"status", &priv->status,
					"total-items", &priv->total_items,
					"prev-icon", &priv->prev,
					"next-icon", &priv->next,
					NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->ui_group));

	priv->account = CLUTTER_ACTOR (gnome_app_account_ui_new (NULL));
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->account_group), priv->account);

	priv->categories = create_category_list (ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->categories_group), priv->categories);

	priv->icon_view = NULL;
	priv->pagesize = -1;
	priv->task = NULL;

//TODO can we define icon name in script? or other configure place ? 
	filename = open_app_get_pixmap_uri ("search");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->search_icon), filename, NULL);
	g_free (filename);

	filename = open_app_get_pixmap_uri ("go-previous");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->prev), filename, NULL);
	g_free (filename);

	filename = open_app_get_pixmap_uri ("go-next");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->next), filename, NULL);
	g_free (filename);

//TODO script connect did not work?
//
        g_signal_connect (priv->search_entry, "event", G_CALLBACK (on_search_entry_event), ui);
	g_signal_connect (priv->search_entry, "activate", G_CALLBACK (on_search_entry_activate), ui);
	g_signal_connect (priv->search_entry, "text_changed", G_CALLBACK (on_search_entry_text_changed), ui);

	g_signal_connect (priv->search_icon, "button-press-event", G_CALLBACK (on_icon_press), ui);
	g_signal_connect (priv->search_icon, "enter-event", G_CALLBACK (on_icon_enter), ui);
	g_signal_connect (priv->search_icon, "leave-event", G_CALLBACK (on_icon_leave), ui);

	g_signal_connect (priv->prev, "button-press-event", G_CALLBACK (on_icon_press), ui);
	g_signal_connect (priv->prev, "enter-event", G_CALLBACK (on_icon_enter), ui);
	g_signal_connect (priv->prev, "leave-event", G_CALLBACK (on_icon_leave), ui);

	g_signal_connect (priv->next, "button-press-event", G_CALLBACK (on_icon_press), ui);
	g_signal_connect (priv->next, "enter-event", G_CALLBACK (on_icon_enter), ui);
	g_signal_connect (priv->next, "leave-event", G_CALLBACK (on_icon_leave), ui);
}

static void
frame_ui_set_property (GObject      *object,
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
frame_ui_get_property (GObject      *object,
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
frame_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->dispose (object);
}

static void
frame_ui_finalize (GObject *object)
{
	GnomeAppFrameUI *ui = GNOME_APP_FRAME_UI (object);
	GnomeAppFrameUIPrivate *priv = ui->priv;

	if (priv->app)
		g_object_unref (priv->app);
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

        object_class->set_property = frame_ui_set_property;
	object_class->get_property = frame_ui_get_property;
	object_class->dispose = frame_ui_dispose;
	object_class->finalize = frame_ui_finalize;

	g_type_class_add_private (object_class, sizeof (GnomeAppFrameUIPrivate));
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

GnomeAppFrameUI *
gnome_app_frame_ui_new_with_app (GnomeAppApplication *app)
{
	GnomeAppFrameUI *ui;
	GnomeAppFrameUIPrivate *priv;

	ui = g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);
	priv = ui->priv;
	priv->app = g_object_ref (app);
	priv->icon_view = gnome_app_icon_view_new_with_app (app);
	priv->pagesize = gnome_app_icon_view_get_pagesize (priv->icon_view);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->icon_view_group), CLUTTER_ACTOR (priv->icon_view));

	frame_ui_set_default_data (ui);

	return ui;
}

