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

#ifndef FC__SPRITE_H
#define FC__SPRITE_H

#include "sprite_g.h"

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

/**
 * Helper structs for accessing pixel values directly. For efficiency, treat them as shorts/ints
 *
 * */

struct pixel32 {
	unsigned char r, g, b, a;
};

struct pixel565 {
	unsigned short b:5;
	unsigned short g:6;
	unsigned short r:5;
};


struct pixel16 {
	unsigned short a:4;
	unsigned short b:4;
	unsigned short g:4;
	unsigned short r:4;

};

struct sprite {
	struct pixel16 *data;
	int width, height;
};




void setup_sprite_disk_store();

#endif				/* FC__SPRITE_H */
