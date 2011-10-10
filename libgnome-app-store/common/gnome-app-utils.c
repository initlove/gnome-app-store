/* gnome-app-utils.c -

   Copyright 2011

   The Gnome appitem lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appitem lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Liang chenye <liangchenye@gmail.com>
*/
#ifndef GMENU_I_KNOW_THIS_IS_UNSTABLE
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <gmenu-tree.h>

#include "gnome-app-utils.h"

	
static GList *local_categories = NULL;

static void
gather_entries_recurse (GMenuTreeDirectory *trunk)
{
        GSList *contents;
        GSList *iter;
        contents = gmenu_tree_directory_get_contents (trunk);

        for (iter = contents; iter; iter = iter->next) {
                GMenuTreeItem *item = iter->data;
                GMenuTreeDirectory *dir;
                const gchar *name;

                switch (gmenu_tree_item_get_type (item)) {
                        case GMENU_TREE_ITEM_DIRECTORY:
                                dir = GMENU_TREE_DIRECTORY (item);
				name = gmenu_tree_directory_get_name (dir);
				local_categories = g_list_prepend (local_categories, g_strdup (name));
				/*FIXME: if the return value of get_local_categories change to a tree on day.
				  we should enable this ...
                                gather_entries_recurse (dir);
				*/
                                break;
                        default:
                                break;
                }
                gmenu_tree_item_unref (item);
        }
        g_slist_free (contents);
}

const GList *
gnome_app_get_local_categories ()
{
	if (local_categories)
		return local_categories;

	GMenuTree *apps_tree;
        GMenuTreeDirectory *trunk;

        apps_tree = gmenu_tree_lookup ("applications.menu", GMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
        trunk = gmenu_tree_get_root_directory (apps_tree);
        gather_entries_recurse (trunk);

        gmenu_tree_item_unref (trunk);
        gmenu_tree_unref (apps_tree);

	return (const GList *) local_categories;
}


