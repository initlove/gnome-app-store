/* open-services.h - 

   Copyright 2011, Novell, Inc.

   The Gnome appservices lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Gnome appservices lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __OPEN_SERVICES_H__
#define __OPEN_SERVICES_H__

#include "open-request.h"
#include "open-result.h"

G_BEGIN_DECLS


#define COMMENT_REQUEST 	OPEN_REQUEST
#define	CommentRequest		OpenRequest
#define comment_request_get	open_request_get
#define comment_request_set	open_request_set
#define comment_request_new()	open_request_new_with_services ("comments")

#define APP_REQUEST		OPEN_REQUEST
#define AppRequest		OpenRequest
#define app_request_get		open_request_get
#define app_request_set		open_request_set
#define app_request_new()	open_request_new_with_services ("content")


#define APP_INFO 		OPEN_RESULT
#define AppInfo 		OpenResult
#define app_info_get 		open_result_get

G_END_DECLS

#endif
