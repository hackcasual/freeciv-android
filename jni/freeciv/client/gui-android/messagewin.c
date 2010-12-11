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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "messagewin.h"


/**************************************************************************
  Display the message dialog.  Optionally raise it.
  Typically triggered by F10.
**************************************************************************/
void popup_meswin_dialog(bool raise)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/**************************************************************************
  Return whether the message dialog is open.
**************************************************************************/
bool is_meswin_open(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
  return TRUE;
}

/**************************************************************************
  Do the work of updating (populating) the message dialog.
**************************************************************************/
void real_update_meswin_dialog(void)
{
	//freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
	java_show_messages();
}
