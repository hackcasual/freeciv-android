/**********************************************************************


   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#include <png.h>
#include "png_loader.h"
#include "sprite.h"
#include "config.h"
#include "log.h"

#define abort_(...) do { freelog(LOG_ERROR, __VA_ARGS__);  exit(EXIT_FAILURE); } while(0)

struct sprite *read_png_file(const char* file_name)
{
	struct sprite *rval;
	char header[8];	// 8 is the maximum size that can be checked
	int width, height;
	struct pixel32 *pngdata;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	int obd, orl;

	int y;

	freelog(LOG_VERBOSE, "Reading %s", file_name);

	rval = (struct sprite *)fc_malloc(sizeof(struct sprite));
	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_byte*)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (png_get_valid(png_ptr, info_ptr,
        PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");




	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	orl = info_ptr->rowbytes;
	obd = info_ptr->bit_depth;

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

    if (bit_depth == 16)
    	png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
    	png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
    		PNG_COLOR_TYPE_GRAY_ALPHA)
    	png_set_gray_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB ||
    	color_type == PNG_COLOR_TYPE_GRAY ||
    	color_type == PNG_COLOR_TYPE_PALETTE)
    	png_set_add_alpha(png_ptr, 0xFFFFFFFF, PNG_FILLER_AFTER);



	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (info_ptr->rowbytes % 4 != 0 || info_ptr->rowbytes / 4 != width || info_ptr->bit_depth != 8) {
		abort_("PNG type conversion failed for %s [%dx%d:%d], rowbytes %d bit_depth %d", file_name, width, height, info_ptr->color_type, info_ptr->rowbytes, info_ptr->bit_depth);
	}

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during read_image");

	row_pointers = (png_bytep*) fc_malloc(sizeof(png_bytep) * height);
	pngdata = fc_malloc(info_ptr->rowbytes * height);
	rval->data = fc_malloc(width * height * 2);
	rval->width = width;
	rval->height = height;

	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*)(((char *)pngdata) + info_ptr->rowbytes * y);

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr,
	       NULL);

	int i;

	for (i = 0; i < width * height; i++) {
		rval->data[i].a = (pngdata[i].a >> 4);
		rval->data[i].r = (pngdata[i].r >> 4);
		rval->data[i].g = (pngdata[i].g >> 4);
		rval->data[i].b = (pngdata[i].b >> 4);
	}

	FC_FREE(row_pointers);
	FC_FREE(pngdata);

    fclose(fp);

    return rval;
}
