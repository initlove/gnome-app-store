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

#include "open-result.h"

G_DEFINE_ABSTRACT_TYPE (OpenResult, open_result, G_TYPE_OBJECT)

static void
open_result_init (OpenResult *result)
{
}

static void
open_result_class_init (OpenResultClass *klass)
{
}

const gchar *
open_result_get (OpenResult *result, const gchar *prop)
{
	g_return_val_if_fail (result && IS_OPEN_RESULT (result), NULL);
	g_return_val_if_fail (prop, NULL);

	return OPEN_RESULT_GET_CLASS (result)->get (result, prop);
}

const gchar *
open_result_get_backend_type (OpenResult *result)
{
	g_return_val_if_fail (result && IS_OPEN_RESULT (result), NULL);

	return OPEN_RESULT_GET_CLASS (result)->get_backend_type (result);
}

gchar **
open_result_get_props (OpenResult *result)
{
	g_return_val_if_fail (result && IS_OPEN_RESULT (result), NULL);

	return OPEN_RESULT_GET_CLASS (result)->get_props (result);
}

GList *
open_result_get_child (OpenResult *result)
{
	g_return_val_if_fail (result && IS_OPEN_RESULT (result), NULL);

	return OPEN_RESULT_GET_CLASS (result)->get_child (result);
}

void
open_result_debug (OpenResult *result)
{
	const gchar *val;
	gchar **props;
	gint i;

	g_return_if_fail (result && IS_OPEN_RESULT (result));

	props = open_result_get_props (result);
	g_debug ("-----------result debug begin ---------");
	for (i = 0; props [i]; i++) {
		val = open_result_get (result, props [i]);
		if (val)
			g_debug ("%s -- %s", props [i], val);
	}
	g_debug ("-----------result debug end   ---------");

	g_strfreev (props);
}


