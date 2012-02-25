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
#include <cogl/cogl.h>
#include <cogl-pango/cogl-pango.h>
#include <clutter/clutter.h>
#include "open-app-utils.h"
#include "gnome-app-widget.h"
#include "gnome-app-button.h"

#define SCALE_UP_RATE 1.5

enum {
	BUTTON_NORMAL,
	BUTTON_SELECT,
	BUTTON_NONE,
	BUTTON_TEXT,
	BUTTON_TEXTURE,
};

struct _GnomeAppButtonPrivate
{
	ClutterActor *texture;
	ClutterActor *text;

        ClutterState *state;

	gint type;
	gint mode;
	gboolean selected;
};

/* Properties */
enum
{
	PROP_0,
	PROP_BUTTON_MODE,
	PROP_TEXT,
	PROP_FONT_NAME,
	PROP_COLOR,
	PROP_FILENAME,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppButton, gnome_app_button, GNOME_APP_TYPE_WIDGET)


static void
gnome_app_button_paint (ClutterActor *self)
{
	GnomeAppButton *button;
	GnomeAppButtonPrivate *priv;
	ClutterActorBox allocation = { 0, };
	gfloat width, height;

	button = GNOME_APP_BUTTON (self);
	priv = button->priv;

	if (clutter_actor_is_scaled (CLUTTER_ACTOR (button))) {
		CLUTTER_ACTOR_CLASS (gnome_app_button_parent_class)->paint (self);
		return;
	}

	clutter_actor_get_allocation_box (CLUTTER_ACTOR (button), &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);
/*TODO: the draw in texture did not work well... */
	/*TODO: the button select is not done */
	if (priv->mode == BUTTON_NORMAL) {
		if (gnome_app_widget_get_entered (GNOME_APP_WIDGET (button))) {
			if (priv->type != BUTTON_TEXTURE) {
				cogl_set_source_color4ub (128, 128, 128, 255);
				cogl_path_rectangle (-5.0, -5.0, width+5.0, height+5.0);
				cogl_path_stroke ();
			}
			cogl_set_source_color4ub (180, 180, 180, 64);
			cogl_rectangle (-4.0, -4.0, width + 4.0, height + 4.0);
		}
	} else if (priv->mode == BUTTON_SELECT) {
		if (gnome_app_widget_get_entered (GNOME_APP_WIDGET (button))) {
			cogl_set_source_color4ub (180, 180, 180, 64);
			cogl_rectangle (-4.0, -4.0, width + 4.0, height + 4.0);
		} else if (priv->selected) {
			cogl_set_source_color4ub (128, 128, 180, 64);
			cogl_rectangle (-4.0, -4.0, width + 4.0, height + 4.0);
		}
	}
	
      	/* this will take care of painting every child */
	CLUTTER_ACTOR_CLASS (gnome_app_button_parent_class)->paint (self);
}

static gboolean
gnome_app_button_button_press (ClutterActor         *actor,
		ClutterButtonEvent   *event)
{
	GnomeAppButton *button;
	GnomeAppButtonPrivate *priv;

	button = GNOME_APP_BUTTON (actor);
	priv = button->priv;
	
	if (priv->mode == BUTTON_SELECT)
		gnome_app_button_set_selected (button, TRUE);
	clutter_state_set_state (priv->state, "scaled-up");

	return FALSE;
}

static void
scale_state_complete (ClutterState *state, GnomeAppButton *button)
{
	if (clutter_actor_is_scaled (CLUTTER_ACTOR (button)))
		clutter_state_set_state (state, "not-scaled");
}

static void
gnome_app_button_init (GnomeAppButton *button)
{
	GnomeAppButtonPrivate *priv;

	button->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (button,
	                                                 GNOME_APP_TYPE_BUTTON,
	                                                 GnomeAppButtonPrivate);

	priv->type = BUTTON_NONE;
	priv->mode = BUTTON_NORMAL;
	clutter_actor_set_reactive (CLUTTER_ACTOR (button), TRUE);
	priv->text = clutter_text_new ();
	clutter_text_set_single_line_mode (CLUTTER_TEXT (priv->text), TRUE);
        clutter_text_set_editable (CLUTTER_TEXT (priv->text), FALSE);
	clutter_text_set_selectable (CLUTTER_TEXT (priv->text), FALSE);
	clutter_container_add_actor (CLUTTER_CONTAINER (button), CLUTTER_ACTOR (priv->text));

	priv->texture = clutter_texture_new ();
	clutter_container_add_actor (CLUTTER_CONTAINER (button), priv->texture);

	g_object_set (G_OBJECT (button), "scale-gravity", CLUTTER_GRAVITY_CENTER, NULL);
	priv->state = clutter_state_new ();
	clutter_state_set_duration (priv->state, NULL, NULL, 100);
	clutter_state_set (priv->state, NULL, "not-scaled",
			button, "scale-x", CLUTTER_LINEAR, 1.0,
			button, "scale-y", CLUTTER_LINEAR, 1.0,
			NULL);
	clutter_state_set (priv->state, NULL, "scaled-up",
			button, "scale-x", CLUTTER_LINEAR, SCALE_UP_RATE,
			button, "scale-y", CLUTTER_LINEAR, SCALE_UP_RATE,
			NULL);
	clutter_state_warp_to_state (priv->state, "not-scaled");
	        
	g_signal_connect (priv->state, "completed", G_CALLBACK (scale_state_complete), button);
}

static void
gnome_app_button_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppButton *button;
	GnomeAppButtonPrivate *priv;
	ClutterColor color;
	const gchar *str;
	gchar *filename;
	gfloat width, height;

	button = GNOME_APP_BUTTON (object);
	priv = button->priv;
	switch (prop_id)
	{
		case PROP_BUTTON_MODE:
			str = g_value_get_string (value);
			if (strcmp (str, "select") == 0)
				priv->mode = BUTTON_SELECT;
			else
				priv->mode = BUTTON_NORMAL;
			break;
		case PROP_TEXT:
			priv->type = BUTTON_TEXT;
			str = g_value_get_string (value);
			clutter_text_set_text (CLUTTER_TEXT (priv->text), _(str));
			/*make the texture a background */
			/*TODO: or useless as gnome-app-widget already have the background */
			clutter_actor_set_opacity (priv->texture, 128);
			break;
		case PROP_FONT_NAME:
			clutter_text_set_font_name (CLUTTER_TEXT (priv->text), g_value_get_string (value));
			break;
		case PROP_COLOR:
			str = g_value_get_string (value);
			if (str) {
				clutter_color_from_string (&color, str);
				clutter_text_set_color (CLUTTER_TEXT (priv->text), &color);
			}
			break;
		case PROP_FILENAME:
			priv->type = BUTTON_TEXTURE;
			str = g_value_get_string (value);
			filename = open_app_get_pixmap_uri (str);

			if (filename) {
				clutter_actor_get_size (CLUTTER_ACTOR (button), &width, &height);
				clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->texture), filename, NULL);
				clutter_actor_set_size (priv->texture, width, height);
				g_free (filename);
			}
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_button_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppButton *button;
	GnomeAppButtonPrivate *priv;
	ClutterColor color;
	gchar *str;
	gunichar password_char;

	button = GNOME_APP_BUTTON (object);
	priv = button->priv;
	switch (prop_id)
	{
		case PROP_TEXT:
			g_value_set_string (value, clutter_text_get_text (CLUTTER_TEXT (priv->text)));
			break;
		case PROP_FONT_NAME:
			g_value_set_string (value, clutter_text_get_font_name (CLUTTER_TEXT (priv->text)));
			break;
		case PROP_COLOR:
			clutter_text_get_color (CLUTTER_TEXT (priv->text), &color);
			str = clutter_color_to_string (&color);
			g_value_set_string (value, str);
			g_free (str);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_button_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_button_parent_class)->dispose (object);
}

static void
gnome_app_button_finalize (GObject *object)
{
	GnomeAppButton *button = GNOME_APP_BUTTON (object);
	GnomeAppButtonPrivate *priv = button->priv;
	        
	g_object_unref (G_OBJECT (priv->state));

	G_OBJECT_CLASS (gnome_app_button_parent_class)->finalize (object);
}

static void
gnome_app_button_class_init (GnomeAppButtonClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  
        object_class->set_property = gnome_app_button_set_property;
	object_class->get_property = gnome_app_button_get_property;
	object_class->dispose = gnome_app_button_dispose;
	object_class->finalize = gnome_app_button_finalize;

	actor_class->button_press_event = gnome_app_button_button_press;
	actor_class->paint = gnome_app_button_paint;

	g_object_class_install_property (object_class,
			PROP_BUTTON_MODE,
			g_param_spec_string ("button-mode",
				"the mode of the button",
				"the mode of the button",
				"normal",
				G_PARAM_WRITABLE));

	g_object_class_install_property (object_class,
			PROP_TEXT,
			g_param_spec_string ("text",
				"text",
				"text",
				NULL,
				G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
			PROP_FONT_NAME,
			g_param_spec_string ("text-font-name",
				"Text Font name",
				"Name of the font used by the text",
				NULL,
				G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
			PROP_COLOR,
			g_param_spec_string ("text-color",
				"Text Font Color",
				"Color of the font used by the text",
				NULL,
				G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
			PROP_FILENAME,
			g_param_spec_string ("filename",
				"filename of the image",
				"filename of the image",
				NULL,
				G_PARAM_WRITABLE));

	g_type_class_add_private (object_class, sizeof (GnomeAppButtonPrivate));
}

GnomeAppButton *
gnome_app_button_new ()
{
	GnomeAppButton *button;

	button = g_object_new (GNOME_APP_TYPE_BUTTON, NULL);

	return button;
}

gboolean
gnome_app_button_get_selected (GnomeAppButton *button)
{
	GnomeAppButtonPrivate *priv;

	priv = button->priv;

	return priv->selected;
}

void
gnome_app_button_set_selected (GnomeAppButton *button, gboolean selected)
{
	GnomeAppButtonPrivate *priv;

	priv = button->priv;

	if (priv->mode != BUTTON_SELECT)
		priv->mode = BUTTON_SELECT;

	if (priv->selected != selected) {
		priv->selected = selected;
		clutter_actor_queue_redraw (CLUTTER_ACTOR (button));
	}
}

void
gnome_app_button_set_text (GnomeAppButton *button, gchar *text)
{
	GnomeAppButtonPrivate *priv;

	priv = button->priv;
	clutter_text_set_text (CLUTTER_TEXT (priv->text), text);
}
