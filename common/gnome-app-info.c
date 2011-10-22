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

struct _GnomeAppInfoPrivate
{
	gchar *		id;
	gchar *		name;
	/* The way to install the pkg. */
	gchar *		pkgname;
	gchar *		icon;
	gchar *		screenshot;
	gchar *		summary;
	gchar *		description;
	/* type1;type2 */
	gchar *		categories;
	/* mime1;mime2 */
	gchar *		mimetypes;
	gchar *		license;

	/*the var begin with '_' may have function binding */
	gchar *		_local_icon_url;
	gchar *		_local_screenshot_url;
	glong		_comment_counts;
	glong		_download_counts;
	gint		_score;

};

enum {
        PROP_0,
        PROP_ID,
        PROP_NAME,
        PROP_PKGNAME,
        PROP_ICON,
        PROP_SCREENSHOT,
        PROP_SUMMARY,
        PROP_DESCRIPTION,
        PROP_CATEGORIES,
	PROP_MIMETYPES,
	PROP_LICENSE,
	PROP_COMMENT_COUNTS,
	PROP_DOWNLOAD_COUNTS,
	PROP_SCORE
};


G_DEFINE_TYPE (GnomeAppInfo, gnome_app_info, G_TYPE_OBJECT)

static void
gnome_app_info_init (GnomeAppInfo *info)
{
	GnomeAppInfoPrivate *priv;

	info->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info,
                                                   GNOME_APP_TYPE_INFO,
                                                   GnomeAppInfoPrivate);

	priv->id = NULL;
	priv->name = NULL;
	priv->pkgname = NULL;
	priv->icon = NULL;
	priv->screenshot = NULL;
	priv->summary = NULL;
	priv->description = NULL;
	priv->categories = NULL;
	priv->mimetypes = NULL;
	priv->license = NULL;
	priv->_local_icon_url = NULL;
	priv->_local_screenshot_url = NULL;
	priv->_comment_counts = -1;
	priv->_download_counts = -1;
	priv->_score = -1;
}

static void
gnome_app_info_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_parent_class)->dispose (object);
}

static void
gnome_app_info_finalize (GObject *object)
{
	GnomeAppInfo *info = GNOME_APP_INFO (object);
	GnomeAppInfoPrivate *priv = info->priv;

	if (priv->id)
		g_free (priv->id);
	if (priv->name)
		g_free (priv->name);
	if (priv->pkgname)
		g_free (priv->pkgname);
	if (priv->icon)
		g_free (priv->icon);
	if (priv->screenshot)
		g_free (priv->screenshot);
	if (priv->_local_icon_url)
		g_free (priv->_local_icon_url);
	if (priv->_local_screenshot_url)
		g_free (priv->_local_screenshot_url);
	if (priv->summary)
		g_free (priv->summary);
	if (priv->description)
		g_free (priv->description);
	if (priv->categories)
		g_free (priv->categories);
	if (priv->mimetypes)
		g_free (priv->mimetypes);
	if (priv->license)
		g_free (priv->license);

	G_OBJECT_CLASS (gnome_app_info_parent_class)->finalize (object);
}

static void
gnome_app_info_set_property (GObject      *object,
				guint         prop_id,
				const GValue *value,
				GParamSpec   *pspec)
{
	GnomeAppInfo *info = GNOME_APP_INFO (object);
	GnomeAppInfoPrivate *priv = info->priv;

	switch (prop_id) {
		case PROP_ID:
			if (priv->id)
				g_free (priv->id);
			priv->id = g_strdup (g_value_get_string (value));
			break;
		case PROP_NAME:
			if (priv->name)
				g_free (priv->name);
			priv->name = g_strdup (g_value_get_string (value));
			break;
		case PROP_PKGNAME:
			if (priv->pkgname)
				g_free (priv->pkgname);
			priv->pkgname = g_strdup (g_value_get_string (value));
			break;
		case PROP_ICON:
			if (priv->icon)
				g_free (priv->icon);
			priv->icon = g_strdup (g_value_get_string (value));
			break;
		case PROP_SCREENSHOT:
			if (priv->screenshot)
				g_free (priv->screenshot);
			priv->screenshot = g_strdup (g_value_get_string (value));
			break;
		case PROP_SUMMARY:
			if (priv->summary)
				g_free (priv->summary);
			priv->summary = g_strdup (g_value_get_string (value));
			break;
		case PROP_DESCRIPTION:
			if (priv->description)
				g_free (priv->description);
			priv->description = g_strdup (g_value_get_string (value));
			break;
		case PROP_CATEGORIES:
			if (priv->categories)
				g_free (priv->categories);
			priv->categories = g_strdup (g_value_get_string (value));
			break;
		case PROP_MIMETYPES:
			if (priv->mimetypes)
				g_free (priv->mimetypes);
			priv->mimetypes = g_strdup (g_value_get_string (value));
			break;
		case PROP_LICENSE:
			if (priv->license)
				g_free (priv->license);
			priv->license = g_strdup (g_value_get_string (value));
			break;
		case PROP_COMMENT_COUNTS:
			priv->_comment_counts = g_value_get_long (value);
			break;
		case PROP_DOWNLOAD_COUNTS:
			priv->_download_counts = g_value_get_long (value);
			break;
		case PROP_SCORE:
			priv->_score = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
        }
}

static void
gnome_app_info_get_property (GObject        *object,
                          guint           prop_id,
                          GValue         *value,
                          GParamSpec     *pspec)
{
	GnomeAppInfo *info = GNOME_APP_INFO (object);
	GnomeAppInfoPrivate *priv = info->priv;

	switch (prop_id) {
		case PROP_ID:
			g_value_set_string (value, priv->id);
			break;
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_PKGNAME:
			g_value_set_string (value, priv->pkgname);
			break;
		case PROP_ICON:
			g_value_set_string (value, priv->icon);
			break;
		case PROP_SCREENSHOT:
			g_value_set_string (value, priv->screenshot);
			break;
		case PROP_SUMMARY:
			g_value_set_string (value, priv->summary);
			break;
		case PROP_DESCRIPTION:
			g_value_set_string (value, priv->description);
			break;
		case PROP_CATEGORIES:
			g_value_set_string (value, priv->categories);
			break;
		case PROP_MIMETYPES:
			g_value_set_string (value, priv->mimetypes);
			break;
		case PROP_LICENSE:
			g_value_set_string (value, priv->license);
			break;
		case PROP_COMMENT_COUNTS:
			g_value_set_long (value, priv->_comment_counts);
			break;
		case PROP_DOWNLOAD_COUNTS:
			g_value_set_long (value, priv->_download_counts);
			break;
		case PROP_SCORE:
			g_value_set_int (value, priv->_score);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
        }
}
                                                                    
static void
gnome_app_info_class_init (GnomeAppInfoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_info_set_property;
	object_class->get_property = gnome_app_info_get_property;
	object_class->dispose = gnome_app_info_dispose;
	object_class->finalize = gnome_app_info_finalize;
	 
	g_object_class_install_property (object_class, PROP_ID,
		g_param_spec_string ("id",
                                     "id", "The id of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_NAME,
		g_param_spec_string ("name",
                                     "name", "The name of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_PKGNAME,
		g_param_spec_string ("pkgname",
                                     "pkgname", "The package name of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_ICON,
		g_param_spec_string ("icon",
                                     "icon", "The icon name of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SCREENSHOT,
		g_param_spec_string ("screenshot",
                                     "screenshot", "The screenshot of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SUMMARY,
		g_param_spec_string ("summary",
                                     "summary", "The summary of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_DESCRIPTION,
		g_param_spec_string ("description",
                                     "description", "The description of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CATEGORIES,
		g_param_spec_string ("categories",
                                     "categories", "The categories of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_MIMETYPES,
		g_param_spec_string ("mimetypes",
                                     "mimetypes", "The mimetypes of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_LICENSE,
		g_param_spec_string ("license",
                                     "license", "The license of the app",
                                     NULL,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_COMMENT_COUNTS,
		g_param_spec_long ("comment-counts",
                                     "comment-counts", "The comment counts of the app",
                                     -1,
				     G_MAXINT32,
				     -1,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_DOWNLOAD_COUNTS,
		g_param_spec_long ("download-counts",
                                     "download-counts", "The download counts of the app",
                                     -1,
				     G_MAXINT32,
				     -1,
                                     G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_SCORE,
		g_param_spec_int ("score",
                                     "score", "The score of the app",
                                     -1,
				     100,
				     -1,
                                     G_PARAM_READWRITE));

	klass->get_local_icon_url = NULL;
	klass->get_local_screenshot_url = NULL;
	klass->get_comment_counts = NULL;
	klass->get_score = NULL;

	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPrivate));
}

GnomeAppInfo *
gnome_app_info_new (void)
{
	return g_object_new (GNOME_APP_TYPE_INFO, NULL);
}

const gchar *
gnome_app_info_get_id (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->id;
}

const gchar *
gnome_app_info_get_pkgname (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->pkgname;
}

const gchar *
gnome_app_info_get_name (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->name;
}

const gchar *
gnome_app_info_get_summary (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->summary;
}

const gchar *
gnome_app_info_get_description (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->description;
}

const gchar *
gnome_app_info_get_icon_name (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->icon;
}

const gchar *
gnome_app_info_get_screenshot (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->screenshot;
}

const gchar *
gnome_app_info_get_categories (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->categories;
}

const gchar *
gnome_app_info_get_license (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;

	return priv->license;
}

/*FIXME: should ask the sever every time ? */
glong
gnome_app_info_get_comment_counts (GnomeAppInfo *info)
{
	GnomeAppInfoClass *class = GNOME_APP_INFO_GET_CLASS (info);

	if (class->get_comment_counts)
		return class->get_comment_counts (info);
	else
		return info->priv->_comment_counts;
}

gint
gnome_app_info_get_score (GnomeAppInfo *info)
{
	GnomeAppInfoClass *class = GNOME_APP_INFO_GET_CLASS (info);

	if (class->get_score)
		return class->get_score (info);
	else
		return info->priv->_score;
}

const gchar *
gnome_app_info_get_local_icon_url (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;
	GnomeAppInfoClass *class = GNOME_APP_INFO_GET_CLASS (info);

	if (!priv->_local_icon_url) {
		if (class->get_local_icon_url)
			priv->_local_icon_url = class->get_local_icon_url (info);
	}

	return (const gchar *)priv->_local_icon_url;
}

const gchar *
gnome_app_info_get_local_screenshot_url (GnomeAppInfo *info)
{
        GnomeAppInfoPrivate *priv = info->priv;
	GnomeAppInfoClass *class = GNOME_APP_INFO_GET_CLASS (info);

	if (!priv->_local_screenshot_url) {
		if (class->get_local_screenshot_url)
			priv->_local_screenshot_url = class->get_local_screenshot_url (info);
	}

	return (const gchar *)priv->_local_screenshot_url;
}

void
gnome_app_info_debug (GnomeAppInfo *info)
{
	printf ("id %s\tname %s\t, icon %s\t, mime %s, cate %s\n",
		info->priv->id, info->priv->name, info->priv->icon,
		info->priv->mimetypes, info->priv->categories);
			
}
