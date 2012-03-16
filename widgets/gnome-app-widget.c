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
#include <clutter/clutter.h>
#include "gnome-app-widget.h"

struct _GnomeAppWidgetPrivate
{
	gboolean enter;
	ClutterActor *background;
};

/* Properties */
enum
{
	PROP_0,
	PROP_BACKGROUND,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppWidget, gnome_app_widget, CLUTTER_TYPE_GROUP)

static gboolean
gnome_app_widget_enter_event (ClutterActor *self,
		ClutterCrossingEvent *event)
{
	GnomeAppWidget *widget;
	GnomeAppWidgetPrivate *priv;

	widget = GNOME_APP_WIDGET (self);
	priv = widget->priv;
	priv->enter = TRUE;
	clutter_actor_queue_redraw (self);

	return FALSE;
}

static gboolean
gnome_app_widget_leave_event (ClutterActor *self,
		ClutterCrossingEvent *event)
{
	GnomeAppWidget *widget;
	GnomeAppWidgetPrivate *priv;

	widget = GNOME_APP_WIDGET (self);
	priv = widget->priv;
	priv->enter = FALSE;
	clutter_actor_queue_redraw (self);

	return FALSE;
}

static void
gnome_app_widget_init (GnomeAppWidget *widget)
{
	GnomeAppWidgetPrivate *priv;

	widget->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (widget,
	                                                 GNOME_APP_TYPE_WIDGET,
	                                                 GnomeAppWidgetPrivate);
	priv->enter = FALSE;
	priv->background = NULL;
}

static void
gnome_app_widget_set_property (GObject *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppWidget *widget;
	GnomeAppWidgetPrivate *priv;
	const gchar *str;

	widget = GNOME_APP_WIDGET (object);
	priv = widget->priv;
	switch (prop_id)
	{
		case PROP_BACKGROUND:
			str = g_value_get_string (value);
			if (!str)
				return;
			if (!priv->background) {
				priv->background = clutter_texture_new ();
				clutter_container_add_actor (CLUTTER_CONTAINER (widget), priv->background);
				clutter_actor_set_position (priv->background, 0, 0);
				clutter_actor_lower (priv->background, CLUTTER_ACTOR (widget));
				// TODO:
				// set the size of something... 
			}
			clutter_texture_set_from_file (CLUTTER_TEXTURE (priv->background), str, NULL);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_widget_get_property (GObject *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppWidget *widget;
	GnomeAppWidgetPrivate *priv;
	ClutterActor *actor;

	widget = GNOME_APP_WIDGET (object);
	priv = widget->priv;
	switch (prop_id)
	{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_widget_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_widget_parent_class)->dispose (object);
}

static void
gnome_app_widget_finalize (GObject *object)
{
	GnomeAppWidget *widget = GNOME_APP_WIDGET (object);
	GnomeAppWidgetPrivate *priv = widget->priv;

	G_OBJECT_CLASS (gnome_app_widget_parent_class)->finalize (object);
}

static void
gnome_app_widget_class_init (GnomeAppWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  
        object_class->set_property = gnome_app_widget_set_property;
	object_class->get_property = gnome_app_widget_get_property;
	object_class->dispose = gnome_app_widget_dispose;
	object_class->finalize = gnome_app_widget_finalize;

	actor_class->enter_event = gnome_app_widget_enter_event;
	actor_class->leave_event = gnome_app_widget_leave_event;

	g_object_class_install_property (object_class,
			PROP_BACKGROUND,
			g_param_spec_string ("background",
				"Background image",
				"Background image",
				NULL,
				G_PARAM_WRITABLE));

	g_type_class_add_private (object_class, sizeof (GnomeAppWidgetPrivate));
}

GnomeAppWidget *
gnome_app_widget_new ()
{
	GnomeAppWidget *widget;

	widget = g_object_new (GNOME_APP_TYPE_WIDGET, NULL);

	return widget;
}

gboolean
gnome_app_widget_get_entered (GnomeAppWidget *widget)
{
	GnomeAppWidgetPrivate *priv;

	priv = widget->priv;

	return priv->enter;
}
