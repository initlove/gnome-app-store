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
#include <stdio.h>
#include <string.h>
#include "gnome-app-query.h"

struct _GnomeAppQueryPrivate
{
	GHashTable *query_request;
};

G_DEFINE_TYPE (GnomeAppQuery, gnome_app_query, G_TYPE_OBJECT)

static void
gnome_app_query_init (GnomeAppQuery *query)
{
	GnomeAppQueryPrivate *priv;

	query->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (query,
	                                                 GNOME_APP_TYPE_QUERY,
	                                                 GnomeAppQueryPrivate);
	/* The key is const */
	priv->query_request = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

static void
gnome_app_query_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_query_parent_class)->dispose (object);
}

static void
gnome_app_query_finalize (GObject *object)
{
	GnomeAppQuery *query = GNOME_APP_QUERY (object);
	GnomeAppQueryPrivate *priv = query->priv;

	g_hash_table_destroy (priv->query_request);

	G_OBJECT_CLASS (gnome_app_query_parent_class)->finalize (object);
}

static void
gnome_app_query_set_property (GObject        *object,
                          guint           prop_id,
                          const GValue         *value,
                          GParamSpec     *pspec)
{
        GnomeAppQuery *query = GNOME_APP_QUERY (object);
        GnomeAppQueryPrivate *priv = query->priv;
	const gchar *prop_name;
	gchar *prop_value;

	if ((prop_id < PROP_QUERY_GROUP) || (prop_id > PROP_QUERY_LAST)) {
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		return;
	}

	prop_name = query_units [prop_id].name;
	prop_value = g_hash_table_lookup (priv->query_request, prop_name);
	if (prop_value)
		g_hash_table_replace (priv->query_request, g_strdup (prop_name), g_strdup (g_value_get_string (value)));
	else
		g_hash_table_insert (priv->query_request, g_strdup (prop_name), g_strdup (g_value_get_string (value)));
}

static void
gnome_app_query_get_property (GObject        *object,
                          guint           prop_id,
                          GValue         *value,
                          GParamSpec     *pspec)
{
        GnomeAppQuery *query = GNOME_APP_QUERY (object);
        GnomeAppQueryPrivate *priv = query->priv;

	if ((prop_id < PROP_QUERY_GROUP) || (prop_id > PROP_QUERY_LAST)) {
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		return;
	}
	gchar *prop_value;
	prop_value = g_hash_table_lookup (priv->query_request, query_units [prop_id].name);
	g_value_set_string (value, prop_value);
}

static void
gnome_app_query_class_init (GnomeAppQueryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_query_set_property;
	object_class->get_property = gnome_app_query_get_property;
	object_class->dispose = gnome_app_query_dispose;
	object_class->finalize = gnome_app_query_finalize;
	
	gint prop_id;

	for (prop_id = PROP_QUERY_GROUP; prop_id < PROP_QUERY_LAST; prop_id ++) {
		g_object_class_install_property (object_class, prop_id,
				g_param_spec_string (query_units [prop_id].name, 
						query_units [prop_id].name, query_units [prop_id].description,
						NULL,
						G_PARAM_READWRITE));
	}

	g_type_class_add_private (object_class, sizeof (GnomeAppQueryPrivate));
}

GnomeAppQuery *
gnome_app_query_new (void)
{
	return g_object_new (GNOME_APP_TYPE_QUERY, NULL);
}

gboolean
gnome_app_query_is_valid (GnomeAppQuery *query)
{
	g_return_val_if_fail (query && GNOME_APP_IS_QUERY (query), FALSE);

        GnomeAppQueryPrivate *priv = query->priv;
	gint prop_id;
	gchar *prop_value;

	for (prop_id = PROP_QUERY_GROUP; prop_id < PROP_QUERY_LAST; prop_id ++) {
		prop_value = g_hash_table_lookup (priv->query_request, query_units [prop_id].name);
		if (prop_value && prop_value [0])
			return TRUE;
	}

	return FALSE;
}

