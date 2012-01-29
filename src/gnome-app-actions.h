/* libgnome-app-actions.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appactions lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appactions lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_ACTIONS_H__
#define __GNOME_APP_ACTIONS_H__

#include <clutter/clutter.h>
#include "gnome-app-application.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_ACTIONS            (gnome_app_actions_get_type ())
#define GNOME_APP_ACTIONS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_ACTIONS, GnomeAppActions))
#define GNOME_APP_ACTIONS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_ACTIONS, GnomeAppActionsClass))
#define GNOME_APP_IS_ACTIONS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_ACTIONS))
#define GNOME_APP_IS_ACTIONS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_ACTIONS))
#define GNOME_APP_ACTIONS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_ACTIONS, GnomeAppActionsClass))

typedef struct _GnomeAppActions GnomeAppActions;
typedef struct _GnomeAppActionsClass GnomeAppActionsClass;
typedef struct _GnomeAppActionsPrivate GnomeAppActionsPrivate;

struct _GnomeAppActions
{
	ClutterGroup parent_instance;
	GnomeAppActionsPrivate	*priv;
};

struct _GnomeAppActionsClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_actions_get_type              (void);
GnomeAppActions *	gnome_app_actions_new_with_app          (GnomeAppApplication *app);
void			gnome_app_actions_set_with_data		(GnomeAppActions *actions, ClutterActor *actor);

G_END_DECLS

#endif
