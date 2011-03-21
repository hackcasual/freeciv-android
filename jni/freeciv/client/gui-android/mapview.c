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

/* common & utility */
#include "fcintl.h"
#include "game.h"
#include "support.h"

/* client */
#include "client_main.h"
#include "climisc.h"
#include "control.h"
#include "mapctrl_common.h"

#include "canvas.h"

#include "client.h"

#include "mapview.h"

#include "javaharness/callbacks.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

struct canvas *overview_canvas = 0;

//struct rect_list *rects = callback_list_new();

/****************************************************************************
  Typically an info box is provided to tell the player about the state
  of their civilization.  This function is called when the label is
  changed.
****************************************************************************/
void update_info_label(void)
{
	java_update_info();
  char buffer[512];

  my_snprintf(buffer, sizeof(buffer),
	      _("Population: %s\n"
		"Year: %s\n"
		"Gold %d\n"
		"Tax: %d Lux: %d Sci: %d"),
	      population_to_text(civ_population(client.conn.playing)),
	      textyear(game.info.year),
	      client.conn.playing->economic.gold,
	      client.conn.playing->economic.tax,
	      client.conn.playing->economic.luxury,
	      client.conn.playing->economic.science);

  /* ... */
}

/****************************************************************************
  Update the information label which gives info on the current unit
  and the tile under the current unit, for specified unit.  Note that
  in practice punit is always the focus unit.

  Clears label if punit is NULL.

  Typically also updates the cursor for the map_canvas (this is
  related because the info label may includes "select destination"
  prompt etc).  And it may call update_unit_pix_label() to update the
  icons for units on this tile.
****************************************************************************/
void update_unit_info_label(struct unit_list *punitlist)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/****************************************************************************
  Update the mouse cursor. Cursor type depends on what user is doing and
  pointing.
****************************************************************************/
void update_mouse_cursor(enum cursor_type new_cursor_type)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/****************************************************************************
  Update the timeout display.  The timeout is the time until the turn
  ends, in seconds.
****************************************************************************/
void update_timeout_label(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
    
  /* set some widget based on get_timeout_label_text() */
}

/****************************************************************************
  If do_restore is FALSE it should change the turn button style (to
  draw the user's attention to it).  If called regularly from a timer
  this will give a blinking turn done button.  If do_restore is TRUE
  this should reset the turn done button to the default style.
****************************************************************************/
void update_turn_done_button(bool do_restore)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  static bool flip = FALSE;
  
  if (!get_turn_done_button_state()) {
    return;
  }

  if ((do_restore && flip) || !do_restore) {
    /* ... */

    flip = !flip;
  }
  /* PORTME */
}

/****************************************************************************
  Set information for the indicator icons typically shown in the main
  client window.  The parameters tell which sprite to use for the
  indicator.
****************************************************************************/
void set_indicator_icons(struct sprite *bulb, struct sprite *sol,
			 struct sprite *flake, struct sprite *gov)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/****************************************************************************
  Return a canvas that is the overview window.
****************************************************************************/
struct canvas *get_overview_window(void)
{
	if (overview_canvas == 0) {
		LOGI("Creating a canvas for the overview");
		overview_canvas = canvas_create(156, 104);
	}
  return overview_canvas;
}

/****************************************************************************
  Flush the given part of the canvas buffer (if there is one) to the
  screen.
****************************************************************************/
void flush_mapcanvas(int canvas_x, int canvas_y,
		     int pixel_width, int pixel_height)
{
	LOGI("displayCanvas %p data %p", displayCanvas, displayCanvas->data);
	canvas_copy(displayCanvas, mapview.store, canvas_x, canvas_y, pixel_width, pixel_height, displayCanvas->width, displayCanvas->height);
	update_display();
}

/****************************************************************************
  Mark the rectangular region as "dirty" so that we know to flush it
  later.
****************************************************************************/
void dirty_rect(int canvas_x, int canvas_y,
		int pixel_width, int pixel_height)
{
	struct rect *prect = (struct rect *)fc_malloc(sizeof(struct rect));
	prect->x = canvas_x;
	prect->y = canvas_y;
	prect->w = pixel_width;
	prect->h = pixel_height;

	rect_list_prepend(rects, prect);
}

/****************************************************************************
  Mark the entire screen area as "dirty" so that we can flush it later.
****************************************************************************/
void dirty_all(void)
{
	if (!mapview.store)
		return;
	rect_list_clear(rects);
	dirty_rect(0,0, mapview.store->width, mapview.store->height);
}

/****************************************************************************
  Flush all regions that have been previously marked as dirty.  See
  dirty_rect and dirty_all.  This function is generally called after we've
  processed a batch of drawing operations.
****************************************************************************/
void flush_dirty(void)
{
	struct rect *prect;
	LOGI("Flush dirtydisplayCanvas %p data %p", displayCanvas, displayCanvas->data);
	rect_list_iterate(rects, prect) {
		canvas_copy(displayCanvas, mapview.store, prect->x, prect->y, prect->x, prect->y, prect->w, prect->h);

		if (prect->w == mapview.store->width && prect->h == mapview.store->height)
			break;
	} rect_list_iterate_end;

	rect_list_clear(rects);
	LOGI("Updating display");
	update_display();
}

/****************************************************************************
  Do any necessary synchronization to make sure the screen is up-to-date.
  The canvas should have already been flushed to screen via flush_dirty -
  all this function does is make sure the hardware has caught up.
****************************************************************************/
void gui_flush(void)
{

}

/****************************************************************************
  Update (refresh) the locations of the mapview scrollbars (if it uses
  them).
****************************************************************************/
void update_map_canvas_scrollbars(void)
{
/* Android doesn't use them */
}

/****************************************************************************
  Update the size of the sliders on the scrollbars.
****************************************************************************/
void update_map_canvas_scrollbars_size(void)
{
	/* Android doesn't use them */
}

/****************************************************************************
  Update (refresh) all city descriptions on the mapview.
****************************************************************************/
void update_city_descriptions(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  update_map_canvas_visible();
}

/****************************************************************************
  Draw a cross-hair overlay on a tile.
****************************************************************************/
void put_cross_overlay_tile(struct tile *ptile)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/****************************************************************************
 Area Selection
****************************************************************************/
void draw_selection_rectangle(int canvas_x, int canvas_y, int w, int h)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  /* PORTME */
}

/****************************************************************************
  This function is called when the tileset is changed.
****************************************************************************/
void tileset_changed(void)
{
  /* PORTME */
  /* Here you should do any necessary redraws (for instance, the city
   * dialogs usually need to be resized). */
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
}

/****************************************************************************
  Return the dimensions of the area (container widget; maximum size) for
  the overview.
****************************************************************************/
void get_overview_area_dimensions(int *width, int *height)
{
  *width = 156;
  *height = 104;
}

/****************************************************************************
  Called when the map size changes. This may be used to change the
  size of the GUI element holding the overview canvas. The
  overview.width and overview.height are updated if this function is
  called.
****************************************************************************/
void overview_size_changed(void)
{
	map_canvas_resized(android_width, android_height);
}
