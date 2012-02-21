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
#include <gmodule.h>
#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-stage.h"
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

G_DEFINE_TYPE (GnomeAppRegister, gnome_app_register, CLUTTER_TYPE_GROUP)

G_MODULE_EXPORT gboolean
on_back_button_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppStage *app_stage;

	app_stage = gnome_app_stage_get_default ();
	gnome_app_stage_load (app_stage, "GnomeAppLogin", NULL);

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

G_MODULE_EXPORT gboolean
on_register_button_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      userdata)
{
	enum data_enum {
		USERNAME = 0,
		PASSWORD,
		FIRSTNAME,
		LASTNAME,
		EMAIL,
	};
	struct actor_pattern {
		const gchar *an;
		const gchar *info;
		const gchar *pn;
		const gchar *val;
	};

	GnomeAppRegister *regist;
	GnomeAppRegisterPrivate *priv;
	ClutterActor *entry_actor, *info_actor;
	ClutterActor *register_button;
	GError *error;
	gint i;
	gboolean validation;
	struct actor_pattern data [] = { 
		{"username-entry", "username-info", "notblank", NULL},
		{"password-entry", "password-info", "password", NULL},
		{"firstname-entry", "firstname-info", "notblank", NULL},
		{"lastname-entry", "lastname-info", "notblank", NULL},
		{"email-entry", "email-info", "email", NULL},
	};

	regist = GNOME_APP_REGISTER (userdata);
	priv = regist->priv;
	clutter_script_get_objects (priv->script,
			"regist-button", &register_button,
			NULL);

	validation = TRUE;
	for (i = 0; i < G_N_ELEMENTS (data); i++) {
		clutter_script_get_objects (priv->script,
			       	data [i].an, &entry_actor,
				data [i].info, &info_actor,
				NULL);
		data [i].val = clutter_text_get_text (CLUTTER_TEXT (entry_actor));
		error = NULL;
		if (open_app_pattern_match (data [i].pn, data [i].val, &error)) {
			clutter_text_set_text (CLUTTER_TEXT (info_actor), "");
		} else {
			clutter_text_set_text (CLUTTER_TEXT (info_actor), error->message);
			g_error_free (error);
			if (validation)
				validation = FALSE;
		}
	}

	if (validation) {
		GnomeAppTask *task;

		task = gnome_app_task_new (regist, "POST", "/v1/person/add");
		gnome_app_task_set_callback (task, register_callback);
		gnome_app_task_add_params (task, 
			"login", data [USERNAME].val,
			"password", data [PASSWORD].val,
			"firstname", data [FIRSTNAME].val,
			"lastname", data [LASTNAME].val,
			"email", data [EMAIL].val,
			NULL);
		gnome_app_task_push (task);
	}
	return TRUE;
}

static void
gnome_app_register_init (GnomeAppRegister *regist)
{
	GnomeAppRegisterPrivate *priv;
	ClutterActor *main_ui;
	ClutterActor *username_entry;
	ClutterActor *password_entry;
	ClutterActor *firstname_entry;
	ClutterActor *lastname_entry;
	ClutterActor *email_entry;
	ClutterActor *back_button;
	ClutterActor *register_button;
	gchar *filename;

	regist->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (regist,
	                                                 GNOME_APP_TYPE_REGISTER,
	                                                 GnomeAppRegisterPrivate);
	priv->script = gnome_app_script_new_from_file ("app-register");
	if (!priv->script) {
		return;
	}

	clutter_script_connect_signals (priv->script, regist);

	clutter_script_get_objects (priv->script,
			"app-register", &main_ui,
			"username-entry", &username_entry,
			"password-entry", &password_entry,
			"firstname-entry", &firstname_entry,
			"lastname-entry", &lastname_entry,
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

	/* TODO: remove it , reason explained in gnome-app-login 
	filename = open_app_get_pixmap_uri ("login");
	gnome_app_actor_add_background (CLUTTER_ACTOR (regist), filename);
	g_free (filename);
	*/

	clutter_container_add_actor (CLUTTER_CONTAINER (regist), main_ui);
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

	regist = g_object_new (GNOME_APP_TYPE_REGISTER, NULL);

	return regist;
}
