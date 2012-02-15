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
#include <string.h>
#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-login.h"
#include "gnome-app-register.h"

struct _GnomeAppLoginPrivate
{
	ClutterScript *script;
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
on_register_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppRegister *regist;
	GnomeAppLogin *login;

	regist = gnome_app_register_new ();
	gnome_app_register_run (regist);

	login = GNOME_APP_LOGIN (data);
	g_object_unref (login);
	return TRUE;
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

	clutter_script_get_objects (priv->script,
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

		if (priv->script) {
			clutter_script_get_objects (priv->script,
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
		}

		g_object_unref (login);
		gnome_app_application_new ();
	} else {
		g_debug ("error in auth %s\n", open_results_get_meta (results, "message"));
		gnome_app_login_run (login);
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

	priv->script = NULL;
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

	if (priv->script) {
		//FIXME: unref cannot hide the stage? not destroy it ?
		stage = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "app-login"));
		clutter_actor_destroy (stage);
		g_object_unref (priv->script);
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
	if (!priv->script) {
		filename = open_app_get_ui_uri ("app-login");
		priv->script = clutter_script_new ();
		error = NULL;
		clutter_script_load_from_file (priv->script, filename, &error);
		gnome_app_script_po (priv->script);
		g_free (filename);
		if (error) {
			g_error ("fail to load app login script %s\n", error->message);
			g_error_free (error);
			return;
		}
	}

	clutter_script_get_objects (priv->script, "app-login", &stage,
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
	gnome_app_entry_add_hint (username_entry, _("< user name >"));
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_add_hint (password_entry, _("< password >"));
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
