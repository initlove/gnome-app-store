#include <gtk/gtk.h>
#include "open-result.h"
#include "open-results.h"
#include "open-app-utils.h"
#include "gnome-app-task.h"

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
	GtkWidget *app_iconview;
	GtkListStore *store;

	app_iconview = GTK_WIDGET (userdata);
	store = GTK_LIST_STORE (gtk_icon_view_get_model (app_iconview));
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
	GtkTreeModel *model;
	GtkListStore *store;
	OpenResult *app_info;
   	GtkTreeIter iter;

	store = user_data;
	model = gtk_icon_view_get_model (icon_view);

	gtk_tree_model_get_iter (model,	&iter, tree_path);

	gtk_tree_model_get (model, &iter,
			COL_APP_INFO, &app_info,
			-1);
	printf ("clicked on %s\n", open_result_get (app_info, "name"));
	main_view_change (app_info);
}

GtkWidget *
gnome_app_icon_view_new ()
{
	GtkWidget *app_iconview;
	GtkListStore *store;
		
	store = gtk_list_store_new (NUM_COLS,
			G_TYPE_OBJECT,
			G_TYPE_STRING,
			GDK_TYPE_PIXBUF);
			 
	app_iconview = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_icon_view_set_item_width (GTK_ICON_VIEW (app_iconview), 100);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (app_iconview), COL_DISPLAY_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (app_iconview), COL_PIXBUF);

	return app_iconview;
}

void
gnome_app_icon_view_set_with_task (GtkWidget *app_iconview, GnomeAppTask *task)
{
	gnome_app_task_set_callback (task, task_callback);
	gnome_app_task_set_userdata (task, app_iconview);
	gnome_app_task_push (task);

	g_signal_connect (app_iconview, "item-activated",
			G_CALLBACK (item_activated), NULL);

}
