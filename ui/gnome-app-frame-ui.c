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
	ClutterActor *prev;
	ClutterActor *next;
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
	frame_ui = GNOME_APP_FRAME_UI (data);	
	query = gnome_app_query_new ();
	g_object_set (query, QUERY_SEARCH, search, NULL);
	gnome_app_infos_stage_load_query (frame_ui->priv->infos_stage, query);

	g_object_unref (query);
}

/*FIXME: the second args seems broken!!! */
static gboolean
on_category_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	const gchar *label_new;
	GnomeAppFrameUI *frame_ui;
	GnomeAppQuery *query;

        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		label_new = st_button_get_label (ST_BUTTON (actor));
		frame_ui = GNOME_APP_FRAME_UI (data);
		query = gnome_app_query_new ();
		g_object_set (query, QUERY_GROUP, label_new, NULL);
		gnome_app_infos_stage_load_query (frame_ui->priv->infos_stage, query);
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
	GnomeAppFrameUI *frame_ui;

	frame_ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		gnome_app_infos_stage_page_change (frame_ui->priv->infos_stage, -1);
		break;
	}

	return TRUE;
}

static gboolean
on_next_event (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppFrameUI *frame_ui;

	frame_ui = GNOME_APP_FRAME_UI (data);
        switch (event->type)
        {
        case CLUTTER_BUTTON_PRESS:
		gnome_app_infos_stage_page_change (frame_ui->priv->infos_stage, 1);
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
					"prev-icon", &priv->prev,
					"next-icon", &priv->next,
					NULL);
	clutter_container_add_actor (CLUTTER_CONTAINER (ui), priv->ui_group);
	priv->categories = create_category_list (ui);
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->categories_group), priv->categories);
	priv->infos_stage = gnome_app_infos_stage_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (priv->infos_stage_group), CLUTTER_ACTOR (priv->infos_stage));

//script connect did not work?
        g_signal_connect (priv->search_entry, "activate",
                    G_CALLBACK (on_search_activate),
                    ui);
		
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


