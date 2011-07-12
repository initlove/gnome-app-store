#include <glib/gdir.h>
#include <gio/gio.h>
#include <libgnome/gnome-desktop-item.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#define DEBUG 0

void
load_rpm_to_desktop (GHashTable *app_hash)
{
	FILE *fp;
	char buf [1024];
	char *p;
	char *desktop_name, *rpm_name;

	fp = fopen ("./rpm-desktop", "r");
	if (fp == NULL) {
		printf ("cannot find rpm-desktop file!\n");
		return;
	}
	while (!feof (fp)) {
		fgets (buf, 1024, fp);
		p = strchr (buf, ':');
		if (!p)
			continue;
		else
			*p = 0;
		rpm_name = strrchr (buf, '/');
		desktop_name = p + strlen (":/usr/share/applications/");
		p = strchr (desktop_name, '\n');
		if (p)
			*p = 0;
		g_hash_table_insert (app_hash, g_strdup (desktop_name), g_strdup (rpm_name + 1));
	}
}

const gchar *
get_package_from_desktop_file (const gchar *desktop_file)
{
	const gchar *pkg_name = NULL;

        
	static GHashTable *app_hash = NULL;

	if (app_hash == NULL) {
		app_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
		load_rpm_to_desktop (app_hash);
	}
	pkg_name = g_hash_table_lookup (app_hash, desktop_file + strlen ("database/applications/"));

	return pkg_name;
}

gboolean
cmp_uri_priority (gchar *last_uri, gchar *uri)
{
	gchar *key_words [] = {"16", "24", "32", "48", "64", "72", "96", "128", "scalable", "pixmaps", NULL};
	gint last_uri_value, uri_value, i;

	last_uri_value = uri_value = -1;
	for (i = 0; key_words [i]; i++) {
		if (last_uri_value <0) {
			if (strstr (last_uri, key_words [i]))
				last_uri_value = i;
		}
		if (uri_value <0) {
			if (strstr (uri, key_words [i]))
				uri_value = i;
		}
		if ((uri_value >= 0) && (last_uri_value >= 0))
			break;
	}
	return last_uri_value >= uri_value;
}

void			
add_icon_to_hash (GHashTable *icon_hash, const gchar *basename, gchar *uri)
{
	gchar *last_uri;
	gchar *icon_name, *p;

	icon_name = g_strdup (basename);
	p = strchr (icon_name, '.');
	if (p) {
		*p = 0;
	}

	last_uri = g_hash_table_lookup (icon_hash, icon_name);
	if (!last_uri) {
		g_hash_table_insert (icon_hash, g_strdup (icon_name), g_strdup (uri));
	} else {
		/*last_uri >= uri, doing nothing */
		if (cmp_uri_priority (last_uri, uri)) {
		} else {
			g_hash_table_replace (icon_hash, g_strdup (icon_name), g_strdup (uri));
		}
	}
	g_free (icon_name);
}

void
load_icons_from_dir (GHashTable *icon_hash, gchar *dirname)
{
	const gchar *basename = NULL;
	gchar *filename;
	GDir *dir;

	dir = g_dir_open (dirname, 0, NULL);
	if (!dir) {
		printf ("cannot open path %s\n", dirname);
		return ;
	}
	while ((basename = g_dir_read_name (dir)) != NULL) {
		filename = g_build_filename (dirname, basename, NULL);

		if (g_file_test (filename, G_FILE_TEST_IS_DIR)) {
			load_icons_from_dir (icon_hash, filename);
		} else if (g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
			add_icon_to_hash (icon_hash, basename, filename);
		}
		g_free (filename);
	}
	g_dir_close (dir);
}

void	
save_the_icon_file (const gchar *content)
{
	static GHashTable *icon_hash = NULL;

	if (icon_hash == NULL) {
		/*The key name should be naked...without any .png  */
		icon_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
		load_icons_from_dir (icon_hash, "database/icons");
		load_icons_from_dir (icon_hash, "database/pixmaps");
	}
/*FIXME: cannot get the icon in some cases */
	gchar *uri;
	gchar *str, *p;
	
	str = g_strdup (content);
	p = strchr (str, '.');
	if (p)
		*p = 0;
	uri = g_hash_table_lookup (icon_hash, str);
	g_free (str);
printf ("content %s uri is %s\n", content, uri);
}

void
add_pkgname (xmlNodePtr app_node, const gchar *desktop_file)
{
	const gchar *pkg_name;

	pkg_name = get_package_from_desktop_file (desktop_file);
	xmlNewTextChild (app_node, NULL, "pkgname", pkg_name);
}

void
add_name (xmlNodePtr app_node, GnomeDesktopItem *desktop_item)
{
	xmlNodePtr sub_node;
	GList *list, *l;
	const gchar *name, *full_name;
	const gchar *lang;
	const gchar *content;
	gchar *name_with_lang;

	/*FIXME: here is standard issuse: X-GNOME-FullName Name GenericName should be checked more strickly */
	name = "Name";
	full_name = "X-GNOME-FullName";

	list = gnome_desktop_item_get_languages (desktop_item, full_name);
	if (list) {
	/*FIXME: should list and lang be free? */
		content = gnome_desktop_item_get_string (desktop_item, full_name);
		sub_node = xmlNewTextChild (app_node, NULL, "name", content);

		for (l = list; l; l = l->next) {
			lang = (gchar *) (l->data);
			name_with_lang = g_strdup_printf ("%s[%s]", full_name, lang);
			content = gnome_desktop_item_get_string (desktop_item, name_with_lang);
			g_free (name_with_lang);
			sub_node = xmlNewTextChild (app_node, NULL, "name", content);
			xmlSetProp (sub_node, "lang", lang);
		}
		g_list_free (list);
	} else {
		content = gnome_desktop_item_get_string (desktop_item, name);
		sub_node = xmlNewTextChild (app_node, NULL, "name", content);
	}

}

void
add_summary (xmlNodePtr app_node, GnomeDesktopItem *desktop_item)
{
	xmlNodePtr sub_node;
	GList *l, *list;
	const gchar *summary;
	const gchar *lang;
	const gchar *content;
	gchar *summary_with_lang;

	summary = "Comment";

	content = gnome_desktop_item_get_string (desktop_item, summary);
	sub_node = xmlNewTextChild (app_node, NULL, "summary", content);
	
	list = gnome_desktop_item_get_languages (desktop_item, summary);
	if (list) {
		for (l = list; l; l = l->next) {
			lang = (gchar *) (l->data);
			summary_with_lang = g_strdup_printf ("%s[%s]", summary, lang);
			content = gnome_desktop_item_get_string (desktop_item, summary_with_lang);
			g_free (summary_with_lang);
			sub_node = xmlNewTextChild (app_node, NULL, "summary", content);
			xmlSetProp (sub_node, "lang", lang);
		}
		g_list_free (list);
	}
}

void
add_icon (xmlNodePtr app_node, GnomeDesktopItem *desktop_item)
{
	xmlNodePtr icon_node;
	const gchar *content;

	content = gnome_desktop_item_get_string (desktop_item, "Icon");
	
	if (!content)
		return;

	/* some desktop with icon put into the certain dir */
	const gchar *p;
	p = strrchr (content, '/');
	if (p)
		p++;
	else
		p = content;

	icon_node = xmlNewTextChild (app_node, NULL, "icon", p);
	xmlSetProp (icon_node, "type", "stock");

	save_the_icon_file (p);
}

void
add_categories (xmlNodePtr app_node, GnomeDesktopItem *desktop_item)
{
	xmlNodePtr category_nodes, sub_node;
	const gchar *category;
	const gchar *content;
	gchar       **str_array;
	int i;

	category_nodes = xmlNewTextChild (app_node, NULL, "appcategories", NULL);

	category = "Categories";

	content = gnome_desktop_item_get_string (desktop_item, category);

	if (!content)
		return;

	str_array = g_strsplit (content, ";", -1);
	for (i = 0; str_array [i]; i++) {
		sub_node = xmlNewTextChild (category_nodes, NULL, "appcategory", str_array [i]);
	}
	g_strfreev (str_array);
}

void
add_mimetypes (xmlNodePtr app_node, GnomeDesktopItem *desktop_item)
{
	xmlNodePtr mimetype_nodes, sub_node;
	const gchar *mimetype;
	const gchar *content;
	gchar       **str_array;
	int i;

	mimetype_nodes = xmlNewTextChild (app_node, NULL, "mimetypes", NULL);

	mimetype = "MimeType";

	content = gnome_desktop_item_get_string (desktop_item, mimetype);

	if (!content)
		return;

	str_array = g_strsplit (content, ";", -1);
	for (i = 0; str_array [i]; i++) {
		sub_node = xmlNewTextChild (mimetype_nodes, NULL, "mimetype", str_array [i]);
	}
	g_strfreev (str_array);
}

void
add_app (xmlNodePtr root_node, const gchar *desktop_file)
{
	GnomeDesktopItem *desktop_item;
	char *str;
	const gchar *id;

	id = desktop_file + strlen ("database/applications/");

	desktop_item = gnome_desktop_item_new_from_file (desktop_file, 0, NULL);

	
	xmlNodePtr app_node, sub_node;
	app_node = xmlNewTextChild (root_node, NULL, "application", NULL);

	sub_node = xmlNewTextChild (app_node, NULL, "id", id);
	xmlSetProp (sub_node, "type", "desktop");

	add_pkgname (app_node, desktop_file);
	add_name (app_node, desktop_item);
	add_summary (app_node, desktop_item);
	add_icon (app_node, desktop_item);
	add_categories (app_node, desktop_item);
	add_mimetypes (app_node, desktop_item);
}

void
load_app_dir (xmlNodePtr node, const gchar *dirname)
{
	const gchar *basename = NULL;
	gchar *filename;
	GDir *dir;

	dir = g_dir_open (dirname, 0, NULL);
	if (!dir) {
		printf ("cannot open path %s\n", dirname);
		return ;
	}
	while ((basename = g_dir_read_name (dir)) != NULL) {
		filename = g_build_filename (dirname, basename, NULL);

		if (g_file_test (filename, G_FILE_TEST_IS_DIR)) {
#if DEBUG
			printf ("filename %s is dir\n", filename);
#endif
			load_app_dir (node, filename);
		} else if (g_str_has_suffix (basename, ".desktop")) {
#if DEBUG
			printf ("it is desktop file %s\n", filename);
#endif
			add_app (node, filename);
		}
		g_free (filename);
	}
	g_dir_close (dir);
}

int main ()
{
	xmlDocPtr doc_ptr;
	
	g_type_init ();

	doc_ptr = xmlNewDoc (NULL);

	xmlNodePtr root_node = xmlNewNode (NULL, "applications");
	xmlSetProp (root_node, "version", "0.1");

	xmlDocSetRootElement (doc_ptr,root_node);



	char *app_dirs;
	app_dirs = "database/applications";
	load_app_dir (root_node, app_dirs);

	xmlSaveFile ("./appdata.xml", doc_ptr);
	xmlFreeDoc(doc_ptr);

	return 0;
}

