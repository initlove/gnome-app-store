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
#include <clutter/x11/clutter-x11.h>
#include <X11/Xatom.h>

#include "open-app-utils.h"
#include "gnome-app-ui-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-stage.h"
#include "gnome-app-info-page.h"
#include "gnome-app-login.h"
#include "gnome-app-register.h"
#include "gnome-app-frame.h"

struct _GnomeAppStagePrivate
{
	GnomeAppStore *store;
	GHashTable *table;
	ClutterActor *background;
	//TODO: learn from nautilus or firefox, make history a list if necessary
	ClutterActor *history;

	gfloat default_width;
	gfloat default_height;
};

enum {
	PROP_0,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppStage, gnome_app_stage, CLUTTER_TYPE_STAGE)

static void	set_position_on_show (ClutterActor *stage, gint position);

static gpointer
auth_valid_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppStage *app_stage;
	GnomeAppStagePrivate *priv;
	OpenResults *results;

	results = OPEN_RESULTS (func_result);
	app_stage = GNOME_APP_STAGE (userdata);
	priv = app_stage->priv;
	if (results && open_results_get_status (results)) {
		gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_NEW, "GnomeAppFrame", NULL);
	} else {
		gchar *username;
		gchar *password;

        	g_object_get (G_OBJECT (priv->store), 
			"username", &username, 
			"password", &password, 
			NULL);
		gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_DEFAULT,
				"GnomeAppLogin",
				"username", &username,
				"password", &password,
				NULL);
	}
}

static void
auth_valid (GnomeAppStage *app_stage)
{
	GnomeAppStagePrivate *priv;
	GnomeAppTask *task;
        gchar *username;
        gchar *password;

	priv = app_stage->priv;
        g_object_get (G_OBJECT (priv->store), 
			"username", &username, 
			"password", &password, 
			NULL);
        if (username && password) {
		task = gnome_app_task_new (app_stage, "POST", "/v1/person/check");
		gnome_app_task_set_callback (task, auth_valid_callback);
		gnome_app_task_add_params (task,
				"login", username,
				"password", password,
				NULL);
		gnome_app_task_push (task);
        } else {
                gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_DEFAULT,
				"GnomeAppLogin",
				"username", username,
				"password", password, 
				NULL);
	}
}

static void
app_actor_hide (gpointer key,
		GObject *actor,
		GObject *app_actor)
{
/*TODO To make frame ui faster, we hide it donnot destroy it */
	if (actor != app_actor)
		clutter_actor_hide (CLUTTER_ACTOR (actor));
}

static void
gnome_app_stage_show (GnomeAppStage *app_stage, gint mode, GObject *app_actor)
{
	GnomeAppStagePrivate *priv;
	gfloat width, height;
	gfloat stage_width, stage_height;
	gchar *filename;

	priv = app_stage->priv;
	g_hash_table_foreach (priv->table, (GHFunc) app_actor_hide, app_actor);
		
	clutter_actor_get_size (CLUTTER_ACTOR (app_actor), &width, &height);
	//TODO: this should improve
	if ((width < priv->default_width/2) && (height < priv->default_height/2)) {
		stage_width = width;
       		stage_height = height;
	} else {
		stage_width = priv->default_width;
		stage_height = priv->default_height;
	}
	clutter_actor_set_size (CLUTTER_ACTOR (app_stage), stage_width, stage_height);
	set_position_on_show (CLUTTER_ACTOR (app_stage), GNOME_APP_STAGE_POSITION_CENTER);

	priv->history = CLUTTER_ACTOR (app_actor);
	clutter_actor_show (CLUTTER_ACTOR (app_actor));
#if 0
	/*TODO: should get from theme, for example */
	if (GNOME_APP_IS_LOGIN (app_actor)) {
		filename = open_app_get_pixmap_uri ("login");
	} else if (GNOME_APP_IS_REGISTER (app_actor)) {
		filename = open_app_get_pixmap_uri ("login");
	} else {
		filename = NULL;
	}
	if (filename) {
		//TODO: need good background to reopen it
		clutter_actor_set_size (CLUTTER_ACTOR (priv->background), stage_width, stage_height);
		clutter_actor_show (CLUTTER_ACTOR (priv->background));
		clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->background), filename, NULL);
		g_free (filename);
	} else {
		clutter_actor_hide (CLUTTER_ACTOR (priv->background));
	}
#endif
}

void
gnome_app_stage_load (GnomeAppStage *app_stage, gint mode, const gchar *type_name, ...)
{
	GnomeAppStagePrivate *priv;
	GObject *app_actor;
	va_list args;
	const gchar *first_name;

	priv = app_stage->priv;
	app_actor = g_hash_table_lookup (priv->table, type_name);
	if (app_actor) {
		if (mode == GNOME_APP_STAGE_LOAD_NEW) {
			clutter_container_remove_actor (CLUTTER_CONTAINER (app_stage), CLUTTER_ACTOR (app_actor));
			g_hash_table_remove (priv->table, type_name);
			app_actor = g_object_new (g_type_from_name (type_name), NULL);
			clutter_container_add_actor (CLUTTER_CONTAINER (app_stage), CLUTTER_ACTOR (app_actor));
			g_hash_table_insert (priv->table, g_strdup (type_name), app_actor);
		}
	} else {
		app_actor = g_object_new (g_type_from_name (type_name), NULL);
		clutter_container_add_actor (CLUTTER_CONTAINER (app_stage), CLUTTER_ACTOR (app_actor));
		g_hash_table_insert (priv->table, g_strdup (type_name), app_actor);
	}

	va_start (args, type_name);
	first_name = va_arg (args, const gchar *);
	if (first_name) {
		g_object_set_valist (app_actor, first_name, args);
	}
	va_end (args);

	gnome_app_stage_show (app_stage, mode, app_actor);
}

static void
gnome_app_stage_init (GnomeAppStage *app_stage)
{
	GnomeAppStagePrivate *priv;

	app_stage->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (app_stage,
							 GNOME_APP_TYPE_STAGE,
							 GnomeAppStagePrivate);

	priv->table = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
	priv->store = gnome_app_store_get_default ();
	priv->history = NULL;
	priv->default_width = 1000;
	priv->default_height = 700;
#if 0
        gnome_app_stage_remove_decorate (CLUTTER_ACTOR (app_stage));
#endif
	clutter_stage_set_title (CLUTTER_STAGE (app_stage), "App Store");
	gnome_app_stage_set_position (CLUTTER_ACTOR (app_stage), GNOME_APP_STAGE_POSITION_CENTER);
	clutter_actor_set_reactive (CLUTTER_ACTOR (app_stage), TRUE);

	priv->background = clutter_texture_new ();
	clutter_actor_set_opacity (priv->background, 128);
	clutter_actor_set_position (priv->background, 0, 0);
	clutter_container_add_actor (CLUTTER_CONTAINER (app_stage), CLUTTER_ACTOR (priv->background));
        g_signal_connect (app_stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);
#if 0
	//DEBUG
	gnome_app_stage_load (app_stage, GNOME_APP_STAGE_LOAD_DEFAULT, "GnomeAppFrame", NULL);
#else
	auth_valid (app_stage);
#endif
}

static void
gnome_app_stage_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_stage_parent_class)->dispose (object);
}

static void
gnome_app_stage_finalize (GObject *object)
{
	GnomeAppStage *app = GNOME_APP_STAGE (object);
	GnomeAppStagePrivate *priv = app->priv;

	if (priv->table)                
		g_hash_table_destroy (priv->table);

	G_OBJECT_CLASS (gnome_app_stage_parent_class)->finalize (object);
}

static void
gnome_app_stage_get_property (GObject    *object,
		guint       prop_id,
		GValue     *value,
		GParamSpec *pspec)
{
	GnomeAppStage *self;

		
	self = GNOME_APP_STAGE (object);

	switch (prop_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_stage_class_init (GnomeAppStageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_stage_dispose;
	object_class->finalize = gnome_app_stage_finalize;
	
	object_class->get_property = gnome_app_stage_get_property;

	g_type_class_add_private (object_class, sizeof (GnomeAppStagePrivate));
}

GnomeAppStage *
gnome_app_stage_new ()
{
        return g_object_new (GNOME_APP_TYPE_STAGE, NULL);
}

GnomeAppStage *
gnome_app_stage_get_default ()
{
	static GnomeAppStage *app = NULL;

	if (!app)
		app = g_object_new (GNOME_APP_TYPE_STAGE, NULL);

	return app;
}

typedef struct {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long input_mode;
        unsigned long status;
} MotifWmHints, MwmHints;

#define MWM_HINTS_DECORATIONS   (1L << 1)

static void
remove_decorate_on_show (ClutterActor *stage,
		gpointer userdata)
{
	Window window;
	Display *display;
	MotifWmHints *old_hints;
	MotifWmHints hints;
	Atom hints_atom = None;
	Atom type;
	int format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *data;

	window = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));
	display = clutter_x11_get_default_display ();
	/* initialize to zero to avoid writing uninitialized data to socket */
	memset(&hints, 0, sizeof(hints));
	hints.flags = MWM_HINTS_DECORATIONS;
	hints.decorations = MWM_HINTS_DECORATIONS;
	hints_atom = XInternAtom (display, "_MOTIF_WM_HINTS", TRUE);
	XGetWindowProperty (display, window,
	                    hints_atom, 0, sizeof (MotifWmHints)/sizeof (long),
		            False, AnyPropertyType, &type, &format, &nitems,
		            &bytes_after, &data);
	if (type == None) {
	     	old_hints = &hints;
	} else {
	    	old_hints = (MotifWmHints *)data;
		old_hints->flags |= MWM_HINTS_DECORATIONS;
		old_hints->decorations = hints.decorations;
	}
	XChangeProperty (display, window,
		hints_atom, hints_atom, 32, PropModeReplace,
		(unsigned char *)old_hints, sizeof (MotifWmHints)/sizeof (long));

	if (old_hints != &hints)
    		  XFree (old_hints);

	g_signal_handlers_disconnect_by_func (stage,
			remove_decorate_on_show, 
			NULL);
}

/* remove decorate or set position should be used after the actor was show
 * but if we
 * 		clutter_actor_show -> remove_decorate_on_show 
 * 			->set_position_on_show
 * 	the stage will move (the height of title bar) very obviously
 *
 * so, I add the connect here, now
 * 		remove -> set_position -> actor_show
 *
 * TODO: maybe a bug or may have better way, it is very tricky currently.
 * Still we have problem in the first gui ...
 */
void
gnome_app_stage_remove_decorate (ClutterActor *stage)
{
	g_signal_connect (stage, "show", G_CALLBACK (remove_decorate_on_show), NULL);
}

void
gnome_app_stage_move (ClutterActor *stage, gint x, gint y)
{
	Window xwindow;
	Display *display;
		
	xwindow = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));
	display = clutter_x11_get_default_display ();
	
	XMoveWindow (display, xwindow, x, y);
}

static void
set_position_on_show (ClutterActor *stage, gint position)
{
	Window xwindow;
	Display *display;
	gint screen;
	gint width, height;
	gfloat stage_width, stage_height;
	gint x, y;

	xwindow = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));
	display = clutter_x11_get_default_display ();
	screen = clutter_x11_get_default_screen ();
	switch (position) {
		case GNOME_APP_STAGE_POSITION_CENTER:
			width = XDisplayWidth (display, screen);
			height = XDisplayHeight (display, screen);
			clutter_actor_get_size (stage, &stage_width, &stage_height);
			x = (width - stage_width) / 2;
			y = (height - stage_height) / 2;
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			XMoveWindow (display, xwindow, x, y);
			break;
		case GNOME_APP_STAGE_POSITION_MOUSE:
			break;
		default:
			break;
	}
}

void
gnome_app_stage_set_position (ClutterActor *stage, gint position)
{
	g_signal_connect (stage, "show", G_CALLBACK (set_position_on_show), (gpointer) position);
}

void
gnome_app_stage_close (GnomeAppStage *app_stage)
{
	g_object_unref (app_stage);
}

G_MODULE_EXPORT void
gnome_app_default_stage_close (void)
{
	GnomeAppStage *app_stage;

	app_stage = gnome_app_stage_get_default ();
	gnome_app_stage_close (app_stage);
}

