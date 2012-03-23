/*
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Spagete 330,
Boston, MA 02111-1307, USA.

Author:  David Liang <dliang@novell.com>
*/
#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "gnome-app-widgets.h"
#include "gnome-app-task.h"
#include "gnome-app-stage.h"
#include "gnome-app-download.h"
#include "gnome-app-ui-utils.h"


struct _GnomeAppDownloadPrivate
{
	OpenResult *info;
	gfloat balance;
	gboolean download_expand;
	gint download_count;
	gint download_links;

	ClutterActor *button;
	ClutterLayoutManager *layout;
	ClutterActor *layout_box;
	ClutterLayoutManager *table_layout;
	ClutterActor *table_layout_box;
};

/* Properties */
enum
{
	PROP_0,
	PROP_APP_INFO,
	PROP_LAST
};

G_DEFINE_TYPE (GnomeAppDownload, gnome_app_download, CLUTTER_TYPE_GROUP)

static gboolean	on_download_item_press (ClutterActor *actor,
					ClutterEvent *event,
			                gpointer      data);
static void	gnome_app_download_set_with_data (GnomeAppDownload *download, OpenResult *info);

static void
set_download_expand (GnomeAppDownload *download)
{
	GnomeAppDownloadPrivate *priv;

	priv = download->priv;
	if (priv->download_expand) {
		clutter_actor_show (CLUTTER_ACTOR (priv->table_layout_box));
		gnome_app_button_set_selected (GNOME_APP_BUTTON (priv->button), TRUE);
	} else {
		clutter_actor_hide (CLUTTER_ACTOR (priv->table_layout_box));
		gnome_app_button_set_selected (GNOME_APP_BUTTON (priv->button), FALSE);
	}
}

static gboolean
on_download_button_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppDownload *download;
	GnomeAppDownloadPrivate *priv;
	GnomeAppTask *task;
	gchar *function;

	download = GNOME_APP_DOWNLOAD (data);
	priv = download->priv;

	priv->download_expand = !priv->download_expand;

	set_download_expand (download);

	return TRUE;
}

static void
gnome_app_download_init (GnomeAppDownload *download)
{
	GnomeAppDownloadPrivate *priv;

	download->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (download,
							 GNOME_APP_TYPE_DOWNLOAD,
							 GnomeAppDownloadPrivate);
	priv->info = NULL;
	priv->download_expand = FALSE;
    priv->download_count = 0;
	priv->balance = gnome_app_stage_get_balance (gnome_app_stage_get_default ());
	priv->layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (priv->layout), TRUE);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (priv->layout), 20);
	priv->layout_box = clutter_box_new (priv->layout);

	priv->button = CLUTTER_ACTOR (gnome_app_button_new ());
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->layout), priv->button,
			FALSE,  /* expand */
			FALSE, /* x-fill */
			FALSE, /* y-fill */
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	priv->table_layout = clutter_table_layout_new ();
	clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (priv->table_layout), 10);
	clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (priv->table_layout), 10);
	priv->table_layout_box = clutter_box_new (priv->table_layout);
	clutter_box_layout_pack (CLUTTER_BOX_LAYOUT (priv->layout), priv->table_layout_box,
			FALSE,  /* expand */
			FALSE, /* x-fill */
			FALSE, /* y-fill */
			CLUTTER_BOX_ALIGNMENT_START,
			CLUTTER_BOX_ALIGNMENT_START);

	clutter_actor_hide (CLUTTER_ACTOR (priv->table_layout_box));
	clutter_container_add_actor (CLUTTER_CONTAINER (download), priv->layout_box);

	g_signal_connect (priv->button, "button-press-event", G_CALLBACK (on_download_button_press), download);
}

static void
gnome_app_download_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	GnomeAppDownload *download;
	GnomeAppDownloadPrivate *priv;

	download = GNOME_APP_DOWNLOAD (object);
	priv = download->priv;

	switch (prop_id)
	{
		case PROP_APP_INFO:
			gnome_app_download_set_with_data (download, g_value_get_object (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_download_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	GnomeAppDownload *download;
	GnomeAppDownloadPrivate *priv;

	download = GNOME_APP_DOWNLOAD (object);
	priv = download->priv;

	switch (prop_id)
	{
		case PROP_APP_INFO:
			g_value_set_object (value, priv->info);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gnome_app_download_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_download_parent_class)->dispose (object);
}

static void
gnome_app_download_finalize (GObject *object)
{
	GnomeAppDownload *page = GNOME_APP_DOWNLOAD (object);
	GnomeAppDownloadPrivate *priv = page->priv;

	if (priv->info)
		g_object_unref (priv->info);

	G_OBJECT_CLASS (gnome_app_download_parent_class)->finalize (object);
}

static void
gnome_app_download_class_init (GnomeAppDownloadClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gnome_app_download_set_property;
	object_class->get_property = gnome_app_download_get_property;
	object_class->dispose = gnome_app_download_dispose;
	object_class->finalize = gnome_app_download_finalize;

	g_object_class_install_property (object_class,
  			PROP_APP_INFO,
			g_param_spec_object ("info",
				"app info",
				"app info",
				G_TYPE_OBJECT,
				G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GnomeAppDownloadPrivate));
}

static void
proxy_call_async_cb (RestProxyCall *call,
                const GError  *error,
                        GObject       *weak_object,
                                gpointer       userdata)
{
	GnomeAppDownload *page;
	GnomeAppDownloadPrivate *priv;
    const gchar *payload;
    goffset len;
	OpenResults *results;

	page = GNOME_APP_DOWNLOAD (userdata);
	priv = page->priv;

    payload = rest_proxy_call_get_payload (call);
    len = rest_proxy_call_get_payload_length (call);
    results = (OpenResults *) open_ocs_get_results (payload, len);
    if (!open_results_get_status (results)) {
		g_debug ("Fail to get the download info: %s\n", open_results_get_meta (results, "message"));
	} else {
		GList *list;
		OpenResult *result;
		gchar *str;
		ClutterActor *download_count;
		const gchar *repo, *pkgname;
		GError *error;
		gchar *cmd;

		list = open_results_get_data (results);
		result = OPEN_RESULT (list->data);
		repo = open_result_get (result, "repo");
        pkgname = open_result_get (result, "pkgname");

printf ("repo %s, pkgname %s\n", repo, pkgname);
#if 0
		if (uri) {
			cmd = g_strdup_printf ("gnome-open \"%s\" ", uri);
			error = NULL;
			if (!g_spawn_command_line_async (cmd, &error)) {
				g_debug ("Error in run cmd %s: %s\n", cmd, error->message);
				g_error_free (error);
			}
			g_free (cmd);
		}
#endif
		priv->download_count ++;
		str = g_strdup_printf (_("%d downloads  (%d link)"), priv->download_count, priv->download_links);
		gnome_app_button_set_text (GNOME_APP_BUTTON (priv->button), str);
		g_free (str);
	}
}

static gboolean
on_download_item_press (ClutterActor *actor,
                ClutterEvent *event,
                gpointer      data)
{
	GnomeAppDownload *page;
	GnomeAppDownloadPrivate *priv;
    const RestProxy *proxy;
    RestProxyCall *call;
 	gchar *function;
	const gchar *content_id;
	const gchar *item_id;

	page = GNOME_APP_DOWNLOAD (data);
	priv = page->priv;

	content_id = open_result_get (priv->info, "id");
	item_id = (gchar *) g_object_get_data (G_OBJECT (actor), "itemid");
	function = g_strdup_printf ("content/download/%s/%s", content_id, item_id);

    proxy = gnome_app_get_proxy ();
    call = rest_proxy_new_call (proxy);
    rest_proxy_call_set_function (call, function);
    rest_proxy_call_async (call,
            proxy_call_async_cb,
            NULL,
            page,
            NULL);
		
	g_free (function);

    return TRUE;
}

GnomeAppDownload *
gnome_app_download_new (void)
{
	GnomeAppDownload *download;

	download = g_object_new (GNOME_APP_TYPE_DOWNLOAD, NULL);

	return download;
}

GnomeAppDownload *
gnome_app_download_new_with_info (OpenResult *info)
{
	GnomeAppDownload *download;

	download = g_object_new (GNOME_APP_TYPE_DOWNLOAD, NULL);
	gnome_app_download_set_with_data (download, info);

	return download;
}

static void
gnome_app_download_set_with_data (GnomeAppDownload *download, OpenResult *info)
{
	g_return_if_fail (info != NULL);

	GnomeAppDownloadPrivate *priv;
	ClutterActor *download_name;
	ClutterActor *download_price;
	gboolean balance_display;
	const gchar *val;
	gchar *str;
	gint i;
	gfloat price;

	priv = download->priv;
	priv->info = g_object_ref (info);
	balance_display = FALSE;
	val = open_result_get (info, "downloads");
    if (val)
    	priv->download_count = atoi (val);
    GList *list, *l;
    list = open_result_get_child (info, "downloadinfos");
    i = 1;
	/*OCS standard, type begin with 1 .*/
	for (l = list; l; l=l->next) {
        OpenResult *d_info;
        d_info = (OpenResult *) l->data;        
		download_name = clutter_text_new ();
        download_price = CLUTTER_ACTOR (gnome_app_button_new ());
        val = open_result_get (d_info, "name");
		if (val && val [0]) {
			clutter_text_set_text (CLUTTER_TEXT (download_name), (gchar *)val);
		} else {
			clutter_text_set_text (CLUTTER_TEXT (download_name), _("Download Link"));
		}
		
        val = open_result_get (d_info, "price");
        price = atof (val);
		if (price == 0) {
			gnome_app_button_set_text (GNOME_APP_BUTTON (download_price), _("Free"));
			g_object_set (G_OBJECT (download_price), "text-color", "green", NULL);
		} else {
			if (!balance_display)
				balance_display = TRUE;
			gnome_app_button_set_text (GNOME_APP_BUTTON (download_price), (gchar *)val);
			if (price > priv->balance)
				g_object_set (G_OBJECT (download_price), "text-color", "red", NULL);
			else
				g_object_set (G_OBJECT (download_price), "text-color", "yellow", NULL);
		}
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->table_layout), CLUTTER_ACTOR (download_price), 0, i);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->table_layout), CLUTTER_ACTOR (download_name), 1, i);
	    g_object_set_data (G_OBJECT (download_price), "itemid", (gpointer) (open_result_get (d_info, "way")));
		g_signal_connect (download_price, "button-press-event", G_CALLBACK (on_download_item_press), download);
        i++;
	}
	priv->download_links = i - 1;
	str = g_strdup_printf (_("%d downloads  (%d link)"), priv->download_count, priv->download_links);
	gnome_app_button_set_text (GNOME_APP_BUTTON (priv->button), str);
	g_free (str);

	if (balance_display) {
		ClutterActor *actor;

		actor = clutter_text_new ();
		clutter_text_set_text (CLUTTER_TEXT (actor), _("Your balance is: "));
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->table_layout), CLUTTER_ACTOR (actor), 0, 0);

		str = g_strdup_printf ("%f", priv->balance);
		actor = clutter_text_new ();
		clutter_text_set_text (CLUTTER_TEXT (actor), str);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (priv->table_layout), CLUTTER_ACTOR (actor), 1, 0);
		g_free (str);
	}
}
