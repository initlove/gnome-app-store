/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author:  Lance Wang <lzwang@suse.com>
	 David Liang <dliang@novell.com>
*/
#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-widgets.h"
#include "gnome-app-score.h"
#include "gnome-app-task.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"
#include "gnome-app-stage.h"
#include "gnome-app-info-page.h"
#include "gnome-app-ui-utils.h"

typedef enum {
	FAN_NOT_DEFINED,
	IS_FAN,
	NOT_FAN,
	FAN_ERROR
} FAN_STATUS;

struct _GnomeAppInfoPagePrivate
{
	OpenResult *info;
	ClutterScript *script;
	ClutterActor *main_ui;
	gint fan_status;
	gint fan_count;
	gboolean fan_lock;
	gint download_count;
	gint download_links;
	gint pic_count;
	gint current_pic;

	ClutterActor *layout_box;
	gboolean download_expand;
};

/* Properties */
enum
{
	PROP_0,
	PROP_APP_INFO,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static gboolean	on_download_item_press (ClutterActor *actor,
					ClutterEvent *event,
			                gpointer      data);
static void	gnome_app_info_page_set_with_data (GnomeAppInfoPage *info_page, OpenResult *info);
static void	draw_pic (GnomeAppInfoPage *info_page);

static void
draw_download_buttons (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *download_price;
	ClutterActor *download_name;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	ClutterActor *download_count;
	ClutterActor *download_group;
	GList *list;
	gint i;
	gfloat balance, price;
	gboolean balance_display;
	const gchar *val;
	gchar *str;

	priv = info_page->priv;

	g_return_if_fail (priv->info);

	clutter_script_get_objects (priv->script, 
				"download-count", &download_count,
				"download-group", &download_group,
				NULL);

	layout = clutter_table_layout_new ();
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (layout), 10);
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (layout), 10);
	layout_box = clutter_box_new (layout);

	balance = gnome_app_stage_get_balance (gnome_app_stage_get_default ());
	balance_display = FALSE;
	/*OCS standard, type begin with 1 .*/
	for (i = 1; ; i++) {
		str = g_strdup_printf ("downloadlink%d", i);
		val = open_result_get (priv->info, str);
		g_free (str);
		if (val) {
			download_name = clutter_text_new ();
			download_price = CLUTTER_ACTOR (gnome_app_button_new ());
			str = g_strdup_printf ("downloadname%d", i);
			val = open_result_get (priv->info, str);
			g_free (str);
			if (val && val [0]) {
				clutter_text_set_text (CLUTTER_TEXT (download_name), (gchar *)val);
			} else {
				clutter_text_set_text (CLUTTER_TEXT (download_name), _("Download Link"));
			}
			str = g_strdup_printf ("downloadprice%d", i);
			val = open_result_get (priv->info, str);
			g_free (str);
			price = atof (val);
			if (price == 0) {
				gnome_app_button_set_text (GNOME_APP_BUTTON (download_price), _("Free"));
				g_object_set (G_OBJECT (download_price), "text-color", "green", NULL);
			} else {
				if (!balance_display)
					balance_display = TRUE;
				gnome_app_button_set_text (GNOME_APP_BUTTON (download_price), (gchar *)val);
				if (price > balance)
					g_object_set (G_OBJECT (download_price), "text-color", "red", NULL);
				else
					g_object_set (G_OBJECT (download_price), "text-color", "yellow", NULL);
			}
			clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (download_price), 0, i);
			clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (download_name), 1, i);
			g_object_set_data (G_OBJECT (download_price), "itemid", (gpointer) i);
			g_signal_connect (download_price, "button-press-event", G_CALLBACK (on_download_item_press), info_page);
		} else {
			break;
		}
	}
	if (balance_display) {
		ClutterActor *actor;

		actor = clutter_text_new ();
		clutter_text_set_text (CLUTTER_TEXT (actor), _("Your balance is: "));
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (actor), 0, 0);

		str = g_strdup_printf ("%f", balance);
		actor = clutter_text_new ();
		clutter_text_set_text (CLUTTER_TEXT (actor), str);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (actor), 1, 0);
		g_free (str);
	}
	priv->download_links = i - 1;
	/*TODO: the download count may not good, should check with the server*/
	str = g_strdup_printf (_("%d downloads  (%d link)"), priv->download_count, priv->download_links);
	gnome_app_button_set_text (GNOME_APP_BUTTON (download_count), str);
	g_free (str);
	if (priv->layout_box)
		g_object_unref (priv->layout_box);
	priv->layout_box = layout_box;
}

static void
set_download_expand (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *download_group;
	ClutterActor *download_count;
	GList *list;

	priv = info_page->priv;
	clutter_script_get_objects (priv->script, 
			"download-group", &download_group,
			"download-count", &download_count,
			NULL);
	if (priv->download_expand) {
		gnome_app_button_set_selected (GNOME_APP_BUTTON (download_count), TRUE);
		clutter_container_add_actor (CLUTTER_CONTAINER (download_group), priv->layout_box);
	} else {
		/*remove_actor will destroy the widget, I should ref it, this is by the clutter doc*/
		/*doing this becase 'hide' did not work well ... */
		gnome_app_button_set_selected (GNOME_APP_BUTTON (download_count), FALSE);
		g_object_ref (priv->layout_box);
		for (list = clutter_container_get_children (CLUTTER_CONTAINER (download_group)); list; list = list->next)
			clutter_container_remove_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (list->data));
	}
}

static void
on_drag_end (ClutterDragAction   *action,
		ClutterActor        *actor,
		gfloat               event_x,
		gfloat               event_y,
		ClutterModifierType  modifiers,
		GnomeAppInfoPage *info_page)
{
	gfloat total_height;
	gfloat visual_height;
	gfloat x, y;

        y = clutter_actor_get_y (actor);
	total_height = clutter_actor_get_height (actor);
	visual_height = clutter_actor_get_height (CLUTTER_ACTOR (gnome_app_stage_get_default ()));

	if (total_height <= visual_height) {
  		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 50,
	 			"y",  0.0, NULL);
		return;
	}
	if (y > 0) {
  		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 50,
	 			"y", 0.0, NULL);
		return;
	}

	if (y < (-1.0 * total_height + visual_height)) {
  		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 50,
	 			"y", -1.0 * total_height + visual_height - 50, NULL);
		return;
	}

}

static void
gnome_app_info_page_init (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *next_button, *prev_button;
	ClutterActor *fan_button;
	ClutterActor *comment_entry, *comment_button;
	ClutterActor *return_button;
	ClutterAction *action;

	info_page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info_page,
							 GNOME_APP_TYPE_INFO_PAGE,
							 GnomeAppInfoPagePrivate);
	priv->script = gnome_app_script_new_from_file ("app-info-page");
	if (!priv->script) {
		return ;
	}
	priv->info = NULL;
	priv->fan_status = FAN_NOT_DEFINED;
	clutter_script_connect_signals (priv->script, info_page);
	clutter_script_get_objects (priv->script,
			"info-page", &priv->main_ui,
			"fan-button", &fan_button,
			"next", &next_button,
			"prev", &prev_button,
			"comment-entry", &comment_entry,
			"comment-button", &comment_button,
			"return-button", &return_button,
			NULL);

	clutter_actor_set_reactive (CLUTTER_ACTOR (info_page), TRUE);
	clutter_container_add_actor (CLUTTER_CONTAINER (info_page), priv->main_ui);
	action = clutter_drag_action_new ();
	clutter_actor_add_action (CLUTTER_ACTOR (info_page), action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (action),
		CLUTTER_DRAG_Y_AXIS);
	        
	g_signal_connect (action, "drag-end", G_CALLBACK (on_drag_end), info_page);
}

static void
gnome_app_info_page_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;

	info_page = GNOME_APP_INFO_PAGE (object);
	priv = info_page->priv;

	switch (prop_id)
	{
		case PROP_APP_INFO:
			gnome_app_info_page_set_with_data (info_page, g_value_get_object (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_info_page_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;

	info_page = GNOME_APP_INFO_PAGE (object);
	priv = info_page->priv;

	switch (prop_id)
	{
		case PROP_APP_INFO:
			g_value_set_object (value, priv->info);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_info_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->dispose (object);
}

static void
gnome_app_info_page_finalize (GObject *object)
{
	GnomeAppInfoPage *page = GNOME_APP_INFO_PAGE (object);
	GnomeAppInfoPagePrivate *priv = page->priv;

	if (priv->info)
		g_object_unref (priv->info);
	if (priv->script)
		g_object_unref (priv->script);
	if (priv->layout_box)
		g_object_unref (priv->layout_box);

	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->finalize (object);
}

static void
gnome_app_info_page_class_init (GnomeAppInfoPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_info_page_set_property;
	object_class->get_property = gnome_app_info_page_get_property;
	object_class->dispose = gnome_app_info_page_dispose;
	object_class->finalize = gnome_app_info_page_finalize;

	g_object_class_install_property (object_class,
  			PROP_APP_INFO,
			g_param_spec_object ("info",
				"app info",
				"app info",
				G_TYPE_OBJECT,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPagePrivate));
}

static gpointer
download_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	page = GNOME_APP_INFO_PAGE (userdata);
	priv = page->priv;
	results = OPEN_RESULTS (func_result);
        if (!open_results_get_status (results)) {
		g_debug ("Fail to get the download info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		GList *list;
		OpenResult *result;
		gchar *str;
		ClutterActor *download_count;
		const gchar *uri;
		GError *error;
		gchar *cmd;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);
	/*TODO: check mimetype, download it */
		uri = open_result_get (result, "downloadlink");
		if (uri) {
			cmd = g_strdup_printf ("gnome-open \"%s\" ", uri);
			error = NULL;
			if (!g_spawn_command_line_async (cmd, &error)) {
				g_debug ("Error in run cmd %s: %s\n", cmd, error->message);
				g_error_free (error);
			}
			g_free (cmd);
		}

		priv->download_count ++;
		download_count = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "download-count"));
		str = g_strdup_printf (_("%d downloads  (%d link)"), priv->download_count, priv->download_links);
		gnome_app_button_set_text (GNOME_APP_BUTTON (download_count), str);
		g_free (str);
	}

	return NULL;
}

G_MODULE_EXPORT gboolean
on_next_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *page;

	page = GNOME_APP_INFO_PAGE (data);
	if (page->priv->current_pic < page->priv->pic_count) {
		page->priv->current_pic ++;
		draw_pic (page);
	}

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_prev_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;

	info_page = GNOME_APP_INFO_PAGE (data);
	priv = info_page->priv;
	if (priv->current_pic > 1) {
		priv->current_pic --;
		draw_pic (info_page);
	}

	return TRUE;
}

static gpointer
comment_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	info_page = GNOME_APP_INFO_PAGE (userdata);
	priv = info_page->priv;
	results = OPEN_RESULTS (func_result);
        if (!open_results_get_status (results)) {
		g_debug ("Fail to comment: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		printf ("commented!\n");
		//TODO: we need to refresh it , force comment entry to reload ..
		ClutterActor *comments_details;
	        ClutterActor *comments_details_actor;
		GList *list;

		comments_details = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comments-details"));
		comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (priv->info, "id"), NULL));
		for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
			clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
		clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));
	}
}

G_MODULE_EXPORT gboolean
on_comment_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *comment_entry;
	const gchar *subject;
	const gchar *message;

	page = GNOME_APP_INFO_PAGE (data);
	priv = page->priv;
	comment_entry = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comment-entry"));
	//TODO: subject needed? make it simple?
	subject = "thanks!";
	message = gnome_app_text_get_text (GNOME_APP_TEXT (comment_entry));
	if (open_app_pattern_match ("blank", message, NULL)) {
		//TODO: doing sth
		return FALSE;
	}

	GnomeAppTask *task;
	gchar *function;

	task = gnome_app_task_new (page, "POST", "/v1/comments/add");
	gnome_app_task_add_params (task,
				"type", "1",
				"content", open_result_get (priv->info, "id"),
				"content2", "0",
				"subject", subject,
				"message", message,
				NULL);
	gnome_app_task_set_callback (task, comment_callback);
	gnome_app_task_push (task);
}

static gboolean
on_download_item_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
 	gchar *function;
	const gchar *content_id;
	gint item_id;

	page = GNOME_APP_INFO_PAGE (data);
	priv = page->priv;

	content_id = open_result_get (priv->info, "id");
	item_id = (gint) g_object_get_data (G_OBJECT (actor), "itemid");

	function = g_strdup_printf ("/v1/content/download/%s/%d", content_id, item_id);
	task = gnome_app_task_new (page, "GET", function);
	gnome_app_task_set_callback (task, download_callback);
	gnome_app_task_push (task);
		
	g_free (function);

        return TRUE;
}

static void
draw_fan_status (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *actor;

	priv = page->priv;

	actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "fan-button"));
	switch (priv->fan_status) {
		case IS_FAN:
			clutter_actor_set_opacity (actor, 255);
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), _("- Remove Fan"));
			break;
		case NOT_FAN:
			clutter_actor_set_opacity (actor, 255);
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), _("+ Add Fan"));
			break;
		default:
			clutter_actor_set_opacity (actor, 0);
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), NULL);
			break;
	}

}

static gpointer
add_remove_fan_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	page = GNOME_APP_INFO_PAGE (userdata);
	priv = page->priv;
	results = OPEN_RESULTS (func_result);
        if (!open_results_get_status (results)) {
		g_debug ("Fail to add fan %s\n", open_results_get_meta (results, "message"));
	} else {
		const gchar *val;
		gchar *str;
		ClutterActor *fans;

		val = open_result_get (priv->info, "fans");
		if (priv->fan_status == NOT_FAN) {
			priv->fan_status = IS_FAN;
			priv->fan_count ++;
		} else {
			priv->fan_status = NOT_FAN;
			priv->fan_count --;
		}
		str = g_strdup_printf ("%d fans", priv->fan_count);
		fans = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "fans"));
		clutter_text_set_text (CLUTTER_TEXT (fans), str);
		g_free (str);

		draw_fan_status (page);
	}
	priv->fan_lock = FALSE;	
	return NULL;
}

G_MODULE_EXPORT gboolean
on_downloads_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	info_page = GNOME_APP_INFO_PAGE (data);
	priv = info_page->priv;

	priv->download_expand = !priv->download_expand;

	set_download_expand (info_page);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_fan_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	page = GNOME_APP_INFO_PAGE (data);
	priv = page->priv;
	if (priv->fan_lock)
		return FALSE;
	function = g_strdup_printf ("/v1/fan/%s/%s", 
			priv->fan_status == NOT_FAN ? "add" : "remove", 
			open_result_get (priv->info, "id"));
	task = gnome_app_task_new (page, "POST", function);
	gnome_app_task_set_callback (task, add_remove_fan_callback);
	gnome_app_task_push (task);

	g_free (function);
	return TRUE;
}

static gpointer
fan_status_callback (gpointer userdata, gpointer func_result)
{
	OpenResults *results;
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate*priv;

	results = OPEN_RESULTS (func_result);
	page = GNOME_APP_INFO_PAGE (userdata);
	priv = page->priv;
        if (!open_results_get_status (results)) {
		g_debug ("Fail to get the fan status info: %s\n", open_results_get_meta (results, "message"));
		priv->fan_status = FAN_ERROR;
	} else {
		GList *list;
		OpenResult *result;
		const gchar *val;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);
		val = open_result_get (result, "status");
		if (strcmp (val, "fan") == 0)
			priv->fan_status = IS_FAN;
		else
			priv->fan_status = NOT_FAN;
	}

	draw_fan_status (page);

	return NULL;
}

static void
set_fan_status (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	priv = info_page->priv;

	g_return_if_fail (priv->info);

	function = g_strdup_printf ("/v1/fan/status/%s", open_result_get (priv->info, "id"));
	task = gnome_app_task_new (info_page, "GET", function);
	gnome_app_task_set_callback (task, fan_status_callback);
	gnome_app_task_push (task);

	g_free (function);
}

G_MODULE_EXPORT gboolean
on_return_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppStage *stage;

	stage = gnome_app_stage_get_default ();
	gnome_app_stage_load (stage, GNOME_APP_STAGE_LOAD_DEFAULT, "GnomeAppFrame", NULL);

        return TRUE;
}

static void
draw_pic (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *big_pic, *next, *prev;
	gchar *str;

	priv = page->priv;

	g_return_if_fail (priv->info);

	clutter_script_get_objects (priv->script,
			"big-pic", &big_pic,
			"next", &next,
			"prev", &prev,
			NULL);
	/*Use the small icon first, as it was already cached*/
	gnome_app_set_icon (big_pic, open_result_get (priv->info, "smallpreviewpic1"));

	str = g_strdup_printf ("previewpic%d", priv->current_pic);
	gnome_app_set_icon (big_pic, open_result_get (priv->info, str));
	g_free (str);

	if (priv->current_pic > priv->pic_count) {
		g_critical ("current pic is over pic count !");
	} else if (priv->current_pic == priv->pic_count) {
		clutter_actor_hide (next);
	} else {
		clutter_actor_show (next);
	}
	if (priv->current_pic < 1) {
		g_critical ("current pic is less than zero ? ");
	} else if (priv->current_pic == 1) {
		clutter_actor_hide (prev);
	} else if (priv->current_pic > 1) {
		clutter_actor_show (prev);
	}
}

GnomeAppInfoPage *
gnome_app_info_page_new (void)
{
	GnomeAppInfoPage *info_page;

	info_page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);

	return info_page;
}


static void
load_comments (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *comments_details;
	ClutterActor *comments_details_actor;
	GList *list;

	priv = info_page->priv;
	/*TODO: if no comments, no need to load the comment */
	comments_details = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comments-details"));
	comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (priv->info, "id"), NULL));
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));
}

static gpointer
load_details_info_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	info_page = GNOME_APP_INFO_PAGE (userdata);
	priv = info_page->priv;
	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		g_debug ("Fail to get the details info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		ClutterActor *fans, *download_count;
		ClutterActor *comments_details, *comments_details_actor;
		GList *list;
		OpenResult *result;
		const gchar *val;
		gchar *str;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);

		fans = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "fans"));
		val = open_result_get (result, "fans");
		priv->fan_count = atoi (val);
		str = g_strdup_printf (_("%d fans"), priv->fan_count);
		clutter_text_set_text (CLUTTER_TEXT (fans), str);
		g_free (str);

		/*the download count will change, the download link will not */
		download_count = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "download-count"));
		val = open_result_get (result, "downloads");
		priv->download_count = atoi (val);
		str = g_strdup_printf (_("%d downloads  (%d link)"), priv->download_count, priv->download_links);
		gnome_app_button_set_text (GNOME_APP_BUTTON (download_count), str);
		g_free (str);
		load_comments (info_page);
	}
}

static void
load_details_info (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	priv = info_page->priv;

	g_return_if_fail (priv->info);

	function = g_strdup_printf ("/v1/content/data/%s", open_result_get (priv->info, "id"));
	task = gnome_app_task_new (info_page, "GET", function);
	gnome_app_task_set_callback (task, load_details_info_callback);
	gnome_app_task_push (task);

	g_free (function);
}

static void
gnome_app_info_page_set_with_data (GnomeAppInfoPage *info_page, OpenResult *info)
{
	g_return_if_fail (info != NULL);

	GnomeAppInfoPagePrivate *priv;
	const gchar *val;
	gchar *str;
	gint i;

	ClutterActor *next, *prev;
	ClutterActor *score;
	ClutterActor *license;
	ClutterActor *fans;
	ClutterActor *download_count;
	ClutterActor *comment_count, *comment_entry;
	ClutterActor *name;
	ClutterActor *personid, *personicon;
	ClutterActor *big_pic;
	ClutterActor *description, *description_actor;
	ClutterActor *comments_details, *comments_details_actor;
	ClutterActor *return_button;
	ClutterAction *action;
	GList *list;
	gint count;

	priv = info_page->priv;

	if (priv->info)
		g_object_unref (priv->info);
	priv->info = g_object_ref (info);
	priv->fan_status = FAN_NOT_DEFINED;
	priv->download_expand = FALSE;
	priv->download_count = 0;
	priv->download_links = 0;
	priv->layout_box = NULL;
	priv->fan_lock = FALSE;
	clutter_actor_set_y (CLUTTER_ACTOR (info_page), 0.0);

	clutter_script_get_objects (priv->script,
			"name", &name,
			"score", &score,
			"license", &license,
			"fans", &fans,
			"next", &next,
			"prev", &prev,
			"download-count", &download_count,
			"comment-count", &comment_count,
			"comment-entry", &comment_entry,
			"personid", &personid,
			"personicon", &personicon,
			"description", &description,
			"comments-details", &comments_details,
			"return-button", &return_button,
			NULL);
	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	clutter_text_set_text (CLUTTER_TEXT (personid), open_result_get (info, "personid"));
	gnome_app_button_set_selected (GNOME_APP_BUTTON (download_count), FALSE);

	for (count = 0; ; count ++) {
		str = g_strdup_printf ("smallpreviewpic%d", count + 1);
		val = open_result_get (info, str);
		g_free (str);
		if (!val)
			break;
	}
	/* assume we should at least one pic, if only one pic, donnot add to small pics */
	priv->pic_count = count;
	priv->current_pic = 1;

	g_object_set (G_OBJECT (score), "score", open_result_get (info, "score"), NULL);

	clutter_text_set_text (CLUTTER_TEXT (license), open_result_get (info, "license"));

	val = open_result_get (info, "downloads");
	priv->download_count = atoi (val);

	val = open_result_get (info, "fans");
	priv->fan_count = atoi (val);
	str = g_strdup_printf (_("%d fans"), priv->fan_count);
	clutter_text_set_text (CLUTTER_TEXT (fans), str);
	g_free (str);

	gnome_app_text_set_text (GNOME_APP_TEXT (comment_entry), NULL);

	str = g_strdup_printf (_("%s comments"), open_result_get (info, "comments"));
	clutter_text_set_text (CLUTTER_TEXT (comment_count), str);
	g_free (str);
	
	clutter_text_set_text (CLUTTER_TEXT (description), open_result_get (info, "description"));

	/*clean the comments_details .. */
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));

	draw_pic (info_page);
	set_fan_status (info_page);
	draw_download_buttons (info_page);
	set_download_expand (info_page);	
	load_comments (info_page);

/* TODO: this make our info page with higher quality, but will make one more request
 * 	also we need the lock the interface, currently, I disable it.
	load_details_info (info_page);
*/
}
