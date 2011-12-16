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
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-message-ui.h"

struct _GnomeAppMessageUIPrivate
{
	ClutterLayoutManager *main_layout;
	ClutterActor	*main_box;
	ClutterLayoutManager *folder_layout;
	ClutterActor	*folder_box;
	ClutterLayoutManager *message_layout;
	ClutterActor	*message_box;

	gint		folder_count;
	gchar		*current_folder;
};

G_DEFINE_TYPE (GnomeAppMessageUI, gnome_app_message_ui, CLUTTER_TYPE_GROUP)

static void
gnome_app_message_ui_init (GnomeAppMessageUI *ui)
{
	GnomeAppMessageUIPrivate *priv;

	ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (ui,
	                                                 GNOME_APP_TYPE_MESSAGE_UI,
	                                                 GnomeAppMessageUIPrivate);
	priv->folder_count = 0;
	priv->current_folder = NULL;
}

static void
gnome_app_message_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_message_ui_parent_class)->dispose (object);
}

static void
gnome_app_message_ui_finalize (GObject *object)
{
	GnomeAppMessageUI *ui = GNOME_APP_MESSAGE_UI (object);
	GnomeAppMessageUIPrivate *priv = ui->priv;

	if (priv->current_folder)
		g_free (priv->current_folder);

	G_OBJECT_CLASS (gnome_app_message_ui_parent_class)->finalize (object);
}

static void
gnome_app_message_ui_class_init (GnomeAppMessageUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_message_ui_dispose;
	object_class->finalize = gnome_app_message_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppMessageUIPrivate));
}

static void
message_folder_clean (GnomeAppMessageUI *ui)
{
	clutter_actor_destroy (ui->priv->message_box);
               
	ui->priv->message_layout = clutter_table_layout_new ();
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), 20);
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), 20);
	ui->priv->message_box = clutter_box_new (ui->priv->message_layout);
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (ui->priv->main_layout), ui->priv->message_box,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);
}

static void
message_box_add (GnomeAppMessageUI *ui, OpenResult *result, gint row)
{
	ClutterActor	*icon;
	ClutterActor	*text;
	ClutterActor	*sender;
	const gchar *subject;
	const gchar *status;
	gint status_i;
	const gchar *first;
	const gchar *last;
	gchar *name;
	gchar *label;
	gchar *icon_uri;

	icon = clutter_texture_new ();
	clutter_actor_set_width (icon, 24);
	clutter_actor_set_height (icon, 24);
	text = clutter_text_new ();
	clutter_text_set_ellipsize (CLUTTER_TEXT (text), PANGO_ELLIPSIZE_END);
	sender = clutter_text_new ();
	clutter_text_set_ellipsize (CLUTTER_TEXT (sender), PANGO_ELLIPSIZE_END);

	subject = open_result_get (result, "subject");
	first = open_result_get (result, "firstname");
	last = open_result_get (result, "lastname");
	status = open_result_get (result, "status");
	name = g_strdup_printf ("%s %s", first, last);
	clutter_text_set_text (CLUTTER_TEXT (sender), name);
	g_free (name);

	status_i = atoi (status);
	/* 0: unread, 1: read, 2: answered */
	switch (status_i) {
		case 0:
			label = g_strdup_printf ("<b>%s</b>", subject);
			clutter_text_set_markup (CLUTTER_TEXT (text), label);
			icon_uri = open_app_get_pixmap_uri ("unread");
			clutter_texture_set_from_file (CLUTTER_TEXTURE (icon), icon_uri, NULL);
			g_free (label);
			g_free (icon_uri);
			break;
		case 1:
			clutter_text_set_text (CLUTTER_TEXT (text), subject);
			icon_uri = open_app_get_pixmap_uri ("read");
			clutter_texture_set_from_file (CLUTTER_TEXTURE (icon), icon_uri, NULL);
			g_free (icon_uri);
			break;
		case 2:
			clutter_text_set_text (CLUTTER_TEXT (text), subject);
			icon_uri = open_app_get_pixmap_uri ("reply");
			clutter_texture_set_from_file (CLUTTER_TEXTURE (icon), icon_uri, NULL);
			g_free (icon_uri);
			break;
		default:
			g_critical ("Server status error in message list!");
			break;
	}
	                
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), CLUTTER_ACTOR (icon), 0, row);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), CLUTTER_ACTOR (text), 1, row);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), CLUTTER_ACTOR (sender), 2, row);
}

static gpointer
message_folder_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppMessageUI *ui;
	OpenResult *result;
	OpenResults *results;
	GList *l;
	const gchar *val;
	const gchar *pic;

	results = OPEN_RESULTS (func_result);
	ui = GNOME_APP_MESSAGE_UI (userdata);

	message_folder_clean (ui);

        if (!open_results_get_status (results)) {
		/*TODO: fill the account with default val */
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} 

	ClutterActor *actor;
	gint count;
	gint row;

	count = open_results_get_total_items (results);
	if (count == 0) {
		actor = clutter_text_new ();
		clutter_text_set_text (CLUTTER_TEXT (actor), "(No message)");
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), CLUTTER_ACTOR (actor), 1, 0);
		return NULL;
	}
	row = 0;
	for (l = open_results_get_data (results); l; l = l->next) {
		result = OPEN_RESULT (l->data);
		message_box_add (ui, result, row);
		row ++;
	}

	return NULL;
}

static gboolean
on_message_info_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppMessageUI *ui;
	const gchar *id;

	ui = GNOME_APP_MESSAGE_UI (data);
	id = (const gchar *) g_object_get_data (G_OBJECT (actor), "id");
	if (!ui->priv->current_folder) {
		ui->priv->current_folder = g_strdup (id);
	} else if (strcmp (id, ui->priv->current_folder) == 0) {
		return TRUE;
	} else {
		g_free (ui->priv->current_folder);
		ui->priv->current_folder = g_strdup (id);
	}

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/message/%s", ui->priv->current_folder);
	task = gnome_app_task_new (ui, "GET", function);
	gnome_app_task_add_params (task,
			"page", "0",
			"pagesize", "10",
			NULL);

	gnome_app_task_set_callback (task, message_folder_callback);
	gnome_app_task_push (task);

	g_free (function);

	return TRUE;
}

static gboolean
on_close_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppMessageUI *ui;
	ui = GNOME_APP_MESSAGE_UI (data);
	printf ("on close press\n");
	clutter_actor_destroy (CLUTTER_ACTOR (ui));

	return TRUE;
}
		
static void
set_default_message_box (GnomeAppMessageUI *ui)
{
	ui->priv->current_folder = g_strdup ("0");

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/message/%s", ui->priv->current_folder);
	task = gnome_app_task_new (ui, "GET", function);
	gnome_app_task_add_params (task,
			"page", "0",
			"pagesize", "10",
			NULL);

	gnome_app_task_set_callback (task, message_folder_callback);
	gnome_app_task_push (task);

	g_free (function);
}

static gpointer
set_message_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppMessageUI *ui;
        OpenResult *result;
	OpenResults *results;
	ClutterActor *actor;
        GList *l;
	const gchar *val;

	ui = GNOME_APP_MESSAGE_UI (userdata);
 	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		/*TODO: fill the message with default val */
		g_debug ("Fail to get the message folders info of %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		ui->priv->folder_count = open_results_get_total_items (results);
		/*TODO: if no folders, different ui */

		ui->priv->main_layout = clutter_box_layout_new ();
	 	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (ui->priv->main_layout), TRUE);
		ui->priv->main_box = clutter_box_new (ui->priv->main_layout);
		clutter_container_add_actor (CLUTTER_CONTAINER (ui), CLUTTER_ACTOR (ui->priv->main_box));

		ui->priv->folder_layout = clutter_box_layout_new ();
	 	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (ui->priv->folder_layout), FALSE);
		clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (ui->priv->folder_layout), 20);
		ui->priv->folder_box = clutter_box_new (ui->priv->folder_layout);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (ui->priv->main_layout), ui->priv->folder_box,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);

                ui->priv->message_layout = clutter_table_layout_new ();
		clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), 20);
		clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (ui->priv->message_layout), 20);
		ui->priv->message_box = clutter_box_new (ui->priv->message_layout);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (ui->priv->main_layout), ui->priv->message_box,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);

		for (l = open_results_get_data (results); l ; l = l->next) {
			const gchar *name;
			const gchar *id;
			gint count;

			result = OPEN_RESULT (l->data);
			id = open_result_get (result, "id");
			name = open_result_get (result, "name");
			/*TODO: Display this in the box ? */
			val = open_result_get (result, "messagecount");
			count = atoi (val);

			actor = clutter_text_new ();
			clutter_text_set_text (CLUTTER_TEXT (actor), name);
			g_object_set_data (G_OBJECT (actor), "id", (gpointer)id);
			clutter_actor_set_reactive (actor, TRUE);
			clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (ui->priv->folder_layout), actor,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);
			g_signal_connect (actor, "button-press-event", G_CALLBACK (on_message_info_press), ui);
		}

		gchar *uri;

		actor = clutter_texture_new ();
		clutter_actor_set_reactive (actor, TRUE);
		uri = open_app_get_pixmap_uri ("close");
		clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), uri, NULL);
		clutter_actor_set_width (actor, 24);
		clutter_actor_set_height (actor, 24);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (ui->priv->folder_layout), actor,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);
		g_signal_connect (actor, "button-press-event", G_CALLBACK (on_close_press), ui);

		g_free (uri);

		set_default_message_box (ui);
	}

	return NULL;
}

/*TODO: background color */
GnomeAppMessageUI *
gnome_app_message_ui_new (void)
{
	GnomeAppMessageUI *ui;
	GnomeAppMessageUIPrivate *priv;
	GnomeAppStore *store;
	GnomeAppTask *task;
	const gchar *id = NULL;

	store = gnome_app_store_get_default ();
	id = gnome_app_store_get_username (store);

	if (!id) {
		/*not login */
		/*TODO: need a better way to check the login status */
		return NULL;
	}

	ui = g_object_new (GNOME_APP_TYPE_MESSAGE_UI, NULL);
	priv = ui->priv;

	task = gnome_app_task_new (ui, "GET", "/v1/message");
	gnome_app_task_set_callback (task, set_message_callback);
	gnome_app_task_push (task);
		
	return ui;
}
