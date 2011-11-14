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
#include "open-query.h"

struct _OpenQueryPrivate
{
	GHashTable *query_keys;
};

G_DEFINE_TYPE (OpenQuery, open_query, G_TYPE_OBJECT)

static void
open_query_init (OpenQuery *query)
{
	OpenQueryPrivate *priv;

	query->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (query,
	                                                 TYPE_OPEN_QUERY,
	                                                 OpenQueryPrivate);
	priv->query_keys = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

static void
open_query_dispose (GObject *object)
{
	G_OBJECT_CLASS (open_query_parent_class)->dispose (object);
}

static void
open_query_finalize (GObject *object)
{
	OpenQuery *query = OPEN_QUERY (object);
	OpenQueryPrivate *priv = query->priv;

	g_hash_table_destroy (priv->query_keys);

	G_OBJECT_CLASS (open_query_parent_class)->finalize (object);
}

static void
open_query_class_init (OpenQueryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = open_query_dispose;
	object_class->finalize = open_query_finalize;
	
	g_type_class_add_private (object_class, sizeof (OpenQueryPrivate));
}

OpenQuery *
open_query_new ()
{
	return g_object_new (TYPE_OPEN_QUERY, NULL);
}

void
open_query_set (OpenQuery *query, gchar *key_name, gchar *value)
{
	g_return_if_fail (query && IS_OPEN_QUERY (query));
	g_return_if_fail (key_name);

	OpenQueryPrivate *priv = query->priv;
	gchar *val;

	val = g_hash_table_lookup (priv->query_keys, key_name);
	if (val)
		g_hash_table_replace (priv->query_keys, g_strdup (key_name), g_strdup (value));
	else
		g_hash_table_insert (priv->query_keys, g_strdup (key_name), g_strdup (value));
}

const gchar *
open_query_get (OpenQuery *query, gchar *key)
{
	g_return_val_if_fail (query && IS_OPEN_QUERY (query), NULL);
	g_return_val_if_fail (key, NULL);

	OpenQueryPrivate *priv = query->priv;
	const gchar *value;

	value = g_hash_table_lookup (priv->query_keys, key);

	return value;
}
