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
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include "gnome-app-task.h"
#include "gnome-app-ui-utils.h"

enum {
	MOUSE_NONE,
	MOUSE_ENTER,
	MOUSE_LEAVE,
};

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
	GnomeAppTask *task;
		
	task = gnome_download_task_new (actor, uri);
	gnome_app_task_set_callback (task, set_pic_callback);
	gnome_app_task_push (task);
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

void
gnome_app_stage_set_position (ClutterActor *stage, gint position)
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
		case GNOME_APP_POSITION_CENTER:
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
		case GNOME_APP_POSITION_MOUSE:
			break;
		default:
			break;
	}
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
on_gnome_app_entry_paint (ClutterActor *actor,
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
		cogl_path_rectangle (1, 1, width, height);
		cogl_path_stroke ();
		cogl_set_source_color4ub (128, 128, 128, 64);
		cogl_path_rectangle (2, 2, width - 1, height - 1);
		cogl_path_stroke ();
	} else {
		cogl_set_source_color4ub (128, 128, 128, 255);
		cogl_path_rectangle (1, 1, width, height);
		cogl_path_stroke ();

		mouse_status = (gint) g_object_get_data (G_OBJECT (actor), "mouse-status");
		switch (mouse_status) {
			case MOUSE_ENTER:
				cogl_set_source_color4ub (128, 128, 128, 64);
				cogl_path_rectangle (2, 2, width - 1, height - 1);
				cogl_path_stroke ();
				break;
		}
	}
}

static void
on_gnome_app_entry_key_focus_in (ClutterActor *actor,
		gpointer user_data)
{
	set_hint_status (actor);
}

static void
on_gnome_app_entry_key_focus_out (ClutterActor *actor,
		gpointer user_data)
{
	set_hint_status (actor);
}

void
gnome_app_entry_binding (ClutterActor *actor)
{
	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_NONE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_entry_paint), NULL);
	g_signal_connect (actor, "key_focus_in", G_CALLBACK (on_gnome_app_entry_key_focus_in), NULL);
	g_signal_connect (actor, "key_focus_out", G_CALLBACK (on_gnome_app_entry_key_focus_out), NULL);
}

void
gnome_app_entry_add_hint (ClutterActor *actor, const gchar *hint)
{
	g_return_if_fail (hint);

	ClutterActor *hint_actor, *parent;

	gfloat x, y;
	gfloat width, height;
	gfloat hint_x, hint_y;
	gfloat hint_width, hint_height;
	PangoFontDescription *desc, *hint_desc;
	gint size, hint_size;

	hint_actor = clutter_text_new ();
	clutter_actor_set_opacity (hint_actor, 128);
	clutter_actor_set_reactive (hint_actor, FALSE);
	clutter_text_set_editable (CLUTTER_TEXT (hint_actor), FALSE);
	clutter_text_set_ellipsize (CLUTTER_TEXT (hint_actor), PANGO_ELLIPSIZE_END);
	clutter_text_set_selectable (CLUTTER_TEXT (hint_actor), FALSE);
	clutter_text_set_text (CLUTTER_TEXT (hint_actor), hint);

	clutter_actor_get_position (actor, &x, &y);
	clutter_actor_get_size (actor, &width, &height);
	hint_width = width * 4/5;
	hint_height = height * 4/5;
	clutter_actor_set_position (hint_actor, x + height/4, y + height/4 - 2);
	clutter_actor_set_size (hint_actor, hint_width, hint_height);

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
on_gnome_app_check_box_press (ClutterActor *self,
      		ClutterEvent *event,
		gpointer      userdata)
{
	ClutterActor *actor;
	gboolean selected;

	actor = CLUTTER_ACTOR (userdata);
	selected = (gboolean) g_object_get_data (G_OBJECT (actor), "selected");
	selected = !selected;
	g_object_set_data (G_OBJECT (actor), "selected", (gpointer) selected);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

void
gnome_app_check_box_binding (ClutterActor *actor, ClutterActor *label)
{
	g_object_set_data (G_OBJECT (actor), "mouse-status", (gpointer) MOUSE_NONE);
	g_object_set_data (G_OBJECT (actor), "selected", (gpointer) FALSE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (label, "enter-event", G_CALLBACK (on_gnome_app_widget_enter), actor);
	g_signal_connect (label, "leave-event", G_CALLBACK (on_gnome_app_widget_leave), actor);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_check_box_paint), NULL);
	g_signal_connect (actor, "button-press-event", G_CALLBACK (on_gnome_app_check_box_press), actor);
	g_signal_connect (label, "button-press-event", G_CALLBACK (on_gnome_app_check_box_press), actor);
}

gboolean
gnome_app_check_box_get_selected (ClutterActor *actor)
{
	gboolean selected;

	selected = (gboolean) g_object_get_data (G_OBJECT (actor), "selected");

	return selected;
}

