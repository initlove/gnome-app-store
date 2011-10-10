#include <glib.h>

/*FIXME: donnot add this
should try g_type register g_type_query */
typedef struct _server_type {
	gchar *type_name;
	GType type;
} server_type;

server_type *supported_server_type []= {"ocs", TYPE_OCS_SERVER};
