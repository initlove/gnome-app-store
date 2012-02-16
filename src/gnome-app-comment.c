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
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-comment.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppCommentPrivate
{
        ClutterScript *script;
	ClutterGroup    *ui_group;
	ClutterActor *reply_entry;
	ClutterActor *reply_button;
	ClutterActor *submit_button;

	OpenResult *comment;
	gchar *content;
	gchar *content2;
};

enum
{
	REFRESH,
	LAST_SIGNAL
};

static guint comment_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppComment, gnome_app_comment, CLUTTER_TYPE_GROUP)

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
	priv->script = NULL;
	priv->comment = NULL;
	priv->reply_entry = NULL;
	priv->content = NULL;
	priv->content2 = NULL;
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

static gboolean
on_submit_button_press (ClutterActor *actor,
	             ClutterEvent *event,
	             gpointer      data)
{
	GnomeAppComment *comment;
	GnomeAppCommentPrivate *priv;
	const gchar *str;

	comment = GNOME_APP_COMMENT (data);
	priv = comment->priv;
	str = clutter_text_get_text (CLUTTER_TEXT (priv->reply_entry));
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

static gboolean
on_reply_button_press (ClutterActor *actor,
	             ClutterEvent *event,
	             gpointer      data)
{
	GnomeAppComment *comment;
	GnomeAppCommentPrivate *priv;
	gboolean activatable;

	comment = GNOME_APP_COMMENT (data);
	priv = comment->priv;

	activatable = clutter_text_get_activatable (CLUTTER_TEXT (priv->reply_entry));
	if (!activatable) {
		clutter_text_set_text (CLUTTER_TEXT (priv->reply_button), _("Cancel"));
		clutter_actor_set_size (priv->reply_entry, 300, 100);
		clutter_text_set_editable (CLUTTER_TEXT (priv->reply_entry), TRUE);
		clutter_text_set_text (CLUTTER_TEXT (priv->submit_button), _("Submit"));
	} else {
		// why should I do this: becase when I hide reply_entry, the reply_button will be hide .
		// no idea why this happen
		clutter_actor_set_size (priv->reply_entry, 1, 1);
		clutter_text_set_text (CLUTTER_TEXT (priv->reply_entry), NULL);
		clutter_text_set_editable (CLUTTER_TEXT (priv->reply_entry), FALSE);
		clutter_text_set_text (CLUTTER_TEXT (priv->reply_button), _("Reply"));
		clutter_text_set_text (CLUTTER_TEXT (priv->submit_button), "");
	}
	clutter_text_set_activatable (CLUTTER_TEXT (priv->reply_entry), !activatable);

	return TRUE;
}

static void
on_reply_entry_paint (ClutterActor *actor,
	              gpointer      data)
{
	GnomeAppComment *comment;
	GnomeAppCommentPrivate *priv;
	ClutterActorBox allocation = { 0, };
	gfloat width, height;

	comment = GNOME_APP_COMMENT (data);
	priv = comment->priv;

	if (!clutter_text_get_activatable (CLUTTER_TEXT (actor)))
		return;

	clutter_actor_get_allocation_box (actor, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	cogl_set_source_color4ub (0, 0, 0, 255);
	cogl_path_rectangle (1, 1, width, height);
	cogl_path_stroke ();
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

GnomeAppComment *
gnome_app_comment_new_with_comment (OpenResult *comment)
{
	GnomeAppComment *app_comment;
	GnomeAppCommentPrivate *priv;

	app_comment = g_object_new (GNOME_APP_TYPE_COMMENT, NULL);
	priv = app_comment->priv;
	priv->comment = g_object_ref (comment);

	ClutterLayoutManager *layout, *child_layout;
	ClutterActor *box, *child_box;
	ClutterActor *subject;
	ClutterActor *user;
	ClutterActor *date;
	ClutterActor *text;
	ClutterActor *usericon;
        gchar *filename;
        GError *error;
        const gchar *val;
	gchar *str;

        error = NULL;
        filename = open_app_get_ui_uri ("app-comment");
        priv->script = clutter_script_new ();
        clutter_script_load_from_file (priv->script, filename, &error);
	gnome_app_script_po (priv->script);
	g_free (filename);
        if (error) {
                g_critical ("error in load script %s\n", error->message);
                g_error_free (error);
		g_object_unref (priv->script);
		return app_comment;
        }

        clutter_script_get_objects (priv->script, 
				"app-comment", &priv->ui_group,
				"subject", &subject,
				"user", &user,
				"date", &date,
				"text", &text,
				"usericon", &usericon,
			       	NULL);
                
	val = open_result_get (comment, "subject");
	if (val)	
		clutter_text_set_text (CLUTTER_TEXT (subject), val);

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

	val = open_result_get (comment, "text");
	if (val)	
		clutter_text_set_text (CLUTTER_TEXT (text), val);

	str = open_app_get_pixmap_uri ("person");
	if (str) {
		clutter_texture_set_from_file (CLUTTER_TEXTURE (usericon), str, NULL);
		g_free (str);
	}

	val = open_result_get (comment, "user");
	if (val)
		set_user_icon (usericon, val);

        layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
	box = clutter_box_new (layout);
        
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout),
			CLUTTER_ACTOR (priv->ui_group),
			FALSE, /*expand*/
			FALSE, /*x-fill*/
			FALSE, /*y-fill*/
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	priv->reply_entry = clutter_text_new ();
	clutter_actor_set_reactive (priv->reply_entry, TRUE);
	clutter_text_set_line_wrap (CLUTTER_TEXT (priv->reply_entry), TRUE);
	clutter_text_set_line_wrap_mode (CLUTTER_TEXT (priv->reply_entry), PANGO_WRAP_WORD);
	clutter_text_set_selectable (CLUTTER_TEXT (priv->reply_entry), TRUE);
	clutter_text_set_activatable (CLUTTER_TEXT (priv->reply_entry), FALSE);
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout),
			priv->reply_entry,
			TRUE, /*expand*/
			TRUE, /*x-fill*/
			TRUE, /*y-fill*/
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

        child_layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (child_layout), FALSE);
	child_box = clutter_box_new (child_layout);

	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (layout),
			child_box,
			TRUE, /*expand*/
			TRUE, /*x-fill*/
			TRUE, /*y-fill*/
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	priv->submit_button = clutter_text_new ();
	clutter_actor_set_reactive (priv->submit_button, TRUE);
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (child_layout),
			priv->submit_button,
			TRUE, /*expand*/
			FALSE, /*x-fill*/
			TRUE, /*y-fill*/
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	priv->reply_button = clutter_text_new ();
	clutter_actor_set_reactive (priv->reply_button, TRUE);
	clutter_text_set_text (CLUTTER_TEXT (priv->reply_button), _("Reply"));
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (child_layout),
			priv->reply_button,
			FALSE, /*expand*/
			FALSE, /*x-fill*/
			TRUE, /*y-fill*/
			CLUTTER_BOX_ALIGNMENT_END,
			CLUTTER_BOX_ALIGNMENT_END);

	g_signal_connect (priv->reply_entry, "paint", G_CALLBACK (on_reply_entry_paint), app_comment);
	g_signal_connect (priv->submit_button, "button-press-event", G_CALLBACK (on_submit_button_press), app_comment);
	g_signal_connect (priv->reply_button, "button-press-event", G_CALLBACK (on_reply_button_press), app_comment);

	clutter_container_add_actor (CLUTTER_CONTAINER (app_comment), CLUTTER_ACTOR (box));
	g_signal_connect (app_comment, "paint", G_CALLBACK (on_app_comment_paint), app_comment);

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
