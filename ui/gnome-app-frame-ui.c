/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - 330,
Boston, MA 02111-1307, USA.

Author: Liang chenye <liangchenye@gmail.com>

*/
#include "st.h"
#include <stdio.h>
#include <string.h>
#include <clutter/clutter.h>
#include "open-services.h"
#include "open-request.h"
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-store.h"
#include "gnome-app-frame-ui.h"
#include "gnome-app-infos-stage.h"

struct _GnomeAppFrameUIPrivate
{
	ClutterGroup	*ui_group;
	ClutterActor	*search_icon;
	ClutterActor	*search_entry;
	ClutterActor 	*search_hint;
	gboolean	is_search_enabled;
	gboolean	is_search_hint_enabled;
	ClutterGroup 	*infos_stage_group;
	ClutterGroup	*categories_group;
	ClutterActor	*prev;
	ClutterActor	*next;
	ClutterActor	*categories;
	ClutterActor	*status;
	ClutterActor	*total_items;
        GnomeAppInfosStage *infos_stage;
        ClutterScript	*script;

	GnomeAppStore	*store;
	OpenRequest	*request;

	gint		pagesize;
	gint 		current_page;
};

G_DEFINE_TYPE (GnomeAppFrameUI, gnome_app_frame_ui, CLUTTER_TYPE_GROUP)

void
gnome_app_frame_ui_load_request (GnomeAppFrameUI *ui)
{
	g_return_if_fail (ui && ui->priv->request);

	OpenResults *results;
	gchar *pagesize, *page;
	gchar *message;

	pagesize = g_strdup_printf ("%d", ui->priv->pagesize);
	page = g_strdup_printf ("%d", ui->priv->current_page);
	app_request_set (ui->priv->request, "operation", "list");
	app_request_set (ui->priv->request, "pagesize", pagesize);
	app_request_set (ui->priv->request, "page", page);
	g_free (page);	
	g_free (pagesize);

	results = gnome_app_store_get_results (ui->priv->store, ui->priv->request);
	if (open_results_get_status (results)) {
		clutter_actor_hide (ui->priv->status);
	} else {
		clutter_actor_hide (ui->priv->prev);
		clutter_actor_hide (ui->priv->next);
		message = g_strdup_printf ("Error: %s!", open_results_get_meta (results, "message"));
		clutter_text_set_text (CLUTTER_TEXT (ui->priv->status), message);

		g_free (message);
		g_object_unref (results);
		return ;
	}

	const GList *list;
	gint total_items;

	total_items = open_results_get_total_items (results);
	if (total_items > 0) {
		gchar *total_items_text;
		total_items_text = g_strdup_printf ("%d apps", total_items);
		clutter_text_set_text (CLUTTER_TEXT (ui->priv->total_items), total_items_text);
		g_free (total_items_text);

		if (ui->priv->current_page > 0) {
			clutter_actor_show (ui->priv->prev);
		} else {
			clutter_actor_hide (ui->priv->prev);
		}
		if (ui->priv->current_page * ui->priv->pagesize <= total_items) {
			clutter_actor_show (ui->priv->next);
		} else {
			clutter_actor_hide (ui->priv->next);
		}

		list = open_results_get_data (results);
		gnome_app_infos_stage_load (ui->priv->infos_stage, list);
	} else {
		clutter_actor_hide (ui->priv->prev);
		clutter_actor_hide (ui->priv->next);
	}
	g_object_unref (results);
}


static gboolean
is_blank_text (const gchar *text)
{
	if (!text)
		return TRUE;

	int i, len;
	len = strlen (text);
	for (i = 0; i < len; i++) {
		if (*(text + i) == '\t' || *(text + i) == ' ') {
			continue;
		} else
			return FALSE;
	}
	return TRUE;
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

	if (ui->priv->request)
		g_object_unref (ui->priv->request);

	ui->priv->request = app_request_new ();
	ui->priv->current_page = 0;

	app_request_set (ui->priv->request, "search", search);
	gnome_app_frame_ui_load_request (ui);
}

static gboolean
on_search_entry_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	gchar *search;
	GnomeAppFrameUI *ui;

	ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		if (ui->priv->is_search_hint_enabled) {
			clutter_actor_hide (ui->priv->search_hint);
			ui->priv->is_search_hint_enabled = FALSE;
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
	gchar *search;

	ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		on_search_entry_activate (ui->priv->search_entry, ui);
		break;
	}

	return TRUE;
}

static gboolean
on_category_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	const gchar *label_new;
	GnomeAppFrameUI *ui;
	AppRequest *request;

        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		label_new = st_button_get_label (ST_BUTTON (actor));
		ui = GNOME_APP_FRAME_UI (data);
		if (ui->priv->request)
			g_object_unref (ui->priv->request);
		ui->priv->current_page = 0;
/*TODO: category_name is very special, did not know the proper way to deal with this in ocs standard */
		app_request_set (ui->priv->request, "category_name", label_new);
		gnome_app_frame_ui_load_request (ui);
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

	ui = GNOME_APP_FRAME_UI (data);

	if (actor == ui->priv->search_icon) {
		on_search_entry_activate (ui->priv->search_entry, ui);
	} else {
		if (actor == ui->priv->prev) {
			if (ui->priv->current_page > 0)
				ui->priv->current_page --;
		} else if (actor == ui->priv->next) {
			ui->priv->current_page ++;
		}
		if (!ui->priv->request) {
			g_critical ("the request did not initialized!");
			return FALSE;
		}
		gnome_app_frame_ui_load_request (ui);
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
		actor = (ClutterActor *)st_button_new_with_label (name);
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
	ui->priv->is_search_enabled = FALSE;
	ui->priv->is_search_hint_enabled = TRUE;
	ui->priv->request = NULL;
	ui->priv->store = gnome_app_store_new ();
	ui->priv->current_page = 0;

        const gchar *filename;
	GError *error;
        gint i;

        filename = "/home/novell/gnome-app-store/ui/scripts/frame-ui.json";

        priv->script = clutter_script_new ();
	error = NULL;
        clutter_script_load_from_file (priv->script, filename, &error);
        if (error) {
                printf ("error in load script %s!\n", error->message);
                g_error_free (error);
        }

	clutter_script_get_objects (priv->script, "frame-ui", &priv->ui_group,
					"search-icon", &priv->search_icon,
					"search-hint", &priv->search_hint,
					"search-entry", &priv->search_entry, 
					"infos-stage", &priv->infos_stage_group,
					"categories", &priv->categories_group,
					"status", &priv->status,
					"total-items", &priv->total_items,
					"prev-icon", &priv->prev,
					"next-icon", &priv->next,
					NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->ui_group));
	priv->categories = create_category_list (ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->categories_group), priv->categories);
	priv->infos_stage = gnome_app_infos_stage_new ();
	priv->pagesize = gnome_app_infos_stage_get_pagesize (priv->infos_stage);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->infos_stage_group), CLUTTER_ACTOR (priv->infos_stage));

//script connect did not work?
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
gnome_app_frame_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->dispose (object);
}

static void
gnome_app_frame_ui_finalize (GObject *object)
{
	GnomeAppFrameUI *ui = GNOME_APP_FRAME_UI (object);
	GnomeAppFrameUIPrivate *priv = ui->priv;

	if (priv->request)
		g_object_unref (priv->request);
	if (priv->store)
		g_object_unref (priv->store);

	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->finalize (object);
}

static void
gnome_app_frame_ui_class_init (GnomeAppFrameUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_frame_ui_dispose;
	object_class->finalize = gnome_app_frame_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppFrameUIPrivate));
}

GnomeAppFrameUI *
gnome_app_frame_ui_new (void)
{
	return g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);
}

void
gnome_app_frame_ui_set_default_request (GnomeAppFrameUI *ui, AppRequest *request)
{
	if (ui->priv->request)
		g_object_unref (ui->priv->request);
	ui->priv->request = g_object_ref (request);
}
