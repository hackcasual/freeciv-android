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

#ifndef FC__CANVAS_H
#define FC__CANVAS_H

#include <sys/types.h>

#include "canvas_g.h"

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

enum {
	CANVAS_RGB565,
	CANVAS_ARGB4444
};

struct canvas {
	int width, height;
	void * data;
	uint8_t byte_width;
	uint8_t type;
};

#define CANVAS_BYTE_WIDTH 2

struct canvas *displayCanvas;

struct canvas * makeDisplay(int w, int h);
struct canvas *canvas_create_alpha(int width, int height);

#endif				/* FC__CANVAS_H */
