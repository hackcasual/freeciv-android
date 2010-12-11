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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#endif

/* utility */
#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "support.h"

/* common */
#include "game.h"

/* server */
#include "notify.h"
#include "srv_main.h"

#include "console.h"

static bool console_show_prompt = FALSE;
static bool console_prompt_is_showing = FALSE;
static bool console_rfcstyle = FALSE;
#ifdef HAVE_LIBREADLINE
static bool readline_received_enter = TRUE;
#else
static int con_dump(enum rfc_status rfc_status, const char *message, ...);
#endif

/************************************************************************
Function to handle log messages.
This must match the log_callback_fn typedef signature.
************************************************************************/
static void con_handle_log(int level, const char *message, bool file_too)
{
  if (LOG_ERROR == level) {
    notify_conn(NULL, NULL, E_LOG_ERROR, ftc_warning, "%s", message);
  } else if (LOG_FATAL >= level) {
    /* Make sure that message is not left to buffers when server dies */
    conn_list_iterate(game.est_connections, pconn) {
      pconn->send_buffer->do_buffer_sends = 0;

#ifdef USE_COMPRESSION
      pconn->compression.frozen_level = 0;
#endif
    } conn_list_iterate_end;

    notify_conn(NULL, NULL, E_LOG_FATAL, ftc_warning, "%s", message);
    notify_conn(NULL, NULL, E_LOG_FATAL, ftc_warning,
                _("Please report this message at %s"),
                BUG_URL);
  }

  /* Write debug/verbose message to console only when not written to file. */
  if (!file_too || level <= LOG_NORMAL) {
    if (console_rfcstyle) {
      con_write(C_LOG_BASE + level, "%s", message);
    } else {
      con_write(C_LOG_BASE + level, "%d: %s", level, message);
    }
  }
}

/************************************************************************
Print the prompt if it is not the last thing printed.
************************************************************************/
static void con_update_prompt(void)
{
  if (console_prompt_is_showing || !console_show_prompt)
    return;

#ifdef HAVE_LIBREADLINE
  if (readline_received_enter) {
    readline_received_enter = FALSE;
  } else {
    rl_forced_update_display();
  }
#else
  con_dump(C_READY,"> ");
  con_flush();
#endif

  console_prompt_is_showing = TRUE;
}

/************************************************************************
  Initialize logging via console.
************************************************************************/
void con_log_init(const char *log_filename, int log_level)
{
  log_init(log_filename, log_level, con_handle_log);
}

#ifndef HAVE_LIBREADLINE
/************************************************************************
Write to console without line-break, don't print prompt.
************************************************************************/
static int con_dump(enum rfc_status rfc_status, const char *message, ...)
{
  static char buf[MAX_LEN_CONSOLE_LINE];
  va_list args;
  
  va_start(args, message);
  my_vsnprintf(buf, sizeof(buf), message, args);
  va_end(args);

  if (console_prompt_is_showing) {
    fc_printf("\n");
  }
  if ((console_rfcstyle) && (rfc_status >= 0)) {
    fc_printf("%.3d %s", rfc_status, buf);
  } else {
    fc_printf("%s", buf);
  }
  console_prompt_is_showing = FALSE;
  return (int) strlen(buf);
}
#endif

/************************************************************************
Write to console and add line-break, and show prompt if required.
************************************************************************/
void con_write(enum rfc_status rfc_status, const char *message, ...)
{
  /* First buffer contains featured text tags */
  static char buf1[(MAX_LEN_CONSOLE_LINE * 3) / 2];
  static char buf2[MAX_LEN_CONSOLE_LINE];
  va_list args;

  va_start(args, message);
  my_vsnprintf(buf1, sizeof(buf1), message, args);
  va_end(args);

  /* remove all format tags */
  featured_text_to_plain_text(buf1, buf2, sizeof(buf2), NULL);
  con_puts(rfc_status, buf2);
}

/************************************************************************
Write to console and add line-break, and show prompt if required.
Same as con_write, but without the format string stuff.
The real reason for this is because __attribute__ complained
with con_write(C_COMMENT,"") of "warning: zero-length format string";
this allows con_puts(C_COMMENT,"");
************************************************************************/
void con_puts(enum rfc_status rfc_status, const char *str)
{
  if (console_prompt_is_showing) {
    fc_printf("\n");
  }
  if ((console_rfcstyle) && (rfc_status >= 0)) {
    fc_printf("%.3d %s\n", rfc_status, str);
  } else {
    fc_printf("%s\n", str);
  }
  console_prompt_is_showing = FALSE;
  con_update_prompt();
}

/************************************************************************
Ensure timely update. 
************************************************************************/
void con_flush(void)
{
  fflush(stdout);
}

/************************************************************************
Set style.
************************************************************************/
void con_set_style(bool i)
{
  console_rfcstyle = i;
  if (console_rfcstyle) 
    con_puts(C_OK, _("Ok. RFC-style set."));
  else
    con_puts(C_OK, _("Ok. Standard style set."));
}

/************************************************************************
Returns rfc-style.
************************************************************************/
bool con_get_style(void)
{
  return console_rfcstyle;
}

/************************************************************************
Initialize prompt; display initial message.
************************************************************************/
void con_prompt_init(void)
{
  static bool first = TRUE;

  if (first) {
    con_puts(C_COMMENT, "");
    con_puts(C_COMMENT, _("For introductory help, type 'help'."));
    first = FALSE;
  }
}

/************************************************************************
Make sure a prompt is printed, and re-printed after every message.
************************************************************************/
void con_prompt_on(void)
{
  console_show_prompt = TRUE;
  con_update_prompt();
}

/************************************************************************
Do not print a prompt after log messages.
************************************************************************/
void con_prompt_off(void)
{
  console_show_prompt = FALSE;
}

/************************************************************************
User pressed enter: will need a new prompt 
************************************************************************/
void con_prompt_enter(void)
{
  console_prompt_is_showing = FALSE;
#ifdef HAVE_LIBREADLINE
  readline_received_enter = TRUE;
#endif
}

/************************************************************************
Clear "user pressed enter" state (used in special cases).
************************************************************************/
void con_prompt_enter_clear(void)
{
  console_prompt_is_showing = TRUE;
#ifdef HAVE_LIBREADLINE
  readline_received_enter = FALSE;
#endif
}
