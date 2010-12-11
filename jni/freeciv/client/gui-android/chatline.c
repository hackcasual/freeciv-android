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

#include "climisc.h"      /* for write_chatline_content */

#include "chatline.h"

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

/**************************************************************************
  Appends the string to the chat output window.  The string should be
  inserted on its own line, although it will have no newline.
**************************************************************************/
void real_output_window_append(const char *astring,
                               const struct text_tag_list *tags,
                               int conn_id)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
	LOGI("### CHATLINE ###> %s", astring);
}

/**************************************************************************
  Get the text of the output window, and call write_chatline_content() to
  log it.
**************************************************************************/
void log_output_window(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
  write_chatline_content(NULL);
}

/**************************************************************************
  Clear all text from the output window.
**************************************************************************/
void clear_output_window(void)
{
	freelog(LOG_VERBOSE, "Port Me %s [@%s:%d]", __func__, __FILE__, __LINE__);
#if 0
  set_output_window_text(_("Cleared output window."));
#endif
}
