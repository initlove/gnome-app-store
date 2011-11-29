/* libgnome-app-comment.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appcomment lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appcomment lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __GNOME_APP_COMMENT_H__
#define __GNOME_APP_COMMENT_H__

#include <clutter/clutter.h>
#include "open-result.h"

G_BEGIN_DECLS

#define GNOME_APP_TYPE_COMMENT            (gnome_app_comment_get_type ())
#define GNOME_APP_COMMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOME_APP_TYPE_COMMENT, GnomeAppComment))
#define GNOME_APP_COMMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GNOME_APP_TYPE_COMMENT, GnomeAppCommentClass))
#define GNOME_APP_IS_COMMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_APP_TYPE_COMMENT))
#define GNOME_APP_IS_COMMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GNOME_APP_TYPE_COMMENT))
#define GNOME_APP_COMMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GNOME_APP_TYPE_COMMENT, GnomeAppCommentClass))

typedef struct _GnomeAppComment GnomeAppComment;
typedef struct _GnomeAppCommentClass GnomeAppCommentClass;
typedef struct _GnomeAppCommentPrivate GnomeAppCommentPrivate;

struct _GnomeAppComment
{
	ClutterGroup parent_instance;
	GnomeAppCommentPrivate	*priv;
};

struct _GnomeAppCommentClass
{
        ClutterGroupClass parent_class;
};

GType			gnome_app_comment_get_type              	(void);
GnomeAppComment *	gnome_app_comment_new_with_comment		(OpenResult *comment);

G_END_DECLS

#endif
