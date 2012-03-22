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
#include "gnome-app-download.h"

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
	gboolean lock;
	gint pic_count;
	gint current_pic;
	ClutterActor *download;
};

/* Properties */
enum
{
	PROP_0,
	PROP_APP_DATA,
	PROP_LOCK_STATUS,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, CLUTTER_TYPE_GROUP)

static void	gnome_app_info_page_set_with_data (GnomeAppInfoPage *info_page, OpenResult *info);
static void	draw_pic (GnomeAppInfoPage *info_page);

static void
gnome_app_info_page_set_lock (GnomeAppInfoPage *info_page, const gchar *str)
{
	GnomeAppInfoPagePrivate *priv;
	ClutterActor *fan_button;
	ClutterActor *comment_button;
	ClutterActor *return_button;

	priv = info_page->priv;
	clutter_script_get_objects (priv->script, 
			"fan-button", &fan_button,
			"comment-button", &comment_button,
			"return-button", &return_button,
			NULL);
	if (strcmp (str, "lock") == 0) {
		priv->lock = TRUE;
		clutter_actor_set_reactive (return_button, FALSE);
		clutter_actor_set_reactive (fan_button, FALSE);
		clutter_actor_set_reactive (comment_button, FALSE);
	} else {
		priv->lock = FALSE;
		clutter_actor_set_reactive (return_button, TRUE);
		clutter_actor_set_reactive (fan_button, TRUE);
		clutter_actor_set_reactive (comment_button, TRUE);
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
	priv->lock = FALSE;
	if (!priv->script) {
		return ;
	}
	priv->info = NULL;
    priv->fan_count = 0;
	priv->fan_status = FAN_NOT_DEFINED;
	clutter_script_connect_signals (priv->script, info_page);
	clutter_script_get_objects (priv->script,
			"info-page", &priv->main_ui,
			"fan-button", &fan_button,
			"next", &next_button,
			"prev", &prev_button,
			"comment-entry", &comment_entry,
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
	const gchar *str;

	info_page = GNOME_APP_INFO_PAGE (object);
	priv = info_page->priv;
	switch (prop_id)
	{
		case PROP_APP_DATA:
			gnome_app_info_page_set_with_data (info_page, g_value_get_object (value));
			break;
		case PROP_LOCK_STATUS:
			str = g_value_get_string (value);
			if (!str)
				return;
			gnome_app_info_page_set_lock (info_page, str);
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
		case PROP_APP_DATA:
			g_value_set_object (value, priv->info);
			break;
		case PROP_LOCK_STATUS:
			/*TODO: should get the child widget status */
			if (priv->lock)
				g_value_set_string (value, "lock");
			else
				g_value_set_string (value, "unlock");
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
  			PROP_APP_DATA,
			g_param_spec_object ("data",
				"app info",
				"app info",
				G_TYPE_OBJECT,
				G_PARAM_READWRITE));
	        
	g_object_class_install_property (object_class,
			PROP_LOCK_STATUS,
			g_param_spec_string ("lock-status",
				"Lock Status",
				"Lock Status",
				NULL,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPagePrivate));
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

static void
proxy_call_async_cb (RestProxyCall *call,
        const GError  *error,
        GObject       *weak_object,
        gpointer       userdata)
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;
    const gchar *payload;
    goffset len;

	info_page = GNOME_APP_INFO_PAGE (userdata);
	priv = info_page->priv;
    payload = rest_proxy_call_get_payload (call);

    len = rest_proxy_call_get_payload_length (call);
    results = (OpenResults *) open_ocs_get_results (payload, len);
    if (!open_results_get_status (results)) {
		g_debug ("Fail to comment: %s\n", open_results_get_meta (results, "message"));
    } else {
		printf ("commented!\n");
		//TODO: we need to refresh it , force comment entry to reload ..
		ClutterActor *comments_details;
        ClutterActor *comments_details_actor;
		GList *list;

        clutter_threads_enter ();
		comments_details = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "comments-details"));
		comments_details_actor = CLUTTER_ACTOR (gnome_app_comments_new_with_content (open_result_get (priv->info, "id"), NULL));
		for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
			clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));
		clutter_container_add_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (comments_details_actor));
        clutter_threads_leave();
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
	/*subject needed? make it simple */
	subject = " ";
	message = gnome_app_text_get_text (GNOME_APP_TEXT (comment_entry));
	if (open_app_pattern_match ("blank", message, NULL)) {
		//TODO: doing sth
		return FALSE;
	}

/*TODO: final the call ..*/
    RestProxy *proxy;
    RestProxyCall *call;
    proxy = gnome_app_get_proxy ();
    call = rest_proxy_new_call (proxy);
    rest_proxy_call_set_method (call, "POST");
    rest_proxy_call_set_function (call, "comments/add");
    rest_proxy_call_add_params (call,
				"type", "1",
				"content", open_result_get (priv->info, "id"),
				"content2", "0",
				"subject", subject,
				"message", message,
                /*TODO: rest cannot parse user/password? */
                "guestname", "first guest",
                "guestemail", "guest@guest.com",
				NULL);

    rest_proxy_call_async (call,
            proxy_call_async_cb,
            NULL,
            page,
            NULL);

    return;
}

static void
draw_fan_status (GnomeAppInfoPage *info_page)
{
	/* TODO: we need to do lots of this,
	 * in fact we need to have stop or cancel all the task when we finalize the info page
	 */
	g_return_if_fail (info_page && GNOME_APP_IS_INFO_PAGE (info_page));

	GnomeAppInfoPagePrivate *priv;
	ClutterActor *actor;

	priv = info_page->priv;

	actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "fan-button"));
	if (!actor)
		return;
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
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	OpenResults *results;

	info_page = GNOME_APP_INFO_PAGE (userdata);
	priv = info_page->priv;
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

		draw_fan_status (info_page);
	}
	return NULL;
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
	return TRUE;
}

static gpointer
fan_status_callback (gpointer userdata, gpointer func_result)
{
	OpenResults *results;
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate*priv;

	results = OPEN_RESULTS (func_result);
	info_page = GNOME_APP_INFO_PAGE (userdata);
	priv = info_page->priv;
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

	draw_fan_status (info_page);

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
#if 1
    gchar *suffix [] = {".png", ".svg", ".xpm", ".icon", NULL};
    gchar *icon_uri;
    gint i;
     
    for (i = 0; suffix [i]; i++) {
        icon_uri = g_strdup_printf ("/home/dliang/work/icons/%s%s", open_result_get (priv->info, "icon"), suffix [i]);
        if (g_file_test (icon_uri, G_FILE_TEST_EXISTS)) {
            g_object_set (G_OBJECT (big_pic), "filename", icon_uri, NULL);
            g_free (icon_uri);
            break;
        } else
            g_free (icon_uri);
    }
#else
	gnome_app_set_icon (big_pic, open_result_get (priv->info, "smallpreviewpic1"));
#endif

	str = g_strdup_printf ("previewpic%d", priv->current_pic);
	gnome_app_set_icon (big_pic, open_result_get (priv->info, str));
	g_free (str);

	if (priv->current_pic > priv->pic_count) {
		/* This happens when no smallpreviewpic */
		clutter_actor_hide (next);
	} else if (priv->current_pic == priv->pic_count) {
		clutter_actor_hide (next);
	} else {
		clutter_actor_show (next);
	}
	if (priv->current_pic < 1) {
		g_critical ("current pic is less than zero ? ");
		clutter_actor_hide (prev);
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
		ClutterActor *fans;
		GList *list;
		OpenResult *result;
		const gchar *val;
		gchar *str;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);

		fans = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "fans"));
		val = open_result_get (result, "fans");
        if (val)
    		priv->fan_count = atoi (val);
		str = g_strdup_printf (_("%d fans"), priv->fan_count);
		clutter_text_set_text (CLUTTER_TEXT (fans), str);
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
	ClutterActor *download_group;
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
	priv->lock = FALSE;
	clutter_actor_set_y (CLUTTER_ACTOR (info_page), 0.0);

	clutter_script_get_objects (priv->script,
			"name", &name,
			"score", &score,
			"license", &license,
			"fans", &fans,
			"next", &next,
			"prev", &prev,
			"download-group", &download_group,
			"comment-count", &comment_count,
			"comment-entry", &comment_entry,
			"personid", &personid,
			"personicon", &personicon,
			"description", &description,
			"comments-details", &comments_details,
			"return-button", &return_button,
			NULL);

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

	clutter_text_set_text (CLUTTER_TEXT (name), open_result_get (info, "name"));
	clutter_text_set_text (CLUTTER_TEXT (personid), open_result_get (info, "personid"));
	g_object_set (G_OBJECT (score), "score", open_result_get (info, "score"), NULL);

	clutter_text_set_text (CLUTTER_TEXT (license), open_result_get (info, "license"));

	val = open_result_get (info, "fans");
    if (val)
    	priv->fan_count = atoi (val);
	str = g_strdup_printf (_("%d fans"), priv->fan_count);
	clutter_text_set_text (CLUTTER_TEXT (fans), str);
	g_free (str);

	if (priv->download)
		clutter_container_remove_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (priv->download));
	priv->download = CLUTTER_ACTOR (gnome_app_download_new_with_info (priv->info));
	clutter_container_add_actor (CLUTTER_CONTAINER (download_group), CLUTTER_ACTOR (priv->download));

	gnome_app_text_set_text (GNOME_APP_TEXT (comment_entry), NULL);
    val = open_result_get (info, "comments");
    if (val)
    	str = g_strdup_printf (_("%s comments"), val);
    else
        str = g_strdup_printf ("No comment yet");
	clutter_text_set_text (CLUTTER_TEXT (comment_count), str);
	g_free (str);
	
	clutter_text_set_text (CLUTTER_TEXT (description), open_result_get (info, "description"));

	/*clean the comments_details .. */
	for (list = clutter_container_get_children (CLUTTER_CONTAINER (comments_details)); list; list = list->next)
		clutter_container_remove_actor (CLUTTER_CONTAINER (comments_details), CLUTTER_ACTOR (list->data));

	draw_pic (info_page);
	set_fan_status (info_page);
	load_comments (info_page);

/* TODO: this make our info page with higher quality, but will make one more request
 * 	also we need the lock the interface, currently, I disable it.
	load_details_info (info_page);
*/
}
