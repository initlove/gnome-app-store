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
#include <string.h>
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-ui-utils.h"

enum {
	MOUSE_NONE,
	MOUSE_ENTER,
	MOUSE_LEAVE,
};

#define SCALE_UP_RATE  1.5

static gpointer
set_pic_callback (gpointer userdata, gpointer func_re)
{
	ClutterActor *actor;
	gchar *dest_url;

	actor = CLUTTER_ACTOR (userdata);
	dest_url = (gchar *) func_re;
	clutter_texture_set_from_file (CLUTTER_TEXTURE (actor), dest_url, NULL);

	return NULL;
}

void
gnome_app_set_icon (ClutterActor *actor, const gchar *uri)
{
	g_return_if_fail (actor && uri);

	GnomeAppTask *task;
		
	task = gnome_download_task_new (actor, uri);
	gnome_app_task_set_callback (task, set_pic_callback);
	gnome_app_task_push (task);
}

static gboolean
on_gnome_app_widget_enter (ClutterActor *self,
		ClutterEvent *event,
		gpointer userdata)
{
	ClutterActor *actor;

	actor = CLUTTER_ACTOR (userdata);
	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_ENTER);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

static gboolean
on_gnome_app_widget_leave (ClutterActor *self,
		          ClutterEvent *event,
			  gpointer userdata)
{
	ClutterActor *actor;

	actor = CLUTTER_ACTOR (userdata);
	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_LEAVE);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

static void
on_gnome_app_button_paint (ClutterActor *actor,
		gpointer      userdata)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height;
	gint mouse_status;
	gboolean selected;

	clutter_actor_get_allocation_box (actor, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	mouse_status = (gint) g_object_get_data (G_OBJECT (actor), "mouse-status");
	switch (mouse_status) {
		case MOUSE_ENTER:
			cogl_set_source_color4ub (128, 128, 128, 255);
			cogl_path_rectangle (-5, -5, width+5, height+5);
			cogl_path_stroke ();
			cogl_set_source_color4ub (255, 255, 255, 64);
			cogl_rectangle (-4, -4, width + 4, height + 4);
			break;
	}
}

void
gnome_app_actor_add_background (ClutterActor *actor, gchar *filename)
{
	ClutterActor *texture;
	ClutterActor *parent;
	GError *error;
	gfloat width, height;
	gfloat x, y;

	error = NULL;
	texture = clutter_texture_new_from_file (filename, &error);
	clutter_actor_set_opacity (texture, 128);
	if (error) {
		g_error ("Error in add background: %s\n", error->message);
		g_error_free (error);
		return;
	}
	clutter_actor_get_size (actor, &width, &height);
	clutter_actor_set_size (texture, width, height);
	if (CLUTTER_IS_CONTAINER (actor)) {
		clutter_container_add_actor (CLUTTER_CONTAINER (actor), texture);
		clutter_actor_set_position (texture, 0, 0);
	} else {
		parent = clutter_actor_get_parent (actor);
		clutter_container_add_actor (CLUTTER_CONTAINER (parent), texture);
		clutter_actor_get_position (actor, &x, &y);
		clutter_actor_set_position (texture, x, y);
	}
	/*This make it real background ...
	 * TODO: how about both par/child actor with background */
	clutter_actor_lower_bottom (texture);
}

static void
scale_state_complete (ClutterState *state, gpointer userdata)
{
	ClutterActor *actor;

	actor = CLUTTER_ACTOR (userdata);
	  
	if (clutter_actor_is_scaled (actor))
	      clutter_state_set_state (state, "not-scaled");
}

static void
on_scale_state_destroy (ClutterActor *actor,
		gpointer userdata)
{
	ClutterState *state;

	state = CLUTTER_STATE (userdata);

	g_object_unref (G_OBJECT (state));
}

static gboolean
on_scale_state_button_press (ClutterActor *self,
      		ClutterEvent *event,
		gpointer      userdata)
{
	ClutterState *state;

	state = CLUTTER_STATE (userdata);
	clutter_state_set_state (state, "scaled-up");
//TODO
	return FALSE;
}

void
gnome_app_actor_add_scale_state (ClutterActor *actor)
{
	ClutterState *state;

	g_object_set (G_OBJECT (actor),
		"scale-gravity", CLUTTER_GRAVITY_CENTER,
		NULL);
	state = clutter_state_new ();
	clutter_state_set_duration (state, NULL, NULL, 100);
      	clutter_state_set (state, NULL, "not-scaled",
		      	actor, "scale-x", CLUTTER_LINEAR, 1.0,
			actor, "scale-y", CLUTTER_LINEAR, 1.0,
			NULL);
	clutter_state_set (state, NULL, "scaled-up",
			actor, "scale-x", CLUTTER_LINEAR, SCALE_UP_RATE,
			actor, "scale-y", CLUTTER_LINEAR, SCALE_UP_RATE,
			NULL);
	clutter_state_warp_to_state (state, "not-scaled");
	g_signal_connect (state, "completed", G_CALLBACK (scale_state_complete), actor);
	g_signal_connect (actor, "button-press-event", G_CALLBACK (on_scale_state_button_press), state);
	g_signal_connect (actor, "destroy", G_CALLBACK (on_scale_state_destroy), state);
}

/*I have to do it, because implement po in script load need lots of time, for me. */
static void
gnome_app_script_preload (ClutterScript *script)
{
	GList *l;
	ClutterActor *actor;
	const gchar *raw;
	const gchar *real;

	for (l = clutter_script_list_objects (script); l; l = l->next) {
		actor = CLUTTER_ACTOR (l->data);
		if (CLUTTER_IS_TEXT (actor)) {
			raw = clutter_text_get_text (CLUTTER_TEXT (actor));
			/*
			 * Bad, all because of script did not recognize the po file.
			 * set_text will finalize the content of clutter_text first
			 * if we set_text by get_text,
			 * the pointer will be 'wide'...
			 * so does gtk_label_set_label.
			 *
			 * It is the bug of clutter and gtk, 
			 *  	altough it may not worth to fix it ..
			 */
			if (raw && raw [0]) {
				real = _(raw);
				if (raw != real)
					clutter_text_set_text (CLUTTER_TEXT (actor), real);
			}
		}
	}
}

ClutterScript *
gnome_app_script_new_from_file (const gchar *script_name)
{
	ClutterScript *script;
	GError *error;
	gchar *filename;
	gint i, len;
	/*TODO */
	const gchar *path [] = {
		"/home/dliang/gnome-app-store/pixmaps/", 
		NULL};

	filename = open_app_get_ui_uri (script_name);
	if (!filename)
		return NULL;

	script = clutter_script_new ();
	for (len = 0; path [len]; len++) {
	}
        clutter_script_add_search_paths (script, path, len);
	error = NULL;
	clutter_script_load_from_file (script, filename, &error);
	if (error) {
		g_error ("fail to load app login script %s\n", error->message);
		g_error_free (error);
		g_object_unref (script);
		script = NULL;
	}
	gnome_app_script_preload (script);
	g_free (filename);

	return script;
}
