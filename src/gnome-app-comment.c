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
#include <config.h>
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-widgets.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-comment.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppCommentPrivate
{
        ClutterScript *script;
	ClutterGroup *main_ui;
	ClutterActor *reply_entry;
	ClutterActor *reply_button;
	ClutterActor *submit_button;

	OpenResult *comment;
	gchar *content;
	gchar *content2;
	gboolean reply_entry_expand;
};

enum
{
	REFRESH,
	LAST_SIGNAL
};

static guint comment_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppComment, gnome_app_comment, CLUTTER_TYPE_GROUP)

static void
app_comment_expand_entry (GnomeAppComment *comment)
{
	GnomeAppCommentPrivate *priv;

	priv = comment->priv;
	if (priv->reply_entry_expand) {
		clutter_actor_set_height (priv->reply_entry, -1);
		clutter_actor_set_opacity (priv->reply_entry, 255);
		clutter_actor_set_opacity (priv->submit_button, 255);
		gnome_app_button_set_text (GNOME_APP_BUTTON (priv->reply_button), _("Cancel"));
	} else {
		clutter_actor_set_height (priv->reply_entry, 0);
		clutter_actor_set_opacity (priv->reply_entry, 0);
		clutter_actor_set_opacity (priv->submit_button,0);
		gnome_app_button_set_text (GNOME_APP_BUTTON (priv->reply_button), _("Reply"));
	}
}

static void
on_app_comment_paint (ClutterActor *actor,
	              gpointer      data)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height, gap;

	clutter_actor_get_allocation_box (actor, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);
	gap = width / 10;
        cogl_set_source_color4ub (0, 0, 0, 100);
	cogl_path_line (gap * 2, 2, width - gap, 0);

	cogl_path_stroke ();
}

static gpointer
_set_user_icon_1 (gpointer userdata, gpointer func_result)
{
        OpenResults *results;

        results = OPEN_RESULTS (func_result);
	if (!open_results_get_status (results)) {
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
        	GList *list;
        	OpenResult *result;
		const gchar *val;
		const gchar *pic;

		list = open_results_get_data (results);
		result = list->data;
		val = open_result_get (result, "avatarpicfound");
		if (val && (strcmp (val, "1") == 0)) {
			pic = open_result_get (result, "avatarpic");
			if (pic) {
				gnome_app_set_icon (userdata, pic);
				return NULL;
			}
		} else {
			val = open_result_get (result, "bigavatarpicfound");
			if (val && (strcmp (val, "1") == 0)) {
				pic = open_result_get (result, "bigavatarpic");
				if (pic) {
					gnome_app_set_icon (userdata, pic);
					return NULL;
				}
			}
		}
	}

	return NULL;
}

static void
set_user_icon (ClutterActor *actor, const gchar *user)
{
	g_return_if_fail (actor && user);

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/person/data/%s", user);
	task = gnome_app_task_new (actor, "GET", function);
	gnome_app_task_set_callback (task, _set_user_icon_1);
	gnome_app_task_push (task);

	g_free (function);
}

static void
gnome_app_comment_init (GnomeAppComment *comment)
{
	GnomeAppCommentPrivate *priv;

	comment->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (comment,
	                                                 GNOME_APP_TYPE_COMMENT,
	                                                 GnomeAppCommentPrivate);
	priv->comment = NULL;
	priv->content = NULL;
	priv->content2 = NULL;
	priv->reply_entry_expand = FALSE;

        priv->script = gnome_app_script_new_from_file ("app-comment");
        if (!priv->script) {
		return ;
        }

	clutter_script_connect_signals (priv->script, comment);
        clutter_script_get_objects (priv->script, 
				"app-comment", &priv->main_ui,
				"submit-button", &priv->submit_button,
				"reply-button", &priv->reply_button,
				"reply-entry", &priv->reply_entry,
			       	NULL);
	app_comment_expand_entry (comment);
	clutter_container_add_actor (CLUTTER_CONTAINER (comment), CLUTTER_ACTOR (priv->main_ui));

	return ;
}

static void
gnome_app_comment_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_comment_parent_class)->dispose (object);
}

static void
gnome_app_comment_finalize (GObject *object)
{
	GnomeAppComment *comment = GNOME_APP_COMMENT (object);
	GnomeAppCommentPrivate *priv = comment->priv;

	if (priv->script)
		g_object_unref (priv->script);
	if (priv->comment)
		g_object_unref (priv->comment);

	if (priv->content)
		g_free (priv->content);
	if (priv->content2)
		g_free (priv->content2);

	G_OBJECT_CLASS (gnome_app_comment_parent_class)->finalize (object);
}

static void
gnome_app_comment_class_init (GnomeAppCommentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_comment_dispose;
	object_class->finalize = gnome_app_comment_finalize;

	/**
	 * GnomeAppComment::refresh:
	 * @self: the #GnomeAppComment that e	priv->callback = NULL;mitted the signal
	 *
	 * The ::refresh signal is emitted after we make a reply
	 *
	 */
	comment_signals[REFRESH] =
 		g_signal_new (g_intern_static_string ("refresh"),
				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (GnomeAppCommentClass, refresh),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	g_type_class_add_private (object_class, sizeof (GnomeAppCommentPrivate));
}

static gpointer
comment_reply_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppComment *app_comment;
	OpenResults *results;

	app_comment = GNOME_APP_COMMENT (userdata);
 	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		g_debug ("Fail to reply %s\n", open_results_get_meta (results, "message"));
	} else {
		g_signal_emit (app_comment, comment_signals[REFRESH], 0);
	}

	return NULL;
}

G_MODULE_EXPORT gboolean
on_submit_button_press (ClutterActor *actor,
	             ClutterEvent *event,
	             gpointer      data)
{
	GnomeAppComment *comment;
	GnomeAppCommentPrivate *priv;
	const gchar *str;

	comment = GNOME_APP_COMMENT (data);
	priv = comment->priv;
	str = gnome_app_text_get_text (GNOME_APP_TEXT (priv->reply_entry));
	if (open_app_pattern_match ("blank", str, NULL)) {
		//TODO: doing sth
		return FALSE;
	}

	GnomeAppTask *task;
	const gchar *parent;

	parent = open_result_get (priv->comment, "id");
	task = gnome_app_task_new (comment, "POST", "/v1/comments/add");
	gnome_app_task_add_params (task,
				"type", "1",
				"content", priv->content,
				"content2", "0",
				"parent", parent,
				"subject", "Reply Subject",
				"message", str,
				NULL);
									        
	gnome_app_task_set_callback (task, comment_reply_callback);
	gnome_app_task_push (task);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_reply_button_press (ClutterActor *actor,
	             ClutterEvent *event,
	             gpointer      data)
{
	GnomeAppComment *comment;
	GnomeAppCommentPrivate *priv;

	comment = GNOME_APP_COMMENT (data);
	priv = comment->priv;

	priv->reply_entry_expand = !priv->reply_entry_expand;
	app_comment_expand_entry (comment);

	return TRUE;
}

GnomeAppComment *
gnome_app_comment_new_with_comment (OpenResult *comment)
{
	GnomeAppComment *app_comment;
	GnomeAppCommentPrivate *priv;
	ClutterActor *subject;
	ClutterActor *user;
	ClutterActor *date;
	ClutterActor *text;
	ClutterActor *usericon;
        const gchar *val;
	gchar *str;

	app_comment = g_object_new (GNOME_APP_TYPE_COMMENT, NULL);
	priv = app_comment->priv;
	priv->comment = g_object_ref (comment);

        clutter_script_get_objects (priv->script, 
				"subject", &subject,
				"user", &user,
				"date", &date,
				"text", &text,
				"usericon", &usericon,
			       	NULL);
                
	clutter_text_set_text (CLUTTER_TEXT (subject), open_result_get (comment, "subject"));

	val = open_result_get (comment, "user");
	if (val) {
		str = g_strdup_printf (_("by %s"), val);
		clutter_text_set_text (CLUTTER_TEXT (user), str);
		g_free (str);
	}

	val = open_result_get (comment, "date");
	if (val) {
		str = g_strdup_printf (_("on: %10s"), val);
		clutter_text_set_text (CLUTTER_TEXT (date), str);
		g_free (str);
	}

	clutter_text_set_text (CLUTTER_TEXT (text), open_result_get (comment, "text"));
	set_user_icon (usericon, open_result_get (comment, "user"));

	return app_comment;
}

void
gnome_app_comment_set_content (GnomeAppComment *app_comment, gchar *content, gchar *content2)
{
	GnomeAppCommentPrivate *priv;

	priv = app_comment->priv;
	if (priv->content)
		g_free (priv->content);
	if (priv->content2)
		g_free (priv->content2);
	if (content)
		priv->content = g_strdup (content);
	else
		priv->content = NULL;
	if (content2)
		priv->content2 = g_strdup (content2);
	else
		priv->content2 = NULL;
}
