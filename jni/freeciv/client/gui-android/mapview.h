/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifndef FC__MAPVIEW_H
#define FC__MAPVIEW_H

#include "mapview_g.h"
#include "log.h"

unsigned char isDirty;


struct rect {
  short x, y, w, h;
};

#define SPECLIST_TAG rect
#define SPECLIST_TYPE struct rect
#include "speclist.h"

struct rect_list *rects;

#define rect_list_iterate(rl, prect) \
    TYPED_LIST_ITERATE(struct rect, rl, prect)
#define rect_list_iterate_end  LIST_ITERATE_END

struct canvas *overview_canvas;

#endif				/* FC__MAPVIEW_H */
