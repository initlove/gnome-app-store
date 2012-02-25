/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author: David Liang <lzwang@suse.com>

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

	gint fan_status;
	gint fan_count;
	gint download_count;
	gint pic_count;
	gint current_pic;
};

/* Properties */
enum
{
	PROP_0,
	PROP_APP_INFO,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static void	gnome_app_info_page_set_with_data (GnomeAppInfoPage *info_page, OpenResult *info);
static void	draw_pic (GnomeAppInfoPage *info_page);

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
/*TODO: Problem in getting the real allocated height and the visual height */
        y = clutter_actor_get_y (actor);
	total_height = clutter_actor_get_height (actor);
	visual_height = 700;

	if (y > 0) {
  		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 250,
	 			"y", 0.0, NULL);
		return;
	}

	if (y < (-1.0 * total_height + visual_height)) {
  		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 250,
	 			"y", -1.0 * total_height + 700, NULL);
		return;
	}

}

static void
on_drag_motion (ClutterDragAction   *action,
		ClutterActor        *actor,
		gfloat               delta_x,
		gfloat               delta_y,
		GnomeAppInfoPage *info_page)
{
//printf ("motion %f %f\n", delta_x, delta_y);
}

static void
on_drag_begin (ClutterDragAction   *action,
		ClutterActor        *actor,
		gfloat               event_x,
		gfloat               event_y,
		ClutterModifierType  modifiers,
		GnomeAppInfoPage *info_page)
{
	return;
	gfloat x, y;
	x = clutter_actor_get_x (actor);
        y = clutter_actor_get_y (actor);
	printf ("event x %f y %f\n", event_x, event_y);
	printf ("actor %f %f\n", x, y);

	if (y > 0) {
		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, 100,
				"y", 0.0, NULL);
	}
}


static void
gnome_app_info_page_init (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *main_ui;
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
			"info-page", &main_ui,
			"fan-button", &fan_button,
			"next", &next_button,
			"prev", &prev_button,
			"comment-entry", &comment_entry,
			"comment-button", &comment_button,
			"return-button", &return_button,
			NULL);

	clutter_actor_set_reactive (CLUTTER_ACTOR (info_page), TRUE);
	clutter_actor_set_clip_to_allocation (CLUTTER_ACTOR (info_page), TRUE);
	clutter_container_add_actor (CLUTTER_CONTAINER (info_page), main_ui);
	action = clutter_drag_action_new ();
	clutter_actor_add_action (CLUTTER_ACTOR (info_page), action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (action),
		CLUTTER_DRAG_Y_AXIS);
	        
	g_signal_connect (action, "drag-end", G_CALLBACK (on_drag_end), info_page);
	g_signal_connect (action, "drag-motion", G_CALLBACK (on_drag_motion), info_page);
	g_signal_connect (action, "drag-begin", G_CALLBACK (on_drag_begin), info_page);
/*TODO the entry should be automatic extend .. 
 *
	the comment - entry should be better done.. it might be gnome-app-text	
*/
	return ;
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
		const gchar *uri;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);

open_result_debug (result);
/*TODO: check mimetype, download it */
		uri = open_result_get (result, "downloadlink");
		GError *error = NULL;
		gchar *cmd;
		cmd = g_strdup_printf ("firefox \"%s\" ", uri);
		if (!g_spawn_command_line_async (cmd, &error)) {
			g_debug ("Error in run cmd %s: %s\n", cmd, error->message);
			g_error_free (error);
		}
		g_free (cmd);

		gchar *str;
		ClutterActor *download;

		priv->download_count ++;
		str = g_strdup_printf ("%d downloads", priv->download_count);
		download = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "downloads"));
		clutter_text_set_text (CLUTTER_TEXT (download), str);
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
	GnomeAppInfoPage *page;

	page = GNOME_APP_INFO_PAGE (data);
	if (page->priv->current_pic > 1) {
		page->priv->current_pic --;
		draw_pic (page);
	}

	return TRUE;
}

static gpointer
comment_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	page = GNOME_APP_INFO_PAGE (userdata);
	priv = page->priv;
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
	subject = "thanks!";
	message = clutter_text_get_text (CLUTTER_TEXT (comment_entry));
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

G_MODULE_EXPORT gboolean
on_download_button_press (ClutterActor *actor,
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
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), _("- Remove Fan"));
			break;
		case NOT_FAN:
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), _("+ Add Fan"));
			break;
		default:
			/*TODO:*/
			gnome_app_button_set_text (GNOME_APP_BUTTON (actor), _("Not connect"));
			break;
	}

}

/*TODO: after we POST the fan or something, we need to refresh the proxy info page */
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
		return NULL;
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

	function = g_strdup_printf ("/v1/fan/%s/%s", 
			priv->fan_status == NOT_FAN ? "add" : "remove", 
			open_result_get (priv->info, "id"));
	task = gnome_app_task_new (page, "POST", function);
	gnome_app_task_set_callback (task, add_remove_fan_callback);
	gnome_app_task_push (task);

	g_free (function);
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

static void
draw_download_buttons (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *download_group;
	ClutterActor *button;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	GList *list;
	gint i;
	const gchar *val;
	gchar *str;

	priv = info_page->priv;

	g_return_if_fail (priv->info);

	download_group = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "download-group"));

	layout = clutter_box_layout_new ();
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (layout), 10);
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
	layout_box = clutter_box_new (layout);
	       
	/*OCS standard, type begin with 1 .*/
	for (i = 1; ; i++) {
		str = g_strdup_printf ("downloadlink%d", i);
		val = open_result_get (priv->info, str);
		g_free (str);
		if (val) {
			button = CLUTTER_ACTOR (gnome_app_button_new ());
			str = g_strdup_printf ("downloadname%d", i);
			val = open_result_get (priv->info, str);
			g_free (str);
			if (val && val [0]) {
				gnome_app_button_set_text (GNOME_APP_BUTTON (button), (gchar *)val);
			} else {
				gnome_app_button_set_text (GNOME_APP_BUTTON (button), _("Download"));
			}
			clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), button,
				TRUE,
				TRUE,
				TRUE,			
				CLUTTER_BOX_ALIGNMENT_START,
				CLUTTER_BOX_ALIGNMENT_START);
			g_object_set_data (G_OBJECT (button), "itemid", (gpointer) i);
			g_signal_connect (button, "button-press-event", G_CALLBACK (on_download_button_press), info_page);
		} else {
			break;
		}
	}	
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (download_group)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (layout_box));
}

G_MODULE_EXPORT gboolean
on_return_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppStage *stage;

	stage = gnome_app_stage_get_default ();
	gnome_app_stage_load (stage, "GnomeAppFrameUI", NULL);

        return TRUE;
}

static ClutterActor *
get_description_actor (const gchar *desc)
{
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	ClutterActor *group, *ret;
	ClutterActor *text;

	ret = clutter_group_new ();
//	clutter_actor_set_clip_to_allocation (ret, TRUE);

	group = clutter_group_new ();
	text = clutter_text_new ();
	/*TODO, not fixed width */
	clutter_actor_set_width (text, 400);
	clutter_text_set_text (CLUTTER_TEXT (text), desc);
	clutter_text_set_line_wrap (CLUTTER_TEXT (text), TRUE);
	clutter_actor_set_reactive (group, TRUE);
	clutter_container_add_actor (CLUTTER_CONTAINER (group), text);
	clutter_container_add_actor (CLUTTER_CONTAINER (ret), group);

	return ret;
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

static gpointer
load_details_info_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppInfoPage *page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	page = GNOME_APP_INFO_PAGE (userdata);
	priv = page->priv;
	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		g_debug ("Fail to get the details info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		ClutterActor *fans, *downloads;
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

		downloads = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "downloads"));
		val = open_result_get (result, "downloads");
		priv->download_count = atoi (val);
		str = g_strdup_printf (_("%d downloads"), priv->download_count);
		clutter_text_set_text (CLUTTER_TEXT (downloads), str);
		g_free (str);

		/*TODO: if no comments, no need to load the comment */
		comments_details = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comments-details"));
		comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (priv->info, "id"), NULL));
		for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
			clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
		clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));
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
	ClutterActor *downloads, *download_group;
	ClutterActor *fans, *fan_button;
	ClutterActor *comments, *comment_entry, *comment_button;
	ClutterActor *name;
	ClutterActor *personid, *personicon;
	ClutterActor *big_pic;
	ClutterActor *description, *description_actor;
	ClutterActor *comments_details, *comments_details_actor;
	ClutterActor *return_button;
	ClutterAction *action;
	GList *list;

	priv = info_page->priv;

	if (priv->info)
		g_object_unref (priv->info);
	priv->info = g_object_ref (info);
	priv->fan_status = FAN_NOT_DEFINED;

	clutter_script_get_objects (priv->script,
			"score", &score,
			"license", &license,
			"downloads", &downloads,
			"download-group", &download_group,
			"fans", &fans,
			"fan-button", &fan_button,
			"next", &next,
			"prev", &prev,
			"comments", &comments,
			"comment-entry", &comment_entry,
			"comment-button", &comment_button,
			"name", &name,
			"personid", &personid,
			"personicon", &personicon,
			"description", &description,
			"comments-details", &comments_details,
			"return-button", &return_button,
			NULL);


	gint count;
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

	g_object_set (G_OBJECT (score), "score", open_result_get (info, "score"));
	clutter_text_set_text (CLUTTER_TEXT (license), open_result_get (info, "license"));

	val = open_result_get (info, "downloads");
	priv->download_count = atoi (val);
	str = g_strdup_printf (_("%d downloads"), priv->download_count);
	clutter_text_set_text (CLUTTER_TEXT (downloads), str);
	g_free (str);

	set_fan_status (info_page);

	val = open_result_get (info, "fans");
	priv->fan_count = atoi (val);
	str = g_strdup_printf (_("%d fans"), priv->fan_count);
	clutter_text_set_text (CLUTTER_TEXT (fans), str);
	g_free (str);

	str = g_strdup_printf (_("%s comments"), open_result_get (info, "comments"));
	clutter_text_set_text (CLUTTER_TEXT (comments), str);
	g_free (str);

	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	clutter_text_set_text (CLUTTER_TEXT (personid), open_result_get (info, "personid"));
	description_actor = get_description_actor (open_result_get (info, "description"));
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (description)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (description), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (description), CLUTTER_ACTOR (description_actor));
	/*clean the comments_details .. */
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));

	draw_pic (info_page);
	draw_fan_status (info_page);
	set_fan_status (info_page);
	draw_download_buttons (info_page);
	load_details_info (info_page);
}
