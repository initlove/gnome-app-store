/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Sframe_uite 330,
Boston, MA 02111-1307, USA.

Author: Liang chenye <liangchenye@gmail.com>

*/
#include "st.h"

#include <clutter/clutter.h>
#include "gnome-app-query.h"
#include "gnome-app-frame-ui.h"
#include "gnome-app-infos-stage.h"

struct _GnomeAppFrameUIPrivate
{
	ClutterGroup *ui_group;
	ClutterText *search_entry;
	ClutterGroup *infos_stage_group;
	ClutterGroup *categories_group;
	ClutterActor *categories;
        GnomeAppInfosStage *infos_stage;
        ClutterScript *script;
};

G_DEFINE_TYPE (GnomeAppFrameUI, gnome_app_frame_ui, CLUTTER_TYPE_GROUP)

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
on_search_activate (ClutterText *text,
                   gpointer     data)
{
	GnomeAppFrameUI *frame_ui;
	gchar *search;
	GnomeAppQuery *query;

	search = (gchar *)clutter_text_get_text (text);
printf ("query %s\n", search);	
	query = gnome_app_query_new ();
	g_object_set (query, QUERY_SEARCH, search, NULL);
	frame_ui = gnome_app_frame_ui_get_default ();
	gnome_app_infos_stage_load_query (frame_ui->priv->infos_stage, query);

	g_object_unref (query);
}

static ClutterActor *
create_search_entry (GnomeAppFrameUI *frame_ui)
{
	ClutterActor *entry;
	gchar *text = "Search";

	entry = clutter_text_new_full (NULL, text, CLUTTER_COLOR_LightGray);
	clutter_text_set_editable (CLUTTER_TEXT (entry), TRUE);
	clutter_text_set_selectable (CLUTTER_TEXT (entry), TRUE);
	clutter_text_set_activatable (CLUTTER_TEXT (entry), TRUE);
	clutter_text_set_single_line_mode (CLUTTER_TEXT (entry), TRUE);
	clutter_actor_set_reactive (entry, TRUE);
	clutter_actor_grab_key_focus (entry);
	g_signal_connect (entry, "activate",
                    G_CALLBACK (on_search_activate),
                    NULL);
	return entry;
}

/*FIXME: the second args seems broken!!! */
static void
category_click_cb (StButton *button, gpointer userdata)
{
	const gchar *label_new;
	label_new = st_button_get_label (button);


	GnomeAppQuery *query;
	query = gnome_app_query_new ();
	g_object_set (query, QUERY_GROUP, label_new, NULL);
//	gnome_app_infos_stage_load_query (frame_ui->priv->infos_stage, query);
	g_object_unref (query);
}

static void
prev_cb (StButton *button, gpointer userdata)
{
//FIXME: should add time gap
	GnomeAppFrameUI *frame_ui;

	frame_ui = gnome_app_frame_ui_get_default ();
	gnome_app_infos_stage_page_change (frame_ui->priv->infos_stage, -1);
}

static void
next_cb (StButton *button, gpointer userdata)
{
//FIXME: should add time gap
	GnomeAppFrameUI *frame_ui;

	frame_ui = gnome_app_frame_ui_get_default ();
	gnome_app_infos_stage_page_change (frame_ui->priv->infos_stage, 1);
}

static ClutterActor *
create_category_list (ClutterScript *script)
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
		g_signal_connect (ST_BUTTON (actor), "clicked", G_CALLBACK (category_click_cb), NULL);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (actor), col, row);
		row ++;
	}
#if 0
	actor = (ClutterActor *)st_button_new_with_label ("---------");
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), actor, col, row);
	row ++;
	actor = (ClutterActor *)st_button_new_with_label ("Prev");
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), actor, col, row);
	row ++;
	g_signal_connect (ST_BUTTON (actor), "clicked", G_CALLBACK (prev_cb), NULL);
	actor = (ClutterActor *)st_button_new_with_label ("Next");
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), actor, col, row);
	row ++;
	g_signal_connect (ST_BUTTON (actor), "clicked", G_CALLBACK (next_cb), NULL);
#endif
	return layout_box;
}

static void
gnome_app_frame_ui_init (GnomeAppFrameUI *ui)
{
	GnomeAppFrameUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
	                                                 GNOME_APP_TYPE_FRAME_UI,
	                                                 GnomeAppFrameUIPrivate);
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
					"search-entry", &priv->search_entry, 
					"infos-stage", &priv->infos_stage_group,
					"categories", &priv->categories_group,
					NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), priv->ui_group);
	priv->categories = create_category_list (priv->script);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->categories_group), priv->categories);
	priv->infos_stage = gnome_app_infos_stage_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->infos_stage_group), CLUTTER_ACTOR (priv->infos_stage));
}

static void
gnome_app_frame_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_frame_ui_parent_class)->dispose (object);
}

static void
gnome_app_frame_ui_finalize (GObject *object)
{
	GnomeAppFrameUI *frame_ui = GNOME_APP_FRAME_UI (object);
	GnomeAppFrameUIPrivate *priv = frame_ui->priv;

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
gnome_app_frame_ui_load_query (GnomeAppFrameUI *frame_ui, GnomeAppQuery *query)
{
	gnome_app_infos_stage_load_query (frame_ui->priv->infos_stage, query);
}

GnomeAppFrameUI *
gnome_app_frame_ui_new (void)
{
	return g_object_new (GNOME_APP_TYPE_FRAME_UI, NULL);
}


