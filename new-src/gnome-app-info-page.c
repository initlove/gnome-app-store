/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfo_page lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-info-page.h"

/* Properties */
enum
{
	PROP_0,
	PROP_APPLICATION,
	PROP_ACTIONS
};

struct _GnomeAppInfoPagePrivate
{
	GnomeAppApplication *app;
	GtkBuilder *builder;
	GtkWidget *actions;
};

G_DEFINE_TYPE (GnomeAppInfoPage, gnome_app_info_page, GTK_TYPE_BOX)

static void
gnome_app_info_page_init (GnomeAppInfoPage *info_page)
{
	GnomeAppInfoPagePrivate *priv;

	info_page->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (info_page,
	                                                 GNOME_APP_TYPE_INFO_PAGE,
	                                                 GnomeAppInfoPagePrivate);
	priv->builder = NULL;
}

static void
gnome_app_info_page_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->dispose (object);
}

static void
gnome_app_info_page_finalize (GObject *object)
{
	GnomeAppInfoPage *info_page = GNOME_APP_INFO_PAGE (object);
	GnomeAppInfoPagePrivate *priv = info_page->priv;

	if (priv->builder)
		g_object_unref (priv->builder);

	G_OBJECT_CLASS (gnome_app_info_page_parent_class)->finalize (object);
}

static void
info_page_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppInfoPage *info_page;
	
	info_page = GNOME_APP_INFO_PAGE (object);
			        
	switch (prop_id)
	{
		case PROP_APPLICATION:
			info_page->priv->app = g_value_get_object (value);
			break;
	}
}

static void
info_page_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
 	GnomeAppInfoPage *info_page;

	info_page = GNOME_APP_INFO_PAGE (object);

	switch (prop_id)
	{
		case PROP_APPLICATION:
			g_value_set_object (value, info_page->priv->app);
			break;
		case PROP_ACTIONS:
			g_value_set_object (value, info_page->priv->actions);
			break;
	}
}

static void
gnome_app_info_page_class_init (GnomeAppInfoPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = info_page_set_property;
	object_class->get_property = info_page_get_property;
	object_class->dispose = gnome_app_info_page_dispose;
	object_class->finalize = gnome_app_info_page_finalize;

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
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppInfoPagePrivate));
}

static void
info_page_set_actions (GnomeAppInfoPage *info_page)
{
}

GnomeAppInfoPage *
gnome_app_info_page_new ()
{
	GnomeAppInfoPage *info_page;
	GnomeAppInfoPagePrivate *priv;
	gchar *filename;
	GError *error;

	info_page = g_object_new (GNOME_APP_TYPE_INFO_PAGE, NULL);
	priv = info_page->priv;

	filename = "./main_ui.glade";
	error = NULL;
	priv->builder = gtk_builder_new ();
	gtk_builder_add_from_file (priv->builder, filename, &error);
	if (error) {
		g_error ("Error in load main_ui.glade: %s\n", error->message);
		g_free (error);
	}

	GtkWidget *app_info_page_box;

	app_info_page_box = GTK_WIDGET (gtk_builder_get_object (priv->builder, "app_info_page_box"));
	gtk_box_pack_start (GTK_BOX (info_page), app_info_page_box, TRUE, TRUE, 0);

	info_page_set_actions (info_page);

	return info_page;
}

void
gnome_app_info_page_set_with_app (GnomeAppInfoPage *info_page, OpenResult *info)
{
	GtkBuilder *builder;
	GtkWidget *big_image;
	GtkWidget *images_box;
        GtkWidget *description_textview;
	GtkWidget *app_title;
	GtkWidget *author_name;
	GtkWidget *score_box;
	GtkWidget *license;
	GtkWidget *fans;
	GtkWidget *downloads;
	GtkWidget *comments;
	GtkWidget *comment_box;

	builder = info_page->priv->builder;
	big_image = GTK_WIDGET (gtk_builder_get_object (builder, "big_image"));
	images_box = GTK_WIDGET (gtk_builder_get_object (builder, "images_box"));
	description_textview = GTK_WIDGET (gtk_builder_get_object (builder, "description_textview"));
	app_title = GTK_WIDGET (gtk_builder_get_object (builder, "app_title"));
	author_name = GTK_WIDGET (gtk_builder_get_object (builder, "author_name"));
	score_box = GTK_WIDGET (gtk_builder_get_object (builder, "score_box"));
	license = GTK_WIDGET (gtk_builder_get_object (builder, "license"));
	fans = GTK_WIDGET (gtk_builder_get_object (builder, "fans"));
	downloads = GTK_WIDGET (gtk_builder_get_object (builder, "downloads"));
	comments = GTK_WIDGET (gtk_builder_get_object (builder, "comments"));
	comment_box = GTK_WIDGET (gtk_builder_get_object (builder, "comment_box"));

	GdkPixbuf *pixbuf;
	const gchar *val;
	gchar *str;
	gint width, height;

	val = open_result_get (info, "smallpreviewpic1");
	/*TODO: this image should already be downloaded */
	str = open_app_get_local_icon (val, FALSE);
	gtk_widget_get_size_request (big_image, &width, &height);
	pixbuf = gdk_pixbuf_new_from_file_at_scale (str, width, height, FALSE, NULL);
	gtk_image_set_from_pixbuf (GTK_IMAGE (big_image), pixbuf);
	g_object_unref (pixbuf);
	g_free (str);

//TODO images box: for more than one images 

        GtkTextBuffer *buffer;
	val = open_result_get (info, "description");
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (description_textview));
	gtk_text_buffer_set_text (buffer, val, -1);

	val = open_result_get (info, "name");
	gtk_label_set_text (GTK_LABEL (app_title), val);

	val = open_result_get (info, "personid");
	gtk_label_set_text (GTK_LABEL (author_name), val);

	gint score, i;
	GtkWidget *image;
	GdkPixbuf *star, *nostar;
	gchar *filename;

	filename = open_app_get_pixmap_uri ("starred");
	star = gdk_pixbuf_new_from_file_at_scale (filename, 24, 24, FALSE, NULL);
	g_free (filename);
	filename = open_app_get_pixmap_uri ("non-starred");
	nostar = gdk_pixbuf_new_from_file_at_scale (filename, 24, 24, FALSE, NULL);
	g_free (filename);

	val = open_result_get (info, "score");
	score = atoi (val);
	for (i = 0; i < 5; i++) {
		if (i * 25 < score)
			image = gtk_image_new_from_pixbuf (star);
		else
			image = gtk_image_new_from_pixbuf (nostar);
		gtk_box_pack_start (GTK_BOX (score_box), image, TRUE, TRUE, 0);
		gtk_widget_show (image);
	}
	g_object_unref (star);
	g_object_unref (nostar);

	val = open_result_get (info, "license");
	gtk_label_set_text (GTK_LABEL (license), val);

	str = g_strdup_printf ("%s fans", open_result_get (info, "fans"));
	gtk_label_set_text (GTK_LABEL (fans), str);
	g_free (str);

	str = g_strdup_printf ("%s downloads", open_result_get (info, "downloads"));
	gtk_label_set_text (GTK_LABEL (downloads), str);
	g_free (str);

	str = g_strdup_printf ("%s comments", open_result_get (info, "comments"));
	gtk_label_set_text (GTK_LABEL (comments), str);
	g_free (str);
}
