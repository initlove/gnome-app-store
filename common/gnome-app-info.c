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

#include "gnome-app-info.h"

G_DEFINE_ABSTRACT_TYPE (GnomeAppInfo, gnome_app_info, G_TYPE_OBJECT)

static void
gnome_app_info_init (GnomeAppInfo *info)
{
}

static void
gnome_app_info_class_init (GnomeAppInfoClass *klass)
{
}

const gchar *
gnome_app_info_get (GnomeAppInfo *info, const gchar *prop)
{
	g_return_val_if_fail (info && GNOME_APP_IS_INFO (info), NULL);
	g_return_val_if_fail (prop, NULL);

	return GNOME_APP_INFO_GET_CLASS (info)->get (info, prop);
}

const gchar *
gnome_app_info_get_backend_type (GnomeAppInfo *info)
{
	g_return_val_if_fail (info && GNOME_APP_IS_INFO (info), NULL);

	return GNOME_APP_INFO_GET_CLASS (info)->get_backend_type (info);
}

const gchar **
gnome_app_info_get_props (GnomeAppInfo *info)
{
	g_return_val_if_fail (info && GNOME_APP_IS_INFO (info), NULL);

	return GNOME_APP_INFO_GET_CLASS (info)->get_props (info);
}

void
gnome_app_info_debug (GnomeAppInfo *info)
{
	const gchar **props;
	const gchar *val;
	gint i;

	g_return_if_fail (info && GNOME_APP_IS_INFO (info));

	props = gnome_app_info_get_props (info);
	g_debug ("-----------info debug begin ---------\n");
	for (i = 0; props [i]; i++) {
		val = gnome_app_info_get (info, props [i]);
		if (val)
			g_debug ("%s -- %s\n", props [i], val);
	}
	g_debug ("-----------info debug end   ---------\n");
}


