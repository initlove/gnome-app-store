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
	OpenResults *results;

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
		uri = open_result_get (result, "downloadlink");
		printf ("download link is %s\n", uri);
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

static gboolean
on_download_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppTask *task;
 	gchar *function;
	gchar *content_id;
	gint item_id;

	content_id = (gchar *) g_object_get_data (G_OBJECT (actor), "contentid");
	item_id = (gint) g_object_get_data (G_OBJECT (actor), "itemid");

	function = g_strdup_printf ("/v1/content/download/%s/%d", content_id, item_id);
	task = gnome_app_task_new (data, "GET", function);
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
			clutter_actor_show (actor);
			break;
		case NOT_FAN:
			clutter_text_set_text (CLUTTER_TEXT (actor), "+ Add Fan");
			clutter_actor_show (actor);
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
		gint count;
		ClutterActor *fans;

		val = open_result_get (priv->info, "fans");
		count = atoi (val);
		if (priv->fan_status == NOT_FAN) {
			priv->fan_status = IS_FAN;
			count ++;
		} else {
			priv->fan_status = NOT_FAN;
			count --;
		}
		str = g_strdup_printf ("%d fans", count);
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
	task = gnome_app_task_new (NULL, "POST", function);
	gnome_app_task_set_userdata (task, page);
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

	function = g_strdup_printf ("v1/fan/status/%s", open_result_get (page->priv->info, "id"));
	task = gnome_app_task_new (NULL, "GET", function);
	gnome_app_task_set_userdata (task, page);
	gnome_app_task_set_callback (task, fan_status_callback);
	gnome_app_task_push (task);

	g_free (function);
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

static void
on_description_drag_end (ClutterDragAction   *action,
		ClutterActor        *actor,
		gfloat               event_x,
		gfloat               event_y,
		ClutterModifierType  modifiers,
		gpointer             userdata)
{
	gfloat y, height, group_height;
	ClutterActor *text;

	text = CLUTTER_ACTOR (userdata);	
	y = clutter_actor_get_y (actor);
	height = clutter_actor_get_height (text);
	//TODO
	group_height = 300;
	if (y > 0) {
		clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, group_height,
			"y", 0.0,
			NULL);
	}
	
	if (y < (group_height - height)) {
	       	clutter_actor_animate (actor, CLUTTER_EASE_OUT_BOUNCE, group_height,
			"y", group_height - height,
			NULL);
	}
}

static ClutterActor *
get_description_actor (const gchar *desc)
{
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	ClutterActor *group, *ret;
	ClutterActor *text;
	ClutterAction *action;
#if 0
	/*TODO: in this if, the text cannot be displayed properly
	 * it is a clutter bug ! */
	group = clutter_group_new ();
	clutter_actor_set_clip_to_allocation (group, TRUE);
	text = clutter_text_new ();
	/*TODO, not fixed width */
	clutter_actor_set_width (text, 400);
	clutter_text_set_text (text, desc);
	clutter_text_set_line_wrap (text, TRUE);

	layout = clutter_box_layout_new ();
	layout_box = clutter_box_new (layout);
	                
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout), text,
			TRUE,
			TRUE,
			TRUE,			
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	clutter_actor_set_reactive (layout_box, TRUE);
	action = clutter_drag_action_new ();
	clutter_actor_add_action (layout_box, action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (action),
			CLUTTER_DRAG_Y_AXIS);
	g_signal_connect (action, "drag-end", G_CALLBACK (on_description_drag_end), text);
	clutter_container_add_actor (CLUTTER_CONTAINER (group), layout_box);
#else
	ret = clutter_group_new ();
	//TODO: remove the height setting?
//	clutter_actor_set_height (ret, 300);
	clutter_actor_set_clip_to_allocation (ret, TRUE);

	group = clutter_group_new ();
	text = clutter_text_new ();
	/*TODO, not fixed width */
	clutter_actor_set_width (text, 400);
	clutter_text_set_text (CLUTTER_TEXT (text), desc);
	clutter_text_set_line_wrap (CLUTTER_TEXT (text), TRUE);
	clutter_actor_set_reactive (group, TRUE);
	action = clutter_drag_action_new ();
	clutter_actor_add_action (group, action);
	clutter_drag_action_set_drag_axis (CLUTTER_DRAG_ACTION (action),
			CLUTTER_DRAG_Y_AXIS);
	g_signal_connect (action, "drag-end", G_CALLBACK (on_description_drag_end), text);
	clutter_container_add_actor (CLUTTER_CONTAINER (group), text);
	clutter_container_add_actor (CLUTTER_CONTAINER (ret), group);
#endif

	return ret;
}

static void
draw_pic (GnomeAppInfoPage *page)
{
	ClutterActor *big_pic, *next, *prev;
	gchar *str;

	clutter_script_get_objects (page->priv->script,
			"big-pic", &big_pic,
			"next", &next,
			"prev", &prev,
			NULL);
	str = g_strdup_printf ("previewpic%d", page->priv->current_pic);
	gnome_app_ui_set_icon (big_pic, open_result_get (page->priv->info, str));
	g_free (str);

	if (page->priv->current_pic > page->priv->pic_count) {
		g_critical ("current pic is over pic count !");
	} else if (page->priv->current_pic == page->priv->pic_count) {
		clutter_actor_hide (next);
	} else {
		clutter_actor_show (next);
	}
	if (page->priv->current_pic < 1) {
		g_critical ("current pic is less than zero ? ");
	} else if (page->priv->current_pic == 1) {
		clutter_actor_hide (prev);
	} else if (page->priv->current_pic > 1) {
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

ClutterActor *
gnome_app_info_page_get_actions (GnomeAppInfoPage *page)
{
	GError *error;
	gchar *filename;
	ClutterScript *script;

	ClutterActor *info_page_actions, *fan_button, *download_button, *comment_button, *return_button;

	error = NULL;
	filename = open_app_get_ui_uri ("app-info-page-actions");
	script = clutter_script_new ();
	clutter_script_load_from_file (script, filename, &error);
	g_free (filename);
	if (error) {
		g_critical ("Error in load script %s.", error->message);
		g_object_unref (script);
		g_error_free (error);
		return NULL;
	}
	 
	clutter_script_get_objects (script,
			"info-page-actions", &info_page_actions,
			"fan-button", &fan_button,
			"download-button", &download_button,
			"comment-button", &comment_button,
			"return-button", &return_button,
			NULL);

	filename = open_app_get_pixmap_uri ("back");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (return_button), filename, NULL);
	g_free (filename);
	g_signal_connect (return_button, "button-press-event", G_CALLBACK (on_return_button_press), page);

//TODO: unref the script ? 
	return info_page_actions;
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
	ClutterActor *downloads, *download_button;
	ClutterActor *fans, *fan_button;
	ClutterActor *comments, *comment_entry;
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
			"download-button", &download_button,
			"fans", &fans,
			"fan-button", &fan_button,
			"next", &next,
			"prev", &prev,
			"comments", &comments,
			"comment-entry", &comment_entry,
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
	str = g_strdup_printf ("%s downloads", open_result_get (info, "downloads"));
	clutter_text_set_text (CLUTTER_TEXT (downloads), str);
	g_free (str);
	str = g_strdup_printf ("%s fans", open_result_get (info, "fans"));
	clutter_text_set_text (CLUTTER_TEXT (fans), str);
	g_free (str);
	str = g_strdup_printf ("%s comments", open_result_get (info, "comments"));
	clutter_text_set_text (CLUTTER_TEXT (comments), str);
	g_free (str);

	ClutterColor  cursor_color     = { 0xff, 0x33, 0x33, 0xff };
      	ClutterColor  selected_text_color = { 0x00, 0x00, 0xff, 0xff };

	clutter_text_set_line_wrap (CLUTTER_TEXT (comment_entry), TRUE);
	clutter_actor_set_reactive (comment_entry, TRUE);
 	clutter_text_set_editable (CLUTTER_TEXT (comment_entry), TRUE);
      	clutter_text_set_selectable (CLUTTER_TEXT (comment_entry), TRUE);
	clutter_text_set_activatable (CLUTTER_TEXT (comment_entry), TRUE);
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
	comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (info, "id"), NULL));
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
	clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));

	g_signal_connect (prev, "button-press-event", G_CALLBACK (on_prev_button_press), page);
	g_signal_connect (next, "button-press-event", G_CALLBACK (on_next_button_press), page);

	draw_fan_status (page);
	set_fan_status (page);
//TODO: more it to action or remove action..
#if 1
	filename = open_app_get_pixmap_uri ("back");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (return_button), filename, NULL);
	g_free (filename);
	g_signal_connect (return_button, "button-press-event", G_CALLBACK (on_return_button_press), page);

	g_signal_connect (fan_button, "button-press-event", G_CALLBACK (on_fan_button_press), page);
	g_signal_connect (download_button, "button-press-event", G_CALLBACK (on_download_button_press), page);
#endif
	return ;
}
