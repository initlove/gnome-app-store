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

#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-friends-ui.h"
#include "gnome-app-message-ui.h"
#include "gnome-app-account-ui.h"

struct _GnomeAppAccountUIPrivate
{
	ClutterGroup    *ui_group;
	ClutterActor	*avatar;
	ClutterActor	*username;
	ClutterActor	*company;
	ClutterActor	*homepage;
	ClutterActor	*friends;
	ClutterActor	*messages;

	gchar 		*person_id;
};

G_DEFINE_TYPE (GnomeAppAccountUI, gnome_app_account_ui, CLUTTER_TYPE_GROUP)

static void
gnome_app_account_ui_init (GnomeAppAccountUI *account_ui)
{
	GnomeAppAccountUIPrivate *priv;

	account_ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (account_ui,
	                                                 GNOME_APP_TYPE_ACCOUNT_UI,
	                                                 GnomeAppAccountUIPrivate);
			
	priv->person_id = NULL;
}

static void
gnome_app_account_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_account_ui_parent_class)->dispose (object);
}

static void
gnome_app_account_ui_finalize (GObject *object)
{
	GnomeAppAccountUI *account_ui = GNOME_APP_ACCOUNT_UI (object);
	GnomeAppAccountUIPrivate *priv = account_ui->priv;

	if (priv->person_id)
		g_free (priv->person_id);

	G_OBJECT_CLASS (gnome_app_account_ui_parent_class)->finalize (object);
}

static void
gnome_app_account_ui_class_init (GnomeAppAccountUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_account_ui_dispose;
	object_class->finalize = gnome_app_account_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppAccountUIPrivate));
}

static gpointer
set_account_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppAccountUI *account_ui;
        OpenResult *result;
	OpenResults *results;
        GList *list;
	const gchar *val;
	const gchar *pic;

	account_ui = GNOME_APP_ACCOUNT_UI (userdata);
 	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		/*TODO: fill the account with default val */
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		list = open_results_get_data (results);
		result = list->data;
		val = open_result_get (result, "avatarpicfound");
		if (val && (strcmp (val, "1") == 0)) {
			pic = open_result_get (result, "avatarpic");
			if (pic) {
				gnome_app_set_icon (account_ui->priv->avatar, pic);
			}
		} else {
			val = open_result_get (result, "bigavatarpicfound");
			if (val && (strcmp (val, "1") == 0)) {
				pic = open_result_get (result, "bigavatarpic");
				if (pic) {
					gnome_app_set_icon (account_ui->priv->avatar, pic);
				}
			}
		}

		val = open_result_get (result, "company");
		if (val)
			clutter_text_set_text (CLUTTER_TEXT (account_ui->priv->company), val);

		/*TODO: should add result to priv, when click on homepage, open this homepage */
		val = open_result_get (result, "homepage");
			/*This set_text is just the current effect */
		if (val)
			clutter_text_set_text (CLUTTER_TEXT (account_ui->priv->homepage), val);

		const gchar *first;
		const gchar *last;
	        gchar *name;

		first = open_result_get (result, "firstname");
		last = open_result_get (result, "lastname");
		/*TODO there should have 'space' between name, this is to make my login name looks better */
		name = g_strdup_printf ("%s%s", first, last);

		clutter_text_set_text (CLUTTER_TEXT (account_ui->priv->username), name);
		g_free (name);
	}

	return NULL;
}

static gboolean
on_friend_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	g_debug ("press on friend!");
	GnomeAppAccountUI *account_ui;
	GnomeAppFriendsUI *friend_ui;
	gfloat x, y;

	account_ui = GNOME_APP_ACCOUNT_UI (data);

	friend_ui = gnome_app_friends_ui_new (account_ui->priv->person_id);
	clutter_container_add_actor (CLUTTER_CONTAINER (account_ui), CLUTTER_ACTOR (friend_ui));
	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_set_position (CLUTTER_ACTOR (friend_ui), x + 10, y + 20);

	return TRUE;
}

static gboolean
on_message_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	g_debug ("press on message!");
	GnomeAppAccountUI *account_ui;
	GnomeAppMessageUI *message_ui;
	gfloat x, y;

	account_ui = GNOME_APP_ACCOUNT_UI (data);

	message_ui = gnome_app_message_ui_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (account_ui), CLUTTER_ACTOR (message_ui));
	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_set_position (CLUTTER_ACTOR (message_ui), x - 50, y + 20);

	return TRUE;
}

GnomeAppAccountUI *
gnome_app_account_ui_new (gchar *personid)
{
	GnomeAppAccountUI *app_account_ui;
	GnomeAppAccountUIPrivate *priv;

	app_account_ui = g_object_new (GNOME_APP_TYPE_ACCOUNT_UI, NULL);
	priv = app_account_ui->priv;

	if (personid) {
		priv->person_id = g_strdup (personid);
	} else {
		/*Display the login account */
		GnomeAppStore *store;
		const gchar *username;

		store = gnome_app_store_get_default ();
		username = gnome_app_store_get_username (store);
		if (username)
			priv->person_id = g_strdup (username);
		else
			priv->person_id = NULL;
	}

        gchar *filename;
        GError *error;
        ClutterScript *script;
        ClutterActor *actor;

        error = NULL;
        filename = open_app_get_ui_uri ("app-account-ui");
        script = clutter_script_new ();
        clutter_script_load_from_file (script, filename, &error);
        if (error) {
                printf ("error in load script %s\n", error->message);
                g_error_free (error);
        }

        clutter_script_get_objects (script, "app-account-ui", &priv->ui_group, NULL);
	if (!priv->ui_group) {
		g_critical ("Cannot find 'app-account-ui' in %s!\n", filename);
		g_free (filename);
		return app_account_ui;
	}
	clutter_container_add_actor (CLUTTER_CONTAINER (app_account_ui), CLUTTER_ACTOR (priv->ui_group));

	clutter_script_get_objects (script, "avatar", &priv->avatar, NULL);
	if (!priv->avatar) {
		g_critical ("Cannot find 'avatar' in %s!\n", filename);
	}
 	gchar *user_icon;
	user_icon = open_app_get_pixmap_uri ("person");
	clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->avatar), user_icon, NULL);
	g_free (user_icon);

	clutter_script_get_objects (script, "username", &priv->username, NULL);
	if (!priv->username) {
		g_critical ("Cannot find 'username' in %s!\n", filename);
	}

	clutter_script_get_objects (script, "company", &priv->company, NULL);
	if (!priv->company) {
		g_critical ("Cannot find 'company' in %s!\n", filename);
	}

	clutter_script_get_objects (script, "homepage", &priv->homepage, NULL);
	if (!priv->homepage) {
		g_critical ("Cannot find 'homepage' in %s!\n", filename);
	}

	clutter_script_get_objects (script, "friends", &priv->friends, NULL);
	if (!priv->friends) {
		g_critical ("Cannot find 'friends' in %s!\n", filename);
	} else {
		g_signal_connect (priv->friends, "button-press-event", G_CALLBACK (on_friend_press), app_account_ui);
	}

	clutter_script_get_objects (script, "messages", &priv->messages, NULL);
	if (!priv->messages) {
		g_critical ("Cannot find 'messages' in %s!\n", filename);
	} else {
		g_signal_connect (priv->messages, "button-press-event", G_CALLBACK (on_message_press), app_account_ui);
	}

	g_free (filename);

	if (priv->person_id) {
		GnomeAppTask *task;
		gchar *function;

		function = g_strdup_printf ("/v1/person/data/%s",  priv->person_id);
		task = gnome_app_task_new (app_account_ui, "GET", function);
		gnome_app_task_set_callback (task, set_account_callback);
		gnome_app_task_push (task);

		g_free (function);
	} else {
		/*TODO: display the not login interface */		
	}

	return app_account_ui;
}
