#include <stdio.h>
#include <string.h>
#include <glib/gdir.h>

#ifndef GMENU_I_KNOW_THIS_IS_UNSTABLE
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <gmenu-tree.h>

#include "gnome-app-config.h"
#include "local-app.h"

        
static GHashTable *icon_store = NULL;
static GSList *local_app_store = NULL;

static void
gather_entries_recurse (GMenuTreeDirectory *trunk)
{
        GSList *contents;
        GSList *iter;
        contents = gmenu_tree_directory_get_contents (trunk);

        for (iter = contents; iter; iter = iter->next) {
                GMenuTreeItem *item = iter->data;
                GMenuTreeEntry *entry;
                GMenuTreeDirectory *dir;
		const gchar *id;

                switch (gmenu_tree_item_get_type (item)) {
                        case GMENU_TREE_ITEM_ENTRY:
				entry = GMENU_TREE_ENTRY (item);
				id = gmenu_tree_entry_get_desktop_file_id (entry);
				local_app_store = g_slist_prepend (local_app_store, g_strdup (id));
                                break;
                        case GMENU_TREE_ITEM_DIRECTORY:
                                dir = GMENU_TREE_DIRECTORY (item);
                                gather_entries_recurse (dir);
                                break;
                        default:
                                break;
                }
		gmenu_tree_item_unref (item);
	}
	g_slist_free (contents);
}

static void
init_local_app_store ()
{
        GMenuTree *apps_tree;
        GMenuTreeDirectory *trunk;

        apps_tree = gmenu_tree_lookup ("applications.menu", GMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
        trunk = gmenu_tree_get_root_directory (apps_tree);
	gather_entries_recurse (trunk);

        gmenu_tree_item_unref (trunk);
	gmenu_tree_unref (apps_tree);
}

/*FIXME: add monitor here */
gboolean
is_local_app (const char *id)
{
	if (!local_app_store)
		init_local_app_store ();

	GSList *l;
	gchar *local_id;
	for (l = local_app_store; l; l = l->next) {
		local_id = (gchar *) l->data;
		if (strcmp (id, local_id) == 0)
			return TRUE;
	}
	return FALSE;
}

static void
load_icons ()
{
        gchar *icon_dir;
        GnomeAppConfig *conf;
        GDir *dir;

        conf = gnome_app_config_new ();
	icon_dir = g_build_filename (gnome_app_config_get_cache_dir (conf), "icons", NULL);
        g_object_unref (conf);
        dir = g_dir_open (icon_dir, 0, NULL);

        if (!dir) {
                printf ("cannot open dir\n");
                g_free (icon_dir);
                return;
        }

        const gchar *basename;
        gchar *filename;
        gchar *p, *content;

        while ((basename = g_dir_read_name (dir)) != NULL) {
                filename = g_build_filename (icon_dir, basename, NULL);
                content = g_strdup (basename);
                p = strrchr (content, '.');
                if (p)
                        *p = 0;

                g_hash_table_insert (icon_store, content, filename);
        }
        g_dir_close (dir);
        g_free (icon_dir);
}


const gchar *
get_icon_uri (gchar *icon)
{
	if (!icon_store) {
		icon_store = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
		load_icons ();
	}

	return g_hash_table_lookup (icon_store, icon);
}

gchar *
get_default_icon ()
{
	gchar *default_icon;
	gchar *cache_dir;
        GnomeAppConfig *conf;

        conf = gnome_app_config_new ();
        cache_dir = gnome_app_config_get_cache_dir (conf);
	default_icon = g_build_filename (cache_dir, "icons", "default.png", NULL);
        g_object_unref (conf);

	return default_icon;
}

void
debug_local_apps ()
{
	if (!local_app_store)
		init_local_app_store ();

	GSList *l;
	for (l = local_app_store; l; l = l->next) {
		printf ("id <%s>\n", l->data);
	}
}
