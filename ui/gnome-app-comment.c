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
	OpenResult *comment;
};

G_DEFINE_TYPE (GnomeAppComment, gnome_app_comment, CLUTTER_TYPE_GROUP)


static void
set_pic_callback (gpointer userdata, gpointer func_re)
{
        ClutterActor *actor;
        gchar *dest_url;

        actor = CLUTTER_ACTOR (userdata);
        dest_url = (gchar *) func_re;
/*TODO: why should use this thread? */
        clutter_threads_enter ();
        clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), dest_url, NULL);
        clutter_threads_leave ();
}

static void
_set_user_icon_1 (gpointer userdata, gpointer func_result)
{
	GnomeAppStore *store;
	GnomeAppTask *task;
        OpenResults *results;
        OpenResult *result;
        GList *list;
	const gchar *pic;

	store = gnome_app_store_get_default ();
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
			task = gnome_download_task_new (userdata, pic);
			gnome_app_task_set_callback (task, set_pic_callback);
			gnome_app_task_push (task);
			return;
		}
	}

/*TODO */
	const gchar *user_icon = "/home/novell/gnome-app-store/ui/scripts/person.png";
        clutter_texture_set_from_file (CLUTTER_TEXTURE (userdata), user_icon, NULL);
}

static void			
set_user_icon (ClutterActor *actor, const gchar *user)
{
	GnomeAppStore *store;
	GnomeAppTask *task;
	gchar *function;

	store = gnome_app_store_get_default ();
	function = g_strdup_printf ("/v1/person/data/%s", user);
	task = gnome_app_task_new (actor, "GET", function,
                                NULL);
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

GnomeAppComment *
gnome_app_comment_new_with_comment (OpenResult *comment)
{
	GnomeAppComment *app_comment;

	app_comment = g_object_new (GNOME_APP_TYPE_COMMENT, NULL);
	app_comment->priv->comment = g_object_ref (comment);

        const gchar *filename;
        GError *error;
        ClutterScript *script;
        ClutterActor *actor;

        error = NULL;
        filename = "/home/novell/gnome-app-store/ui/scripts/app-comment.json";
        script = clutter_script_new ();
        clutter_script_load_from_file (script, filename, &error);
        if (error) {
                printf ("error in load script %s\n", error->message);
                g_error_free (error);
        }

        clutter_script_get_objects (script, "app-comment", &app_comment, NULL);

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
		val = open_result_get (comment, "user");
		if (val)
			set_user_icon (actor, val);
	}

	return app_comment;
}
