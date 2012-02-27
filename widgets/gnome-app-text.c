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
#include "gnome-app-text.h"

struct _GnomeAppTextPrivate
{
	ClutterActor *text;
	ClutterActor *hint;
};

/* Properties */
enum
{
	PROP_0,
	PROP_WIDTH,
	PROP_TEXT,
	PROP_FONT_NAME,
	PROP_COLOR,
	PROP_HINT_TEXT,
	PROP_HINT_FONT_NAME,
	PROP_HINT_COLOR,
	PROP_PASSWORD_CHAR,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppText, gnome_app_text, GNOME_APP_TYPE_WIDGET)

static void
on_text_key_focus_in (ClutterActor *self,
			GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;

	priv = text->priv;
	clutter_actor_hide (priv->hint);
}

static void
on_text_key_focus_out (ClutterActor *self,
			GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;
	const gchar *str;

	priv = text->priv;
	str = clutter_text_get_text (CLUTTER_TEXT (priv->text));
	if (str && str [0])
		clutter_actor_hide (priv->hint);
	else
		clutter_actor_show (priv->hint);
}

static void
on_text_paint (ClutterActor *self,
		GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;
	ClutterActorBox allocation = { 0, };
	gfloat width, height;

	priv = text->priv;
	clutter_actor_get_allocation_box (priv->text, &allocation);
	clutter_actor_box_clamp_to_pixel (&allocation);
	clutter_actor_box_get_size (&allocation, &width, &height);

	if (clutter_actor_has_key_focus (priv->text)) {
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

		if (gnome_app_widget_get_entered (GNOME_APP_WIDGET (text))) {
			cogl_set_source_color4ub (128, 128, 128, 64);
			cogl_path_rectangle (-1, -1, width + 1, height + 1);
			cogl_path_stroke ();
			cogl_set_source_color4ub (255, 255, 255, 64);
			cogl_rectangle (0, 0, width, height);
		}
	}
}

static void
on_text_changed (ClutterActor *actor,
		GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;
	const gchar *str;

	priv = text->priv;
	str = clutter_text_get_text (CLUTTER_TEXT (priv->text));
	if (str && str [0])
		clutter_actor_hide (priv->hint);
	else if (clutter_actor_has_key_focus (priv->text))
		clutter_actor_hide (priv->hint);
	else
		clutter_actor_show (priv->hint);
}

static void 
on_allocation_changed (ClutterActor           *self,
		const ClutterActorBox  *box,
		ClutterAllocationFlags  flags,
		GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;
	gfloat width, height;

	/*TODO: get the preferred size */
	priv = text->priv;
	width = clutter_actor_box_get_width (box);
	height = clutter_actor_box_get_height (box);
	clutter_actor_set_width (priv->text, width);
	clutter_actor_set_width (priv->hint, width);
}

static void
gnome_app_text_init (GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;

	text->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (text,
	                                                 GNOME_APP_TYPE_TEXT,
	                                                 GnomeAppTextPrivate);


	priv->hint = clutter_text_new ();
	clutter_actor_set_opacity (priv->hint, 128);
	clutter_actor_set_reactive (priv->hint, FALSE);
	clutter_text_set_single_line_mode (CLUTTER_TEXT (priv->hint), TRUE);
        clutter_text_set_editable (CLUTTER_TEXT (priv->hint), FALSE);
	clutter_text_set_ellipsize (CLUTTER_TEXT (priv->hint), PANGO_ELLIPSIZE_END);
	clutter_text_set_selectable (CLUTTER_TEXT (priv->hint), FALSE);
	clutter_container_add_actor (CLUTTER_CONTAINER (text), priv->hint);
	/*TODO: should be calculated in alloc or other */
	clutter_actor_set_position (priv->hint, 5, 2);

	priv->text = clutter_text_new ();
	clutter_actor_set_reactive (priv->text, TRUE);
	clutter_text_set_line_wrap (CLUTTER_TEXT (priv->text), TRUE);
	clutter_text_set_line_wrap_mode (CLUTTER_TEXT (priv->text), PANGO_WRAP_WORD);
        clutter_text_set_editable (CLUTTER_TEXT (priv->text), TRUE);
	clutter_text_set_selectable (CLUTTER_TEXT (priv->text), TRUE);
	clutter_container_add_actor (CLUTTER_CONTAINER (text), priv->text);
	        
	g_signal_connect (priv->text, "text_changed", G_CALLBACK (on_text_changed), text);
	g_signal_connect (priv->text, "key_focus_in", G_CALLBACK (on_text_key_focus_in), text);
	g_signal_connect (priv->text, "key_focus_out", G_CALLBACK (on_text_key_focus_out), text);
	g_signal_connect (priv->text, "paint", G_CALLBACK (on_text_paint), text);
return;
	g_signal_connect (CLUTTER_ACTOR (text), "allocation-changed",
		                  G_CALLBACK (on_allocation_changed),
		                    text);
}

static void
gnome_app_text_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppText *text;
	GnomeAppTextPrivate *priv;
	ClutterColor color;
	const gchar *str;
	gfloat width;

	text = GNOME_APP_TEXT (object);
	priv = text->priv;
	switch (prop_id)
	{
		case PROP_WIDTH:
			width = g_value_get_float (value);
			clutter_actor_set_width (CLUTTER_ACTOR (text), width);
			clutter_actor_set_width (CLUTTER_ACTOR (priv->text), width);
			clutter_actor_set_width (CLUTTER_ACTOR (priv->hint), width);
			break;
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
		case PROP_HINT_TEXT:
			str = g_value_get_string (value);
			clutter_text_set_text (CLUTTER_TEXT (priv->hint), _(str));
			break;
		case PROP_HINT_FONT_NAME:
			clutter_text_set_font_name (CLUTTER_TEXT (priv->hint), g_value_get_string (value));
			break;
		case PROP_HINT_COLOR:
			str = g_value_get_string (value);
			if (str) {
				clutter_color_from_string (&color, str);
				clutter_text_set_color (CLUTTER_TEXT (priv->hint), &color);
			}
			break;
		case PROP_PASSWORD_CHAR:
			clutter_text_set_password_char (CLUTTER_TEXT (priv->text), g_value_get_uint (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_text_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppText *text;
	GnomeAppTextPrivate *priv;
	ClutterColor color;
	gchar *str;
	gunichar password_char;

	text = GNOME_APP_TEXT (object);
	priv = text->priv;
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
		case PROP_PASSWORD_CHAR:
			password_char = clutter_text_get_password_char (CLUTTER_TEXT (priv->text));
		        g_value_set_uint (value, password_char);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_text_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_text_parent_class)->dispose (object);
}

static void
gnome_app_text_finalize (GObject *object)
{
	GnomeAppText *text = GNOME_APP_TEXT (object);
	GnomeAppTextPrivate *priv = text->priv;

	G_OBJECT_CLASS (gnome_app_text_parent_class)->finalize (object);
}

static void
gnome_app_text_class_init (GnomeAppTextClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  
        object_class->set_property = gnome_app_text_set_property;
	object_class->get_property = gnome_app_text_get_property;
	object_class->dispose = gnome_app_text_dispose;
	object_class->finalize = gnome_app_text_finalize;

	g_object_class_install_property (object_class,
			PROP_WIDTH,
			g_param_spec_float ("width",
				"width of the text",
				"width of the text",
				-G_MAXFLOAT, G_MAXFLOAT,
				0.0,
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
			PROP_HINT_TEXT,
			g_param_spec_string ("hint",
				"hint text",
				"hint text",
				NULL,
				G_PARAM_WRITABLE));

	g_object_class_install_property (object_class,
			PROP_HINT_FONT_NAME,
			g_param_spec_string ("hint-font-name",
				"Hint Font name",
				"Name of the font used by the hint text",
				NULL,
				G_PARAM_WRITABLE));

	g_object_class_install_property (object_class,
			PROP_HINT_COLOR,
			g_param_spec_string ("hint-color",
				"Hint Font Color",
				"Color of the font used by the hint",
				NULL,
				G_PARAM_WRITABLE));

	g_object_class_install_property (object_class,
			PROP_PASSWORD_CHAR,
			g_param_spec_unichar ("password-char",
				"password char",
				"password char",
				0,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppTextPrivate));
}

GnomeAppText *
gnome_app_text_new ()
{
	GnomeAppText *text;

	text = g_object_new (GNOME_APP_TYPE_TEXT, NULL);

	return text;
}

const gchar *
gnome_app_text_get_text (GnomeAppText *text)
{
	GnomeAppTextPrivate *priv;

	priv = text->priv;
	return clutter_text_get_text (CLUTTER_TEXT (priv->text));
}

void
gnome_app_text_set_text (GnomeAppText *text, gchar *str)
{
	GnomeAppTextPrivate *priv;

	priv = text->priv;
	clutter_text_set_text (CLUTTER_TEXT (priv->text), str);
}

