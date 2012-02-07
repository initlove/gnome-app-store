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
gnome_app_ui_set_icon (ClutterActor *actor, const gchar *uri)
{
	GnomeAppTask *task;
		
	task = gnome_download_task_new (actor, uri);
	gnome_app_task_set_callback (task, set_pic_callback);
	gnome_app_task_push (task);
}

void
gnome_app_ui_stage_move (ClutterActor *stage, gint x, gint y)
{
	Window xwindow;
	Display *display;
		
	xwindow = clutter_x11_get_stage_window (CLUTTER_STAGE (stage));
	display = clutter_x11_get_default_display ();
	
	XMoveWindow (display, xwindow, x, y);
}

void
gnome_app_ui_stage_set_position (ClutterActor *stage, gint position)
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

