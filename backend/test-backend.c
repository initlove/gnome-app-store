#include <stdio.h>

#include "ocs-result.h"
#include "ocs-results.h"
#include "open-result.h"
#include "open-results.h"
#include "open-request.h"
#include "gnome-app-config.h"
#include "app-backend.h"

int main ()
{
	GnomeAppConfig *config;
	AppBackend *backend;
	OpenRequest *request;
	OpenResults *results;

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
	results = app_backend_get_results (backend, request);
	list = open_results_get_data (results);
	for (l = list; l; l = l->next) {
		OpenResult *result;
		result = l->data;
	//	open_result_debug (info);
	}
	g_object_unref (request);	
	g_object_unref (results);

	request = open_request_new ();
	open_request_set (request, "services", "comments");
	open_request_set (request, "operation", "get");
	open_request_set (request, "type", "1");
	open_request_set (request, "contentid", "146115");
	open_request_set (request, "contentid2", "0");
	open_request_set (request, "pagesize", "35");
	open_request_set (request, "page", "0");
	url = ocs_get_request_url (backend, request);
	results = app_backend_get_results (backend, request);

	gchar *val;
	val = open_results_get_meta (results, "totalitems");
	printf ("val %s\n", val);
	list = open_results_get_data (results);
	for (l = list; l; l = l->next) {
		OpenResult *result;
		result = l->data;
		open_result_debug (result);
	}

	g_object_unref (request);	
	g_object_unref (results);

	g_object_unref (config);
	g_object_unref (backend);
	return 0;
}
