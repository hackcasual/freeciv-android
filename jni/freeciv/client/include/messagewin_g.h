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
#ifndef FC__MESSAGEWIN_G_H
#define FC__MESSAGEWIN_G_H

#include "packets.h"

#include "messagewin_common.h"

void popup_meswin_dialog(bool raise);
bool is_meswin_open(void);
void real_update_meswin_dialog(void);
     
#endif  /* FC__MESSAGEWIN_G_H */
