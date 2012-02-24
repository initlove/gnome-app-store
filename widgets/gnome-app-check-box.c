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
#include "gnome-app-widget.h"
#include "gnome-app-check-box.h"

#define SCALE_UP_RATE 1.5
struct _GnomeAppCheckBoxPrivate
{
	ClutterActor *box;
	ClutterActor *text;
        ClutterState *state;

	gboolean selected;
};

/* Properties */
enum
{
	PROP_0,
	PROP_TEXT,
	PROP_FONT_NAME,
	PROP_COLOR,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppCheckBox, gnome_app_check_box, GNOME_APP_TYPE_WIDGET)


static void
on_check_box_paint (ClutterActor *self,
		GnomeAppCheckBox *check_box)
{
	GnomeAppCheckBoxPrivate *priv;
	ClutterActorBox allocation = { 0, };
	gfloat width, height;

	priv = check_box->priv;
	clutter_actor_get_allocation_box (priv->box, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	cogl_set_source_color4ub (128, 128, 128, 255);
	cogl_path_rectangle (1, 1, width, height);
	cogl_path_stroke ();

	if (gnome_app_widget_get_entered (GNOME_APP_WIDGET (check_box))) {
		cogl_set_source_color4ub (128, 128, 128, 64);
		cogl_path_rectangle (2, 2, width - 1, height - 1);
		cogl_path_stroke ();
		cogl_set_source_color4ub (255, 255, 255, 64);
		cogl_rectangle (2, 2, width - 2, height - 2);
	}
	if (priv->selected) {
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
gnome_app_check_box_button_press (ClutterActor         *actor,
		ClutterButtonEvent   *event)
{
	GnomeAppCheckBox *check_box;
	GnomeAppCheckBoxPrivate *priv;

	check_box = GNOME_APP_CHECK_BOX (actor);
	priv = check_box->priv;
	
	gnome_app_check_box_set_selected (check_box, !priv->selected);
	clutter_state_set_state (priv->state, "scaled-up");

	return FALSE;
}

static void
scale_state_complete (ClutterState *state, GnomeAppCheckBox *check_box)
{
	GnomeAppCheckBoxPrivate *priv;
	
	priv = check_box->priv;

	if (clutter_actor_is_scaled (priv->box))
		clutter_state_set_state (state, "not-scaled");
}

static void
gnome_app_check_box_init (GnomeAppCheckBox *check_box)
{
	GnomeAppCheckBoxPrivate *priv;

	check_box->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (check_box,
	                                                 GNOME_APP_TYPE_CHECK_BOX,
	                                                 GnomeAppCheckBoxPrivate);

	priv->box = CLUTTER_ACTOR (gnome_app_widget_new ());
	clutter_actor_set_height (priv->box, 16);
	clutter_actor_set_width (priv->box, 16);

	g_signal_connect (priv->box, "paint", G_CALLBACK (on_check_box_paint), check_box); 
	priv->text = clutter_text_new ();
	clutter_text_set_single_line_mode (CLUTTER_TEXT (priv->text), TRUE);
        clutter_text_set_editable (CLUTTER_TEXT (priv->text), FALSE);
	clutter_text_set_selectable (CLUTTER_TEXT (priv->text), FALSE);

	clutter_container_add_actor (CLUTTER_CONTAINER (check_box), CLUTTER_ACTOR (priv->box));
	clutter_container_add_actor (CLUTTER_CONTAINER (check_box), priv->text);
	clutter_actor_set_position (priv->text, 20, 0);

	g_object_set (G_OBJECT (priv->box), "scale-gravity", CLUTTER_GRAVITY_CENTER, NULL);
	priv->state = clutter_state_new ();
	clutter_state_set_duration (priv->state, NULL, NULL, 100);
	clutter_state_set (priv->state, NULL, "not-scaled",
			priv->box, "scale-x", CLUTTER_LINEAR, 1.0,
			priv->box, "scale-y", CLUTTER_LINEAR, 1.0,
			NULL);
	clutter_state_set (priv->state, NULL, "scaled-up",
			priv->box, "scale-x", CLUTTER_LINEAR, SCALE_UP_RATE,
			priv->box, "scale-y", CLUTTER_LINEAR, SCALE_UP_RATE,
			NULL);
	clutter_state_warp_to_state (priv->state, "not-scaled");
	        
	g_signal_connect (priv->state, "completed", G_CALLBACK (scale_state_complete), check_box);
}

static void
gnome_app_check_box_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppCheckBox *check_box;
	GnomeAppCheckBoxPrivate *priv;
	ClutterColor color;
	const gchar *str;

	check_box = GNOME_APP_CHECK_BOX (object);
	priv = check_box->priv;
	switch (prop_id)
	{
		case PROP_TEXT:
			str = g_value_get_string (value);
			clutter_text_set_text (CLUTTER_TEXT (priv->text), _(str));
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
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_check_box_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppCheckBox *check_box;
	GnomeAppCheckBoxPrivate *priv;
	ClutterColor color;
	gchar *str;
	gunichar password_char;

	check_box = GNOME_APP_CHECK_BOX (object);
	priv = check_box->priv;
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
gnome_app_check_box_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_check_box_parent_class)->dispose (object);
}

static void
gnome_app_check_box_finalize (GObject *object)
{
	GnomeAppCheckBox *check_box = GNOME_APP_CHECK_BOX (object);
	GnomeAppCheckBoxPrivate *priv = check_box->priv;
	        
	g_object_unref (G_OBJECT (priv->state));

	G_OBJECT_CLASS (gnome_app_check_box_parent_class)->finalize (object);
}

static void
gnome_app_check_box_class_init (GnomeAppCheckBoxClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  
        object_class->set_property = gnome_app_check_box_set_property;
	object_class->get_property = gnome_app_check_box_get_property;
	object_class->dispose = gnome_app_check_box_dispose;
	object_class->finalize = gnome_app_check_box_finalize;

	actor_class->button_press_event = gnome_app_check_box_button_press;

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

	g_type_class_add_private (object_class, sizeof (GnomeAppCheckBoxPrivate));
}

GnomeAppCheckBox *
gnome_app_check_box_new ()
{
	GnomeAppCheckBox *check_box;

	check_box = g_object_new (GNOME_APP_TYPE_CHECK_BOX, NULL);

	return check_box;
}

gboolean
gnome_app_check_box_get_selected (GnomeAppCheckBox *check_box)
{
	GnomeAppCheckBoxPrivate *priv;

	priv = check_box->priv;

	return priv->selected;
}

void
gnome_app_check_box_set_selected (GnomeAppCheckBox *check_box, gboolean selected)
{
	GnomeAppCheckBoxPrivate *priv;

	priv = check_box->priv;

	if (priv->selected != selected) {
		priv->selected = selected;
		clutter_actor_queue_redraw (CLUTTER_ACTOR (check_box));
	}
}

