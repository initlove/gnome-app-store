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

struct _GnomeAppRegisterPrivate
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
	REGIST,
	LAST_SIGNAL
};

static guint register_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (GnomeAppRegister, gnome_app_register, G_TYPE_OBJECT)

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
on_back_button_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppRegister *regist;

	gnome_app_login_run ();
	regist = GNOME_APP_REGISTER (data);
	g_object_unref (regist);

	return FALSE;
}

static gpointer
register_callback (gpointer userdata, gpointer func_result)
{
	OpenResults *results;
	GnomeAppRegister *regist;
	GnomeAppRegisterPrivate *priv;
	ClutterActor *actor;
	const gchar *val;
	gint code;

	results = OPEN_RESULTS (func_result);
	regist = GNOME_APP_REGISTER (userdata);
	priv = regist->priv;

	if (open_results_get_status (results)) {
//TODO: while the *info set, clean it if the related entry was changed.
//	should done it by gnome_app_entry_add_connector 
//TODO: 
printf ("You should login you email to confirm the regist\n");
	} else {
		val = open_results_get_meta (results, "statuscode");
		code = atoi (val);
		switch (code) {
			case 101:
			case 102:
			case 103:
			case 106:
				g_debug ("This should be handled by the client.\n");
				g_debug ("TODO if have time .\n");
				break;
			case 104:
/*TODO: check it while type it */
				actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "username-info"));
				val = open_results_get_meta (results, "message");
				clutter_text_set_text (CLUTTER_TEXT (actor), val);
				break;
			case 105:
				actor = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "email-info"));
				val = open_results_get_meta (results, "message");
				clutter_text_set_text (CLUTTER_TEXT (actor), val);
				break;
			default:
				break;
		}
	}

	return NULL;
}

static gboolean
on_register_button_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppRegister *regist;
	GnomeAppRegisterPrivate *priv;
	ClutterActor *stage;
	ClutterActor *username_label, *username_entry, *username_info;
	ClutterActor *password_label, *password_entry, *password_info;
	ClutterActor *firstname_label, *firstname_entry, *firstname_info;
	ClutterActor *lastname_label, *lastname_entry, *lastname_info;
	ClutterActor *email_label, *email_entry, *email_info;
	ClutterActor *register_button;
	ClutterScript *script;

	GError *error;
	gboolean validation;
	const gchar *username;
	const gchar *password;
	const gchar *firstname;
	const gchar *lastname;
	const gchar *email;

	regist = GNOME_APP_REGISTER (data);
	priv = regist->priv;
	clutter_script_get_objects (priv->script, "app-register", &stage,
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
			"regist-button", &register_button,
			NULL);
	validation = TRUE;

	gnome_app_entry_binding (username_entry);
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_binding (firstname_entry);
	gnome_app_entry_binding (lastname_entry);
	gnome_app_entry_binding (email_entry);
	gnome_app_button_binding (register_button);

	username = clutter_text_get_text (CLUTTER_TEXT (username_entry));
	error = NULL;
	if (open_app_pattern_match ("notblank", username, &error)) {
		clutter_text_set_text (CLUTTER_TEXT (username_info), "");
	} else {
		clutter_text_set_text (CLUTTER_TEXT (username_info), error->message);
		g_error_free (error);
		validation = FALSE;
	}

	password = clutter_text_get_text (CLUTTER_TEXT (password_entry));
	error = NULL;
	if (open_app_pattern_match ("password", password, &error)) {
		clutter_text_set_text (CLUTTER_TEXT (password_info), "");
	} else {
		clutter_text_set_text (CLUTTER_TEXT (password_info), error->message);
		g_error_free (error);
		validation = FALSE;
	}

	firstname = clutter_text_get_text (CLUTTER_TEXT (firstname_entry));
	error = NULL;
	if (open_app_pattern_match ("notblank", firstname, &error)) {
		clutter_text_set_text (CLUTTER_TEXT (firstname_info), "");
	} else {
		clutter_text_set_text (CLUTTER_TEXT (firstname_info), error->message);
		g_error_free (error);
		validation = FALSE;
	}

	lastname = clutter_text_get_text (CLUTTER_TEXT (lastname_entry));
	error = NULL;
	if (open_app_pattern_match ("notblank", lastname, &error)) {
		clutter_text_set_text (CLUTTER_TEXT (lastname_info), "");
	} else {
		clutter_text_set_text (CLUTTER_TEXT (lastname_info), error->message);
		g_error_free (error);
		validation = FALSE;
	}

	email = clutter_text_get_text (CLUTTER_TEXT (email_entry));
	error = NULL;
	if (open_app_pattern_match ("email", email, &error)) {
		clutter_text_set_text (CLUTTER_TEXT (email_info), "");
	} else {
		clutter_text_set_text (CLUTTER_TEXT (email_info), error->message);
		g_error_free (error);
		validation = FALSE;
	}

	if (validation) {
		GnomeAppTask *task;

		task = gnome_app_task_new (regist, "POST", "/v1/person/add");
		gnome_app_task_set_callback (task, register_callback);
		gnome_app_task_add_params (task, 
			"register", username,
			"password", password,
			"firstname", firstname,
			"lastname", lastname,
			"email", email,
			NULL);
		gnome_app_task_push (task);
	}
}

static void
gnome_app_register_init (GnomeAppRegister *regist)
{
	GnomeAppRegisterPrivate *priv;

	regist->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (regist,
	                                                 GNOME_APP_TYPE_REGISTER,
	                                                 GnomeAppRegisterPrivate);

	priv->script = NULL;
}

static void
gnome_app_register_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppRegister *regist;

	regist = GNOME_APP_REGISTER (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_register_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppRegister *regist;

	regist = GNOME_APP_REGISTER (object);

	switch (prop_id)
	{
	}
}

static void
gnome_app_register_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_register_parent_class)->dispose (object);
}

static void
gnome_app_register_finalize (GObject *object)
{
	GnomeAppRegister *regist = GNOME_APP_REGISTER (object);
	GnomeAppRegisterPrivate *priv = regist->priv;
	ClutterActor *stage;

	if (priv->script) {
		//FIXME: unref cannot hide the stage? not destroy it ?
		stage = CLUTTER_ACTOR (clutter_script_get_object (priv->script, "app-register"));
		clutter_actor_destroy (stage);
		g_object_unref (priv->script);
	}

	G_OBJECT_CLASS (gnome_app_register_parent_class)->finalize (object);
}

static void
gnome_app_register_class_init (GnomeAppRegisterClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gnome_app_register_set_property;
	object_class->get_property = gnome_app_register_get_property;
	object_class->dispose = gnome_app_register_dispose;
	object_class->finalize = gnome_app_register_finalize;

        /**
	 *          * GnomeAppComment::refresh:
	 *                   * @self: the #GnomeAppComment that e   priv->callback = NULL;mitted the signal
	 *                            *
	 *                                     * The ::refresh signal is emitted after we make a reply
	 *                                              *
	 *                                                       */
	register_signals [REGIST] = 	
		g_signal_new (g_intern_static_string ("regist"),
   				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (GnomeAppRegisterClass, regist),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	g_type_class_add_private (object_class, sizeof (GnomeAppRegisterPrivate));
}

GnomeAppRegister *
gnome_app_register_new (void)
{
	GnomeAppRegister *regist;
	GnomeAppRegisterPrivate *priv;
	GnomeAppStore *store;

	regist = g_object_new (GNOME_APP_TYPE_REGISTER, NULL);
	priv = regist->priv;
	priv->script = gnome_app_script_new_from_file ("app-register");
	if (!priv->script) {
		g_object_unref (regist);
		return NULL;
	}

	return regist;
}

void
gnome_app_register_run (GnomeAppRegister *regist)
{
	GnomeAppRegisterPrivate *priv;
	ClutterActor *stage;
	ClutterActor *username, *username_entry;
	ClutterActor *password, *password_entry;
	ClutterActor *firstname, *firstname_entry;
	ClutterActor *lastname, *lastname_entry;
	ClutterActor *email, *email_entry;
	ClutterActor *back_button;
	ClutterActor *register_button;
	GError *error;
	gchar *filename;

	priv = regist->priv;
	
	clutter_script_get_objects (priv->script, "app-register", &stage,
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
			"back", &back_button,
			"register", &register_button,
			NULL);

	gnome_app_entry_binding (username_entry);
	gnome_app_entry_binding (password_entry);
	gnome_app_entry_binding (firstname_entry);
	gnome_app_entry_binding (lastname_entry);
	gnome_app_entry_binding (email_entry);
	gnome_app_button_binding (back_button);
	gnome_app_button_binding (register_button);

	gnome_app_stage_remove_decorate (stage);
	gnome_app_stage_set_position (stage, GNOME_APP_POSITION_CENTER);
	
	filename = open_app_get_pixmap_uri ("login");
	gnome_app_actor_add_background (stage, filename);
	g_free (filename);

	clutter_actor_show (stage);

	g_signal_connect (register_button, "button-press-event", G_CALLBACK (on_register_button_press), regist);
	g_signal_connect (back_button, "button-press-event", G_CALLBACK (on_back_button_press), regist);
}
