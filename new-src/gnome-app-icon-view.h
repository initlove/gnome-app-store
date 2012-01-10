#ifndef __GNOME_APP_ICON_VIEW_H__
#define __GNOME_APP_ICON_VIEW_H__

#include "open-result.h"
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-task.h"

GtkWidget *	gnome_app_icon_view_new ();
void		gnome_app_icon_view_set_with_task (GtkWidget *app_iconview, GnomeAppTask *task);

#endif
