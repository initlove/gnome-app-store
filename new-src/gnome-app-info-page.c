/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/

#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "open-app-utils.h"
#include "gnome-app-task.h"
#include "gnome-app-info-page.h"

GtkWidget *
gnome_app_info_page_new_with_app (OpenResult *info)
{
	GtkBuilder *builder;
	GError *error;
	gchar *filename;

	error = NULL;
	filename = "./main_ui.glade";
	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, filename, &error);
	
	GtkWidget *app_info_page_box;
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

	app_info_page_box = GTK_WIDGET (gtk_builder_get_object (builder, "app_info_page_box"));
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

	return app_info_page_box;
}
