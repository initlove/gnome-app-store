#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>
#include "open-result.h"
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-login-user.h"
#include "gnome-app-search-entry.h"
#include "gnome-app-icon-view.h"

GtkBuilder *builder;

void
main_view_change (OpenResult *info)
{
	GtkWidget *info_page;
	GtkWidget *app_info_page_place, *app_iconview_place;

	info_page = gnome_app_info_page_new_with_app (info);
	app_info_page_place = GTK_WIDGET (gtk_builder_get_object (builder, "app_info_page_place"));
	app_iconview_place = GTK_WIDGET (gtk_builder_get_object (builder, "app_iconview_place"));
	gtk_widget_hide (app_iconview_place);

	gtk_box_pack_start (GTK_BOX (app_info_page_place), info_page, TRUE, TRUE, 0);
}

int main (gint argc, gchar **argv)
{
	GtkWidget *window = NULL;
	GtkWidget *iconview_box, *search_box, *login_user_box;
	GtkWidget **left_arrow, *right_arrow;
	GtkWidget *search_entry, *iconview, *login_user;

      	GError *err = NULL;
 	gchar *filename;

     	if (gtk_clutter_init_with_args (&argc, &argv,
				NULL,
				NULL,
				NULL,
				&err) != CLUTTER_INIT_SUCCESS) {
		if (err) {
			g_critical ("Unable to initialize Clutter-GTK: %s", err->message);
			g_error_free (err);
		} else
			g_error ("Unable to initialize Clutter-GTK");
		return -1;
	}

	GnomeAppStore *store;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, gdk_threads_enter);
	gnome_app_store_set_unlock_function (store, gdk_threads_leave);
	gnome_app_store_init_category (store);

      	builder = gtk_builder_new ();
      	filename = "./main_ui.glade";
 	gtk_builder_add_from_file (builder, filename, &err);

      	if (err) {
		g_error ("ERROR: %s\n", err->message);
		return -1;
        }
 	gtk_builder_connect_signals (builder, NULL);

      	window = GTK_WIDGET (gtk_builder_get_object (builder, "app_store_window"));
	iconview_box = GTK_WIDGET (gtk_builder_get_object (builder, "iconview_box"));
	login_user_box = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_box"));
	left_arrow = GTK_WIDGET (gtk_builder_get_object (builder, "left_arrow"));
	right_arrow = GTK_WIDGET (gtk_builder_get_object (builder, "right_arrow"));
	search_box = GTK_WIDGET (gtk_builder_get_object (builder, "search_box"));


	GnomeAppTask *task;
	const gchar *pagesize = "24";
	const gchar *page = "0";
	task = gnome_app_task_new (NULL, "GET", "/v1/content/data");
	gnome_app_task_add_params (task,
			"sortmode", "new",
			"pagesize", pagesize,
			"page", page,
			NULL);
	iconview = gnome_app_icon_view_new ();
	gnome_app_icon_view_set_with_task (iconview, task);
	gtk_box_pack_start (GTK_BOX (iconview_box), iconview, TRUE, TRUE, 0);

	search_entry = gnome_app_search_entry_new ();
	gtk_box_pack_start (GTK_BOX (search_box), search_entry, TRUE, TRUE, 0);

	login_user = gnome_app_login_user_new ();
	gtk_box_pack_start (GTK_BOX (login_user_box), login_user, TRUE, TRUE, 0);

      	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
