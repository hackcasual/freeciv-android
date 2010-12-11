/********************************************************************** 
 Freeciv - Copyright (C) 2003-2004 - The Freeciv Project
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

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#ifdef HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#ifdef HAVE_LIBCHARSET
#include <libcharset.h>
#endif

#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "mem.h"
#include "support.h"

static bool is_init = FALSE;
static char convert_buffer[4096];

/*
  See PR#40028 for additional explanation.

  The data_encoding is used in all data files and network transactions.
  This is UTF-8.  Currently, the rulesets are in latin1 (ISO-8859-1).

  The internal_encoding is used internally within freeciv.  This is always
  UTF-8 at the server, but can be configured by the GUI client.  (When your
  charset is the same as your GUI library, GUI writing is easier.)

  The local_encoding is the one supported on the command line.  This is not
  under our control, and all output to the command line must be converted.
*/
#ifdef HAVE_ICONV
static const char *local_encoding, *data_encoding, *internal_encoding;
static const char *transliteration_string;
#else
/* Hack to confuse the compiler into working. */
#  define local_encoding get_local_encoding()
#  define data_encoding get_local_encoding()
#  define internal_encoding get_local_encoding()
#  define transliteration_string get_local_encoding()
#endif

/***************************************************************************
  Must be called during the initialization phase of server and client to
  initialize the character encodings to be used.

  Pass an internal encoding of NULL to use the local encoding internally.
***************************************************************************/
void init_character_encodings(const char *my_internal_encoding,
			      bool my_use_transliteration)
{
#ifdef HAVE_ICONV
  if (my_use_transliteration) {
    transliteration_string = "//TRANSLIT";
  } else {
    transliteration_string = "";
  }

  /* Set the data encoding - first check $FREECIV_DATA_ENCODING,
   * then fall back to the default. */
  data_encoding = getenv("FREECIV_DATA_ENCODING");
  if (!data_encoding) {
    /* Currently the rulesets are in latin1 (ISO-8859-1). */
    data_encoding = FC_DEFAULT_DATA_ENCODING;
  }

  /* Set the local encoding - first check $FREECIV_LOCAL_ENCODING,
   * then ask the system. */
  local_encoding = getenv("FREECIV_LOCAL_ENCODING");
  if (!local_encoding) {
#ifdef HAVE_LIBCHARSET
    local_encoding = locale_charset();
#else
#ifdef HAVE_LANGINFO_CODESET
    local_encoding = nl_langinfo(CODESET);
#else
    local_encoding = "";
#endif
#endif
    if (mystrcasecmp(local_encoding, "ANSI_X3.4-1968") == 0
	|| mystrcasecmp(local_encoding, "ASCII") == 0
	|| mystrcasecmp(local_encoding, "US-ASCII") == 0) {
      /* HACK: use latin1 instead of ascii in typical cases when the
       * encoding is unconfigured. */
      local_encoding = "ISO-8859-1";
    }

    if (mystrcasecmp(local_encoding, "646") == 0) {
      /* HACK: On Solaris the encoding always comes up as "646" (ascii),
       * which iconv doesn't understand.  Work around it by using UTF-8
       * instead. */
      local_encoding = "UTF-8";
    }
  }

  /* Set the internal encoding - first check $FREECIV_INTERNAL_ENCODING,
   * then check the passed-in default value, then fall back to the local
   * encoding. */
  internal_encoding = getenv("FREECIV_INTERNAL_ENCODING");
  if (!internal_encoding) {
    internal_encoding = my_internal_encoding;

    if (!internal_encoding) {
      internal_encoding = local_encoding;
    }
  }

#ifdef ENABLE_NLS
  bind_textdomain_codeset(PACKAGE, internal_encoding);
#endif

#ifdef DEBUG
  /* FIXME: Remove this output when this code has stabilized. */
  fprintf(stderr, "Encodings: Data=%s, Local=%s, Internal=%s\n",
	     data_encoding, local_encoding, internal_encoding);
#endif

#else
   /* freelog may not work at this point. */
  fprintf(stderr,
	     _("You are running Freeciv without using iconv.  Unless\n"
	       "you are using the latin1 character set, some characters\n"
	       "may not be displayed properly.  You can download iconv\n"
	       "at http://gnu.org/.\n"));
#endif

  is_init = TRUE;
}

/***************************************************************************
  Return the data encoding (usually UTF-8).
***************************************************************************/
const char *get_data_encoding(void)
{
  assert(is_init);
  return data_encoding;
}

/***************************************************************************
  Return the local encoding (dependent on the system).
***************************************************************************/
const char *get_local_encoding(void)
{
#ifdef HAVE_ICONV
  assert(is_init);
  return local_encoding;
#else
#  ifdef HAVE_LIBCHARSET
  return locale_charset();
#  else
#    ifdef HAVE_LANGINFO_CODESET
  return nl_langinfo(CODESET);
#    else
  return "";
#    endif
#  endif
#endif
}

/***************************************************************************
  Return the internal encoding.  This depends on the server or GUI being
  used.
***************************************************************************/
const char *get_internal_encoding(void)
{
  assert(is_init);
  return internal_encoding;
}

/***************************************************************************
  Convert the text.  Both 'from' and 'to' must be 8-bit charsets.  The
  result will be put into the buf buffer unless it is NULL, in which case it
  will be allocated on demand.

  Don't use this function if you can avoid it.  Use one of the
  xxx_to_yyy_string functions.
***************************************************************************/
char *convert_string(const char *text,
		     const char *from,
		     const char *to,
		     char *buf, size_t bufsz)
{
#ifdef HAVE_ICONV
  iconv_t cd = iconv_open(to, from);
  size_t from_len = strlen(text) + 1, to_len;
  bool alloc = (buf == NULL);

  assert(is_init && from != NULL && to != NULL);
  assert(text != NULL);

  if (cd == (iconv_t) (-1)) {
    /* TRANS: "Could not convert text from <encoding a> to <encoding b>:" 
     *        <externally translated error string>."*/
    freelog(LOG_ERROR, _("Could not convert text from %s to %s: %s"), from,
	    to, fc_strerror(fc_get_errno()));
    /* The best we can do? */
    if (alloc) {
      return mystrdup(text);
    } else {
      my_snprintf(buf, bufsz, "%s", text);
      return buf;
    }
  }

  if (alloc) {
    to_len = from_len;
  } else {
    to_len = bufsz;
  }

  do {
    size_t flen = from_len, tlen = to_len, res;
    const char *mytext = text;
    char *myresult;

    if (alloc) {
      buf = fc_malloc(to_len);
    }

    myresult = buf;

    /* Since we may do multiple translations, we may need to reset iconv
     * in between. */
    iconv(cd, NULL, NULL, NULL, NULL);

    res = iconv(cd, (ICONV_CONST char **)&mytext, &flen, &myresult, &tlen);
    if (res == (size_t) (-1)) {
      if (errno != E2BIG) {
	/* Invalid input. */
	freelog(LOG_ERROR, "Invalid string conversion from %s to %s.",
		from, to);
	iconv_close(cd);
	if (alloc) {
	  free(buf);
	  return mystrdup(text); /* The best we can do? */
	} else {
	  my_snprintf(buf, bufsz, "%s", text);
	  return buf;
	}
      }
    } else {
      /* Success. */
      iconv_close(cd);

      /* There may be wasted space here, but there's nothing we can do
       * about it. */
      return buf;
    }

    if (alloc) {
      /* Not enough space; try again. */
      buf[to_len - 1] = 0;
      freelog(LOG_VERBOSE, "   Result was '%s'.", buf);

      free(buf);
      to_len *= 2;
    }
  } while (alloc);

  return buf;
#else /* HAVE_ICONV */
  if (buf) {
    strncpy(buf, text, bufsz);
    buf[bufsz - 1] = '\0';
    return buf;
  } else {
    return mystrdup(text);
  }
#endif /* HAVE_ICONV */
}

#define CONV_FUNC_MALLOC(src, dst)                                          \
char *src ## _to_ ## dst ## _string_malloc(const char *text)                \
{                                                                           \
  const char *encoding1 = (dst ## _encoding);				    \
  char encoding[strlen(encoding1) + strlen(transliteration_string) + 1];    \
									    \
  my_snprintf(encoding, sizeof(encoding),				    \
	      "%s%s", encoding1, transliteration_string);		    \
  return convert_string(text, (src ## _encoding),			    \
			(encoding), NULL, 0);				    \
}

#define CONV_FUNC_BUFFER(src, dst)                                          \
char *src ## _to_ ## dst ## _string_buffer(const char *text,                \
					   char *buf, size_t bufsz)         \
{                                                                           \
  const char *encoding1 = (dst ## _encoding);				    \
  char encoding[strlen(encoding1) + strlen(transliteration_string) + 1];    \
									    \
  my_snprintf(encoding, sizeof(encoding),				    \
	      "%s%s", encoding1, transliteration_string);		    \
  return convert_string(text, (src ## _encoding),			    \
                        encoding, buf, bufsz);				    \
}

#define CONV_FUNC_STATIC(src, dst)                                          \
char *src ## _to_ ## dst ## _string_static(const char *text)                \
{                                                                           \
  (src ## _to_ ## dst ## _string_buffer)(text,                              \
					convert_buffer,                     \
					sizeof(convert_buffer));            \
  return convert_buffer;                                                    \
}

CONV_FUNC_MALLOC(data, internal)
CONV_FUNC_MALLOC(internal, data)
CONV_FUNC_MALLOC(internal, local)
CONV_FUNC_MALLOC(local, internal)

CONV_FUNC_BUFFER(local, internal)
CONV_FUNC_BUFFER(internal, local)

static CONV_FUNC_STATIC(internal, local)

/***************************************************************************
  Do a fprintf from the internal charset into the local charset.
***************************************************************************/
void fc_fprintf(FILE *stream, const char *format, ...)
{
	return;
  va_list ap;
  char string[4096];
  const char *output;
  static bool recursion = FALSE;

  /* The recursion variable is used to prevent a recursive loop.  If
   * an iconv conversion fails, then freelog will be called and an
   * fc_fprintf will be done.  But below we do another iconv conversion
   * on the error messages, which is of course likely to fail also. */
  if (recursion) {
    return;
  }

  va_start(ap, format);
  my_vsnprintf(string, sizeof(string), format, ap);
  va_end(ap);

  /*recursion = TRUE;
  if (is_init) {
    output = internal_to_local_string_static(string);
  } else {*/
    output = string;
  //}
  recursion = FALSE;

  fputs(output, stream);
  fflush(stream);
}

/****************************************************************************
  Return the length, in *characters*, of the string.  This can be used in
  place of strlen in some places because it returns the number of characters
  not the number of bytes (with multi-byte characters in UTF-8, the two
  may not be the same).

  Use of this function outside of GUI layout code is probably a hack.  For
  instance the demographics code uses it, but this should instead pass the
  data directly to the GUI library for formatting.
****************************************************************************/
size_t get_internal_string_length(const char *text)
{
  int text2[(strlen(text) + 1)]; /* UCS-4 text */
  int i = 0;

  convert_string(text, internal_encoding, "UCS-4",
		 (char *)text2, sizeof(text2));
  assert(text2[0] != 0x0000FEFF && text2[0] != 0xFFFE0000); /* No BOM */
  for (i = 0; ; i++) {
    if (text2[i] == 0) {
      return i;
    }
  }
}
