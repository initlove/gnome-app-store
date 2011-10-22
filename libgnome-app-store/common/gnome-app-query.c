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

/* from ocs 'list' standard*/
struct _GnomeAppQueryPrivate
{
 	gchar *categories;
	gchar *group; 	/*group is the local policy to get categories */
	gchar *search;
	gchar *user;
	gchar *external;
	gchar *distribution;
	gchar *license;
	gchar *sortmode;
	gint	page;
	gint	pagesize;
};

enum {
	PROP_0,
	PROP_CATEGORIES,
	PROP_GROUP,
	PROP_SEARCH,
	PROP_USER,
	PROP_EXTERNAL,
	PROP_DISTRIBUTION,
	PROP_LICENSE,
	PROP_SORTMODE,
	PROP_PAGE,
	PROP_PAGESIZE,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppQuery, gnome_app_query, G_TYPE_OBJECT)

static void
gnome_app_query_init (GnomeAppQuery *query)
{
	GnomeAppQueryPrivate *priv;
	GError  *error;
	gchar *filename;

	query->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (query,
	                                                 GNOME_APP_TYPE_QUERY,
	                                                 GnomeAppQueryPrivate);

	priv->categories = NULL;
	priv->search = NULL;
	priv->user = NULL;
	priv->external = NULL;
	priv->distribution = NULL;
	priv->license = NULL;
	priv->sortmode = NULL;
	priv->page = 0;
	priv->pagesize = 0;
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

	if (priv->categories)
		g_free (priv->categories);
	if (priv->search)
		g_free (priv->search);
	if (priv->user)
		g_free (priv->user);
	if (priv->external)
		g_free (priv->external);
	if (priv->distribution)
		g_free (priv->distribution);
	if (priv->license)
		g_free (priv->license);
	if (priv->sortmode)
		g_free (priv->sortmode);

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

        switch (prop_id) {
                case PROP_CATEGORIES:
			if (priv->categories)
				g_free (priv->categories);
			priv->categories = g_strdup (g_value_get_string (value));
			break;
                case PROP_GROUP:
			if (priv->group)
				g_free (priv->group);
			priv->group = g_strdup (g_value_get_string (value));
			break;
                case PROP_SEARCH:
			if (priv->search)
				g_free (priv->search);
			priv->search = g_strdup (g_value_get_string (value));
			break;
                case PROP_USER:
			if (priv->user)
				g_free (priv->user);
			priv->user = g_strdup (g_value_get_string (value));
			break;
                case PROP_EXTERNAL:
			if (priv->external)
				g_free (priv->external);
			priv->external = g_strdup (g_value_get_string (value));
			break;
                case PROP_DISTRIBUTION:
			if (priv->distribution)
				g_free (priv->distribution);
			priv->distribution = g_strdup (g_value_get_string (value));
			break;
                case PROP_LICENSE:
			if (priv->license)
				g_free (priv->license);
			priv->license = g_strdup (g_value_get_string (value));
			break;
                case PROP_SORTMODE:
			if (priv->sortmode)
				g_free (priv->sortmode);
			priv->sortmode = g_strdup (g_value_get_string (value));
			break;
                case PROP_PAGE:
			priv->page = g_value_get_int (value);
			break;
                case PROP_PAGESIZE:
			priv->pagesize = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_query_get_property (GObject        *object,
                          guint           prop_id,
                          GValue         *value,
                          GParamSpec     *pspec)
{
        GnomeAppQuery *query = GNOME_APP_QUERY (object);
        GnomeAppQueryPrivate *priv = query->priv;

        switch (prop_id) {
                case PROP_CATEGORIES:
			g_value_set_string (value, priv->categories);
			break;
                case PROP_GROUP:
			g_value_set_string (value, priv->group);
			break;
                case PROP_SEARCH:
			g_value_set_string (value, priv->search);
			break;
                case PROP_USER:
			g_value_set_string (value, priv->user);
			break;
                case PROP_EXTERNAL:
			g_value_set_string (value, priv->external);
			break;
                case PROP_DISTRIBUTION:
			g_value_set_string (value, priv->distribution);
			break;
                case PROP_LICENSE:
			g_value_set_string (value, priv->license);
			break;
                case PROP_SORTMODE:
			g_value_set_string (value, priv->sortmode);
			break;
                case PROP_PAGE:
			g_value_set_int (value, priv->page);
			break;
                case PROP_PAGESIZE:
			g_value_set_int (value, priv->pagesize);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_query_class_init (GnomeAppQueryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_query_set_property;
	object_class->get_property = gnome_app_query_get_property;
	object_class->dispose = gnome_app_query_dispose;
	object_class->finalize = gnome_app_query_finalize;
	         
	g_object_class_install_property (object_class, PROP_CATEGORIES,
                g_param_spec_string ("categories",
                                     "categories", "ids of the requested category ids seperated by 'X'",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_GROUP,
                g_param_spec_string ("group",
                                     "group", "the group, certain set of categories",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SEARCH,
                g_param_spec_string ("search",
                                     "search", "the part of the name of the query you want to find", 
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_USER,
                g_param_spec_string ("user",
                                     "user", "show only contents from on specific user",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_EXTERNAL,
                g_param_spec_string ("external",
                                     "external", "if set = off, only contents which are hosted on the same server are shown",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_DISTRIBUTION,
                g_param_spec_string ("distribution",
                                     "distribution", "the parameter is a comma seperated list of ids",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LICENSE,
                g_param_spec_string ("license",
                                     "license", "the parameter is a comma seperated list of ids",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SORTMODE,
                g_param_spec_string ("sortmode",
                                     "sortmode", "the sortmode of the list. 'new', 'alpha', 'high', 'down'",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_PAGE,
                g_param_spec_int    ("page",
                                     "page", "The content page, the first page is '0'",
                                     0,
				     G_MAXINT16,
				     0,	
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_PAGESIZE,
                g_param_spec_int    ("pagesize",
                                     "pagesize", "The amount of entries per page",
                                     0,
				     G_MAXINT16,
				     0,	
                                     G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppQueryPrivate));
}

GnomeAppQuery *
gnome_app_query_new (void)
{
	return g_object_new (GNOME_APP_TYPE_QUERY, NULL);
}

static gchar *
set_ids_from_list (GList *list, gchar sep)
{
	GList *l;
	GString *ids = NULL;
	gchar *str;

	for (l = list; l; l = l->next) {
                str = (gchar *) l->data;
		if (!str)
			continue;
                if (!ids) {
                        ids = g_string_new (str);
                } else {
                        g_string_append_c (ids, sep);
                        g_string_append (ids, str);
                }
        }

	if (ids) {
		str = ids->str;
		g_string_free (ids, FALSE);
	} else {
		str = NULL;
	}

	return str;
}

void
gnome_app_query_set_from_list (GnomeAppQuery *query, gchar *prop, GList *list)
{
	g_return_if_fail (list && prop);
	gchar *str;
	gchar sep = '0';

	if (strcmp (prop, "categories") == 0) {
		sep = 'x';
	} else if (strcmp (prop, "distribution") == 0) {
		sep = ',';
	} else if (strcmp (prop, "license") == 0) {
		sep = ',';
	}

	if (sep != '0') {
		str = set_ids_from_list (list, sep);
		g_object_set (query, prop, str, NULL);
		g_free (str);
	}
}
