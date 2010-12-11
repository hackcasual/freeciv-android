/********************************************************************** 
 Freeciv - Copyright (C) 1996-2004 - The Freeciv Team
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

#include "connectdlg_g.h"

#include "pages.h"

/**************************************************************************
  Sets the "page" that the client should show.  See also pages_g.h.
**************************************************************************/
void set_client_page(enum client_pages page)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/****************************************************************************
  Set the list of available rulesets.  The default ruleset should be
  "default", and if the user changes this then set_ruleset() should be
  called.
****************************************************************************/
void gui_set_rulesets(int num_rulesets, char **rulesets)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/**************************************************************************
  Returns current client page
**************************************************************************/
enum client_pages get_client_page(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
  return PAGE_MAIN;
}

/**************************************************************************
  update the start page.
**************************************************************************/
void update_start_page(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */    
}
