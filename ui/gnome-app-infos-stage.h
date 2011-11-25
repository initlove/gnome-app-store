/* libgnome-app-infos_stage.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appinfos_stage lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfos_stage lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_INFOS_STAGE_H__
#define __GNOME_APP_INFOS_STAGE_H__

#include <clutter/clutter.h>
#include "gnome-app-store.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_INFOS_STAGE            (gnome_app_infos_stage_get_type ())
#define GNOME_APP_INFOS_STAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_INFOS_STAGE, GnomeAppInfosStage))
#define GNOME_APP_INFOS_STAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_INFOS_STAGE, GnomeAppInfosStageClass))
#define GNOME_APP_IS_INFOS_STAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_INFOS_STAGE))
#define GNOME_APP_IS_INFOS_STAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_INFOS_STAGE))
#define GNOME_APP_INFOS_STAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_INFOS_STAGE, GnomeAppInfosStageClass))

typedef struct _GnomeAppInfosStage GnomeAppInfosStage;
typedef struct _GnomeAppInfosStageClass GnomeAppInfosStageClass;
typedef struct _GnomeAppInfosStagePrivate GnomeAppInfosStagePrivate;

struct _GnomeAppInfosStage
{
	ClutterGroup parent_instance;
	GnomeAppInfosStagePrivate	*priv;
};

struct _GnomeAppInfosStageClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_infos_stage_get_type              	(void);
GnomeAppInfosStage *    gnome_app_infos_stage_new                   	(void);
GnomeAppInfosStage *	gnome_app_infos_stage_new_with_store		(GnomeAppStore *store);
void			gnome_app_infos_stage_clean			(GnomeAppInfosStage *infos_stage);
void			gnome_app_infos_stage_add_actor			(GnomeAppInfosStage *infos_stage, ClutterActor *actor);
void			gnome_app_infos_stage_load			(GnomeAppInfosStage *infos_stage, const GList *data);
gint			gnome_app_infos_stage_get_pagesize		(GnomeAppInfosStage *infos_stage);

G_END_DECLS

#endif
