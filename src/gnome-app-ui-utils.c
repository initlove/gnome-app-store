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

static void
on_gnome_app_entry_paint (ClutterActor *actor,
		gpointer      data)
{
	ClutterActorBox allocation = { 0, };
	gfloat width, height;
	gint mouse_status;
printf ("paint\n");
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

		mouse_status = g_object_get_data (actor, "mouse-status");
		switch (mouse_status) {
			case MOUSE_ENTER:
				cogl_set_source_color4ub (128, 128, 128, 64);
				cogl_path_rectangle (2, 2, width - 1, height - 1);
				cogl_path_stroke ();
				break;
		}
	}
}

static gboolean
on_gnome_app_entry_enter (ClutterActor *actor,
		ClutterEvent *event)
{
	g_object_set_data (actor, "mouse-status", MOUSE_ENTER);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

static gboolean
on_gnome_app_entry_leave (ClutterActor *actor,
		          ClutterEvent *event)
{
	g_object_set_data (actor, "mouse-status", MOUSE_LEAVE);
	clutter_actor_queue_redraw (actor);

	return FALSE;
}

void
gnome_app_entry_binding (ClutterActor *actor)
{
	g_object_set_data (actor, "mouse-status", MOUSE_NONE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_entry_enter), NULL);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_entry_leave), NULL);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_entry_paint), NULL);
}

ClutterActor *
gnome_app_entry_new ()
{
	ClutterActor *actor;

	actor = clutter_text_new ();
	clutter_actor_set_size (actor, 100, 50);
	clutter_text_set_text (actor, "test");
	clutter_actor_set_reactive (actor, TRUE);
	clutter_text_set_editable (CLUTTER_TEXT (actor), TRUE);
	clutter_text_set_selectable (CLUTTER_TEXT (actor), TRUE);
	clutter_text_set_single_line_mode (CLUTTER_TEXT (actor), TRUE);

	        
	g_object_set_data (actor, "mouse-status", MOUSE_NONE);
	g_signal_connect (actor, "enter-event", G_CALLBACK (on_gnome_app_entry_enter), NULL);
	g_signal_connect (actor, "leave-event", G_CALLBACK (on_gnome_app_entry_leave), NULL);
	g_signal_connect (actor, "paint", G_CALLBACK (on_gnome_app_entry_paint), NULL);

	return actor;
}

