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
#include <glib/gi18n.h>
#include <clutter/clutter.h>

#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-application.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

static void	gnome_app_login 		(gchar *username);
static gboolean check_user 			(gchar *username, gchar *password);

static gboolean
email_valid (gchar *email)
{
	g_return_val_if_fail (email, FALSE);
/*TODO: more strict */
	if (strchr (email, '@'))
		return TRUE;
	else
		return FALSE;
}

static gboolean
on_real_register_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	ClutterActor *stage;
	ClutterActor *username_label, *username_entry, *username_info;
	ClutterActor *password_label, *password_entry, *password_info;
	ClutterActor *firstname_label, *firstname_entry, *firstname_info;
	ClutterActor *lastname_label, *lastname_entry, *lastname_info;
	ClutterActor *email_label, *email_entry, *email_info;
	ClutterActor *register_button;
	ClutterScript *script;

	gboolean validation;
	gchar *login;
	gchar *password;
	gchar *firstname;
	gchar *lastname;
	gchar *email;

	script = CLUTTER_SCRIPT (data);
	clutter_script_get_objects (script, "app-register", &stage,
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

	login = clutter_text_get_text (CLUTTER_TEXT (username_entry));
	if (is_blank_text (login)) {
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
			"login", login,
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
	ClutterActor *stage;
	ClutterActor *username, *username_entry;
	ClutterActor *password, *password_entry;
	ClutterActor *firstname, *firstname_entry;
	ClutterActor *lastname, *lastname_entry;
	ClutterActor *email, *email_entry;
	ClutterActor *register_button;
	ClutterScript *script;
	GError *error;
	gchar *filename;

	script = CLUTTER_SCRIPT (data);
	stage = clutter_script_get_object (script, "app-login");
	clutter_actor_destroy (stage);
	g_object_unref (script);
	
	filename = open_app_get_ui_uri ("app-register");
	script = clutter_script_new ();
	error = NULL;
	clutter_script_load_from_file (script, filename, &error);
	g_free (filename);
	if (error) {
		g_error_free (error);
		g_object_unref (script);
		return ;
	}
	                                      
	clutter_script_get_objects (script, "app-register", &stage,
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

	clutter_stage_set_title (CLUTTER_STAGE (stage), "register");
	clutter_actor_set_name (stage, "register");
	clutter_actor_show (stage);
	gnome_app_stage_set_position (stage, GNOME_APP_POSITION_CENTER);

	g_signal_connect (register_button, "button-press-event", G_CALLBACK (on_real_register_press), script);
}

static gboolean
on_login_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	ClutterScript *script;
	ClutterActor *stage;
	ClutterActor *username_entry;
	ClutterActor *password_entry;
	gchar *username;
	gchar *password;

	script = CLUTTER_SCRIPT (data);
	clutter_script_get_objects (script,
		       	"username-entry", &username_entry,
			"password-entry", &password_entry,
			NULL);
	username = clutter_text_get_text (CLUTTER_TEXT (username_entry));
	password = clutter_text_get_text (CLUTTER_TEXT (password_entry));

	if (check_user (username, password)) {
		GnomeAppStore *store;
		store = gnome_app_store_get_default ();
		g_object_set (store,
			"username", username,
			"password", password,
			NULL);
		stage = clutter_script_get_object (script, "app-login");
		clutter_actor_destroy (stage);
		g_object_unref (script);

		gnome_app_application_run ();
	} else {
	}

	return TRUE;
}


static void
gnome_app_login (gchar *username_content)
{
	ClutterActor *stage;
	ClutterActor *username_entry;
	ClutterActor *password_entry;
	ClutterActor *auto_login_check_box, *auto_login_label;
	ClutterActor *register_button, *login_button;
	ClutterScript *script;
	GError *error;
	gchar *filename;

	filename = open_app_get_ui_uri ("app-login");
	script = clutter_script_new ();
	error = NULL;
	clutter_script_load_from_file (script, filename, &error);
	g_free (filename);
	if (error) {
		g_error_free (error);
		g_object_unref (script);
		return ;
	}
	                                      
	clutter_script_get_objects (script, "app-login", &stage,
			"username-entry", &username_entry,
			"password-entry", &password_entry,
			"auto-login-check-box", &auto_login_check_box,
			"auto-login-label", &auto_login_label,
			"register", &register_button,
			"login", &login_button,
			NULL);

	clutter_stage_set_title (CLUTTER_STAGE (stage), "login");
	clutter_actor_set_name (stage, "login");
	clutter_actor_show (stage);
	gnome_app_stage_set_position (stage, GNOME_APP_POSITION_CENTER);

	if (username_content)
		clutter_text_set_text (CLUTTER_TEXT (username_entry), username_content);

	gnome_app_entry_binding (username_entry);
	gnome_app_entry_add_hint (username_entry, "user name");
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_add_hint (password_entry, "password");
	gnome_app_check_box_binding (auto_login_check_box, auto_login_label);

	g_signal_connect (register_button, "button-press-event", G_CALLBACK (on_register_press), script);
	g_signal_connect (login_button, "button-press-event", G_CALLBACK (on_login_press), script);

}

void
gnome_app_auth (void)
{
	GnomeAppStore *store;
	gchar *username;
	gchar *password;

	store = gnome_app_store_get_default ();
	g_object_get (store, "username", &username, "password", &password, NULL);
	if (!username || !password) {
		gnome_app_login (username);
	} else {
		if (check_user (username, password)) {
printf ("success\n");
//			gnome_app_application_run (app);
		} else {
			//TODO: set the fail info 
			gnome_app_login (username);
		}
	}
}

static gboolean
check_user (gchar *username, gchar *password)
{
	g_return_val_if_fail (username && password, FALSE);

	GnomeAppTask *task;
	OpenResults *results;
	gboolean val;

	task = gnome_app_sync_task_new ("POST", "/v1/person/check");
	gnome_app_task_add_params (task, 
			"login", username,
			"password", password,
			NULL);
	results = gnome_app_sync_task_push (task);
	g_object_unref (task);
	if (!results)
	       return FALSE;

	if (open_results_get_status (results)) {
		val = TRUE;
	} else {
		val = FALSE;
	}
		
	g_object_unref (results);

	return val;
}       

