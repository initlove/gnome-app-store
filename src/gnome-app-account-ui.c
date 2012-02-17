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
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-friends-ui.h"
#include "gnome-app-message-ui.h"
#include "gnome-app-account-ui.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppAccountUIPrivate
{
	ClutterScript	*script;
	ClutterGroup    *ui_group;
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
			
	priv->script = NULL;
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

	if (priv->script)
		g_object_unref (priv->script);
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
	GnomeAppAccountUIPrivate *priv;
	OpenResults *results;

 	results = OPEN_RESULTS (func_result);
	account_ui = GNOME_APP_ACCOUNT_UI (userdata);
	priv = account_ui->priv;
	if (!open_results_get_status (results)) {
		/*TODO: fill the account with default val */
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} 

        OpenResult *result;
	ClutterActor *avatar;
	ClutterActor *company;
	ClutterActor *homepage;
	ClutterActor *username;
        GList *list;
	const gchar *val;
	gchar *name;

	list = open_results_get_data (results);
	result = list->data;
       	clutter_script_get_objects (priv->script, 
			"avatar", &avatar,
			"company", &company,
			"homepage", &homepage,
			"username", &username,
			NULL);
	val = open_result_get (result, "avatarpicfound");
	if (val && (strcmp (val, "1") == 0)) {
		val = open_result_get (result, "avatarpic");
		gnome_app_set_icon (avatar, val);
	} else {
		val = open_result_get (result, "bigavatarpicfound");
		if (val && (strcmp (val, "1") == 0)) {
			val = open_result_get (result, "bigavatarpic");
			gnome_app_set_icon (avatar, val);
		}
	}

	val = open_result_get (result, "company");
	clutter_text_set_text (CLUTTER_TEXT (company), val);

	/*TODO: should add result to priv, when click on homepage, open this homepage */
	val = open_result_get (result, "homepage");
	clutter_text_set_text (CLUTTER_TEXT (homepage), val);

	/*TODO there should have 'space' between name, this is to make my login name looks better */
	name = g_strdup_printf ("%s%s", 
				open_result_get (result, "firstname"), 
				open_result_get (result, "lastname"));
	clutter_text_set_text (CLUTTER_TEXT (username), name);
	g_free (name);

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
        ClutterActor *ui_group;
	ClutterActor *friends;
        ClutterActor *messages;

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

        priv->script = gnome_app_script_new_from_file ("app-account-ui");
        if (!priv->script)
		return app_account_ui;

        clutter_script_get_objects (priv->script, 
			"app-account-ui", &ui_group,
			"friends", &friends,
			"messages", &messages,
			NULL);

	clutter_container_add_actor (CLUTTER_CONTAINER (app_account_ui), CLUTTER_ACTOR (ui_group));

	g_signal_connect (friends, "button-press-event", G_CALLBACK (on_friend_press), app_account_ui);
	g_signal_connect (messages, "button-press-event", G_CALLBACK (on_message_press), app_account_ui);

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
