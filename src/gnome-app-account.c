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
#include "gnome-app-stage.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-widgets.h"
#include "gnome-app-friends.h"
#include "gnome-app-message.h"
#include "gnome-app-account.h"
#include "gnome-app-ui-utils.h"

struct _GnomeAppAccountPrivate
{
	ClutterScript	*script;
	ClutterGroup    *main_ui;
	gchar 		*person_id;
};

/* Properties */
enum
{
	PROP_0,
	PROP_PERSON_ID,
       	PROP_LAST
};


G_DEFINE_TYPE (GnomeAppAccount, gnome_app_account, CLUTTER_TYPE_GROUP)

static gpointer
set_account_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppAccount *account;
	GnomeAppAccountPrivate *priv;
	OpenResults *results;

 	results = OPEN_RESULTS (func_result);
	account = GNOME_APP_ACCOUNT (userdata);
	priv = account->priv;
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
	ClutterActor *login;
        GList *list;
	const gchar *val;
	gchar *name;

	list = open_results_get_data (results);
	result = list->data;
       	clutter_script_get_objects (priv->script, 
			"avatar", &avatar,
			"login", &login,
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
	gnome_app_button_set_text (GNOME_APP_BUTTON (homepage), (gchar *)val);

	/*TODO there should have 'space' between name, this is to make my account name looks better */
	name = g_strdup_printf ("%s%s", 
				open_result_get (result, "firstname"), 
				open_result_get (result, "lastname"));
	clutter_text_set_text (CLUTTER_TEXT (username), name);
	g_free (name);

	clutter_actor_hide (login);
	return NULL;
}

static void
gnome_app_account_load (GnomeAppAccount *account)
{
	GnomeAppAccountPrivate *priv;
        ClutterActor *main_ui;
	ClutterActor *login;
	ClutterActor *username;
	ClutterActor *company;
	ClutterActor *homepage;
	ClutterActor *friends;
        ClutterActor *messages;

	priv = account->priv;

	if (priv->person_id) {
		GnomeAppTask *task;
		gchar *function;

       		clutter_script_get_objects (priv->script, 
			"login", &login,
			NULL);
		clutter_actor_hide (login);
		function = g_strdup_printf ("/v1/person/data/%s",  priv->person_id);
		task = gnome_app_task_new (account, "GET", function);
		gnome_app_task_set_callback (task, set_account_callback);
		gnome_app_task_push (task);

		g_free (function);
	} else {
		/*TODO: display the not account interface */		
       		clutter_script_get_objects (priv->script, 
			"login", &login,
			"username", &username,
			"company", &company,
			"homepage", &homepage,
			"friends", &friends,
			"messages", &messages,
			NULL);
		clutter_text_set_text (CLUTTER_TEXT (username), _("Guest"));
		clutter_actor_show (login);
		clutter_actor_hide (company);
		clutter_actor_hide (homepage);
		clutter_actor_hide (friends);
		clutter_actor_hide (messages);
	}
}

G_MODULE_EXPORT gboolean
on_account_login_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppStage *app_stage;

	app_stage = gnome_app_stage_get_default ();
	gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_DEFAULT, "GnomeAppLogin", NULL);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_friends_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	g_debug ("press on friends!");
	GnomeAppAccount *account;
	GnomeAppFriends *friend;
	gfloat x, y;

	account = GNOME_APP_ACCOUNT (data);

	friend = gnome_app_friends_new (account->priv->person_id);
	clutter_container_add_actor (CLUTTER_CONTAINER (account), CLUTTER_ACTOR (friend));
	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_set_position (CLUTTER_ACTOR (friend), x + 10, y + 20);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_message_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	g_debug ("press on message!");
	GnomeAppAccount *account;
	GnomeAppMessage *message;
	gfloat x, y;

	account = GNOME_APP_ACCOUNT (data);

	message = gnome_app_message_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (account), CLUTTER_ACTOR (message));
	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_set_position (CLUTTER_ACTOR (message), x - 50, y + 20);

	return TRUE;
}

static void
gnome_app_account_init (GnomeAppAccount *account)
{
	GnomeAppAccountPrivate *priv;
        ClutterActor *main_ui;
	GnomeAppStore *store;
	const gchar *username;

	account->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (account,
	                                                 GNOME_APP_TYPE_ACCOUNT,
	                                                 GnomeAppAccountPrivate);
			
        priv->script = gnome_app_script_new_from_file ("app-account");
        if (!priv->script)
		return;

        clutter_script_connect_signals (priv->script, account);
        clutter_script_get_objects (priv->script, 
			"app-account", &main_ui,
			NULL);

	clutter_container_add_actor (CLUTTER_CONTAINER (account), CLUTTER_ACTOR (main_ui));

	store = gnome_app_store_get_default ();
	username = gnome_app_store_get_username (store);
	if (username) {
		priv->person_id = g_strdup (username);
	} else
		priv->person_id = NULL;
	gnome_app_account_load (account);
}

static void
gnome_app_account_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_account_parent_class)->dispose (object);
}

static void
gnome_app_account_finalize (GObject *object)
{
	GnomeAppAccount *account = GNOME_APP_ACCOUNT (object);
	GnomeAppAccountPrivate *priv = account->priv;

	if (priv->script)
		g_object_unref (priv->script);
	if (priv->person_id)
		g_free (priv->person_id);

	G_OBJECT_CLASS (gnome_app_account_parent_class)->finalize (object);
}


static void
gnome_app_account_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppAccount *account;
	GnomeAppAccountPrivate *priv;
		
	account = GNOME_APP_ACCOUNT (object);
	priv = account->priv;
	switch (prop_id)
	{
		case PROP_PERSON_ID:
			if (priv->person_id)
				g_free (priv->person_id);
			priv->person_id = g_strdup (g_value_get_string (value));
			gnome_app_account_load (account);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_account_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppAccount *account;
 	GnomeAppAccountPrivate *priv;

	account = GNOME_APP_ACCOUNT (object);
	priv = account->priv;
	switch (prop_id)
	{
		case PROP_PERSON_ID:
			g_value_set_string (value, priv->person_id);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_account_class_init (GnomeAppAccountClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_account_set_property;
	object_class->get_property = gnome_app_account_get_property;
	object_class->dispose = gnome_app_account_dispose;
	object_class->finalize = gnome_app_account_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppAccountPrivate));
}

GnomeAppAccount *
gnome_app_account_new (void)
{
	GnomeAppAccount *account;

	account = g_object_new (GNOME_APP_TYPE_ACCOUNT, NULL);

	return account;
}

