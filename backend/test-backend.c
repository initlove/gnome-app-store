#include <stdio.h>

#include "open-result.h"
#include "open-results.h"
#include "open-request.h"
#include "open-app-config.h"

#include "app-backend.h"
#include "ocs-backend.h"
#include "ocs-result.h"
#include "ocs-results.h"

int main ()
{
	OpenAppConfig *config;
	AppBackend *backend;
	OpenRequest *request;
	OpenResults *results;

	const gchar *type;
	gchar *ids;
	const GList *local_categories;
	GList *l, *list;
	gint totalitems;

	g_type_init ();

	config = open_app_config_new ();
	backend = app_backend_new_from_config (config);
	type = app_backend_get_backend_type (backend);
	ids = ocs_get_categories_by_name (backend, "Accessories");
	printf ("backend type is %s %s\n", type, ids);
	request = open_request_new ();
	open_request_set (request, "services", "content");
	open_request_set (request, "operation", "list");
	open_request_set (request, "pagesize", "35");
	open_request_set (request, "page", "0");
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
