#include <stdio.h>

#include "gnome-app-item.h"
#include "gnome-app-config.h"
#include "app-server.h"

void
debug_apps_by_group (AppServer *server, gchar *group)
{
	GList *cid_list, *appid_list, *l;
	gchar *appid;
	GnomeAppItem *item;

	cid_list = app_server_get_cid_list_by_group (server, group);
	appid_list = app_server_get_appid_list_by_cid_list (server, cid_list);
	for (l = appid_list; l; l = l->next) {
		appid = l->data;
		item = app_server_get_app_by_id (server, appid);
printf ("icon %s\n", gnome_app_item_get_local_icon_url (item));
//		gnome_app_item_debug (item);
	}
	g_list_free (cid_list);
	g_list_free (appid_list);
}

void
debug_categories (AppServer *server)
{
	GList *cid_list, *l;

	cid_list = app_server_get_cid_list_by_group (server, NULL);
	printf ("-------------------------------------\n");
	printf ("Categories in the server: \n");
	for (l = cid_list; l; l = l->next) {
		printf ("\tCategory name %s\n", (gchar *) l->data);
	}
	printf ("-------------------------------------\n");
	g_list_free (cid_list);
}

int main ()
{
	GnomeAppConfig *config;
	AppServer *server;
	const gchar *type;
	const GList *local_categories;
	GList *l;
	gboolean ocs_debug = TRUE;

	g_type_init ();

	config = gnome_app_config_new ();
	server = app_server_new_from_config (config);
	type = app_server_get_server_type (server);
	printf ("server type is %s\n", type);

	if (!ocs_debug) {
		debug_categories (server);
		local_categories = gnome_app_get_local_categories ();
		for (l = local_categories; l; l = l->next)
			debug_apps_by_group (server, (gchar *)l->data);
	} else {
	/*FIXME: in ocs debug, we use api.test.opendesktop.org, so the category should not be the local one ... shit */
		GList *ocs_list, *app_list;
		gchar *id, *name;

		ocs_list = app_server_get_cid_list_by_group (server, NULL);
		for (l = ocs_list; l; l = l->next) {
			id = (gchar *) l->data;
			name = app_server_get_cname_by_id (server, id);
			printf ("%s:%s\n", id, name);
		}

		app_list = app_server_get_appid_list_by_cid_list (server, ocs_list);
		for (l = app_list; l; l = l->next) {
			id = (gchar *) l->data;
			printf ("app id %s\n", id);
		}

		GnomeAppItem *item;
		gchar *local_screenshot_url;
		item = app_server_get_app_by_id (server, id);
		local_screenshot_url = gnome_app_item_get_local_screenshot_url (item);
printf ("local screenshot url %s\n", local_screenshot_url);
		g_list_free (ocs_list);
		g_list_free (app_list);
	}

	g_object_unref (config);
	g_object_unref (server);
	return 0;
}
