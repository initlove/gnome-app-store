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
#include "gnome-app-stage.h"
#include "gnome-app-login.h"
#include "gnome-app-widgets.h"

struct _GnomeAppLoginPrivate
{
	ClutterScript *script;
};

/* Properties */
enum
{
	PROP_0,
	PROP_USERNAME,
	PROP_PASSWORD,
	PROP_LAST
};

enum
{
	AUTH,
	LAST_SIGNAL
};

static guint login_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppLogin, gnome_app_login, CLUTTER_TYPE_GROUP)

static gpointer
auth_valid_callback (gpointer userdata, gpointer func_result)
{
	OpenResults *results;

	results = OPEN_RESULTS (func_result);

	if (results && open_results_get_status (results)) {
		GnomeAppStage *app_stage;
		GnomeAppLogin *login;
		GnomeAppLoginPrivate *priv;
		GnomeAppStore *store;
		ClutterActor *auto_login_check_box;
		ClutterActor *username_entry;
		ClutterActor *password_entry;
		const gchar *username;
	        const gchar *password;
		gboolean save;

		login = GNOME_APP_LOGIN (userdata);
		priv = login->priv;
		clutter_script_get_objects (priv->script,
				"auto-login-check-box", &auto_login_check_box,
				"username-entry", &username_entry,
				"password-entry", &password_entry,
				NULL);
		save = gnome_app_check_box_get_selected (GNOME_APP_CHECK_BOX (auto_login_check_box));
		username = gnome_app_entry_get_text (GNOME_APP_ENTRY (username_entry));
		password = gnome_app_entry_get_text (GNOME_APP_ENTRY (password_entry));
		store = gnome_app_store_get_default ();
		g_object_set (store, "username", username, "password", password,
				"save", save,
				NULL);
/*TODO: remove it, will it be used in the future? 
		g_signal_emit (login, login_signals [AUTH], 0);
*/
		app_stage = gnome_app_stage_get_default ();
		gnome_app_stage_load (app_stage, 
				"GnomeAppFrameUI", 
				NULL);
	} else {
		g_debug ("error in auth %s\n", open_results_get_meta (results, "message"));
/*TODO: add warning here */
	}
	return NULL;
}

G_MODULE_EXPORT gboolean
on_register_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppStage *app_stage;

	app_stage = gnome_app_stage_get_default ();
	gnome_app_stage_load (app_stage, "GnomeAppRegister", NULL);

	return TRUE;
}

G_MODULE_EXPORT gboolean
on_login_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	GnomeAppTask *task;
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
	username = gnome_app_entry_get_text (GNOME_APP_ENTRY (username_entry));
	password = gnome_app_entry_get_text (GNOME_APP_ENTRY (password_entry));
	if (!username || !username [0]) {
//		gnome_app_entry_add_warning (username_entry, "Input username!");
		valid = FALSE;
	}
	if (!password || !password [0]) {
//		gnome_app_entry_add_warning (password_entry, "Input password!");
		if (valid)
			valid = FALSE;
	}

	if (valid) {
		task = gnome_app_task_new (login, "POST", "/v1/person/check");
		gnome_app_task_set_callback (task, auth_valid_callback);
		gnome_app_task_add_params (task, 
			"login", username,
			"password", password,
			NULL);
		gnome_app_task_push (task);
	}

	return FALSE;
}

static void
gnome_app_login_init (GnomeAppLogin *login)
{
	GnomeAppLoginPrivate *priv;
	ClutterActor *main_ui;
	ClutterActor *register_button;
	ClutterActor *login_button;
	gchar *filename;

	login->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (login,
	                                                 GNOME_APP_TYPE_LOGIN,
	                                                 GnomeAppLoginPrivate);

	priv->script = gnome_app_script_new_from_file ("app-login");
	if (!priv->script) {
		return ;
	}

	clutter_script_connect_signals (priv->script, login);
	clutter_script_get_objects (priv->script,
			"app-login", &main_ui,
			"register", &register_button,
			"login", &login_button,
			NULL);
	gnome_app_button_binding (login_button);
	gnome_app_button_binding (register_button);

	/* TODO: It seems not work, currently, this it done in the gnome_app_stage 
	 *  if in the future, some small widget want to set its own background
	 *  re-do it, gnome_app_actor_add_background
	filename = open_app_get_pixmap_uri ("login");
	gnome_app_actor_add_background (CLUTTER_ACTOR (login), filename);
	g_free (filename);
	*/

	clutter_container_add_actor (CLUTTER_CONTAINER (login), main_ui);
}

static void
gnome_app_login_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	ClutterActor *actor;

	login = GNOME_APP_LOGIN (object);
	priv = login->priv;
	switch (prop_id)
	{
		case PROP_USERNAME:
			actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "username-entry"));
			gnome_app_entry_set_text (GNOME_APP_ENTRY (actor), (gchar *) g_value_get_string (value));
			break;
		case PROP_PASSWORD:
			actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "password-entry"));
			gnome_app_entry_set_text (GNOME_APP_ENTRY (actor), (gchar *) g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_login_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppLogin *login;
	GnomeAppLoginPrivate *priv;
	ClutterActor *actor;

	login = GNOME_APP_LOGIN (object);
	priv = login->priv;
	switch (prop_id)
	{
		case PROP_USERNAME:
			actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "username-entry"));
			g_value_set_string (value, gnome_app_entry_get_text (GNOME_APP_ENTRY (actor)));
			break;
		case PROP_PASSWORD:
			actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "password-entry"));
			g_value_set_string (value, gnome_app_entry_get_text (GNOME_APP_ENTRY (actor)));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
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

	        
	g_object_class_install_property (object_class,
			PROP_USERNAME,
			g_param_spec_string ("username",
				"User Name",
				"User Name",
				NULL,
				G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
			PROP_PASSWORD,
			g_param_spec_string ("password",
				"Password",
				"Password",
				NULL,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppLoginPrivate));
}

GnomeAppLogin *
gnome_app_login_new ()
{
	GnomeAppLogin *login;

	login = g_object_new (GNOME_APP_TYPE_LOGIN, NULL);

	return login;
}
