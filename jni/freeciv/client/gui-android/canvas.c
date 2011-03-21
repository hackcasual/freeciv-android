/********************************************************************** 
 Freeciv - Copyright (C) 1996-2005 - Freeciv Development Team
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

#include "canvas.h"
#include "sprite.h"
#include "log.h"
#include "client.h"
#include "colors.h"

struct canvas * makeDisplay(int w, int h) {
	displayCanvas = canvas_create(w, h);

	LOGI("Making displayCanvas %p data %p", displayCanvas, displayCanvas->data);

	return displayCanvas;
}


/****************************************************************************
  Create a canvas of the given size.
****************************************************************************/
struct canvas *canvas_create(int width, int height)
{
	struct canvas* cp = fc_malloc(sizeof(struct canvas));

	cp->width = width;
	cp->height = height;
	cp->data = fc_malloc(width * height * CANVAS_BYTE_WIDTH);
	cp->type = CANVAS_RGB565;
	cp->byte_width = CANVAS_BYTE_WIDTH;

	return cp;
}

/****************************************************************************
  Create a canvas of the given size.
****************************************************************************/
struct canvas *canvas_create_alpha(int width, int height)
{
	struct canvas* cp = fc_malloc(sizeof(struct canvas));

	cp->width = width;
	cp->height = height;
	cp->data = fc_malloc(width * height * CANVAS_BYTE_WIDTH);
	cp->type = CANVAS_ARGB4444;
	cp->byte_width = CANVAS_BYTE_WIDTH;

	return cp;
}


/****************************************************************************
  Free any resources associated with this canvas and the canvas struct
  itself.
****************************************************************************/
void canvas_free(struct canvas *store)
{
	freelog(LOG_VERBOSE, "Deleting canvas");
	FC_FREE(store->data);
}

/****************************************************************************
  Copies an area from the source canvas to the destination canvas.
****************************************************************************/
void canvas_copy(struct canvas *dest, struct canvas *src,
		     int src_x, int src_y, int dest_x, int dest_y, int width,
		     int height)
{

	if (src_x < 0) {
		dest_x -= src_x;
		width += src_x;
		src_x -= src_x;
	}

	if (dest_x < 0) {
		dest_x -= dest_x;
		width += dest_x;
		src_x -= dest_x;
	}

	if (src_y < 0) {
		dest_y -= src_y;
		height += src_y;
		src_y -= src_y;
	}

	if (dest_y < 0) {
		dest_y -= dest_y;
		height += dest_y;
		src_y -= dest_y;
	}

	int w = MIN(src->width - src_x, MIN(dest->width - dest_x, width));
	int h = MIN(src->height - src_y, MIN(dest->height - dest_y, height));

	if (w != width || h != height) {
		LOGI("Copying canvas, had to clip src [%dx%d %d,%d] dst [%dx%d %d,%d] %dx%d (%dx%d)", src->width, src->height, src_x, src_y, dest->width, dest->height, dest_x, dest_y, width, height, w, h);
	}

	//LOGI("Copying canvas in thread %d src %p -> %p [%dx%d %d,%d] dst %p -> %p [%dx%d %d,%d] %dx%d (%dx%d)", pthread_self(), src, src->data, src->width, src->height, src_x, src_y, dest, dest->data, dest->width, dest->height, dest_x, dest_y, width, height, w, h);

	if (w < 1 || h < 1)
		return;

	//LOGI("Copying a canvas src [%dx%d %d,%d] dst [%dx%d %d,%d] %dx%d", src->width, src->height, src_x, src_y, dest->width, dest->height, dest_x, dest_y, width, height);

	// Handle simple case of copying same canvas
	//TODO: Trigger this if the widths match, and X offsets are 0
	if (src_x == 0 && src_y == 0 &&
			dest_x == 0 && dest_y == 0 &&
			src->width == dest->width &&
			src->width == width &&
			src->height == dest->height &&
			src->height == height &&
			w == width &&
			h == height) {
		memcpy(dest->data, src->data, width * height * CANVAS_BYTE_WIDTH);
		return;
	}

	void * s = src->data;
	void * d = dest->data;

	s += (src_x + (src_y * src->width)) * CANVAS_BYTE_WIDTH;
	d += (dest_x + (dest_y * dest->width)) * CANVAS_BYTE_WIDTH;

	int r;

	for (r = 0; r < h; r++) {
		memcpy(d, s, w * CANVAS_BYTE_WIDTH);
		s += src->width * CANVAS_BYTE_WIDTH;
		d += dest->width * CANVAS_BYTE_WIDTH;
	}
}

/****************************************************************************
  Draw some or all of a sprite onto the canvas.
****************************************************************************/
void canvas_put_sprite(struct canvas *pcanvas,
		    int canvas_x, int canvas_y,
		    struct sprite *sprite,
		    int offset_x, int offset_y, int width, int height)
{
	//LOGI("Spriting d[%d,%d] s[%d,%d %dx%d] %dx%d", canvas_x, canvas_y, offset_x, offset_y, sprite->width, sprite->height, width, height);

	if (canvas_x < 0) {
		offset_x -= canvas_x;
		width += canvas_x;
		canvas_x = 0;
	}

	if (canvas_y < 0) {
		offset_y -= canvas_y;
		height += canvas_y;
		canvas_y = 0;
	}

	if (canvas_x > pcanvas->width || canvas_y > pcanvas->height) {
		return;
	}

	width = MIN(pcanvas->width - canvas_x, width);
	height = MIN(pcanvas->height - canvas_y, height);

	//LOGI("Updated Spriting d[%d,%d] s[%d,%d %dx%d] %dx%d", canvas_x, canvas_y, offset_x, offset_y, sprite->width, sprite->height, width, height);

	if (CANVAS_RGB565 == pcanvas->type) {
		struct pixel565 * dst = pcanvas->data;
		struct pixel16 * src = sprite->data;

		dst += (canvas_x + canvas_y * pcanvas->width);
		src += (offset_x + offset_y * sprite->width);

		int r;
		int c;
		for (r = 0; r < height; r++) {
			for (c = 0; c < width; c++) {
				struct pixel16 *src_unpacked = src + c;
				struct pixel565 *dst_unpacked = dst + c;
				if (src_unpacked->a == 0xF) {
					dst_unpacked->r = src_unpacked->r << 1;
					dst_unpacked->g = src_unpacked->g << 2;
					dst_unpacked->b = src_unpacked->b << 1;
				} else if (src_unpacked->a) {

					dst_unpacked->r = (dst_unpacked->r * (16 - src_unpacked->a) + ((src_unpacked->r << 1) * src_unpacked->a)) / 16;
					dst_unpacked->g = (dst_unpacked->g * (16 - src_unpacked->a) + ((src_unpacked->g << 2) * src_unpacked->a)) / 16;
					dst_unpacked->b = (dst_unpacked->b * (16 - src_unpacked->a) + ((src_unpacked->b << 1) * src_unpacked->a)) / 16;
				}
			}
			dst += pcanvas->width;
			src += sprite->width;
		}
	} else if (CANVAS_ARGB4444 == pcanvas->type) {
		struct pixel16 * dst = pcanvas->data;
		struct pixel16 * src = sprite->data;

		dst += (canvas_x + canvas_y * pcanvas->width);
		src += (offset_x + offset_y * sprite->width);

		int r;
		int c;
		for (r = 0; r < height; r++) {
			for (c = 0; c < width; c++) {
				struct pixel16 *src_unpacked = src + c;
				struct pixel16 *dst_unpacked = dst + c;
				if (src_unpacked->a == 0xF) {
					*dst_unpacked = *src_unpacked;
				} else if (src_unpacked->a) {
					dst_unpacked->a = (dst_unpacked->a * (16 - src_unpacked->a) + ((src_unpacked->a) * src_unpacked->a)) / 16;
					dst_unpacked->r = (dst_unpacked->r * (16 - src_unpacked->a) + ((src_unpacked->r) * src_unpacked->a)) / 16;
					dst_unpacked->g = (dst_unpacked->g * (16 - src_unpacked->a) + ((src_unpacked->g) * src_unpacked->a)) / 16;
					dst_unpacked->b = (dst_unpacked->b * (16 - src_unpacked->a) + ((src_unpacked->b) * src_unpacked->a)) / 16;
				}
			}
			dst += pcanvas->width;
			src += sprite->width;
		}
	}
}

/****************************************************************************
  Draw a full sprite onto the canvas.
****************************************************************************/
void canvas_put_sprite_full(struct canvas *pcanvas,
			 int canvas_x, int canvas_y,
			 struct sprite *sprite)
{
	canvas_put_sprite(pcanvas, canvas_x, canvas_y, sprite, 0, 0, sprite->width, sprite->height);
}

/****************************************************************************
  Draw a full sprite onto the canvas.  If "fog" is specified draw it with
  fog.
****************************************************************************/
void canvas_put_sprite_fogged(struct canvas *pcanvas,
			      int canvas_x, int canvas_y,
			      struct sprite *psprite,
			      bool fog, int fog_x, int fog_y)
{
	LOGI("Fogged sprite, unimplementd");
	//freelog(LOG_VERBOSE, "Spriting - fogged");
}

/****************************************************************************
  Draw a filled-in colored rectangle onto canvas.
****************************************************************************/
void canvas_put_rectangle(struct canvas *pcanvas,
		       struct color *pcolor,
		       int canvas_x, int canvas_y, int width, int height)
{
	struct pixel565 dc;

	dc.r = pcolor->r >> 3;
	dc.g = pcolor->g >> 2;
	dc.b = pcolor->b >> 3;

	int i;
	/*if (pcolor->r || pcolor->g || pcolor->b)
		LOGI("Putting a rectangle [%d,%d %dx%d] #%x %x %x", canvas_x, canvas_y, width, height, pcolor->r, pcolor->g, pcolor->b);*/
	if (canvas_x < 0) {
		width += canvas_x;
		canvas_x = 0;
	}

	if (canvas_y < 0) {
		height += canvas_y;
		canvas_y = 0;
	}



	width = MAX(MIN(width, pcanvas->width - canvas_x), 0);
	height = MAX(MIN(height, pcanvas->height - canvas_y), 0);

	if (!width || !height)
		return;

	struct pixel565 * dst = pcanvas->data;
	dst += (canvas_x + canvas_y * pcanvas->width);
	struct pixel565 * dst_fl = dst;


	//LOGI("Putting a rectangle [%d,%d %dx%d] #%x %x %x", canvas_x, canvas_y, width, height, pcolor->r, pcolor->g, pcolor->b);

	for (i = 0; i < width; i++) {
		dst[i] = dc;
	}

	for (i = 1; i < height; i++) {
		dst += pcanvas->width;
		memcpy(dst, dst_fl, width * CANVAS_BYTE_WIDTH);
	}
}

/****************************************************************************
  Fill the area covered by the sprite with the given color.
****************************************************************************/
void canvas_fill_sprite_area(struct canvas *pcanvas,
			     struct sprite *psprite, struct color *pcolor,
			     int canvas_x, int canvas_y)
{
	LOGI("Sprite fill area");
	freelog(LOG_VERBOSE, "Sprite fill area");
}

/****************************************************************************
  Fill the area covered by the sprite with the given color.
****************************************************************************/
void canvas_fog_sprite_area(struct canvas *pcanvas, struct sprite *psprite,
			    int canvas_x, int canvas_y)
{
	LOGI("Sprite fog area");
	freelog(LOG_VERBOSE, "Sprite fog area");
}

/****************************************************************************
  Draw a 1-pixel-width colored line onto the canvas.
****************************************************************************/
void canvas_put_line(struct canvas *pcanvas, struct color *pcolor,
		  enum line_type ltype, int start_x, int start_y,
		  int off_x, int off_y)
{
	LOGI("Drawing line @ [%d,%d] delta [%d,%d]", start_x, start_y, off_x, off_y);



	int dx, dy;

	dx = off_x + start_x;
	dy = off_y + start_y;

	//dx += start_x;
	//dy += start_y;

	int tmp;
	int deltax;
	int deltay;
	int error;
	int x, y;
	int ystep;
	struct pixel565 pixel;
	pixel.r = pcolor->r >> 3;
	pixel.g = pcolor->g >> 2;
	pixel.b = pcolor->b >> 3;

	unsigned char steep = abs(off_y) > abs(off_x);

	if (steep) {
		tmp = start_y;
		start_y = start_x;
		start_x = tmp;

		tmp = dy;
		dy = dx;
		dx = tmp;
	}

	if (start_x > dx) {
		tmp = start_x;
		start_x = dx;
		dx = tmp;

		tmp = start_y;
		start_y = dy;
		dy = tmp;
	}

	deltax = dx - start_x;
	deltay = abs(dy - start_y);
	error = deltax / 2;
	y = start_y;
	ystep = -1;
	if (start_y < dy)
		ystep = 1;

	/*
	 * 	     for x from x0 to x1
	         if steep then plot(y,x) else plot(x,y)
	         error := error - deltay
	         if error < 0 then
	             y := y + ystep
	             error := error + deltax*/

	for (x = start_x; x < dx; x++) {
		if (steep && y < pcanvas->width && y >= 0 && x < pcanvas->height && x >= 0) {
			((struct pixel565 *)pcanvas->data)[pcanvas->width * x + y] = pixel;
		} else if (!steep && x < pcanvas->width && y >= 0 && y < pcanvas->height && x >= 0){
			((struct pixel565 *)pcanvas->data)[pcanvas->width * y + x] = pixel;
		}

		error -= deltay;

		if (error < 0) {
			y = y + ystep;
			error += deltax;
		}
	}

}

/****************************************************************************
  Draw a 1-pixel-width colored curved line onto the canvas.
****************************************************************************/
void canvas_put_curved_line(struct canvas *pcanvas, struct color *pcolor,
                            enum line_type ltype, int start_x, int start_y,
                            int dx, int dy)
{
	LOGI("Sprite curved line");
	freelog(LOG_VERBOSE, "Curved line");
}

/****************************************************************************
  Return the size of the given text in the given font.  This size should
  include the ascent and descent of the text.  Either of width or height
  may be NULL in which case those values simply shouldn't be filled out.
****************************************************************************/
void get_text_size(int *width, int *height,
		   enum client_font font, const char *text)
{
	int w, h;
	measure_string(&w, &h, text);

  if (width) {
    *width = w;
  }
  if (height) {
    *height = h;
  }
}

/****************************************************************************
  Draw the text onto the canvas in the given color and font.  The canvas
  position does not account for the ascent of the text; this function must
  take care of this manually.  The text will not be NULL but may be empty.
****************************************************************************/
void canvas_put_text(struct canvas *pcanvas, int canvas_x, int canvas_y,
		     enum client_font font, struct color *pcolor,
		     const char *text)
{
  struct sprite *text_holder = sprite_string(text);

  canvas_put_sprite(pcanvas, canvas_x, canvas_y, text_holder, 0, 0, text_holder->width, text_holder->height);
}

