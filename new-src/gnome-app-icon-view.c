/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appicon_view lib is distributed in the hope that it will be useful,
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
#include "gnome-app-application.h"

/* Properties */
enum
{
	PROP_0,
	PROP_APPLICATION,
	PROP_ACTIONS
};

struct _GnomeAppIconViewPrivate
{
	GnomeAppApplication *app;
	GtkBuilder *builder;
	GnomeAppTask *task;
	GtkWidget *actions;
};

G_DEFINE_TYPE (GnomeAppIconView, gnome_app_icon_view, GTK_TYPE_ICON_VIEW)

enum
{
	COL_APP_INFO,
	COL_DISPLAY_NAME,
	COL_PIXBUF,
	NUM_COLS
};

static gpointer
set_icon_callback (gpointer userdata, gpointer func_re)
{
	GdkPixbuf *pixbuf;
	gchar *dest_url;
	GtkTreeIter *iter;

	iter = (GtkTreeIter *) userdata;
	dest_url = (gchar *) func_re;

	pixbuf = gdk_pixbuf_new_from_file_at_scale (dest_url, 64, 48, FALSE, NULL);
	gtk_list_store_set (iter->user_data2, iter,
				COL_PIXBUF, pixbuf,
				-1);
//	g_object_unref (pixbuf);
	gtk_tree_iter_free (iter);
	//TODO: cannot free it ?
//	g_free (dest_url);
}

/*TODO: we should fill lots of infos in this function */
static void
set_prev_next_button (GnomeAppIconView *icon_view, OpenResults *results)
{
	GnomeAppIconViewPrivate *priv;
	GtkWidget *prev;
	GtkWidget *next;
	const gchar *val;
	gint total_items;
	gint page;
	gint pagesize;	

	priv = icon_view->priv;
	total_items = open_results_get_total_items (results);
	val = gnome_app_task_get_param_value (priv->task, "page");
	page = atoi (val);
	val = gnome_app_task_get_param_value (priv->task, "pagesize");
	pagesize = atoi (val);

	//page begin with '0'
	prev = GTK_WIDGET (gtk_builder_get_object (priv->builder, "icon_view_prev_button"));
	next = GTK_WIDGET (gtk_builder_get_object (priv->builder, "icon_view_next_button"));
	gtk_widget_set_sensitive (prev, (page == 0) ? FALSE : TRUE);
	gtk_widget_set_sensitive (next, ((page + 2) * pagesize > total_items) ? FALSE : TRUE);
}

static gpointer
task_callback (gpointer userdata, gpointer func_result)
{
	GList *list, *l;
	OpenResults *results;
	GnomeAppIconView *icon_view;
	GtkWidget *app_iconview;
	GtkListStore *store;

	icon_view = GNOME_APP_ICON_VIEW (userdata);
	store = GTK_LIST_STORE (gtk_icon_view_get_model (GTK_ICON_VIEW (icon_view)));
	gtk_list_store_clear (store);

	results = OPEN_RESULTS (func_result);
	list = open_results_get_data (results);
  
	GdkPixbuf *pixbuf;
	gchar *filename;
	const gchar *display_name;
	GnomeAppTask *task;
	GtkTreeIter iter;
	GtkTreeIter *icon_iter;

	filename = open_app_get_pixmap_uri ("missing");
	pixbuf = gdk_pixbuf_new_from_file_at_scale (filename, 64, 48, FALSE, NULL);
	g_free (filename);

	if (!open_results_get_status (results)) {
		/*TODO: */
		g_debug ("Cannot get the page\n");
		return NULL;
	}

	set_prev_next_button (icon_view, results);

	for (l = list; l; l = l->next) {
		display_name = open_result_get (l->data, "name");
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
				COL_APP_INFO, l->data,
				COL_DISPLAY_NAME, display_name,
				COL_PIXBUF, pixbuf,
				-1);
	//	g_object_unref (pixbuf);
		icon_iter = gtk_tree_iter_copy (&iter);
		icon_iter->user_data2 = store;
		task = gnome_download_task_new (icon_iter, open_result_get (l->data, "smallpreviewpic1"));
		gnome_app_task_set_callback (task, set_icon_callback);
		gnome_app_task_push (task);
	}

	return NULL;
}

static void
item_activated (GtkIconView *icon_view,
      		GtkTreePath *tree_path,
		gpointer     user_data)
{
	GnomeAppIconView *app_iconview;
	GtkTreeModel *model;
	OpenResult *app_info;
   	GtkTreeIter iter;

	app_iconview = GNOME_APP_ICON_VIEW (icon_view);
	model = gtk_icon_view_get_model (icon_view);

	gtk_tree_model_get_iter (model,	&iter, tree_path);

	gtk_tree_model_get (model, &iter,
			COL_APP_INFO, &app_info,
			-1);
	gnome_app_application_load (app_iconview->priv->app, APP_INFO_PAGE, app_info);
}

static void
prev_button_clicked (GtkWidget *button, GnomeAppIconView *icon_view)
{
	GnomeAppTask *task;
	gchar *val;
	gint page;

	task = icon_view->priv->task;
	val = gnome_app_task_get_param_value (task, "page");
	page = atoi (val);
	val = g_strdup_printf ("%d", page - 1);
	gnome_app_task_add_param (task, "page", val);
	g_free (val);

	gnome_app_icon_view_set_with_task (icon_view, task);
}

static void
next_button_clicked (GtkWidget *button, GnomeAppIconView *icon_view)
{
	GnomeAppTask *task;
	gchar *val;
	gint page;

	task = icon_view->priv->task;
	val = gnome_app_task_get_param_value (task, "page");
	page = atoi (val);
	val = g_strdup_printf ("%d", page + 1);
	gnome_app_task_add_param (task, "page", val);
	g_free (val);

	gnome_app_icon_view_set_with_task (icon_view, task);
}

static void
icon_view_set_actions (GnomeAppIconView *icon_view)
{
	GtkBuilder *builder;
	GtkWidget *prev_button, *next_button;
	GtkWidget *prev_image, *next_image;
	gchar *filename;
	GError *error = NULL;

	builder = icon_view->priv->builder = gtk_builder_new ();
	filename = "./main_ui.glade";
	gtk_builder_add_from_file (builder, filename, &error);

	icon_view->priv->actions = GTK_WIDGET (gtk_builder_get_object (builder, "icon_view_action_box"));

	prev_button = GTK_WIDGET (gtk_builder_get_object (builder, "icon_view_prev_button"));
	next_button = GTK_WIDGET (gtk_builder_get_object (builder, "icon_view_next_button"));
	prev_image = GTK_WIDGET (gtk_builder_get_object (builder, "icon_view_prev_image"));
	next_image = GTK_WIDGET (gtk_builder_get_object (builder, "icon_view_next_image"));
	gtk_widget_show (prev_image);
	gtk_widget_show (next_image);

	g_signal_connect (prev_button, "clicked", G_CALLBACK (prev_button_clicked), icon_view);
	g_signal_connect (next_button, "clicked", G_CALLBACK (next_button_clicked), icon_view);
}

static void
icon_view_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppIconView *icon_view;

	icon_view = GNOME_APP_ICON_VIEW (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			icon_view->priv->app = g_value_get_object (value);
			break;
	}
}

static void
icon_view_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppIconView *icon_view;

	icon_view = GNOME_APP_ICON_VIEW (object);

  	switch (prop_id)
	{
		case PROP_APPLICATION:
			g_value_set_object (value, icon_view->priv->app);
			break;
		case PROP_ACTIONS:
			g_value_set_object (value, icon_view->priv->actions);
			break;
	}
}

static void
gnome_app_icon_view_init (GnomeAppIconView *icon_view)
{
	GnomeAppIconViewPrivate *priv;

	icon_view->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (icon_view,
	                                                 GNOME_APP_TYPE_ICON_VIEW,
	                                                 GnomeAppIconViewPrivate);
}

static void
gnome_app_icon_view_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->dispose (object);
}

static void
gnome_app_icon_view_finalize (GObject *object)
{
	GnomeAppIconView *icon_view = GNOME_APP_ICON_VIEW (object);
	GnomeAppIconViewPrivate *priv = icon_view->priv;

	G_OBJECT_CLASS (gnome_app_icon_view_parent_class)->finalize (object);
}

static void
gnome_app_icon_view_class_init (GnomeAppIconViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = icon_view_set_property;
	object_class->get_property = icon_view_get_property;
	object_class->dispose = gnome_app_icon_view_dispose;
	object_class->finalize = gnome_app_icon_view_finalize;

        g_object_class_install_property (object_class,
				PROP_APPLICATION,
				g_param_spec_object ("application",
				"Application",
				"The application of the icon view",
				GNOME_APP_TYPE_APPLICATION,
				G_PARAM_READWRITE));

        g_object_class_install_property (object_class,
				PROP_ACTIONS,
				g_param_spec_object ("actions",
				"Actions",
				"The actions of the icon view",
				GTK_TYPE_WIDGET,
				G_PARAM_READABLE));

	g_type_class_add_private (object_class, sizeof (GnomeAppIconViewPrivate));
}

GnomeAppIconView *
gnome_app_icon_view_new (const gchar *personid)
{
	GnomeAppIconView *icon_view;
	GnomeAppIconViewPrivate *priv;
		
	icon_view = g_object_new (GNOME_APP_TYPE_ICON_VIEW, NULL);
	priv = icon_view->priv;

	GtkListStore *store;
		
	store = gtk_list_store_new (NUM_COLS,
			G_TYPE_OBJECT,
			G_TYPE_STRING,
			GDK_TYPE_PIXBUF);
	gtk_icon_view_set_model (GTK_ICON_VIEW (icon_view), GTK_TREE_MODEL (store));
	gtk_icon_view_set_item_width (GTK_ICON_VIEW (icon_view), 100);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view), COL_DISPLAY_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), COL_PIXBUF);

	g_signal_connect (icon_view, "item-activated",
			G_CALLBACK (item_activated), NULL);

	icon_view_set_actions (icon_view);

	return icon_view;
}

void
gnome_app_icon_view_set_with_task (GnomeAppIconView *icon_view, GnomeAppTask *task)
{
	icon_view->priv->task = task;
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_set_userdata (task, icon_view);
	gnome_app_task_push (task);
}
