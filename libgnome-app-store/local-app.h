#ifndef __LOCAL_APP_H__
#define __LOCAL_APP_H__

#include <glib.h>

gboolean		is_local_app (const char *id);
const gchar *		get_icon_uri (gchar *icon);
gchar *			get_default_icon ();

void			debug_local_apps ();

#endif
