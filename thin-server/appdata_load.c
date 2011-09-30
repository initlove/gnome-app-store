#include <glib/gdir.h>
#include <gio/gio.h>
#include <libgnome/gnome-desktop-item.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#define DEBUG 0

typedef enum _PKG_KEY_WORDS {
	PKG_ID = 0,
	PKG_PKGNAME,
	PKG_NAME,
	PKG_SUMMARY,
	PKG_ICON,
	PKG_APPCATEGORIES,
	PKG_MIMETYPES,
	PKG_APPCATEGORY,
	PKG_MIMETYPE,
	PKG_LAST,
} PKG_KEY_WORDS;

PKG_KEY_WORDS
get_type_from_name (gchar *name)
{
	gchar *key_words [] = {"id", "pkgname", "name", "summary", "icon", 
			"appcategories", "mimetypes", "appcategory", "mimetype", NULL};
	int i;

	for (i = 0; i < PKG_LAST; i++) {
		if (strcmp (name, key_words [i]) == 0) {
			return i;
		}
	}

	return PKG_LAST;
}

void			
load_app (xmlNodePtr app_node)
{
	xmlNodePtr node, sub_node;
	PKG_KEY_WORDS type;

	printf ("node infos: \t");
	for (node = app_node->xmlChildrenNode; node; node = node->next) {
		type = get_type_from_name ((gchar *)(node->name));
		switch (type) {
			case PKG_ID: 
				printf ("desktp id %s\t", xmlNodeGetContent (node));
				break;
			case PKG_PKGNAME:
				break;
			case PKG_NAME:
//we have the lang here!
//				printf ("pkg name %s\t", xmlNodeGetContent (node));
				break;
			case PKG_SUMMARY:
				break;
			case PKG_ICON:
				printf ("icon %s\t", xmlNodeGetContent (node));
				break;
			case PKG_APPCATEGORIES:
				printf ("cate is <");
				for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
					if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_APPCATEGORY)
						printf ("%s  ", xmlNodeGetContent (sub_node));
				}
				printf (">\t");
				break;
			case PKG_MIMETYPES:
				printf ("mime is <");
				for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
					if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_MIMETYPE)
						printf ("%s  ", xmlNodeGetContent (sub_node));
				}
				printf (">\t");
				break;
			default:
				break;
		} 
	}
	printf ("\n");
}

int 
main ()
{
	xmlDocPtr doc_ptr;
	xmlNodePtr root_node, apps_node, app_node;
	
	g_type_init ();

	doc_ptr = xmlParseFile ("./appdata.xml");

	root_node = xmlDocGetRootElement (doc_ptr);

	for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
		if (strcmp (app_node->name, "application") == 0) {
			load_app (app_node);
		}
	}

	xmlFreeDoc(doc_ptr);

	return 0;
}

