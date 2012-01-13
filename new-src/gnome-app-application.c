/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appapplication lib is distributed in the hope that it will be useful,
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
#include "gnome-app-icon-view.h"
#include "gnome-app-info-page.h"
#include "gnome-app-application.h"

struct _GnomeAppApplicationPrivate
{
	GtkBuilder *builder;
	GtkWidget *search_entry;
        GtkWidget *iconview;
	GtkWidget *login_user;
	GtkWidget *info_page;
	GtkWidget *actions;
};

G_DEFINE_TYPE (GnomeAppApplication, gnome_app_application, GTK_TYPE_APPLICATION)

static void
load_info_page (GnomeAppApplication *application, gpointer userdata)
{
	GnomeAppApplicationPrivate *priv;
	OpenResult *info;

	priv = application->priv;
	info = OPEN_RESULT (userdata);

	gnome_app_info_page_set_with_app (GNOME_APP_INFO_PAGE (priv->info_page), info);
	gtk_widget_hide (priv->iconview);
	gtk_widget_show (priv->info_page);
}

static void
load_icon_view (GnomeAppApplication *application, gpointer userdata)
{
	GnomeAppApplicationPrivate *priv;
	GnomeAppTask *task;
	GtkWidget *action_box;

	priv = application->priv;
	task = GNOME_APP_TASK (userdata);
	gnome_app_icon_view_set_with_task (GNOME_APP_ICON_VIEW (priv->iconview), task);
	action_box = GTK_WIDGET (gtk_builder_get_object (priv->builder, "action_box"));
	if (priv->actions)
		gtk_widget_destroy (priv->actions);
	g_object_get (G_OBJECT (priv->iconview), "actions", &priv->actions, NULL);
	gtk_box_pack_start (GTK_BOX (action_box), priv->actions, TRUE, TRUE, 0);

	gtk_widget_show (priv->actions);
	gtk_widget_show (priv->iconview);
	gtk_widget_hide (priv->info_page);
}

static void
set_init_view (GnomeAppApplication *application)
{
	GnomeAppTask *task;
	const gchar *pagesize = "24";
	const gchar *page = "0";
	task = gnome_app_task_new (NULL, "GET", "/v1/content/data");
	gnome_app_task_add_params (task,
			"sortmode", "new",
			"pagesize", pagesize,
			"page", page,
			NULL);

	gnome_app_application_load (application, APP_ICON_VIEW, task);
}

void
gnome_app_application_load (GnomeAppApplication *app, APP_VIEW_TYPE type, gpointer userdata)
{
	switch (type) {
		case APP_ICON_VIEW:
			load_icon_view (app, userdata);
			break;
		case APP_INFO_PAGE:
			load_info_page (app, userdata);
			break;
	}
}

static void
application_activate (GApplication *g_app)
{
	GnomeAppApplication *app;
	GnomeAppApplicationPrivate *priv;

	app = GNOME_APP_APPLICATION (g_app);
	priv = app->priv;

	GtkBuilder *builder;
	GtkWidget *window = NULL;
	GtkWidget *iconview_box, *search_box, *login_user_box, *info_page_box;

      	GError *error = NULL;
 	gchar *filename;

	GnomeAppStore *store;

	store = gnome_app_store_get_default ();
	gnome_app_store_set_lock_function (store, gdk_threads_enter);
	gnome_app_store_set_unlock_function (store, gdk_threads_leave);
	gnome_app_store_init_category (store);

      	builder = priv->builder = gtk_builder_new ();
      	filename = "./main_ui.glade";
 	gtk_builder_add_from_file (builder, filename, &error);

      	if (error) {
		g_error ("ERROR: %s\n", error->message);
		g_error_free (error);
        }
// 	gtk_builder_connect_signals (builder, NULL);
      	window = GTK_WIDGET (gtk_builder_get_object (builder, "app_store_window"));
	iconview_box = GTK_WIDGET (gtk_builder_get_object (builder, "iconview_box"));
	login_user_box = GTK_WIDGET (gtk_builder_get_object (builder, "login_user_box"));
	search_box = GTK_WIDGET (gtk_builder_get_object (builder, "search_box"));
	iconview_box = GTK_WIDGET (gtk_builder_get_object (builder, "iconview_box"));
	info_page_box = GTK_WIDGET (gtk_builder_get_object (builder, "info_page_box"));

	priv->iconview = GTK_WIDGET (gnome_app_icon_view_new ());
	g_object_set (GNOME_APP_ICON_VIEW (priv->iconview), "application", app, NULL);
	gtk_box_pack_start (GTK_BOX (iconview_box), priv->iconview, TRUE, TRUE, 0);
	priv->search_entry = GTK_WIDGET (gnome_app_search_entry_new ());
	gtk_box_pack_start (GTK_BOX (search_box), priv->search_entry, TRUE, TRUE, 0);
	priv->login_user = GTK_WIDGET (gnome_app_login_user_new ());
	gtk_box_pack_start (GTK_BOX (login_user_box), priv->login_user, TRUE, TRUE, 0);
	priv->info_page = GTK_WIDGET (gnome_app_info_page_new ());
	gtk_box_pack_start (GTK_BOX (info_page_box), priv->info_page, TRUE, TRUE, 0);

	gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (app));
      	gtk_widget_show_all (window);

	set_init_view (app);
}

static void
gnome_app_application_init (GnomeAppApplication *application)
{
	GnomeAppApplicationPrivate *priv;

	application->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (application,
	                                                 GNOME_APP_TYPE_APPLICATION,
	                                                 GnomeAppApplicationPrivate);
}

static void
gnome_app_application_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_application_parent_class)->dispose (object);
}

static void
gnome_app_application_finalize (GObject *object)
{
	GnomeAppApplication *application = GNOME_APP_APPLICATION (object);
	GnomeAppApplicationPrivate *priv = application->priv;

	G_OBJECT_CLASS (gnome_app_application_parent_class)->finalize (object);
}

static void
gnome_app_application_class_init (GnomeAppApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_application_dispose;
	object_class->finalize = gnome_app_application_finalize;

	G_APPLICATION_CLASS (klass)->activate = application_activate;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppApplicationPrivate));
}

GnomeAppApplication *
gnome_app_application_new (void)
{
	GnomeAppApplication *application;
	GnomeAppApplicationPrivate *priv;

	g_type_init ();
	application = g_object_new (GNOME_APP_TYPE_APPLICATION,
				"application-id", "org.gtk.appstore",
				"flags", G_APPLICATION_HANDLES_OPEN,
				 NULL);
	return application;
}
