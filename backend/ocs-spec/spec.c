#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib/gdir.h>
#include <glib.h>
#include <glib-object.h>

typedef enum _SPEC_TYPE {
	SPEC_NONE,
	SPEC_REQUEST,
	SPEC_RESULT,
	SPEC_LAST
} SPEC_TYPE;

static gchar *conf_filename = "_conf";
static SPEC_TYPE create_type = SPEC_NONE;

gboolean
is_type (gchar *group_name, SPEC_TYPE type)
{
/* 	If we are going to generate the request.xml, 
	we should only use the following group:
	Summary, Args, POST_Args, URL_Args
	In Summary, only Syntax and Method are necessary, but we leave Description here
*/
	gchar *request_groups [] = {
		"Summary", "Args", "POST_Args", "URL_Args", NULL,
	};
	gchar *result_groups [] = {
		"Result", NULL, 
	};
	gint i;

	switch (type) {
		case SPEC_REQUEST:
			for (i = 0; request_groups [i]; i++) {
				if (strcmp (group_name, request_groups [i]) == 0)
				return TRUE;
			}
			break;
		case SPEC_RESULT:
			for (i = 0; result_groups [i]; i++) {
				if (strcmp (group_name, result_groups [i]) == 0)
				return TRUE;
			}
			break;
		default:
			break;
	}

	return FALSE;
}

void
ocs_add_file (xmlNodePtr node, gchar *filename)
{
	GError *error;
	GKeyFile *key_file;

	key_file = g_key_file_new ();
	error = NULL;
	g_key_file_load_from_file (key_file, filename, G_KEY_FILE_NONE, &error);
	if (error) {
		printf ("error in load config file %s\n", error->message);
		g_error_free (error);
		g_key_file_free (key_file);
		exit (1);
	}
	
	gint group_loop, key_loop;
	gint group_number, key_numbers;
	gchar **groups, **keys;
	gchar *key, *value;

	groups = g_key_file_get_groups (key_file, &group_number);

	for (group_loop = 0; group_loop < group_number; group_loop ++) {
		if (!is_type (*(groups + group_loop), create_type))
			continue;
		xmlNodePtr group_node;
		group_node = xmlNewTextChild (node, NULL, *(groups + group_loop), NULL);
		keys = g_key_file_get_keys (key_file, *(groups + group_loop), &key_numbers, NULL);
		for (key_loop = 0; key_loop < key_numbers; key_loop ++) {
			key = *(keys + key_loop);
			value = g_key_file_get_value (key_file, *(groups + group_loop), key, NULL);
			xmlNewTextChild (group_node, NULL, key, value);
			g_free (value);
		}
		g_strfreev (keys);
	}
	g_strfreev (groups);
	g_key_file_free (key_file);
}

void
ocs_add_dir (xmlNodePtr node, gchar *dirname)
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
			xmlNodePtr sub_node;
			sub_node = xmlNewTextChild (node, NULL, basename, NULL);
			ocs_add_dir (sub_node, filename);
                } else if (g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
			if (strcmp (basename, conf_filename) == 0) {
				ocs_add_file (node, filename);
			}		
                }

                g_free (filename);
        }
        g_dir_close (dir);
}

gint
main (int argc, gchar **argv)
{
	gchar *name, *version, *url, *services_dir, *conf_file;
	gchar *ocs_config;
	gchar *output;
	GError *error;
	GKeyFile *key_file;

        g_type_init ();

	ocs_config = "ocs.conf";
	if (argc < 2) {
		printf ("spec + action\n");
		return 1;
	}
	if (strcmp (argv [1], "request") == 0) {
		create_type = SPEC_REQUEST;
		output = "ocs-services-request.xml";
	} else if (strcmp (argv [1], "result") == 0) {
		create_type = SPEC_RESULT;
		output = "ocs-services-result.xml";
	} else {
		printf ("input the corrent action!\n");
		return 1;
	}
	

	key_file = g_key_file_new ();
	error = NULL;
	g_key_file_load_from_file (key_file, ocs_config, G_KEY_FILE_NONE, &error);
	if (error) {
		printf ("error in load config file %s\n", error->message);
		g_error_free (error);
		exit (1);
	}

	name = g_key_file_get_value (key_file, "Meta", "Name", NULL);
	version = g_key_file_get_value (key_file, "Meta", "Version", NULL);
	url = g_key_file_get_value (key_file, "Meta", "URL", NULL);
	services_dir = g_key_file_get_value (key_file, "Meta", "ServicesDir", NULL);
	conf_file = g_key_file_get_value (key_file, "Meta", "ConfFileName", NULL);
	if (conf_file)
		conf_filename = conf_file;

	g_key_file_free (key_file);

        xmlDocPtr doc_ptr;
        xmlNodePtr root_node, meta_node, services_node;
	
        doc_ptr = xmlNewDoc (NULL);
	root_node = xmlNewNode (NULL, "ocs");
        xmlSetProp (root_node, "version", "0.1");
        xmlDocSetRootElement (doc_ptr,root_node);

	meta_node = xmlNewTextChild (root_node, NULL, "meta", NULL);
	xmlNewTextChild (meta_node, NULL, "name", name);
	xmlNewTextChild (meta_node, NULL, "version", version);
	xmlNewTextChild (meta_node, NULL, "url", url);

	services_node = xmlNewTextChild (root_node, NULL, "services", NULL);

	ocs_add_dir (services_node, services_dir);

	xmlSaveFile (output, doc_ptr);
	xmlFreeDoc(doc_ptr);

}
