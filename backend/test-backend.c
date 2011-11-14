#include <stdio.h>

#include "open-result.h"
#include "gnome-app-config.h"
#include "open-request.h"
#include "app-backend.h"

int main ()
{
	OpenRequest *request;
	GnomeAppConfig *config;
	AppBackend *backend;
	const gchar *type;
	const GList *local_categories;
	GList *l, *list;
	gchar *url;
	gint totalitems;

	g_type_init ();

	config = gnome_app_config_new ();
	backend = app_backend_new_from_config (config);
	type = app_backend_get_backend_type (backend);
	printf ("backend type is %s\n", type);

	request = open_request_new ();
	open_request_set (request, "services", "content");
	open_request_set (request, "operation", "list");
	open_request_set (request, "pagesize", "35");
	open_request_set (request, "page", "0");
	url = ocs_get_request_url (backend, request);
	list = app_backend_get_results (backend, request, &totalitems);
	for (l = list; l; l = l->next) {
		OpenResult *result;
		result = l->data;
	//	open_result_debug (info);
	}
	g_object_unref (request);	

	request = open_request_new ();
	open_request_set (request, "services", "comments");
	open_request_set (request, "operation", "get");
	open_request_set (request, "type", "1");
	open_request_set (request, "contentid", "146115");
	open_request_set (request, "contentid2", "0");
	open_request_set (request, "pagesize", "35");
	open_request_set (request, "page", "0");
	url = ocs_get_request_url (backend, request);
	g_object_unref (config);
	g_object_unref (backend);
	return 0;
}
