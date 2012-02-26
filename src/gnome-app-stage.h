/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
   Copyright 2011, Novell, Inc.

   The Gnome appinfo lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appinfo lib is distributed in the hope that it will be useful,
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

#include <glib.h>
#include <clutter/clutter.h>

#include "open-result.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_STAGE            (gnome_app_stage_get_type ())
#define GNOME_APP_STAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_STAGE, GnomeAppStage))
#define GNOME_APP_STAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_STAGE, GnomeAppStageClass))
#define GNOME_APP_IS_STAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_STAGE))
#define GNOME_APP_IS_STAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_STAGE))
#define GNOME_APP_STAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_STAGE, GnomeAppStageClass))

typedef struct _GnomeAppStage GnomeAppStage;
typedef struct _GnomeAppStageClass GnomeAppStageClass;
typedef struct _GnomeAppStagePrivate GnomeAppStagePrivate;

struct _GnomeAppStage
{
        ClutterStageClass       parent_instance;
	GnomeAppStagePrivate	*priv;
};

struct _GnomeAppStageClass
{
        ClutterStageClass parent_class;
};

enum {
	GNOME_APP_STAGE_POSITION_CENTER,
	GNOME_APP_STAGE_POSITION_MOUSE,
};

GType			gnome_app_stage_get_type	(void);
GnomeAppStage *		gnome_app_stage_new		(void);
GnomeAppStage *		gnome_app_stage_get_default     (void);
void			gnome_app_stage_move            (ClutterActor *stage, gint x, gint y);
void			gnome_app_stage_set_position	(ClutterActor *stage, gint position);
void			gnome_app_stage_remove_decorate (ClutterActor *stage);
void			gnome_app_stage_load		(GnomeAppStage *app_stage, const gchar *type_name, ...);
void			gnome_app_stage_close		(GnomeAppStage *app_stage);
G_MODULE_EXPORT	void	gnome_app_default_stage_close	(void);

G_END_DECLS

#endif
