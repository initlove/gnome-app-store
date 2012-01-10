/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <clutter/clutter.h>

#include "open-app-utils.h"
#include "open-results.h"
#include "open-result.h"
#include "gnome-app-task.h"
#include "gnome-app-store.h"
#include "gnome-app-friends-ui.h"

struct _GnomeAppFriendsUIPrivate
{
	gchar 		*person_id;
	gint		friends_count;
};

G_DEFINE_TYPE (GnomeAppFriendsUI, gnome_app_friends_ui, CLUTTER_TYPE_GROUP)

static void
gnome_app_friends_ui_init (GnomeAppFriendsUI *friends_ui)
{
	GnomeAppFriendsUIPrivate *priv;

	friends_ui->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (friends_ui,
	                                                 GNOME_APP_TYPE_FRIENDS_UI,
	                                                 GnomeAppFriendsUIPrivate);
			
	priv->person_id = NULL;
	priv->friends_count = -1;
}

static void
gnome_app_friends_ui_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_friends_ui_parent_class)->dispose (object);
}

static void
gnome_app_friends_ui_finalize (GObject *object)
{
	GnomeAppFriendsUI *friends_ui = GNOME_APP_FRIENDS_UI (object);
	GnomeAppFriendsUIPrivate *priv = friends_ui->priv;

	if (priv->person_id)
		g_free (priv->person_id);

	G_OBJECT_CLASS (gnome_app_friends_ui_parent_class)->finalize (object);
}

static void
gnome_app_friends_ui_class_init (GnomeAppFriendsUIClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_friends_ui_dispose;
	object_class->finalize = gnome_app_friends_ui_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppFriendsUIPrivate));
}

/*TODO: better way to merge this to the main class */
/*TODO: use set data */
typedef struct _FriendUIData {
	ClutterActor *icon;
	ClutterActor *info;
} FriendUIData;

static gpointer
add_friend_callback (gpointer userdata, gpointer func_result)
{
	FriendUIData *data;
	OpenResult *result;
	OpenResults *results;
	GList *list;
	const gchar *val;
	const gchar *pic;

	data = (FriendUIData *) userdata;
	results = OPEN_RESULTS (func_result);
        if (!open_results_get_status (results)) {
		/*TODO: fill the account with default val */
		g_debug ("Fail to get the user info: %s\n", open_results_get_meta (results, "message"));
		return NULL;
	} else {
		list = open_results_get_data (results);
		result = list->data;

		val = open_result_get (result, "avatarpicfound");
		if (val && (strcmp (val, "1") == 0)) {
			pic = open_result_get (result, "avatarpic");
			if (pic) {
				gnome_app_ui_set_icon (data->icon, pic);
			}
		} else {
			val = open_result_get (result, "bigavatarpicfound");
			if (val && (strcmp (val, "1") == 0)) {
				pic = open_result_get (result, "bigavatarpic");
				if (pic) {
					gnome_app_ui_set_icon (data->icon, pic);
				}
			}				
		}

		const gchar *first;
		const gchar *last;
		gchar *name;
		
		first = open_result_get (result, "firstname");
		last = open_result_get (result, "lastname");
		/*TODO there should have 'space' between name, this is to make my login name looks better */
		name = g_strdup_printf ("%s%s", first, last);
		clutter_text_set_text (CLUTTER_TEXT (data->info), name);

		g_free (name);
	}

	return NULL;
}

static void
add_friend (ClutterActor *icon, ClutterActor *info, const gchar *person_id)
{
	FriendUIData *data;
	GnomeAppTask *task;
	gchar *function;

	/*TODO: need to free this */
	data = g_new0(FriendUIData, 1);
	data->icon = icon;
	data->info = info;

	function = g_strdup_printf ("/v1/person/data/%s",  person_id);
	task = gnome_app_task_new (data, "GET", function);
	gnome_app_task_set_callback (task, add_friend_callback);
	gnome_app_task_push (task);

	g_free (function);
}


static gboolean
on_close_press (ClutterActor *actor,
		ClutterEvent *event,
		gpointer      data)
{
	GnomeAppFriendsUI *friends_ui;
	friends_ui = GNOME_APP_FRIENDS_UI (data);
	printf ("on close press\n");
	clutter_actor_destroy (CLUTTER_ACTOR (friends_ui));

	return TRUE;
}

static gpointer
set_friends_callback (gpointer userdata, gpointer func_result)
{
	GnomeAppFriendsUI *friends_ui;
        OpenResult *result;
	OpenResults *results;
        GList *l;
	const gchar *val;

	friends_ui = GNOME_APP_FRIENDS_UI (userdata);
 	results = OPEN_RESULTS (func_result);

	if (!open_results_get_status (results)) {
		/*TODO: fill the friends with default val */
		g_debug ("Fail to get the friend info of %s: %s\n", friends_ui->priv->person_id, open_results_get_meta (results, "message"));
		return NULL;
	} else {
		friends_ui->priv->friends_count = open_results_get_total_items (results);
		/*TODO: if none friends, different ui */

		ClutterLayoutManager *layout;
	        ClutterActor *layout_box, *info, *icon;
		gchar *str;
		gint col, row;

		col = row = 0;
		layout = clutter_table_layout_new ();
		clutter_table_layout_set_row_spacing (CLUTTER_TABLE_LAYOUT (layout), 10);
		clutter_table_layout_set_column_spacing (CLUTTER_TABLE_LAYOUT (layout), 20);
		layout_box = clutter_box_new (layout);

		info = clutter_text_new ();
		str = g_strdup_printf ("%d Friends", friends_ui->priv->friends_count);
		clutter_text_set_text (CLUTTER_TEXT (info), str);
		g_free (str);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (info), col, row);

		icon = clutter_texture_new ();
		clutter_actor_set_reactive (icon, TRUE);
		str = open_app_get_pixmap_uri ("close");
		clutter_texture_set_from_file (CLUTTER_TEXTURE (icon), str, NULL);
		g_free (str);
		clutter_actor_set_width (icon, 16);
		clutter_actor_set_height (icon, 16);
		clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon), col + 2, row);
		clutter_table_layout_set_alignment (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon), 
						CLUTTER_TABLE_ALIGNMENT_END, CLUTTER_TABLE_ALIGNMENT_END);
		clutter_table_layout_set_expand (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon),
						FALSE, FALSE);
		clutter_table_layout_set_fill (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon),
						FALSE, FALSE);
		g_signal_connect (icon, "button-press-event", G_CALLBACK (on_close_press), friends_ui);

		row ++;
		for (l = open_results_get_data (results); l ; l = l->next) {
			const gchar *personid;

			result = OPEN_RESULT (l->data);
			personid = open_result_get (result, "personid");

			icon = clutter_texture_new ();
			str = open_app_get_pixmap_uri ("person");
			clutter_texture_set_from_file (CLUTTER_TEXTURE (icon), str, NULL);
			g_free (str);
			clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon), col, row);
			clutter_table_layout_set_expand (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon),
						FALSE, FALSE);
			clutter_table_layout_set_fill (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (icon),
						FALSE, FALSE);

			info = clutter_text_new ();
			clutter_text_set_text (CLUTTER_TEXT (info), personid);
			clutter_table_layout_pack (CLUTTER_TABLE_LAYOUT (layout), CLUTTER_ACTOR (info), col + 1, row);
			row ++;

			add_friend (icon, info, personid);
			/*TODO: send msg */
//			g_signal_connect (actor, "event", G_CALLBACK (on_category_event), ui);
		}
		clutter_container_add_actor (CLUTTER_CONTAINER (friends_ui), CLUTTER_ACTOR (layout_box));
	}

	return NULL;
}

/*TODO: background color */
GnomeAppFriendsUI *
gnome_app_friends_ui_new (const gchar *personid)
{
	GnomeAppFriendsUI *friends_ui;
	GnomeAppFriendsUIPrivate *priv;
	const gchar *id = NULL;

	if (personid) {
		id = personid;
	} else {
		/*Display the login friends */
		GnomeAppStore *store;

		store = gnome_app_store_get_default ();
		id = gnome_app_store_get_username (store);
	}

	if (id) {
		friends_ui = g_object_new (GNOME_APP_TYPE_FRIENDS_UI, NULL);
		priv = friends_ui->priv;
		priv->person_id = g_strdup (id);

		GnomeAppTask *task;
		gchar *function;

		function = g_strdup_printf ("/v1/friend/data/%s",  priv->person_id);
		task = gnome_app_task_new (friends_ui, "GET", function);
		gnome_app_task_add_params (task,
				"page",	"0",
				"pagesize", "10",
				NULL);
		gnome_app_task_set_callback (task, set_friends_callback);
		gnome_app_task_push (task);

		g_free (function);

		return friends_ui;
	} else {
		return NULL;
	}
}
