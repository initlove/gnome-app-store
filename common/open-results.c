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
#include "open-results.h"

G_DEFINE_ABSTRACT_TYPE (OpenResults, open_results, G_TYPE_OBJECT)

static void
open_results_init (OpenResults *results)
{
}

static void
open_results_class_init (OpenResultsClass *klass)
{
}

const gchar *
open_results_get_meta (OpenResults *results, const gchar *prop)
{
	g_return_val_if_fail (results && IS_OPEN_RESULTS (results), NULL);
	g_return_val_if_fail (prop, NULL);

	return OPEN_RESULTS_GET_CLASS (results)->get_meta (results, prop);
}

GList *
open_results_get_data (OpenResults *results)
{
	g_return_val_if_fail (results && IS_OPEN_RESULTS (results), NULL);

	return OPEN_RESULTS_GET_CLASS (results)->get_data (results);
}

gboolean
open_results_get_status (OpenResults *results)
{
	g_return_val_if_fail (results && IS_OPEN_RESULTS (results), FALSE);

	return OPEN_RESULTS_GET_CLASS (results)->get_status (results);
}

gint
open_results_get_total_items (OpenResults *results)
{
	g_return_val_if_fail (results && IS_OPEN_RESULTS (results), -1);

	return OPEN_RESULTS_GET_CLASS (results)->get_total_items (results);
}

