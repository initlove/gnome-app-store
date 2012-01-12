/*
   Copyright 2012, Novell, Inc.

   The Gnome App Store is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appdemo lib is distributed in the hope that it will be useful,
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
#include "gnome-app-demo.h"

struct _GnomeAppDemoPrivate
{
};

G_DEFINE_TYPE (GnomeAppDemo, gnome_app_demo, GTK_TYPE_BOX)

static void
gnome_app_demo_init (GnomeAppDemo *demo)
{
	GnomeAppDemoPrivate *priv;

	demo->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (demo,
	                                                 GNOME_APP_TYPE_DEMO,
	                                                 GnomeAppDemoPrivate);
}

static void
gnome_app_demo_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_demo_parent_class)->dispose (object);
}

static void
gnome_app_demo_finalize (GObject *object)
{
	GnomeAppDemo *demo = GNOME_APP_DEMO (object);
	GnomeAppDemoPrivate *priv = demo->priv;

	G_OBJECT_CLASS (gnome_app_demo_parent_class)->finalize (object);
}

static void
gnome_app_demo_class_init (GnomeAppDemoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_demo_dispose;
	object_class->finalize = gnome_app_demo_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppDemoPrivate));
}

GnomeAppDemo *
gnome_app_demo_new (const gchar *personid)
{
	GnomeAppDemo *demo;
	GnomeAppDemoPrivate *priv;
		
	demo = g_object_new (GNOME_APP_TYPE_DEMO, NULL);
	priv = demo->priv;
		
	return demo;
}
