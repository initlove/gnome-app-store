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
#include "gnome-app-application.h"
#include "gnome-app-info-page.h"
#include "gnome-app-frame-ui.h"

static void	gnome_app_login 		(gchar *username);
static gboolean check_user 			(gchar *username, gchar *password);

static gboolean
on_login_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	ClutterScript *script;
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
	}

	return TRUE;
}


static void
gnome_app_login (gchar *username_content)
{
	ClutterActor *stage;
	ClutterActor *username, *username_entry;
	ClutterActor *password, *password_entry;
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
			"username", &username,
			"username-entry", &username_entry,
			"password", &password,
			"password-entry", &password_entry,
			"register", &register_button,
			"login", &login_button,
			NULL);

	if (username_content)
		clutter_text_set_text (CLUTTER_TEXT (username_entry), username_content);

	clutter_stage_set_title (CLUTTER_STAGE (stage), "login");
	clutter_actor_set_name (stage, "login");
	clutter_actor_show (stage);

	g_signal_connect (login_button, "button-press-event", G_CALLBACK (on_login_press), script);

	//  g_signal_connect (new_stage, "destroy", G_CALLBACK (on_destroy), NULL);
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

