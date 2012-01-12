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
	PROP_APPLICATION
};

struct _GnomeAppIconViewPrivate
{
	GnomeAppApplication *app;
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
	printf ("clicked on %s\n", open_result_get (app_info, "name"));
	gnome_app_application_load (app_iconview->priv->app, APP_INFO_PAGE, app_info);
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
	}
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

	return icon_view;
}

void
gnome_app_icon_view_set_with_task (GnomeAppIconView *icon_view, GnomeAppTask *task)
{
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_set_userdata (task, icon_view);
	gnome_app_task_push (task);
}
