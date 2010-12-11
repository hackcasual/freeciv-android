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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


#include "sprite.h"
#include "png_loader.h"
#include "client.h"
#include "log.h"
#include "colors.h"

int total_sprite_mem;

/****************************************************************************
  Return a NULL-terminated, permanently allocated array of possible
  graphics types extensions.  Extensions listed first will be checked
  first.
****************************************************************************/
const char **gfx_fileextensions(void)
{
  static const char *ext[] = {
    "png",	/* png should be the default. */
    NULL
  };

  return ext;
}

/****************************************************************************
  Load the given graphics file into a sprite.  This function loads an
  entire image file, which may later be broken up into individual sprites
  with crop_sprite.
****************************************************************************/
struct sprite *load_gfxfile(const char *filename)
{
 freelog(LOG_VERBOSE, "Loading filename: %s", filename);

  return read_png_file(filename);
}

void *mask_sprite(struct sprite *source,
			   struct sprite *mask,
			   int x, int y)
{
	struct pixel16 * dst;
	struct pixel16 * msk;
	int m_x, m_y;
	int r, c;
	struct sprite *sp = fc_malloc(sizeof(struct sprite));
	//freelog(LOG_VERBOSE, "Masking sprite: Source [%dx%d] Mask [%dx%d] Offset %d,%d", source->width, source->height, mask->width, mask->height, mask_offset_x, mask_offset_y);

	//struct color *grey = fc_malloc(sizeof(struct color));

	dst = source->data;

	m_x = 0;
	m_y = 0;

	if (x < 0) {
		x = 0;
		m_x = -x;
	}

	if (y < 0) {
		y = 0;
		m_y = -y;
	}

	/*grey->b = grey->r = grey->g = 0x80;
	canvas_put_rectangle(displayCanvas, grey, 0, 0, android_width, android_height);
	canvas_put_sprite(displayCanvas, 10, 10, source, 0, 0, source->width, source->height);*/

	/*int w = MIN(mask->width - m_x, source->width - x);
	int h = MIN(mask->height - m_y, source->height - y);

	msk = mask->data + (m_y * mask->width + m_x);

	for (r = 0; r < h; r++) {
		for (c = 0; c < w; c++) {
			struct pixel16 *dpp = dst + c;
			struct pixel16 *mpp = msk + c;

			int alphaD = dpp->a;
			int alphaM = mpp->a;

			int res = alphaD * alphaM;
			res = res / 16;

			dpp->a = res;
		}
		dst += source->width;
		msk += mask->width;
	}*/

	int w = source->width;
	int h = source->height;

	for (r = 0; r < h; r++) {

		msk = mask->data + ((y + r) * mask->width + x);

		for (c = 0; c < w; c++) {

			int alphaD = dst->a;
			int alphaM = msk->a;

			int res = alphaD * alphaM;
			res = res / 16;

			dst->a = res;
			dst++;
			msk++;
		}
		/*dst += source->width;
		msk += mask->width;*/
	}


	/*canvas_put_sprite(displayCanvas, 10, 20 + source->height, mask, 0, 0, mask->width, mask->height);


	canvas_put_sprite(displayCanvas, 10, 30 + source->height + mask->height, source, 0, 0, source->width, source->height);

	update_display();*/
}

/****************************************************************************
  Create a new sprite by cropping and taking only the given portion of
  the image.

  source gives the sprite that is to be cropped.

  x,y, width, height gives the rectangle to be cropped.  The pixel at
  position of the source sprite will be at (0,0) in the new sprite, and
  the new sprite will have dimensions (width, height).

  mask gives an additional mask to be used for clipping the new
  sprite. Only the transparency value of the mask is used in
  crop_sprite. The formula is: dest_trans = src_trans *
  mask_trans. Note that because the transparency is expressed as an
  integer it is common to divide it by 256 afterwards.

  mask_offset_x, mask_offset_y is the offset of the mask relative to the
  origin of the source image.  The pixel at (mask_offset_x,mask_offset_y)
  in the mask image will be used to clip pixel (0,0) in the source image
  which is pixel (-x,-y) in the new image.
****************************************************************************/
struct sprite *crop_sprite(struct sprite *source,
			   int x, int y, int width, int height,
			   struct sprite *mask,
			   int mask_offset_x, int mask_offset_y)
{
	struct pixel16 * src;
	struct pixel16 * dst;
	char * msk;
	int r;
	struct sprite *sp = fc_malloc(sizeof(struct sprite));
    struct sprite *mp;

	sp->width = width;
	sp->height = height;

	sp->data = fc_malloc(2 * width * height);
	total_sprite_mem += 2 * width * height;



	 src = source->data;
	 dst = sp->data;

	 src += source->width * y + x;



	 int c;
	 for (r = 0; r < height; r++) {
		 memcpy(dst, src, 2 * width);
		 dst += width;
		 src += source->width;
	 }

	 if (mask) {
		 mask_sprite(sp, mask, x - mask_offset_x, y - mask_offset_y);
	 }
    return sp;
}



/****************************************************************************
  Find the dimensions of the sprite.
****************************************************************************/
void get_sprite_dimensions(struct sprite *sprite, int *width, int *height)
{
  *width = sprite->width;
  *height = sprite->height;
}

/****************************************************************************
  Free a sprite and all associated image data.
****************************************************************************/
void free_sprite(struct sprite *s)
{
	LOGI("Freed sprite [%dx%d]", s->width, s->height);
	//total_sprite_mem -= 4 * s->width * s->height;

	FC_FREE(s->data);
}

void setup_sprite_disk_store()
{
	total_sprite_mem = 0;
	return;
}
