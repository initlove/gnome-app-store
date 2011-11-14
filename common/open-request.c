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
#include "open-request.h"

struct _OpenRequestPrivate
{
	GHashTable *request_keys;
};

G_DEFINE_TYPE (OpenRequest, open_request, G_TYPE_OBJECT)

static void
open_request_init (OpenRequest *request)
{
	OpenRequestPrivate *priv;

	request->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (request,
	                                                 TYPE_OPEN_REQUEST,
	                                                 OpenRequestPrivate);
	priv->request_keys = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

static void
open_request_dispose (GObject *object)
{
	G_OBJECT_CLASS (open_request_parent_class)->dispose (object);
}

static void
open_request_finalize (GObject *object)
{
	OpenRequest *request = OPEN_REQUEST (object);
	OpenRequestPrivate *priv = request->priv;

	g_hash_table_destroy (priv->request_keys);

	G_OBJECT_CLASS (open_request_parent_class)->finalize (object);
}

static void
open_request_class_init (OpenRequestClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = open_request_dispose;
	object_class->finalize = open_request_finalize;
	
	g_type_class_add_private (object_class, sizeof (OpenRequestPrivate));
}

OpenRequest *
open_request_new ()
{
	return g_object_new (TYPE_OPEN_REQUEST, NULL);
}

void
open_request_set (OpenRequest *request, gchar *key_name, gchar *value)
{
	g_return_if_fail (request && IS_OPEN_REQUEST (request));
	g_return_if_fail (key_name);

	OpenRequestPrivate *priv = request->priv;
	gchar *val;

	val = g_hash_table_lookup (priv->request_keys, key_name);
	if (val)
		g_hash_table_replace (priv->request_keys, g_strdup (key_name), g_strdup (value));
	else
		g_hash_table_insert (priv->request_keys, g_strdup (key_name), g_strdup (value));
}

const gchar *
open_request_get (OpenRequest *request, gchar *key)
{
	g_return_val_if_fail (request && IS_OPEN_REQUEST (request), NULL);
	g_return_val_if_fail (key, NULL);

	OpenRequestPrivate *priv = request->priv;
	const gchar *value;

	value = g_hash_table_lookup (priv->request_keys, key);

	return value;
}
