#include "open-result.h"
#include "open-results.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-login-user.h"

static gpointer
set_icon_callback (gpointer userdata, gpointer func_re)
{
	GtkWidget *login_user_icon;
	gchar *dest_url;
	gint width, height;
	GdkPixbuf *pixbuf;

	login_user_icon = GTK_WIDGET (userdata);
	dest_url = (gchar *) func_re;
	gtk_widget_get_size_request (login_user_icon, &width, &height);
	pixbuf = gdk_pixbuf_new_from_file_at_scale (dest_url, width, height, FALSE, NULL);
	gtk_image_set_from_pixbuf (login_user_icon, pixbuf);
	g_object_unref (pixbuf);

	return NULL;
}

static gpointer
set_login_user_callback (gpointer userdata, gpointer func_result)
{
	GList *list;
	OpenResult *result;
	OpenResults *results;
	const gchar *val;

	GtkBuilder *builder;
	GtkWidget *login_user_widget;
        GtkWidget *login_user_icon;
	GtkWidget *login_user_name;
        GtkWidget *login_user_company;
	GtkWidget *login_user_homepage;
	GtkWidget *login_user_friend;
	GtkWidget *login_user_message;

	builder = GTK_BUILDER (userdata);
	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		/*TODO: fill the account with default val */
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		list = open_results_get_data (results);
		result = list->data;

		val = open_result_get (result, "company");
		login_user_company = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_company"));
		gtk_label_set_text (login_user_company, val);

		val = open_result_get (result, "homepage");
		login_user_homepage = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_homepage"));
		gtk_link_button_set_uri (login_user_homepage, val);

		gchar *name;
		/*TODO: add blank in bettween, now, just make my id looks better */
		name = g_strdup_printf ("%s%s", open_result_get (result, "firstname"), open_result_get (result, "lastname"));
		login_user_name = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_name"));
		gtk_label_set_text (login_user_name, name);
		g_free (name);

		const gchar *pic = NULL;
		GnomeAppTask *task;

		val = open_result_get (result, "avatarpicfound");
		if (val && (strcmp (val, "1") == 0)) {
			pic = open_result_get (result, "avatarpic");
		} else {
			val = open_result_get (result, "bigavatarpicfound");
			if (val && (strcmp (val, "1") == 0)) {
				pic = open_result_get (result, "bigavatarpic");
			}
		}
		if (pic) {
			login_user_icon = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_icon"));
			task = gnome_download_task_new (login_user_icon, pic);
			gnome_app_task_set_callback (task, set_icon_callback);
			gnome_app_task_push (task);
		}
	}

	return NULL;
}

GtkWidget *
gnome_app_login_user_new ()
{
	GtkBuilder *builder;
	GError *error;
	gchar *filename;
	        
	error = NULL;
	filename = "./main_ui.glade";
	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, filename, &error);

	GtkWidget *login_user_widget;
        GtkWidget *login_user_icon;
	GtkWidget *login_user_name;
        GtkWidget *login_user_company;
	GtkWidget *login_user_homepage;
	GtkWidget *login_user_friend;
	GtkWidget *login_user_message;
	
	login_user_widget = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_widget"));
	login_user_icon = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_icon"));
	login_user_name = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_name"));
	login_user_company = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_company"));
	login_user_homepage = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_homepage"));
	login_user_friend = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_friend"));
	login_user_message = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_message"));

	filename = open_app_get_pixmap_uri ("person");
	gtk_image_set_from_file (login_user_icon, filename);
	g_free (filename);

	GnomeAppStore *store;
	const gchar *username;
	store = gnome_app_store_get_default ();
	username = gnome_app_store_get_username (store);
	if (!username)
		return login_user_widget;

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/person/data/%s",  username);
	task = gnome_app_task_new (builder, "GET", function);
	gnome_app_task_set_callback (task, set_login_user_callback);
	gnome_app_task_push (task);
	g_free (function);


	return login_user_widget;
}
