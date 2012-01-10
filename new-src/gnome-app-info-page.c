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
#include "gnome-app-store-ui.h"
#include "gnome-app-score-ui.h"
#include "gnome-app-info-page.h"

struct _GnomeAppInfoPagePrivate
{
	OpenResult *info;
	ClutterScript *script;

	gint pic_count;
	gint current_pic;
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
	priv->info = NULL;
	priv->script = NULL;
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

	object_class->dispose = gnome_app_info_page_dispose;
	object_class->finalize = gnome_app_info_page_finalize;
	 
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

static gboolean
on_return_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
        GnomeAppStoreUI *store_ui;

	store_ui = gnome_app_store_ui_get_default ();
	gnome_app_store_ui_load_frame_ui (store_ui);

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
	clutter_actor_set_height (ret, 300);
	clutter_actor_set_clip_to_allocation (ret, TRUE);

	group = clutter_group_new ();
	text = clutter_text_new ();
	/*TODO, not fixed width */
	clutter_actor_set_width (text, 400);
	clutter_text_set_text (text, desc);
	clutter_text_set_line_wrap (text, TRUE);
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
gnome_app_info_page_new_with_app (OpenResult *info)
{
	GnomeAppInfoPage *page;

	g_return_val_if_fail (info != NULL, NULL);

	gchar *filename;
	const gchar *val;
	gchar *str;
	GError *error;
	gint i;

	ClutterActor *info_page;
	ClutterActor *next, *prev;
	ClutterActor *score, *score_actor;
	ClutterActor *license;
	ClutterActor *downloads;
	ClutterActor *fans;
	ClutterActor *comments;
	ClutterActor *fan_button;
	ClutterActor *download_button;
	ClutterActor *comment_button;
	ClutterActor *return_button;
	ClutterActor *name;
	ClutterActor *personid, *personicon;
	ClutterActor *big_pic;
	ClutterActor *description, *description_actor;
	ClutterActor *comments_details, *comments_details_actor;
	ClutterAction *action;

	page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
	page->priv->info = g_object_ref (info);

	error = NULL;
	filename = open_app_get_ui_uri ("app-info-page");
	page->priv->script = clutter_script_new ();
	clutter_script_load_from_file (page->priv->script, filename, &error);
	if (error) {
		g_critical ("Error in load script %s.", error->message);
		g_free (filename);
		g_error_free (error);
		return page;
	}
	 
	clutter_script_get_objects (page->priv->script,
			"info-page", &info_page,
			"score", &score,
			"license", &license,
			"downloads", &downloads,
			"fans", &fans,
			"next", &next,
			"prev", &prev,
			"comments", &comments,
			"fan-button", &fan_button,
			"comment-button", &comment_button,
			"download-button", &download_button,
			"return-button", &return_button,
			"name", &name,
			"personid", &personid,
			"personicon", &personicon,
			"description", &description,
			"comments-details", &comments_details,
			NULL);

	g_free (filename);

	gint count;
	for (count = 0; ; count ++) {
		str = g_strdup_printf ("smallpreviewpic%d", count + 1);
		val = open_result_get (info, str);
		g_free (str);
		if (!val)
			break;
	}
	/* assume we should at least one pic, if only one pic, donnot add to small pics */
	page->priv->pic_count = count;
	page->priv->current_pic = 1;
	filename = open_app_get_pixmap_uri ("go-previous");
	clutter_texture_set_from_file (prev, filename, NULL);
	g_free (filename);
	filename = open_app_get_pixmap_uri ("go-next");
	clutter_texture_set_from_file (next, filename, NULL);
	g_free (filename);
	draw_pic (page);

	score_actor = CLUTTER_ACTOR (gnome_app_score_ui_new_with_score (open_result_get (info, "score")));
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
	str = open_app_get_pixmap_uri ("back");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (return_button), str, NULL);
	g_free (str);
	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	clutter_text_set_text (CLUTTER_TEXT (personid), open_result_get (info, "personid"));
	description_actor = get_description_actor (open_result_get (info, "description"));
	clutter_container_add_actor (CLUTTER_CONTAINER (description), CLUTTER_ACTOR (description_actor));
	comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (info, "id"), NULL));
	clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));

	g_signal_connect (prev, "button-press-event", G_CALLBACK (on_prev_button_press), page);
	g_signal_connect (next, "button-press-event", G_CALLBACK (on_next_button_press), page);
	g_signal_connect (download_button, "button-press-event", G_CALLBACK (on_download_button_press), page);
	g_signal_connect (return_button, "button-press-event", G_CALLBACK (on_return_button_press), page);

	clutter_container_add_actor (CLUTTER_CONTAINER (page), info_page);

	return page;
}
