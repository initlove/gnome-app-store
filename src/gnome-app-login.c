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

#include <string.h>
#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-login.h"

struct _GnomeAppLoginPrivate
{
	ClutterScript *login_script;
	ClutterScript *register_script;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LAST
};

enum
{
	AUTH,
	LAST_SIGNAL
};

static guint login_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppLogin, gnome_app_login, G_TYPE_OBJECT)

static void auth_valid (GnomeAppLogin *login, const gchar *username, const gchar *password);

static gboolean
email_valid (const gchar *email)
{
	g_return_val_if_fail (email, FALSE);
/*TODO: more strict */
	if (strchr (email, '@'))
		return TRUE;
	else
		return FALSE;
}

static gboolean
on_real_register (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	ClutterActor *stage;
	ClutterActor *username_label, *username_entry, *username_info;
	ClutterActor *password_label, *password_entry, *password_info;
	ClutterActor *firstname_label, *firstname_entry, *firstname_info;
	ClutterActor *lastname_label, *lastname_entry, *lastname_info;
	ClutterActor *email_label, *email_entry, *email_info;
	ClutterActor *register_button;
	ClutterScript *script;

	gboolean validation;
	const gchar *username;
	const gchar *password;
	const gchar *firstname;
	const gchar *lastname;
	const gchar *email;

	login = GNOME_APP_LOGIN (data);
	priv = login->priv;
	clutter_script_get_objects (priv->register_script, "app-register", &stage,
			"username", &username_label,
			"username-entry", &username_entry,
			"username-info", &username_info,
			"password", &password_label,
			"password-entry", &password_entry,
			"password-info", &password_info,
			"firstname", &firstname_label,
			"firstname-entry", &firstname_entry,
			"firstname-info", &firstname_info,
			"lastname", &lastname_label,
			"lastname-entry", &lastname_entry,
			"lastname-info", &lastname_info,
			"email", &email_label,
			"email-entry", &email_entry,
			"email-info", &email_info,
			"register-button", &register_button,
			NULL);
	validation = TRUE;

	gnome_app_entry_binding (username_entry);
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_binding (firstname_entry);
	gnome_app_entry_binding (lastname_entry);
	gnome_app_entry_binding (email_entry);
	gnome_app_button_binding (register_button);

	username = clutter_text_get_text (CLUTTER_TEXT (username_entry));
	if (is_blank_text (username)) {
		clutter_text_set_text (CLUTTER_TEXT (username_info), "should not blank");
		validation = FALSE;
	} else
		clutter_text_set_text (CLUTTER_TEXT (username_info), "");

//password should have 8 longer
	password = clutter_text_get_text (CLUTTER_TEXT (password_entry));
	if (is_blank_text (password)) {
		clutter_text_set_text (CLUTTER_TEXT (password_info), "should not blank");
		validation = FALSE;
	} else
		clutter_text_set_text (CLUTTER_TEXT (password_info), "");

	firstname = clutter_text_get_text (CLUTTER_TEXT (firstname_entry));
	if (is_blank_text (firstname)) {
		clutter_text_set_text (CLUTTER_TEXT (firstname_info), "should not blank");
		validation = FALSE;
	} else
		clutter_text_set_text (CLUTTER_TEXT (firstname_info), "");

	lastname = clutter_text_get_text (CLUTTER_TEXT (lastname_entry));
	if (is_blank_text (lastname)) {
		clutter_text_set_text (CLUTTER_TEXT (lastname_info), "should not blank");
		validation = FALSE;
	} else
		clutter_text_set_text (CLUTTER_TEXT (lastname_info), "");

	email = clutter_text_get_text (CLUTTER_TEXT (email_entry));
	if (is_blank_text (email)) {
		clutter_text_set_text (CLUTTER_TEXT (email_info), "should not blank");
		validation = FALSE;
	} else if (!email_valid (email)) {
		clutter_text_set_text (CLUTTER_TEXT (email_info), "please enter the valid email");
		validation = FALSE;
	} else
		clutter_text_set_text (CLUTTER_TEXT (email_info), "");


	if (validation) {
		GnomeAppTask *task;
		const gchar *val;
		OpenResults *results;
		gint status_code;

		task = gnome_app_sync_task_new ("POST", "/v1/person/add");
		gnome_app_task_add_params (task, 
			"login", username,
			"password", password,
			"firstname", firstname,
			"lastname", lastname,
			"email", email,
			NULL);
		results = gnome_app_sync_task_push (task);
		g_object_unref (task);
		if (!results) {
			//TODO
			return FALSE;
		}
		if (open_results_get_status (results)) {
printf ("%s %s is good to go, login %s first\n", login, password, email);
		} else {
			val = open_results_get_meta (results, "statuscode");
printf ("status code %s\n", val);
		}
	}
}

static gboolean
on_register_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	ClutterActor *stage;
	ClutterActor *username, *username_entry;
	ClutterActor *password, *password_entry;
	ClutterActor *firstname, *firstname_entry;
	ClutterActor *lastname, *lastname_entry;
	ClutterActor *email, *email_entry;
	ClutterActor *register_button;
	GError *error;
	gchar *filename;

	login = GNOME_APP_LOGIN (data);
	priv = login->priv;
	stage = CLUTTER_ACTOR (clutter_script_get_object (priv->login_script, "app-login"));
	clutter_actor_hide (stage);
	
	clutter_script_get_objects (priv->register_script, "app-register", &stage,
			"username", &username,
			"username-entry", &username_entry,
			"password", &password,
			"password-entry", &password_entry,
			"firstname", &firstname,
			"firstname-entry", &firstname_entry,
			"lastname", &lastname,
			"lastname-entry", &lastname_entry,
			"email", &email,
			"email-entry", &email_entry,
			"register", &register_button,
			NULL);

	gnome_app_entry_binding (username_entry);
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_binding (firstname_entry);
	gnome_app_entry_binding (lastname_entry);
	gnome_app_entry_binding (email_entry);
	gnome_app_button_binding (register_button);

	clutter_stage_set_title (CLUTTER_STAGE (stage), "register");
	clutter_actor_set_name (stage, "register");
	gnome_app_stage_remove_decorate (stage);
	gnome_app_stage_set_position (stage, GNOME_APP_POSITION_CENTER);
	clutter_actor_show (stage);

	g_signal_connect (register_button, "button-press-event", G_CALLBACK (on_real_register), login);
}

static gboolean
on_login_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	ClutterActor *stage;
	ClutterActor *username_entry;
	ClutterActor *password_entry;
	const gchar *username;
	const gchar *password;
	gboolean valid;

	valid = TRUE;
	login = GNOME_APP_LOGIN (data);
	priv = login->priv;

	clutter_script_get_objects (priv->login_script,
		       	"username-entry", &username_entry,
			"password-entry", &password_entry,
			NULL);
	username = clutter_text_get_text (CLUTTER_TEXT (username_entry));
	password = clutter_text_get_text (CLUTTER_TEXT (password_entry));
	if (!username || !username [0]) {
//		gnome_app_entry_add_warning (username_entry, "Input username!");
		valid = FALSE;
	}
	if (!password || !password [0]) {
//		gnome_app_entry_add_warning (password_entry, "Input password!");
		if (valid)
			valid = FALSE;
	}
	if (valid)
		auth_valid (login, username, password);

	return FALSE;
}

static gpointer
auth_valid_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	login = GNOME_APP_LOGIN (userdata);
	priv = login->priv;

	if (results && open_results_get_status (results)) {
		GnomeAppStore *store;
		ClutterActor *stage;
		ClutterActor *auto_login_check_box;
		ClutterActor *username_entry;
	        ClutterActor *password_entry;
		const gchar *username;
        	const gchar *password;
		gboolean save;

		clutter_script_get_objects (priv->login_script,
				"app-login", &stage,
				"auto-login-check-box", &auto_login_check_box,
				"username-entry", &username_entry,
				"password-entry", &password_entry,
				NULL);
		save = gnome_app_check_box_get_selected (auto_login_check_box);
		username = clutter_text_get_text (CLUTTER_TEXT (username_entry));
		password = clutter_text_get_text (CLUTTER_TEXT (password_entry));
		store = gnome_app_store_get_default ();
		g_object_set (store, "username", username, "password", password,
				"save", save,
				NULL);

		g_signal_emit (login, login_signals [AUTH], 0);
	} else {
	}
	return NULL;
}

static void
auth_valid (GnomeAppLogin *login, const gchar *username, const gchar *password)
{
	g_return_if_fail (username && password);

	GnomeAppTask *task;

	task = gnome_app_task_new (login, "POST", "/v1/person/check");
	gnome_app_task_set_callback (task, auth_valid_callback);
	gnome_app_task_add_params (task, 
			"login", username,
			"password", password,
			NULL);
	gnome_app_task_push (task);

	return;
}


static void
gnome_app_login_init (GnomeAppLogin *login)
{
	GnomeAppLoginPrivate *priv;

	login->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (login,
	                                                 GNOME_APP_TYPE_LOGIN,
	                                                 GnomeAppLoginPrivate);

	priv->login_script = NULL;
	priv->register_script = NULL;
}

static void
gnome_app_login_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppLogin *login;

	login = GNOME_APP_LOGIN (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_login_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppLogin *login;

	login = GNOME_APP_LOGIN (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_login_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_login_parent_class)->dispose (object);
}

static void
gnome_app_login_finalize (GObject *object)
{
	GnomeAppLogin *login = GNOME_APP_LOGIN (object);
	GnomeAppLoginPrivate *priv = login->priv;
	ClutterActor *stage;

	if (priv->login_script) {
		//FIXME: unref cannot hide the stage? not destroy it ?
		stage = CLUTTER_ACTOR (clutter_script_get_object (priv->login_script, "app-login"));
		clutter_actor_destroy (stage);
		g_object_unref (priv->login_script);
	}

	if (priv->register_script) {
		stage = CLUTTER_ACTOR (clutter_script_get_object (priv->register_script, "app-register"));
		clutter_actor_destroy (stage);
		g_object_unref (priv->register_script);
	}

	G_OBJECT_CLASS (gnome_app_login_parent_class)->finalize (object);
}

static void
gnome_app_login_class_init (GnomeAppLoginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_login_set_property;
	object_class->get_property = gnome_app_login_get_property;
	object_class->dispose = gnome_app_login_dispose;
	object_class->finalize = gnome_app_login_finalize;

        /**
	 *          * GnomeAppComment::refresh:
	 *                   * @self: the #GnomeAppComment that e   priv->callback = NULL;mitted the signal
	 *                            *
	 *                                     * The ::refresh signal is emitted after we make a reply
	 *                                              *
	 *                                                       */
	login_signals [AUTH] = 	
		g_signal_new (g_intern_static_string ("auth"),
   				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (GnomeAppLoginClass, auth),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	g_type_class_add_private (object_class, sizeof (GnomeAppLoginPrivate));
}

GnomeAppLogin *
gnome_app_login_new (void)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	GnomeAppStore *store;
	GError *error;
	gchar *filename;
	gchar *username;
        gchar *password;

	login = g_object_new (GNOME_APP_TYPE_LOGIN, NULL);
	priv = login->priv;
	filename = open_app_get_ui_uri ("app-login");
	priv->login_script = clutter_script_new ();
	error = NULL;
	clutter_script_load_from_file (priv->login_script, filename, &error);
	g_free (filename);
	if (error) {
		g_error_free (error);
		g_object_unref (login);
		return NULL;
	}

	filename = open_app_get_ui_uri ("app-register");
	priv->register_script = clutter_script_new ();
	error = NULL;
	clutter_script_load_from_file (priv->register_script, filename, &error);
	g_free (filename);
	if (error) {
		g_error_free (error);
		g_object_unref (login);
		return NULL;
	}

	store = gnome_app_store_get_default ();
	g_object_get (store, "username", &username, "password", &password, NULL);
	if (username && password) {
		auth_valid (login, username, password);
	} else {
		gnome_app_login_run (login);
	}

	return login;
}

void
gnome_app_login_run (GnomeAppLogin *login)
{
	g_return_if_fail (login);

	GnomeAppLoginPrivate *priv;
	GnomeAppStore *store;
	ClutterActor *stage;
	ClutterActor *username_entry;
	ClutterActor *password_entry;
	ClutterActor *auto_login_check_box, *auto_login_label;
	ClutterActor *register_button, *login_button;
	gchar *filename;
	GError *error;
	gchar *default_username;

	priv = login->priv;
	clutter_script_get_objects (priv->login_script, "app-login", &stage,
			"username-entry", &username_entry,
			"password-entry", &password_entry,
			"auto-login-check-box", &auto_login_check_box,
			"auto-login-label", &auto_login_label,
			"register", &register_button,
			"login", &login_button,
			NULL);

	gnome_app_stage_remove_decorate (stage);
	gnome_app_stage_set_position (stage, GNOME_APP_POSITION_CENTER);
	filename = open_app_get_pixmap_uri ("login");
	gnome_app_actor_add_background (stage, filename);
	g_free (filename);
	gnome_app_entry_binding (username_entry);
	gnome_app_entry_add_hint (username_entry, "user name");
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_add_hint (password_entry, "password");
	gnome_app_check_box_binding (auto_login_check_box);
	gnome_app_check_box_add_connector (auto_login_check_box, auto_login_label);
	gnome_app_button_binding (login_button);
	gnome_app_button_binding (register_button);

	store = gnome_app_store_get_default ();
	g_object_get (store, "username", &default_username, NULL);
	if (default_username)
		clutter_text_set_text (CLUTTER_TEXT (username_entry), default_username);

	clutter_actor_show (stage);

	g_signal_connect (register_button, "button-press-event", G_CALLBACK (on_register_press), login);
	g_signal_connect (login_button, "button-press-event", G_CALLBACK (on_login_press), login);
}
