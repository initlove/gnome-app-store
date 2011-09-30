/* libgnome-app-stage.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appstage lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appstage lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_STAGE_H__
#define __GNOME_APP_STAGE_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GNOME_TYPE_APP_STAGE            (gnome_app_stage_get_type ())
#define GNOME_APP_STAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_TYPE_APP_STAGE, GnomeAppStage))
#define GNOME_APP_STAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_TYPE_APP_STAGE, GnomeAppStageClass))
#define GNOME_IS_APP_STAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_APP_STAGE))
#define GNOME_IS_APP_STAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_TYPE_APP_STAGE))
#define GNOME_APP_STAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_TYPE_APP_STAGE, GnomeAppStageClass))

typedef struct _GnomeAppStage GnomeAppStage;
typedef struct _GnomeAppStageClass GnomeAppStageClass;
typedef struct _GnomeAppStagePrivate GnomeAppStagePrivate;

struct _GnomeAppStage
{
	ClutterGroup parent_instance;
	
	GnomeAppStagePrivate	*priv;
};

struct _GnomeAppStageClass
{
        ClutterGroupClass parent_class;
};

GType  		          gnome_app_stage_get_type              (void);
GnomeAppStage *        	  gnome_app_stage_new                   (void);
void			  gnome_app_stage_add_actor		(GnomeAppStage *stage, ClutterActor *actor);

G_END_DECLS

#endif
