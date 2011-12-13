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

#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-comment.h"

struct _GnomeAppCommentPrivate
{
	ClutterGroup    *ui_group;
	ClutterActor *reply_entry;

	OpenResult *comment;
	gchar *content;
	gchar *content2;
};

G_DEFINE_TYPE (GnomeAppComment, gnome_app_comment, CLUTTER_TYPE_GROUP)

static void
_set_user_icon_1 (gpointer userdata, gpointer func_result)
{
	GnomeAppTask *task;
        OpenResults *results;
        OpenResult *result;
        GList *list;
	const gchar *pic;

        results = OPEN_RESULTS (func_result);
	list = open_results_get_data (results);
	if (!open_results_get_status (results)) {
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return;
	} else {
		result = list->data;
/*TODO: check avatarpicfound first? */
		pic = open_result_get (result, "avatarpic");
		if (pic) {
			gnome_app_ui_set_icon (userdata, pic);
			return;
		}
	}

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
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppCommentPrivate));
}

static gboolean
on_reply_label_press (ClutterActor *actor,
	             ClutterEvent *event,
	             gpointer      data)
{
	GnomeAppComment *comment;

	comment = GNOME_APP_COMMENT (data);
	clutter_actor_show (comment->priv->reply_entry);

	return TRUE;
}

static gpointer
comment_reply_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppComment *ui_comment;
	OpenResults *results;

	ui_comment = GNOME_APP_COMMENT (userdata);
 	results = OPEN_RESULTS (func_result);

	printf ("result get %s, code %s, msg %s\n", open_results_get_meta (results, "status"),
					open_results_get_meta (results, "statuscode"),
					open_results_get_meta (results, "message"));
	return NULL;
}


static void
on_reply_entry_activate (ClutterActor *actor,
	                 GnomeAppComment *ui_comment)
{
	GnomeAppTask *task;
	gchar *content;
	gchar *parent;
//TODO: make a reply window
	parent = open_result_get (ui_comment->priv->comment, "id");
	task = gnome_app_task_new (ui_comment, "POST", "/v1/comments/add");
	gnome_app_task_add_params (task,
				"type", "1",
				"content", ui_comment->priv->content,
				"content2", "0",
				"parent", parent,
				"subject", "Replytothis",
				"message", "sorryforspam",
				"msg", "sorryforspam",
				NULL);
									        
	gnome_app_task_set_callback (task, comment_reply_callback);
	gnome_app_task_push (task);
}

GnomeAppComment *
gnome_app_comment_new_with_comment (OpenResult *comment)
{
	GnomeAppComment *app_comment;

	app_comment = g_object_new (GNOME_APP_TYPE_COMMENT, NULL);
	app_comment->priv->comment = g_object_ref (comment);

        gchar *filename;
        GError *error;
        ClutterScript *script;
        ClutterActor *actor;

        error = NULL;
        filename = open_app_get_ui_uri ("app-comment");
        script = clutter_script_new ();
        clutter_script_load_from_file (script, filename, &error);
        if (error) {
                printf ("error in load script %s\n", error->message);
                g_error_free (error);
        }

        clutter_script_get_objects (script, "app-comment", &app_comment->priv->ui_group, NULL);
	if (!app_comment->priv->ui_group) {
		g_critical ("Cannot find 'app-comment' in %s!\n", filename);
		g_free (filename);
		return app_comment;
	}
	clutter_container_add_actor (CLUTTER_CONTAINER (app_comment), CLUTTER_ACTOR (app_comment->priv->ui_group));

        gchar *prop [] = {
                "subject", "user", "date", "text", "usericon", "replybutton", 
		NULL};
        const gchar *val;
	gchar *label;

	clutter_script_get_objects (script, "subject", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'subject' in %s!\n", filename);
	} else {
                val = open_result_get (comment, "subject");
		if (val)	
			clutter_text_set_text (CLUTTER_TEXT (actor), val);
	}

	clutter_script_get_objects (script, "user", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'user' in %s!\n", filename);
	} else {
                val = open_result_get (comment, "user");
		if (val) {
			label = g_strdup_printf ("by %s", val);
			clutter_text_set_text (CLUTTER_TEXT (actor), label);
			g_free (label);
		}
	}

	clutter_script_get_objects (script, "date", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'date' in %s!\n", filename);
	} else {
                val = open_result_get (comment, "date");
		if (val) {
			label = g_strdup_printf ("on: %10s", val);
			clutter_text_set_text (CLUTTER_TEXT (actor), label);
			g_free (label);
		}
	}

	clutter_script_get_objects (script, "text", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'text' in %s!\n", filename);
	} else {
                val = open_result_get (comment, "text");
		if (val)	
			clutter_text_set_text (CLUTTER_TEXT (actor), val);
	}

	clutter_script_get_objects (script, "usericon", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'usericon' in %s!\n", filename);
	} else {
		gchar *user_icon;

	        user_icon = open_app_get_pixmap_uri ("person");
	        clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), user_icon, NULL);
		g_free (user_icon);

		val = open_result_get (comment, "user");
		if (val)
			set_user_icon (actor, val);
	}

	clutter_script_get_objects (script, "reply_label", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'reply_label' in %s!\n", filename);
	} else {
		g_signal_connect (actor, "button-press-event", G_CALLBACK (on_reply_label_press), app_comment);
	}

	clutter_script_get_objects (script, "reply_entry", &actor, NULL);
	if (!actor) {
		g_critical ("Cannot find 'reply_entry' in %s!\n", filename);
	} else {
		app_comment->priv->reply_entry = actor;
		clutter_actor_hide (app_comment->priv->reply_entry);
		g_signal_connect (app_comment->priv->reply_entry, "activate", G_CALLBACK (on_reply_entry_activate), app_comment);
	}

	g_free (filename);

	return app_comment;
}

void
gnome_app_comment_set_content (GnomeAppComment *ui_comment, gchar *content, gchar *content2)
{
	if (ui_comment->priv->content)
		g_free (ui_comment->priv->content);
	if (ui_comment->priv->content2)
		g_free (ui_comment->priv->content2);
	if (content)
		ui_comment->priv->content = g_strdup (content);
	if (content2)
		ui_comment->priv->content2 = g_strdup (content2);
}
