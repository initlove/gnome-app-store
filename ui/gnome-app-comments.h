/* libgnome-app-comments.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appcomments lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appcomments lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_COMMENTS_H__
#define __GNOME_APP_COMMENTS_H__

#include <clutter/clutter.h>
#include "gnome-app-store.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_COMMENTS            (gnome_app_comments_get_type ())
#define GNOME_APP_COMMENTS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_COMMENTS, GnomeAppComments))
#define GNOME_APP_COMMENTS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_COMMENTS, GnomeAppCommentsClass))
#define GNOME_APP_IS_COMMENTS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_COMMENTS))
#define GNOME_APP_IS_COMMENTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_COMMENTS))
#define GNOME_APP_COMMENTS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_COMMENTS, GnomeAppCommentsClass))

typedef struct _GnomeAppComments GnomeAppInfosStage;
typedef struct _GnomeAppCommentsClass GnomeAppInfosStageClass;
typedef struct _GnomeAppCommentsPrivate GnomeAppInfosStagePrivate;

struct _GnomeAppComments
{
	ClutterGroup parent_instance;
	GnomeAppCommentsPrivate	*priv;
};

struct _GnomeAppCommentsClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_comments_get_type              	(void);
GnomeAppComments *    	gnome_app_comments_new                   	(void);
void			gnome_app_comments_load				(GnomeAppComments *comments, const GList *data);
void			gnome_app_comments_clean			(GnomeAppComments *comments);

G_END_DECLS

#endif
