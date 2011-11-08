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
#include "gnome-app-query.h"
#include "gnome-app-frame-ui.h"
#include "gnome-app-infos-stage.h"

#define CLEAR_ICON "/home/novell/gnome-app-store/ui/scripts/clear.png"
#define SEARCH_ICON "/home/novell/gnome-app-store/ui/scripts/search.png"

struct _GnomeAppFrameUIPrivate
{
	ClutterGroup	*ui_group;
	ClutterActor	*search_icon;
	ClutterActor	*search_entry;
	ClutterActor 	*search_hint;
	/* search icon and search hint is a little different */
	gboolean	is_clear_mode_enabled;
	gboolean 	is_search_hint_enabled;
	ClutterGroup 	*infos_stage_group;
	ClutterGroup	*categories_group;
	ClutterActor	*prev;
	ClutterActor	*next;
	ClutterActor	*categories;
        GnomeAppInfosStage *infos_stage;
        ClutterScript	*script;
};

G_DEFINE_TYPE (GnomeAppFrameUI, gnome_app_frame_ui, CLUTTER_TYPE_GROUP)

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
		if (ui->priv->is_clear_mode_enabled) {
			clutter_texture_set_from_file (CLUTTER_TEXTURE (ui->priv->search_icon), SEARCH_ICON, NULL);
			ui->priv->is_clear_mode_enabled = FALSE;
		}
		return;
	} else {
		if (!ui->priv->is_clear_mode_enabled) {
			clutter_texture_set_from_file (CLUTTER_TEXTURE (ui->priv->search_icon), CLEAR_ICON, NULL);
			ui->priv->is_clear_mode_enabled = TRUE;
		}
		if (ui->priv->is_search_hint_enabled) {
			ui->priv->is_search_hint_enabled = FALSE;
			clutter_actor_hide (ui->priv->search_hint);
		}
	}
}

static void
on_search_entry_activate (ClutterActor *actor,
		GnomeAppFrameUI *ui)
{
	const gchar *search;
	GnomeAppQuery *query;

	search = clutter_text_get_text (CLUTTER_TEXT (actor));
	if (is_blank_text (search))
		return;
				
	query = gnome_app_query_new ();
	g_object_set (query, QUERY_GROUP, search, NULL);
	gnome_app_infos_stage_load_query (ui->priv->infos_stage, query);
	g_object_unref (query);
}

static gboolean
on_search_entry_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	gchar *search;
	GnomeAppFrameUI *ui;
	GnomeAppQuery *query;

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
		if (ui->priv->is_clear_mode_enabled) {
			if (!ui->priv->is_search_hint_enabled) {
				clutter_text_set_text (CLUTTER_TEXT (ui->priv->search_entry), NULL);
				clutter_actor_show (ui->priv->search_hint);
				ui->priv->is_search_hint_enabled = TRUE;
			}
			ui->priv->is_clear_mode_enabled = FALSE;
			clutter_texture_set_from_file (CLUTTER_TEXTURE (ui->priv->search_icon), SEARCH_ICON, NULL);
		} else {
		/*FIXME: search img icon would never be used, it just a sign */
		}
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
	GnomeAppQuery *query;

        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		label_new = st_button_get_label (ST_BUTTON (actor));
		ui = GNOME_APP_FRAME_UI (data);
		query = gnome_app_query_new ();
		g_object_set (query, QUERY_GROUP, label_new, NULL);
		gnome_app_infos_stage_load_query (ui->priv->infos_stage, query);
		g_object_unref (query);
		break;
	}

	return TRUE;
}

static gboolean
on_prev_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;

	ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		gnome_app_infos_stage_page_change (ui->priv->infos_stage, -1);
		break;
	}

	return TRUE;
}

static gboolean
on_next_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *ui;

	ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		gnome_app_infos_stage_page_change (ui->priv->infos_stage, 1);
		break;
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
	list = (const GList *)gnome_app_get_local_categories ();
	for (l = (GList *)list; l; l = l->next) {
		name = (gchar *)l->data;
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
	ui->priv->is_search_hint_enabled = TRUE;
	ui->priv->is_clear_mode_enabled = FALSE;

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
					"prev-icon", &priv->prev,
					"next-icon", &priv->next,
					NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (priv->ui_group));
	priv->categories = create_category_list (ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->categories_group), priv->categories);
	priv->infos_stage = gnome_app_infos_stage_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->infos_stage_group), CLUTTER_ACTOR (priv->infos_stage));

//script connect did not work?
        g_signal_connect (priv->search_entry, "event", G_CALLBACK (on_search_entry_event), ui);
	g_signal_connect (priv->search_entry, "activate", G_CALLBACK (on_search_entry_activate), ui);
	g_signal_connect (priv->search_entry, "text_changed", G_CALLBACK (on_search_entry_text_changed), ui);
	g_signal_connect (priv->search_icon, "event", G_CALLBACK (on_search_icon_event), ui);
	g_signal_connect (priv->prev, "event", G_CALLBACK (on_prev_event), ui);
	g_signal_connect (priv->next, "event", G_CALLBACK (on_next_event), ui);
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

void
gnome_app_frame_ui_load_query (GnomeAppFrameUI *ui, GnomeAppQuery *query)
{
	gnome_app_infos_stage_load_query (ui->priv->infos_stage, query);
}

GnomeAppFrameUI *
gnome_app_frame_ui_new (void)
{
	return g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);
}


