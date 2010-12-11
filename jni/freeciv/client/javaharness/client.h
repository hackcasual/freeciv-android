/**********************************************************************
 Android-Freeciv - Copyright (C) 2010 - C Vaughn
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifndef FC__JNI_CLIENT_H
#define FC__JNI_CLIENT_H

int android_width;
int android_height;
/*
  ID_UNIT_ORDER_WAKEUP_OTHERS,
  ID_UNIT_ORDER_PATROL,
 */
enum {
	COM_GO_TO = 0,
	COM_BUILD_CITY,
	COM_FORTIFY,
	COM_SENTRY,
	COM_EXPLORE,
	COM_GO_TO_CITY,
	COM_DISBAND,
	COM_ROAD,
	COM_IRRIGATION,
	COM_AUTO_WORKER,
	COM_CONNECT_ROAD,
	COM_CONNECT_IRRIGATION,
	COM_CONNECT_RAILROAD,
	COM_WAIT,
	COM_BUILD_WONDER,
	COM_TRADE_ROUTE,
	COM_MINE,
	COM_TRANSFORM,
	COM_FORTRESS,
	COM_AIRBASE,
	COM_POLLUTION,
	COM_FALLOUT,
	COM_PARADROP,
	COM_PILLAGE,
	COM_HOMECITY,
	COM_UNLOAD_TRANSPORT,
	COM_LOAD,
	COM_UNLOAD,
	COM_AIRLIFT,
	COM_RETURN, //TODO: Re-think
	COM_PATROL, //TODO: Add
	COM_UPGRADE,
	COM_DIPLOMAT,
	COM_NUKE,
	COM_UNIT_LAST,
	COM_END_TURN = 100

};

void update_display();
void client_connected();
void measure_string(int *width, int *height, const char *text);
void report_tileset_progress(const char *text);
struct sprite *sprite_string(const char *text);

void * bufferData;
struct canvas * displayCanvas;

int lastTouchX, lastTouchY;
unsigned char draw_goto_patrol_lines;

//TODO: Size appropriately
#define BUFFER_SIZE 1024 * 1024 * 4

#endif				/* FC__JNI_CLIENT_H */
