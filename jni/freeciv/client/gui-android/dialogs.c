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

#include "game.h"
#include "government.h"

#include "dialogs.h"

/**************************************************************************
  Popup a dialog to display information about an event that has a
  specific location.  The user should be given the option to goto that
  location.
**************************************************************************/
void popup_notify_goto_dialog(const char *headline, const char *lines,
                              const struct text_tag_list *tags,
                              struct tile *ptile)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog to display connection message from server.
**************************************************************************/
void popup_connect_msg(const char *headline, const char *message)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a generic dialog to display some generic information.
**************************************************************************/
void popup_notify_dialog(const char *caption, const char *headline,
			 const char *lines)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup the nation selection dialog.
**************************************************************************/
void popup_races_dialog(struct player *pplayer)
{
  	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Close the nation selection dialog.  This should allow the user to
  (at least) select a unit to activate.
**************************************************************************/
void popdown_races_dialog(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog window to select units on a particular tile.
**************************************************************************/
void popup_unit_select_dialog(struct tile *ptile)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  In the nation selection dialog, make already-taken nations unavailable.
  This information is contained in the packet_nations_used packet.
**************************************************************************/
void races_toggles_set_sensitive(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog asking if the player wants to start a revolution.
**************************************************************************/
void popup_revolution_dialog(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog giving a player choices when their caravan arrives at
  a city (other than its home city).  Example:
    - Establish trade route.
    - Help build wonder.
    - Keep moving.
**************************************************************************/
void popup_caravan_dialog(struct unit *punit,
			  struct city *phomecity, struct city *pdestcity)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Is there currently a caravan dialog open?  This is important if there
  can be only one such dialog at a time; otherwise return FALSE.
**************************************************************************/
bool caravan_dialog_is_open(int *unit_id, int *city_id)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  return FALSE;
}

/**************************************************************************
  Popup a dialog giving a diplomatic unit some options when moving into
  the target tile.
**************************************************************************/
void popup_diplomat_dialog(struct unit *punit, struct tile *ptile)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a window asking a diplomatic unit if it wishes to incite the
  given enemy city.
**************************************************************************/
void popup_incite_dialog(struct city *pcity, int cost)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog asking a diplomatic unit if it wishes to bribe the
  given enemy unit.
**************************************************************************/
void popup_bribe_dialog(struct unit *punit, int cost)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog asking a diplomatic unit if it wishes to sabotage the
  given enemy city.
**************************************************************************/
void popup_sabotage_dialog(struct city *pcity)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Popup a dialog asking the unit which improvement they would like to
  pillage.
**************************************************************************/
void popup_pillage_dialog(struct unit *punit, bv_special may_pillage,
                          bv_bases bases)
{
  /* PORTME */
}

/**************************************************************************
  Ruleset (modpack) has suggested loading certain tileset. Confirm from
  user and load.
**************************************************************************/
void popup_tileset_suggestion_dialog(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Tileset (modpack) has suggested loading certain theme. Confirm from
  user and load.
**************************************************************************/
bool popup_theme_suggestion_dialog(const char *theme_name)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  return FALSE;
}

/**************************************************************************
  This function is called when the client disconnects or the game is
  over.  It should close all dialog windows for that game.
**************************************************************************/
void popdown_all_game_dialogs(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/****************************************************************
  Returns id of a diplomat currently handled in diplomat dialog
*****************************************************************/
int diplomat_handled_in_diplomat_dialog(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  return -1;  
}

/****************************************************************
  Closes the diplomat dialog
****************************************************************/
void close_diplomat_dialog(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/****************************************************************
  Updates caravan dialog
****************************************************************/
void caravan_dialog_update(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}
