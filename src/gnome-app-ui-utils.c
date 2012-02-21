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
set_hint_status (ClutterActor *actor)
{
	ClutterActor *hint_actor;
	const gchar *text;

	hint_actor = CLUTTER_ACTOR (g_object_get_data (G_OBJECT (actor), "hint-actor"));

	if (!hint_actor)
		return;

	if (clutter_actor_has_key_focus (actor)) {
		clutter_actor_hide (hint_actor);
	} else {
		text = clutter_text_get_text (CLUTTER_TEXT (actor));
		if (text && text [0])
			clutter_actor_hide (hint_actor);
		else
			clutter_actor_show (hint_actor);
	}
}

static void
on_hint_entry_paint (ClutterActor *actor,
		gpointer      userdata)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height;
	gint mouse_status;

	clutter_actor_get_allocation_box (actor, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	if (clutter_actor_has_key_focus (actor)) {
		cogl_set_source_color4ub (128, 128, 255, 255);
		cogl_path_rectangle (-2, -2, width + 2, height + 2);
		cogl_path_stroke ();
		cogl_set_source_color4ub (128, 128, 128, 64);
		cogl_path_rectangle (-1, -1, width + 1, height + 1);
		cogl_path_stroke ();

		cogl_set_source_color4ub (255, 255, 255, 64);
		cogl_rectangle (0, 0, width, height);
	} else {
		cogl_set_source_color4ub (128, 128, 128, 255);
		cogl_path_rectangle (-2, -2, width + 2, height + 2);
		cogl_path_stroke ();

		mouse_status = (gint) g_object_get_data (G_OBJECT (actor), "mouse-status");
		switch (mouse_status) {
			case MOUSE_ENTER:
				cogl_set_source_color4ub (128, 128, 128, 64);
				cogl_path_rectangle (-1, -1, width + 1, height + 1);
				cogl_path_stroke ();
				cogl_set_source_color4ub (255, 255, 255, 64);
				cogl_rectangle (0, 0, width, height);
				break;
		}
	}
}

static void
on_hint_entry_key_focus_in (ClutterActor *actor,
		gpointer userdata)
{
	set_hint_status (actor);
}

static void
on_hint_entry_key_focus_out (ClutterActor *actor,
		gpointer userdata)
{
	set_hint_status (actor);
}

static void
on_hint_entry_text_changed (ClutterActor *actor,
		gpointer userdata)
{
	set_hint_status (actor);
}

void
gnome_app_entry_binding (ClutterActor *actor)
{
	gchar *binding;

	binding = g_object_get_data (G_OBJECT (actor), "binding");
	if (binding)
		return;
	else
		g_object_set_data (G_OBJECT (actor), "binding", (gpointer) "binding");

	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_NONE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (actor, "paint", G_CALLBACK (on_hint_entry_paint), NULL);
	g_signal_connect (actor, "key_focus_in", G_CALLBACK (on_hint_entry_key_focus_in), NULL);
	g_signal_connect (actor, "key_focus_out", G_CALLBACK (on_hint_entry_key_focus_out), NULL);
	g_signal_connect (actor, "text_changed", G_CALLBACK (on_hint_entry_text_changed), NULL);
}

void
gnome_app_entry_add_hint (ClutterActor *actor, const gchar *hint)
{
	g_return_if_fail (hint);

	ClutterActor *hint_actor, *parent;

	gfloat x, y;
	gfloat width, height;
	gfloat hint_x, hint_y;
	PangoFontDescription *desc, *hint_desc;
	gint size, hint_size;

	hint_actor = CLUTTER_ACTOR (g_object_get_data (G_OBJECT (actor), "hint-actor"));
	if (hint_actor) {
		clutter_text_set_text (CLUTTER_TEXT (hint_actor), hint);
		set_hint_status (actor);
		return;
	}
	hint_actor = clutter_text_new ();
	clutter_actor_set_opacity (hint_actor, 64);
	clutter_actor_set_reactive (hint_actor, FALSE);
	clutter_text_set_editable (CLUTTER_TEXT (hint_actor), FALSE);
	clutter_text_set_ellipsize (CLUTTER_TEXT (hint_actor), PANGO_ELLIPSIZE_END);
	clutter_text_set_selectable (CLUTTER_TEXT (hint_actor), FALSE);
	clutter_text_set_text (CLUTTER_TEXT (hint_actor), hint);

	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_get_size (actor, &width, &height);
	clutter_actor_set_position (hint_actor, x + height/4, y + height/4 - 2);
	clutter_actor_set_width (hint_actor, width - height/2);

	desc = clutter_text_get_font_description (CLUTTER_TEXT (actor));
	size = pango_font_description_get_size (desc);
	hint_size = size / 2;
	hint_desc = pango_font_description_copy (desc);
	pango_font_description_set_size (hint_desc, hint_size);
	clutter_text_set_font_description (CLUTTER_TEXT (hint_actor), hint_desc);

	parent = clutter_actor_get_parent (actor);
	clutter_container_add_actor (CLUTTER_CONTAINER (parent), hint_actor);

	pango_font_description_free (hint_desc);

	g_object_set_data (G_OBJECT (actor), "hint-actor", (gpointer) hint_actor);

	set_hint_status (actor);
}

static void
on_gnome_app_check_box_paint (ClutterActor *actor,
		gpointer      userdata)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height;
	gint mouse_status;
	gboolean selected;

	clutter_actor_get_allocation_box (actor, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	cogl_set_source_color4ub (128, 128, 128, 255);
	cogl_path_rectangle (1, 1, width, height);
	cogl_path_stroke ();

	mouse_status = (gint) g_object_get_data (G_OBJECT (actor), "mouse-status");
	switch (mouse_status) {
		case MOUSE_ENTER:
			cogl_set_source_color4ub (128, 128, 128, 64);
			cogl_path_rectangle (2, 2, width - 1, height - 1);
			cogl_path_stroke ();
			cogl_set_source_color4ub (255, 255, 255, 64);
			cogl_rectangle (2, 2, width - 2, height - 2);
			break;
	}

	selected = (gboolean) g_object_get_data (G_OBJECT (actor), "selected");
	if (selected) {
		cogl_set_source_color4ub (0, 0, 0, 255);
		cogl_path_move_to (2, height/2);
		cogl_path_line_to (width/2, height-2);
		cogl_path_line_to (width-2, 2);
		cogl_path_move_to (3, height/2);
		cogl_path_line_to (width/2, height-3);
		cogl_path_line_to (width - 3, 2);
		cogl_path_stroke ();
	}
}

static gboolean
on_gnome_app_check_box_press (ClutterActor *actor,
      		ClutterEvent *event,
		gpointer      userdata)
{
	gboolean selected;

	selected = (gboolean) g_object_get_data (G_OBJECT (actor), "selected");
	selected = !selected;
	g_object_set_data (G_OBJECT (actor), "selected", (gpointer) selected);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

void
gnome_app_check_box_binding (ClutterActor *actor)
{
	gchar *binding;

	binding = g_object_get_data (G_OBJECT (actor), "binding");
	if (binding)
		return;
	else
		g_object_set_data (G_OBJECT (actor), "binding", (gpointer) "binding");

	gnome_app_actor_add_scale_state (actor);
	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_NONE);
	g_object_set_data (G_OBJECT (actor), "selected", (gpointer) FALSE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_check_box_paint), NULL);
	g_signal_connect (actor, "button-press-event", G_CALLBACK (on_gnome_app_check_box_press), NULL);
}

static gboolean
on_gnome_app_connector_button_press (ClutterActor *self,
      		ClutterEvent *event,
		gpointer      userdata)
{
	ClutterActor *actor;
	gboolean retval;

	actor = CLUTTER_ACTOR (userdata);
	g_signal_emit_by_name (actor, "button-press-event", NULL, event, &retval);

	return retval;
}

void
gnome_app_check_box_add_connector (ClutterActor *actor, ClutterActor *connector)
{
	g_signal_connect (connector, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (connector, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (connector, "button-press-event", G_CALLBACK (on_gnome_app_connector_button_press), actor);
}

gboolean
gnome_app_check_box_get_selected (ClutterActor *actor)
{
	gboolean selected;

	selected = (gboolean) g_object_get_data (G_OBJECT (actor), "selected");

	return selected;
}


static void
on_gnome_app_button_paint (ClutterActor *actor,
		gpointer      userdata)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height;
	gint mouse_status;

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

/*TODO: the touch screen and PC is different, click to show the effect is good for touch screen
 * move in to show effect is good for PC.
 */
void
gnome_app_button_binding (ClutterActor *actor)
{
	gchar *binding;

	binding = g_object_get_data (G_OBJECT (actor), "binding");
	if (binding)
		return;
	else
		g_object_set_data (G_OBJECT (actor), "binding", (gpointer) "binding");

	gnome_app_actor_add_scale_state (actor);

	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_NONE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_button_paint), NULL);
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
