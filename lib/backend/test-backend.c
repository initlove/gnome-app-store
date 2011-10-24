#include <stdio.h>

#include "gnome-app-info.h"
#include "gnome-app-config.h"
#include "gnome-app-query.h"
#include "app-backend.h"

int main ()
{
	GnomeAppQuery *query;
	GnomeAppConfig *config;
	AppBackend *backend;
	const gchar *type;
	const GList *local_categories;
	GList *l, *list;

	g_type_init ();

	config = gnome_app_config_new ();
	backend = app_backend_new_from_config (config);
	type = app_backend_get_backend_type (backend);
	printf ("backend type is %s\n", type);

	query = gnome_app_query_new ();
	g_object_set (query, QUERY_PAGESIZE, "35", QUERY_PAGE, "0", NULL);
	list = app_backend_get_apps_by_query (backend, query);
	for (l = list; l; l = l->next) {
		GnomeAppInfo *info;
		info = l->data;
		gnome_app_info_debug (info);
	}
	g_object_unref (query);	
	g_object_unref (config);
	g_object_unref (backend);
	return 0;
}
