#include <stdio.h>

#include "gnome-app-item.h"
#include "gnome-app-config.h"
#include "app-backend.h"

void
debug_apps_by_group (AppBackend *backend, gchar *group)
{
	GList *cid_list, *appid_list, *l;
	gchar *appid;
	GnomeAppItem *item;
	const gchar *local_screenshot_url;

	cid_list = app_backend_get_cid_list_by_group (backend, group);
	appid_list = app_backend_get_appid_list_by_cid_list (backend, cid_list);
	for (l = appid_list; l; l = l->next) {
		appid = l->data;
		item = app_backend_get_app_by_id (backend, appid);
		local_screenshot_url = gnome_app_item_get_local_screenshot_url (item);
printf ("local screenshot url %s\n\n", local_screenshot_url);
		gnome_app_item_debug (item);
	}
	g_list_free (cid_list);
	g_list_free (appid_list);
}

void
debug_categories (AppBackend *backend)
{
	GList *cid_list, *l;

	cid_list = app_backend_get_cid_list_by_group (backend, NULL);
	printf ("-------------------------------------\n");
	printf ("Categories in the backend: \n");
	for (l = cid_list; l; l = l->next) {
		printf ("\tCategory name %s\n", (gchar *) l->data);
	}
	printf ("-------------------------------------\n");
	g_list_free (cid_list);
}

int main ()
{
	GnomeAppConfig *config;
	AppBackend *backend;
	const gchar *type;
	const GList *local_categories;
	GList *l;

	g_type_init ();

	config = gnome_app_config_new ();
	backend = app_backend_new_from_config (config);
	type = app_backend_get_backend_type (backend);
	printf ("backend type is %s\n", type);

		
	debug_categories (backend);
	local_categories = gnome_app_get_local_categories ();
		
#if 1
	debug_apps_by_group (backend, NULL);
	return ;
#endif
	for (l = local_categories; l; l = l->next) {
		debug_apps_by_group (backend, (gchar *)l->data);
	}

	g_object_unref (config);
	g_object_unref (backend);
	return 0;
}
