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

static gpointer
set_pic_callback (gpointer userdata, gpointer func_re)
{
	ClutterActor *actor;
	gchar *dest_url;

	actor = CLUTTER_ACTOR (userdata);
	dest_url = (gchar *) func_re;
	g_object_set (G_OBJECT (actor), "filename", dest_url, NULL);

	return NULL;
}

void
gnome_app_set_icon (ClutterActor *actor, const gchar *uri)
{
	if (!uri)
		return;

	GnomeAppTask *task;
		
	task = gnome_download_task_new (actor, uri);
	gnome_app_task_set_callback (task, set_pic_callback);
	gnome_app_task_push (task);
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
