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

#ifdef AUDIO_SDL
#include "SDL.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
/* utility */
#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "netintf.h"

/* common */
#include "unitlist.h"

/* client */
#include "client_main.h"
#include "editgui_g.h"
#include "ggz_g.h"
#include "options.h"
#include "tilespec.h"
#include "mapview.h"
#include "game.h"
#include "gui_main.h"

#include "javaharness/sync.h"
#include "javaharness/client.h"

#include <pthread.h>

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);


const char *client_string = "gui-android";

const char * const gui_character_encoding = "UTF-8";
const bool gui_use_transliteration = FALSE;
static int net_socket = -1;
enum USER_EVENT_ID {
  EVENT_ERROR = 0,
  NET,
  GGZ,
  ANIM,
  TRY_AUTO_CONNECT,
  SHOW_WIDGET_INFO_LABBEL,
  FLUSH,
  MAP_SCROLL,
  EXIT_FROM_EVENT_LOOP
};

struct callback {
  void (*callback)(void *data);
  void *data;
};

#define SPECLIST_TAG callback
#define SPECLIST_TYPE struct callback
#include "speclist.h"

struct callback_list *callbacks;


/****************************************************************************
  Called by the tileset code to set the font size that should be used to
  draw the city names and productions.
****************************************************************************/
void set_city_names_font_sizes(int my_city_names_font_size,
			       int my_city_productions_font_size)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/**************************************************************************
  Do any necessary pre-initialization of the UI, if necessary.
**************************************************************************/
void ui_init(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
	isDirty = 2;
}

/**************************************************************************
  Entry point for whole freeciv client program.
**************************************************************************/
/*int main(int argc, char **argv)
{
  return client_main(argc, argv);
}*/

/**************************************************************************
  The main loop for the UI.  This is called from main(), and when it
  exits the client will exit.
**************************************************************************/
void ui_main(int argc, char *argv[])
{
	static fd_set civfdset;
	static struct timeval tv;
	double nt = 1.0;
	static int result;
  /* PORTME */
	freelog(LOG_VERBOSE, "Kick this pig");
	sz_strlcpy(user_name, "Sparky");
	//sz_strlcpy(user_name, "Charles");
  tileset_init(tileset);
  tileset_load_tiles(tileset);
  tileset_use_prefered_theme(tileset);


  // Handle various initilization steps:


  LOGI("Tileset loaded");
  load_cursors();
  LOGI("Cursors loaded");

  rects = rect_list_new();
  callbacks = callback_list_new();
  usleep(10000);
  set_client_state(C_S_DISCONNECTED);
  usleep(10000);
  LOGI("Auto connecting...");
  try_to_autoconnect();
  LOGI("Connected?");
  while(1) {

	  if ((net_socket >= 0)) {
	      FD_ZERO(&civfdset);

          FD_SET(net_socket, &civfdset);

          tv.tv_sec = 0;
          tv.tv_usec = 10000;/* 10ms */

          result = fc_select(net_socket + 1, &civfdset, NULL, NULL, &tv);

          if (result < 0) {
            if (errno != EINTR) {
            	freelog(LOG_VERBOSE, "Uh-oh unexpected error");
            	usleep(100000);
            } else {
            	freelog(LOG_VERBOSE, "Expected error");
            }
          } else {
            if (result > 0) {
            	freelog(LOG_VERBOSE, "Got something");
    	  if (FD_ISSET(net_socket, &civfdset)) {
    		  civ_lock();
    		  input_from_server(net_socket);
    		  civ_unlock();
    	  }
    	}
          }
	  }
	  usleep(10000);
      if (callbacks && callback_list_size(callbacks) > 0) {
        struct callback *cb = callback_list_get(callbacks, 0);
        callback_list_unlink(callbacks, cb);
        LOGI("ICB Lock");
        civ_lock();
        (cb->callback)(cb->data);
        civ_unlock();
        free(cb);
      }
  };
}

/****************************************************************************
  Extra initializers for client options.
****************************************************************************/
void gui_options_extra_init(void)
{
  /* Nothing to do. */
}

/**************************************************************************
  Do any necessary UI-specific cleanup
**************************************************************************/
void ui_exit()
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Return our GUI type
**************************************************************************/
enum gui_type get_gui_type(void)
{
  return GUI_ANDROID;
}

/**************************************************************************
 Update the connected users list at pregame state.
**************************************************************************/
void update_conn_list_dialog(void)
{
	static unsigned char did_connect = 0;

	//TODO: Handle disconnection here
	if (did_connect)
		return;

    conn_list_iterate(game.all_connections, pconn) {
    	LOGI("Connected player: %p [%s] %d :%s" , pconn->playing, pconn->observer?"Observing":"not Observing", pconn->access_level, pconn->established?"connected":"not connected");

    	if (pconn->playing == client.conn.playing && pconn->established) {
    		did_connect = 1;
  		    tileset_free_big_sprites(tileset);
    		/*send_chat("/set dispersion=5");
    		send_chat("/start");*/

  		    client_connected();

    		return;
    	}
      /*if (pplayer == pconn->playing) {
        if (pconn->observer) {
        	  LOGI("////// Observing...");
        } else {
        	  LOGI("////// Not observing");
        }
      }*/


    } conn_list_iterate_end;
}

/**************************************************************************
  Make a bell noise (beep).  This provides low-level sound alerts even
  if there is no real sound support.
**************************************************************************/
void sound_bell(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Wait for data on the given socket.  Call input_from_server() when data
  is ready to be read.

  This function is called after the client succesfully has connected
  to the server.
**************************************************************************/
void add_net_input(int sock)
{
	net_socket = sock;
}

/**************************************************************************
  Stop waiting for any server network data.  See add_net_input().

  This function is called if the client disconnects from the server.
**************************************************************************/
void remove_net_input(void)
{
	/* Ignored for now, multiplayer not supported */
}

/**************************************************************************
  Called to monitor a GGZ socket.
**************************************************************************/
void add_ggz_input(int sock)
{
  /* Ignored for now, no GGZ support */
}

/**************************************************************************
  Called on disconnection to remove monitoring on the GGZ socket.  Only
  call this if we're actually in GGZ mode.
**************************************************************************/
void remove_ggz_input(void)
{
  /* PORTME */
}

/**************************************************************************
  Set one of the unit icons (specified by idx) in the information area
  based on punit.

  punit is the unit the information should be taken from. Use NULL to
  clear the icon.

  idx specified which icon should be modified. Use idx==-1 to indicate
  the icon for the active unit. Or idx in [0..num_units_below-1] for
  secondary (inactive) units on the same tile.
**************************************************************************/
void set_unit_icon(int idx, struct unit *punit)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/**************************************************************************
  Most clients use an arrow (e.g., sprites.right_arrow) to indicate when
  the units_below will not fit. This function is called to activate or
  deactivate the arrow.

  Is disabled by default.
**************************************************************************/
void set_unit_icons_more_arrow(bool onoff)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

void * run_callback(void *val) {

	struct callback *cb = (struct callback *)val;
	(cb->callback)(cb->data);
	free(cb);
	return NULL;
}

/****************************************************************************
  Enqueue a callback to be called during an idle moment.  The 'callback'
  function should be called sometimes soon, and passed the 'data' pointer
  as its data.
****************************************************************************/
void add_idle_callback(void (callback)(void *), void *data)
{
	  struct callback *cb = fc_malloc(sizeof(*cb));

	  cb->callback = callback;
	  cb->data = data;

	  callback_list_prepend(callbacks, cb);
}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_tileset_changed(void)
{}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_refresh(void)
{}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_popup_properties(const struct tile_list *tiles, int objtype)
{}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_popdown_all(void)
{}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_notify_object_changed(int objtype, int object_id, bool remove)
{}

/****************************************************************************
  Stub for editor function
****************************************************************************/
void editgui_notify_object_created(int tag, int id)
{}

/****************************************************************************
  Stub for ggz function
****************************************************************************/
void gui_ggz_embed_leave_table(void)
{}

/****************************************************************************
  Stub for ggz function
****************************************************************************/
void gui_ggz_embed_ensure_server(void)
{}


/**************************************************************************
  Updates a gui font style.
**************************************************************************/
void gui_update_font(const char *font_name, const char *font_value)
{
  /* PORTME */
}
