/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author: Lance Wang <lzwang@suse.com>

*/
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-comment.h"
#include "gnome-app-comments.h"
#include "gnome-app-application.h"
#include "gnome-app-score-ui.h"
#include "gnome-app-info-page.h"

typedef enum {
	FAN_NOT_DEFINED,
	IS_FAN,
	NOT_FAN,
	FAN_ERROR
} FAN_STATUS;

struct _GnomeAppInfoPagePrivate
{
	GnomeAppApplication *app;
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
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static void	draw_pic (GnomeAppInfoPage *page);

static void
gnome_app_info_page_init (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;

	page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (page,
							 GNOME_APP_TYPE_INFO_PAGE,
							 GnomeAppInfoPagePrivate);
	priv->app = NULL;
	priv->info = NULL;
	priv->script = NULL;
	priv->fan_status = FAN_NOT_DEFINED;
}

static void
info_page_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoPage *info_page;

	info_page = GNOME_APP_INFO_PAGE (object);

	switch (prop_id)
	{
	}
}

static void
info_page_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoPage *info_page;

	info_page = GNOME_APP_INFO_PAGE (object);

	switch (prop_id)
	{
	}
}

static void
info_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->dispose (object);
}

static void
info_page_finalize (GObject *object)
{
	GnomeAppInfoPage *page = GNOME_APP_INFO_PAGE (object);
	GnomeAppInfoPagePrivate *priv = page->priv;

	if (priv->app)
		g_object_unref (priv->app);
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

	object_class->set_property = info_page_set_property;
	object_class->get_property = info_page_get_property;
	object_class->dispose = info_page_dispose;
	object_class->finalize = info_page_finalize;

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

static gboolean
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

static gboolean
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

static gboolean
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
	if (is_blank_text (message))
		return FALSE;

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
			clutter_text_set_text (CLUTTER_TEXT (actor), "- Remove Fan");
			break;
		case NOT_FAN:
			clutter_text_set_text (CLUTTER_TEXT (actor), "+ Add Fan");
			break;
		default:
			clutter_text_set_text (CLUTTER_TEXT (actor), "");
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

static gboolean
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
set_fan_status (GnomeAppInfoPage *page)
{
	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/fan/status/%s", open_result_get (page->priv->info, "id"));
	task = gnome_app_task_new (page, "GET", function);
	gnome_app_task_set_callback (task, fan_status_callback);
	gnome_app_task_push (task);

	g_free (function);
}

static void
draw_download_buttons (GnomeAppInfoPage *page)
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

	priv = page->priv;
	download_group = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "download-group"));

	layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
	layout_box = clutter_box_new (layout);
	       
	/*OCS standard, type begin with 1 .*/
	for (i = 1; ; i++) {
		str = g_strdup_printf ("downloadlink%d", i);
		val = open_result_get (priv->info, str);
		g_free (str);
		if (val) {
			button = clutter_text_new ();
			str = g_strdup_printf ("downloadname%d", i);
			val = open_result_get (priv->info, str);
			g_free (str);
			if (val && val [0]) {
				clutter_text_set_text (CLUTTER_TEXT (button), val);
			} else {
				clutter_text_set_text (CLUTTER_TEXT (button), "Download");
			}
			clutter_actor_set_reactive (button, TRUE);
			clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), button,
				TRUE,
				TRUE,
				TRUE,			
				CLUTTER_BOX_ALIGNMENT_START,
				CLUTTER_BOX_ALIGNMENT_START);
			g_object_set_data (G_OBJECT (button), "itemid", (gpointer) i);
			g_signal_connect (button, "button-press-event", G_CALLBACK (on_download_button_press), page);
		} else {
			break;
		}
	}	
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (download_group)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (layout_box));
}

static gboolean
on_return_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppInfoPage *page;
        GnomeAppApplication *app;

	page = GNOME_APP_INFO_PAGE (data);
	if (page->priv->app)
		gnome_app_application_load (page->priv->app, UI_TYPE_FRAME_UI, NULL);
	else
		g_error ("Set the application to the info-page!\n");

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
	clutter_actor_set_clip_to_allocation (ret, TRUE);

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
	clutter_script_get_objects (priv->script,
			"big-pic", &big_pic,
			"next", &next,
			"prev", &prev,
			NULL);
	/*Use the small icon first, as it was already cached*/
	gnome_app_ui_set_icon (big_pic, open_result_get (priv->info, "smallpreviewpic1"));

	str = g_strdup_printf ("previewpic%d", priv->current_pic);
	gnome_app_ui_set_icon (big_pic, open_result_get (priv->info, str));
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
gnome_app_info_page_new_with_app (GnomeAppApplication *app)
{
	GnomeAppInfoPage *info_page;

	info_page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);

	info_page->priv->app = g_object_ref (app);

	return info_page;
}

static void
on_comment_entry_paint (ClutterActor *actor,
		gpointer      data)
{
	ClutterActorBox allocation = { 0, };
     	gfloat width, height;
   	clutter_actor_get_allocation_box (actor, &allocation);
 	clutter_actor_box_clamp_to_pixel (&allocation);
      	clutter_actor_box_get_size (&allocation, &width, &height);
    	cogl_set_source_color4ub (0, 0, 0, 255);
	cogl_path_rectangle (0, 0, width, height);
	cogl_path_stroke ();
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
		str = g_strdup_printf ("%d fans", priv->fan_count);
		clutter_text_set_text (CLUTTER_TEXT (fans), str);
		g_free (str);

		downloads = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "downloads"));
		val = open_result_get (result, "downloads");
		priv->download_count = atoi (val);
		str = g_strdup_printf ("%d downloads", priv->download_count);
		clutter_text_set_text (CLUTTER_TEXT (downloads), str);
		g_free (str);

		comments_details = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comments-details"));
		comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (priv->info, "id"), NULL));
		for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
			clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
		clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));
	}
}

static void
load_details_info (GnomeAppInfoPage *page)
{
	GnomeAppInfoPagePrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	priv = page->priv;
	function = g_strdup_printf ("/v1/content/data/%s", open_result_get (priv->info, "id"));
	task = gnome_app_task_new (page, "GET", function);
	gnome_app_task_set_callback (task, load_details_info_callback);
	gnome_app_task_push (task);

	g_free (function);
}

void
gnome_app_info_page_set_with_data (GnomeAppInfoPage *page, OpenResult *info)
{
	g_return_if_fail (info != NULL);

	GnomeAppInfoPagePrivate *priv;
	gchar *filename;
	const gchar *val;
	gchar *str;
	GError *error;
	gint i;

	ClutterActor *info_page;
	ClutterActor *next, *prev;
	ClutterActor *score, *score_actor;
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

	priv = page->priv;

	if (priv->info)
		g_object_unref (priv->info);
	priv->info = g_object_ref (info);
	priv->fan_status = FAN_NOT_DEFINED;

	if (!priv->script) {
		error = NULL;
		filename = open_app_get_ui_uri ("app-info-page");
		priv->script = clutter_script_new ();
		clutter_script_load_from_file (page->priv->script, filename, &error);
		g_free (filename);
		if (error) {
			g_critical ("Error in load script %s.", error->message);
			g_error_free (error);
			return ;
		}
		info_page = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "info-page"));
		clutter_container_add_actor (CLUTTER_CONTAINER (page), info_page);
		clutter_actor_set_reactive (CLUTTER_ACTOR (page), TRUE);
		action = clutter_drag_action_new ();
		clutter_actor_add_action (CLUTTER_ACTOR (page), action);
		clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (action),
			CLUTTER_DRAG_Y_AXIS);

	} 
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
	filename = open_app_get_pixmap_uri ("go-previous");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (prev), filename, NULL);
	g_free (filename);
	filename = open_app_get_pixmap_uri ("go-next");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (next), filename, NULL);
	g_free (filename);
	draw_pic (page);

	score_actor = CLUTTER_ACTOR (gnome_app_score_ui_new_with_score (open_result_get (info, "score")));
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (score)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (score), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (score), score_actor);
	clutter_text_set_text (CLUTTER_TEXT (license), open_result_get (info, "license"));

	val = open_result_get (info, "downloads");
	priv->download_count = atoi (val);
	str = g_strdup_printf ("%d downloads", priv->download_count);
	clutter_text_set_text (CLUTTER_TEXT (downloads), str);
	g_free (str);

	val = open_result_get (info, "fans");
	priv->fan_count = atoi (val);
	str = g_strdup_printf ("%d fans", priv->fan_count);
	clutter_text_set_text (CLUTTER_TEXT (fans), str);
	g_free (str);

	str = g_strdup_printf ("%s comments", open_result_get (info, "comments"));
	clutter_text_set_text (CLUTTER_TEXT (comments), str);
	g_free (str);

	g_signal_connect (comment_button, "button-press-event", G_CALLBACK (on_comment_button_press), page);

	ClutterColor  cursor_color     = { 0xff, 0x33, 0x33, 0xff };
      	ClutterColor  selected_text_color = { 0x00, 0x00, 0xff, 0xff };

	clutter_text_set_line_wrap (CLUTTER_TEXT (comment_entry), TRUE);
    	clutter_text_set_cursor_color (CLUTTER_TEXT (comment_entry), &cursor_color);
  	clutter_text_set_selected_text_color (CLUTTER_TEXT (comment_entry), &selected_text_color);
	  
	g_signal_connect (comment_entry, "paint",
			G_CALLBACK (on_comment_entry_paint),
			NULL);

	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	clutter_text_set_text (CLUTTER_TEXT (personid), open_result_get (info, "personid"));
	description_actor = get_description_actor (open_result_get (info, "description"));
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (description)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (description), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (description), CLUTTER_ACTOR (description_actor));

	/*clean the comments_details .. */
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));

	g_signal_connect (prev, "button-press-event", G_CALLBACK (on_prev_button_press), page);
	g_signal_connect (next, "button-press-event", G_CALLBACK (on_next_button_press), page);

	draw_fan_status (page);
	set_fan_status (page);
	g_signal_connect (fan_button, "button-press-event", G_CALLBACK (on_fan_button_press), page);

	draw_download_buttons (page);

	filename = open_app_get_pixmap_uri ("back");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (return_button), filename, NULL);
	g_free (filename);
	g_signal_connect (return_button, "button-press-event", G_CALLBACK (on_return_button_press), page);

	load_details_info (page);
	return ;
}
