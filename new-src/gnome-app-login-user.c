/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome applogin_user lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-login-user.h"

struct _GnomeAppLoginUserPrivate
{
	GtkBuilder *builder;
};

G_DEFINE_TYPE (GnomeAppLoginUser, gnome_app_login_user, GTK_TYPE_BOX)

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
	gtk_image_set_from_pixbuf (GTK_IMAGE (login_user_icon), pixbuf);
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
	GnomeAppLoginUser *login_user;

	GtkBuilder *builder;
	GtkWidget *login_user_widget;
        GtkWidget *login_user_icon;
	GtkWidget *login_user_name;
        GtkWidget *login_user_company;
	GtkWidget *login_user_homepage;
	GtkWidget *login_user_friend;
	GtkWidget *login_user_message;

	login_user = GNOME_APP_LOGIN_USER (userdata);
	builder = login_user->priv->builder;
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
		gtk_label_set_text (GTK_LABEL (login_user_company), val);

		val = open_result_get (result, "homepage");
		login_user_homepage = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_homepage"));
		gtk_link_button_set_uri (GTK_LINK_BUTTON (login_user_homepage), val);

		gchar *name;
		/*TODO: add blank in bettween, now, just make my id looks better */
		name = g_strdup_printf ("%s%s", open_result_get (result, "firstname"), open_result_get (result, "lastname"));
		login_user_name = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_name"));
		gtk_label_set_text (GTK_LABEL (login_user_name), name);
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

static void
gnome_app_login_user_init (GnomeAppLoginUser *login_user)
{
	GnomeAppLoginUserPrivate *priv;

	login_user->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (login_user,
	                                                 GNOME_APP_TYPE_LOGIN_USER,
	                                                 GnomeAppLoginUserPrivate);
	priv->builder = NULL;
}

static void
gnome_app_login_user_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_login_user_parent_class)->dispose (object);
}

static void
gnome_app_login_user_finalize (GObject *object)
{
	GnomeAppLoginUser *login_user = GNOME_APP_LOGIN_USER (object);
	GnomeAppLoginUserPrivate *priv = login_user->priv;

	if (priv->builder)
		g_object_unref (priv->builder);

	G_OBJECT_CLASS (gnome_app_login_user_parent_class)->finalize (object);
}

static void
gnome_app_login_user_class_init (GnomeAppLoginUserClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_login_user_dispose;
	object_class->finalize = gnome_app_login_user_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppLoginUserPrivate));
}

GnomeAppLoginUser *
gnome_app_login_user_new (void)
{
	GnomeAppLoginUser *login_user;
	GnomeAppLoginUserPrivate *priv;
	GtkWidget *login_user_widget;
        GtkWidget *login_user_icon;
	GtkBuilder *builder;
	GError *error;
	gchar *filename;
	        
	login_user = g_object_new (GNOME_APP_TYPE_LOGIN_USER, NULL);
	priv = login_user->priv;
		
	error = NULL;
	filename = "./main_ui.glade";
	builder = priv->builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, filename, &error);

	if (error) {
		g_error ("Error in load main-ui.glade: %s\n", error->message);
		g_error_free (error);
		return login_user;
	}

	login_user_widget = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_widget"));
	gtk_box_pack_start (GTK_BOX (login_user), login_user_widget, TRUE, TRUE, 0);

	login_user_icon = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_icon"));
	filename = open_app_get_pixmap_uri ("person");
	gtk_image_set_from_file (GTK_IMAGE (login_user_icon), filename);
	g_free (filename);

	GnomeAppStore *store;
	const gchar *username;
	store = gnome_app_store_get_default ();
	username = gnome_app_store_get_username (store);
	/*TODO: make it better, like not login? register */
	if (!username)
		return login_user;

	GnomeAppTask *task;
	gchar *function;

	function = g_strdup_printf ("/v1/person/data/%s",  username);
	task = gnome_app_task_new (login_user, "GET", function);
	gnome_app_task_set_callback (task, set_login_user_callback);
	gnome_app_task_push (task);
	g_free (function);

	return login_user;
}
