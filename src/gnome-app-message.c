/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Smessagete 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <config.h>
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-message.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppMessagePrivate
{
	ClutterLayoutManager *main_layout;
	ClutterActor	*main_box;
	ClutterLayoutManager *folder_list_layout;
	ClutterActor	*folder_list_box;
	ClutterLayoutManager *message_list_layout;
	ClutterActor	*message_list_box;
	ClutterLayoutManager *message_layout;
	ClutterActor	*message_box;
	gint		folder_count;
	gchar		*current_folder;
};

G_DEFINE_TYPE (GnomeAppMessage, gnome_app_message, CLUTTER_TYPE_GROUP)

static void
gnome_app_message_init (GnomeAppMessage *message)
{
	GnomeAppMessagePrivate *priv;

	message->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (message,
	                                                 GNOME_APP_TYPE_MESSAGE,
	                                                 GnomeAppMessagePrivate);
	priv->folder_count = 0;
	priv->current_folder = NULL;
}

static void
gnome_app_message_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_message_parent_class)->dispose (object);
}

static void
gnome_app_message_finalize (GObject *object)
{
	GnomeAppMessage *message = GNOME_APP_MESSAGE (object);
	GnomeAppMessagePrivate *priv = message->priv;

	if (priv->current_folder)
		g_free (priv->current_folder);

	G_OBJECT_CLASS (gnome_app_message_parent_class)->finalize (object);
}

static void
gnome_app_message_class_init (GnomeAppMessageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_message_dispose;
	object_class->finalize = gnome_app_message_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppMessagePrivate));
}

static void
message_folder_clean (GnomeAppMessage *message)
{
	GnomeAppMessagePrivate *priv;

	priv = message->priv;
	if (priv->message_list_box) {
		clutter_actor_destroy (priv->message_list_box);
		priv->message_list_layout = clutter_table_layout_new ();
		clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), 10);
		clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), 10);
		priv->message_list_box = clutter_box_new (priv->message_list_layout);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->main_layout), 
					CLUTTER_ACTOR (priv->message_list_box), 
					0, 1);
	}
}

static void
message_clean (GnomeAppMessage *message)
{
	GnomeAppMessagePrivate *priv;

	priv = message->priv;
	if (priv->message_box) {
		clutter_actor_destroy (priv->message_box);
		priv->message_layout = clutter_box_layout_new ();
	 	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->message_layout), FALSE);
		priv->message_box = clutter_box_new (priv->message_layout);
		clutter_table_layout_set_alignment (CLUTTER_TABLE_LAYOUT (priv->main_layout),
					CLUTTER_ACTOR (priv->message_list_box),
					CLUTTER_TABLE_ALIGNMENT_START,
					CLUTTER_TABLE_ALIGNMENT_START);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->main_layout), 
					CLUTTER_ACTOR (priv->message_box), 
					0, 2);
	}
}

static gboolean
on_reply_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppMessage *message;
	OpenResult *result;

	printf ("on reply press\n");
	message = GNOME_APP_MESSAGE (data);
	result = OPEN_RESULT (g_object_get_data (G_OBJECT (actor), "result"));

	return TRUE;
}

static gboolean
on_message_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppMessage *message;
	GnomeAppMessagePrivate *priv;
	OpenResult *result;
	gchar *name;
	ClutterScript *script;
	ClutterActor *message_info_page;
        ClutterActor *sender_label, *sender_info;
	ClutterActor *subject_label, *subject_info;
	ClutterActor *reply_info;
	ClutterActor *date_label, *date_info;
	ClutterActor *body_info;
	ClutterLayoutManager *layout;

	result = OPEN_RESULT (g_object_get_data (G_OBJECT (actor), "result"));
	message = GNOME_APP_MESSAGE (data);
	priv = message->priv;
	message_clean (message);

        script = gnome_app_script_new_from_file ("message-info-page");
	if (!script) {
		return FALSE;
	}
		
	clutter_script_get_objects (script, "message-info-page", &message_info_page, 
					"sender_label", &sender_label,
					"sender_info", &sender_info,
					"subject_label", &subject_label,
					"subject_info", &subject_info,
					"reply_info", &reply_info,
					"date_label", &date_label,
					"date_info", &date_info,
					"body_info", &body_info,
					NULL);

	name = g_strdup_printf ("%s %s", open_result_get (result, "firstname"), open_result_get (result, "lastname"));
	clutter_text_set_text (CLUTTER_TEXT (sender_info), name);
	g_free (name);
	clutter_text_set_text (CLUTTER_TEXT (subject_info), open_result_get (result, "subject"));
	clutter_text_set_text (CLUTTER_TEXT (date_info), open_result_get (result, "senddate"));
	clutter_text_set_text (CLUTTER_TEXT (body_info), open_result_get (result, "body"));

	/*FIXME:TODO: I donnot want to do this, but clutter script did not recognize the layout setting */
        layout = clutter_box_get_layout_manager (CLUTTER_BOX (message_info_page));
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), sender_label, "column", 0, "row", 0, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), sender_info, "column", 1, "row", 0, 
					 "column-span", 2, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), subject_label, "column", 0, "row", 1, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), subject_info, "column", 1, "row", 1, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), reply_info, "column", 2, "row", 1, 
					"x-expand", FALSE, "x-fill", FALSE, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), date_label, "column", 0, "row", 2, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), date_info, "column", 1, "row", 2, 
					"column-span", 2, NULL);
        clutter_layout_manager_child_set (layout, CLUTTER_CONTAINER (message_info_page), body_info, "column", 0, "row", 3, 
					"column-span", 3, NULL);
		
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->message_layout), message_info_page,
					 TRUE,	/*expand*/
					 TRUE, /*x-fill*/
					 TRUE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);

	g_object_set_data (G_OBJECT (reply_info), "result", result);
	g_signal_connect (reply_info, "button-press-event", G_CALLBACK (on_reply_press), message);
/* TODO: mark the message as read, we should use a stronger tree view, iter, or something to deal with the icon */
#if 0
	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/message/%s/", priv->current_folder, open_result_get (result, "id"));
	task = gnome_app_task_new (message, "GET", function);
	gnome_app_task_set_callback (task, message_read_callback);
	gnome_app_task_push (task);

	g_free (function);
#endif
	return TRUE;
}

static void
message_list_box_add (GnomeAppMessage *message, OpenResult *result, gint row)
{
	GnomeAppMessagePrivate *priv;
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

	priv = message->priv;

	icon = clutter_texture_new ();
	clutter_actor_set_width (icon, 24);
	clutter_actor_set_height (icon, 24);
	text = clutter_text_new ();
	clutter_actor_set_reactive (text, TRUE);
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
	                
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), CLUTTER_ACTOR (icon), 0, row);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), CLUTTER_ACTOR (text), 1, row);
	clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), CLUTTER_ACTOR (sender), 2, row);

	g_object_set_data (G_OBJECT (text), "result", (gpointer )result);
	g_signal_connect (text, "button-press-event", G_CALLBACK (on_message_press), message);
//	g_signal_connect (icon, "button-press-event", G_CALLBACK (on_message_press), message);
//	g_signal_connect (sender, "button-press-event", G_CALLBACK (on_message_press), message);
}

static gpointer
message_folder_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppMessage *message;
	GnomeAppMessagePrivate *priv;
	OpenResult *result;
	OpenResults *results;
	GList *l;
	const gchar *val;
	const gchar *pic;

	results = OPEN_RESULTS (func_result);
	message = GNOME_APP_MESSAGE (userdata);
	priv = message->priv;
	message_folder_clean (message);

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
		clutter_text_set_text (CLUTTER_TEXT (actor), _("(No message)"));
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), CLUTTER_ACTOR (actor), 1, 0);
		return NULL;
	}
	row = 0;
	for (l = open_results_get_data (results); l; l = l->next) {
		result = OPEN_RESULT (l->data);
		message_list_box_add (message, result, row);
		row ++;
	}

	return NULL;
}

static gboolean
on_folder_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppMessage *message;
	GnomeAppMessagePrivate *priv;
	const gchar *id;

	message = GNOME_APP_MESSAGE (data);
	priv = message->priv;
	id = (const gchar *) g_object_get_data (G_OBJECT (actor), "id");
	if (!priv->current_folder) {
		priv->current_folder = g_strdup (id);
	} else if (strcmp (id, priv->current_folder) == 0) {
		return TRUE;
	} else {
		g_free (priv->current_folder);
		priv->current_folder = g_strdup (id);
	}

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/message/%s", priv->current_folder);
	task = gnome_app_task_new (message, "GET", function);
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
	GnomeAppMessage *message;
	message = GNOME_APP_MESSAGE (data);
	printf ("on close press\n");
	clutter_actor_destroy (CLUTTER_ACTOR (message));

	return TRUE;
}
		
static void
set_default_message_list_box (GnomeAppMessage *message)
{
	GnomeAppMessagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	priv = message->priv;
	priv->current_folder = g_strdup ("0");
	function = g_strdup_printf ("/v1/message/%s", priv->current_folder);
	task = gnome_app_task_new (message, "GET", function);
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
	GnomeAppMessage *message;
	GnomeAppMessagePrivate *priv;
        OpenResult *result;
	OpenResults *results;
	ClutterActor *actor;
        GList *l;
	const gchar *val;

 	results = OPEN_RESULTS (func_result);
	message = GNOME_APP_MESSAGE (userdata);
	priv = message->priv;
	if (!open_results_get_status (results)) {
		/*TODO: fill the message with default val */
		g_debug ("Fail to get the message folders info of %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		priv->folder_count = open_results_get_total_items (results);
		/*TODO: if no folders, different message */

		priv->main_layout = clutter_table_layout_new ();
		clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->main_layout), 20);
		clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->main_layout), 20);
		priv->main_box = clutter_box_new (priv->main_layout);
		clutter_container_add_actor (CLUTTER_CONTAINER (message), CLUTTER_ACTOR (priv->main_box));

		priv->folder_list_layout = clutter_box_layout_new ();
	 	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->folder_list_layout), FALSE);
		clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (priv->folder_list_layout), 20);
		priv->folder_list_box = clutter_box_new (priv->folder_list_layout);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->main_layout), 
					CLUTTER_ACTOR (priv->folder_list_box), 
					0, 0);

                priv->message_list_layout = clutter_table_layout_new ();
		clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), 10);
		clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->message_list_layout), 10);
		priv->message_list_box = clutter_box_new (priv->message_list_layout);
		clutter_table_layout_set_alignment (CLUTTER_TABLE_LAYOUT (priv->main_layout),
					CLUTTER_ACTOR (priv->message_list_box),
					CLUTTER_TABLE_ALIGNMENT_START,
					CLUTTER_TABLE_ALIGNMENT_START);

		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->main_layout), 
					CLUTTER_ACTOR (priv->message_list_box), 
					0, 1);

		priv->message_layout = clutter_box_layout_new ();
	 	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->message_layout), FALSE);
		priv->message_box = clutter_box_new (priv->message_layout);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->main_layout), 
					CLUTTER_ACTOR (priv->message_box), 
					0, 2);

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
			clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->folder_list_layout), actor,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);
			g_signal_connect (actor, "button-press-event", G_CALLBACK (on_folder_press), message);
		}

		gchar *uri;

		actor = clutter_texture_new ();
		clutter_actor_set_reactive (actor, TRUE);
		uri = open_app_get_pixmap_uri ("close");
		clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), uri, NULL);
		clutter_actor_set_width (actor, 24);
		clutter_actor_set_height (actor, 24);
		clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->folder_list_layout), actor,
					 FALSE,	/*expand*/
					 FALSE, /*x-fill*/
					 FALSE, /*y-fill*/
					 CLUTTER_BOX_ALIGNMENT_START,
					 CLUTTER_BOX_ALIGNMENT_START);
		g_signal_connect (actor, "button-press-event", G_CALLBACK (on_close_press), message);

		g_free (uri);

		set_default_message_list_box (message);
	}

	return NULL;
}

/*TODO: background color */
GnomeAppMessage *
gnome_app_message_new (void)
{
	GnomeAppMessage *message;
	GnomeAppMessagePrivate *priv;
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

	message = g_object_new (GNOME_APP_TYPE_MESSAGE, NULL);
	priv = message->priv;

	task = gnome_app_task_new (message, "GET", "/v1/message");
	gnome_app_task_set_callback (task, set_message_callback);
	gnome_app_task_push (task);
		
	return message;
}
