/**********************************************************************
 Freeciv - Copyright (C) 2004 - The Freeciv Project
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* utility */
#include "capability.h"
#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "mem.h"
#include "registry.h"
#include "shared.h"
#include "support.h"

/* common */
#include "connection.h"
#include "events.h"
#include "game.h"
#include "improvement.h"
#include "map.h"
#include "player.h"
#include "version.h"

/* client */
#include "helpdata.h"
#include "helpdlg_g.h"

/* server */
#include "citytools.h"
#include "commands.h"
#include "connecthand.h"
#include "console.h"
#include "diplhand.h"
#include "gamehand.h"
#include "plrhand.h"
#include "report.h"
#include "ruleset.h"
#include "savegame.h"
#include "settings.h"
#include "srv_main.h"
#include "stdinhand.h"

enum manuals {
  MANUAL_SETTINGS,
  MANUAL_COMMANDS,
  MANUAL_TERRAIN,
  MANUAL_BUILDINGS,
  MANUAL_WONDERS,
  MANUAL_COUNT
};

/* This formats the manual for an HTML wiki. */
#ifdef USE_HTML
#define HEADER "<html><head><link rel=\"stylesheet\" type=\"text/css\" "\
               "href=\"manual.css\"/><meta http-equiv=\"Content-Type\" "\
               "content=\"text/html; charset=UTF-8\"/></head><body>\n\n"
#define SECTION_BEGIN "<h3>"
#define SECTION_END "</h3>"
#define IMAGE_BEGIN "<img src=\""
#define IMAGE_END ".png\">"
#define SEPARATOR " "
#define TAIL "</body></html>"
#else
#define HEADER " "
#define SECTION_BEGIN "==="
#define SECTION_END "==="
#define IMAGE_BEGIN "[[Image:"
#define IMAGE_END ".png]]"
#define SEPARATOR "----\n\n"
#define TAIL " "
#endif

/**************************************************************************
  Replace html special characters ('&', '<' and '>').
**************************************************************************/
static char *html_special_chars(char *str, size_t *len)
{
  char *buf;

  buf = fc_strrep_resize(str, len, "&", "&amp;");
  buf = fc_strrep_resize(buf, len, "<", "&lt;");
  buf = fc_strrep_resize(buf, len, ">", "&gt;");

  return buf;
}

/**************************************************************************
  Useless stubs for compiling client code.
**************************************************************************/
void popup_help_dialog_string(const char *item)
{
  /* Empty stub. */
}

void popdown_help_dialog(void)
{
  /* Empty stub. */
}

/**************************************************************************
  Write a server manual in html format, then quit.
**************************************************************************/
static bool manual_command(void)
{
  FILE *doc;
  char filename[40];
  enum manuals manuals;
  struct connection my_conn;

  /* Default client access. */
  connection_common_init(&my_conn);
  my_conn.access_level = ALLOW_CTRL;

  /* Reset aifill to zero */
  game.info.aifill = 0;
  load_rulesets();
  for (manuals = 0; manuals < MANUAL_COUNT; manuals++) {
    int i;

    my_snprintf(filename, sizeof(filename), "manual%d.html", manuals + 1);

    if (!is_reg_file_for_access(filename, TRUE)
        || !(doc = fc_fopen(filename, "w"))) {
      die(_("Could not write manual file %s."), filename);
      return FALSE;
    }

    fprintf(doc, HEADER);

    switch (manuals) {
    case MANUAL_SETTINGS:
      fprintf(doc, _("<h1>Freeciv %s server options</h1>\n\n"), VERSION_STRING);
      settings_iterate(pset) {
        fprintf(doc, SEPARATOR);
        fprintf(doc, "%s%s - %s%s\n\n", SECTION_BEGIN, setting_name(pset),
                _(setting_short_help(pset)), SECTION_END);
        if (strlen(setting_extra_help(pset)) > 0) {
          char *help = mystrdup(_(setting_extra_help(pset)));
          size_t help_len = strlen(help) + 1;

          help = html_special_chars(help, &help_len);
          fprintf(doc, "<pre>%s</pre>\n\n", help);
          FC_FREE(help);
        }
        fprintf(doc, "<p class=\"misc\">");
        fprintf(doc, _("Level: %s.<br>"), _(setting_level_name(pset)));
        fprintf(doc, _("Category: %s.<br>"), _(setting_category_name(pset)));

        if (!setting_is_changeable(pset, &my_conn, NULL)) {
          fprintf(doc, _("Can only be used in server console. "));
        }
        fprintf(doc, "</p>\n\n");
        switch (setting_type(pset)) {
        case SSET_BOOL:
          fprintf(doc, "<p class=\"bounds\">%s 0, %s %d, %s 1</p>\n\n",
                  _("Minimum:"), _("Default:"),
                  setting_bool_def(pset) ? 1 : 0, _("Maximum:"));
          if (setting_bool_get(pset) != setting_bool_def(pset)) {
            fprintf(doc, _("<p class=\"changed\">Value set to %d</p>\n\n"),
                    setting_bool_get(pset) ? 1 : 0);
          }
          break;
        case SSET_INT:
          fprintf(doc, "<p class=\"bounds\">%s %d, %s %d, %s %d</p>\n\n",
                  _("Minimum:"), setting_int_min(pset),
                  _("Default:"), setting_int_def(pset),
                  _("Maximum:"), setting_int_max(pset));
          if (setting_int_get(pset) != setting_int_def(pset)) {
            fprintf(doc, _("<p class=\"changed\">Value set to %d</p>\n\n"),
                    setting_int_get(pset));
          }
          break;
        case SSET_STRING:
          fprintf(doc, "<p class=\"bounds\">%s \"%s\"</p>\n\n",
                  _("Default:"), setting_str_def(pset));
          if (strcmp(setting_str_get(pset), setting_str_def(pset)) != 0) {
            fprintf(doc, _("<p class=\"changed\">Value set to %s</p>\n\n"),
                    setting_str_get(pset));
          }
          break;
        }
      } settings_iterate_end;
      break;

    case MANUAL_COMMANDS:
      fprintf(doc, _("<h1>Freeciv %s server commands</h1>\n\n"),
              VERSION_STRING);
      for (i = 0; i < CMD_NUM; i++) {
        const struct command *cmd = command_by_number(i);

        fprintf(doc, SEPARATOR);
        fprintf(doc, "%s%s  -  %s%s\n\n", SECTION_BEGIN, command_name(cmd),
                command_short_help(cmd), SECTION_END);
        if (command_synopsis(cmd)) {
          char *cmdstr = mystrdup(command_synopsis(cmd));
          size_t cmdstr_len = strlen(cmdstr) + 1;

          cmdstr = html_special_chars(cmdstr, &cmdstr_len);
          fprintf(doc, _("<table>\n<tr>\n<td valign=\"top\">"
                         "<pre>Synopsis:</pre></td>\n<td>"));
          fprintf(doc, "<pre>%s</pre></td></tr></table>", cmdstr);
          FC_FREE(cmdstr);
        }
        fprintf(doc, _("<p class=\"level\">Level: %s</p>\n\n"),
                cmdlevel_name(command_level(cmd)));
        if (command_extra_help(cmd)) {
          char *help = mystrdup(command_extra_help(cmd));
          size_t help_len = strlen(help) + 1;

          help = html_special_chars(help, &help_len);
          fprintf(doc, _("<p>Description:</p>\n\n"));
          fprintf(doc, "<pre>%s</pre>\n\n", help);
          FC_FREE(help);
        }
      }
      break;

    case MANUAL_TERRAIN:
      fprintf(doc, _("<h1>Freeciv %s terrain help</h1>\n\n"),
              VERSION_STRING);
      fprintf(doc, "<table><tr bgcolor=#9bc3d1><th colspan=2>%s</th>", _("Terrain"));
      fprintf(doc, "<th>F/P/T</th><th>%s</th>", _("Resources"));
      fprintf(doc, "<th>%s<br/>%s</th>", _("Move cost"), _("Defense bonus"));
      fprintf(doc, "<th>%s<br/>%s<br/>%s<br/>%s<br/>(%s)</th>",
              _("Irrigation"), _("Mining"), _("Road"), _("Transform"), _("turns"));
      fprintf(doc, "<th>%s<br/>%s<br/>%s</th></tr>\n\n",
              _("Rail"),
              _("Clean pollution"), _("Clean fallout"));
      terrain_type_iterate(pterrain) {
        struct resource **r;

        if (0 == strlen(terrain_rule_name(pterrain))) {
          /* Must be a disabled piece of terrain */
          continue;
        }

        fprintf(doc, "<tr><td>" IMAGE_BEGIN "%s" IMAGE_END "</td><td>%s</td>",
                pterrain->graphic_str, terrain_name_translation(pterrain));
        fprintf(doc, "<td>%d/%d/%d</td>\n",
                pterrain->output[O_FOOD], pterrain->output[O_SHIELD],
                pterrain->output[O_TRADE]);

        fprintf(doc, "<td><table width=\"100%%\">\n");
        for (r = pterrain->resources; *r; r++) {
          fprintf(doc, "<tr><td>" IMAGE_BEGIN "%s" IMAGE_END "</td><td>%s</td>"
                  "<td align=\"right\">%d/%d/%d</td></tr>\n",
                  (*r)->graphic_str,
                  resource_name_translation(*r),
                  (*r)->output[O_FOOD],
                  (*r)->output[O_SHIELD],
                  (*r)->output[O_TRADE]);
        }
        fprintf(doc, "</table></td>\n");

        fprintf(doc, "<td align=\"center\">%d<br/>+%d%%</td>\n",
                pterrain->movement_cost, pterrain->defense_bonus);

        fprintf(doc, "<td><table width=\"100%%\">\n");
        if (pterrain->irrigation_result == pterrain) {
          fprintf(doc, "<tr><td>+%d F</td><td align=\"right\">(%d)</td></tr>\n",
                  pterrain->irrigation_food_incr, pterrain->irrigation_time);
        } else if (pterrain->irrigation_result == T_NONE) {
          fprintf(doc, "<tr><td>%s</td></tr>\n", _("impossible"));
        } else {
          fprintf(doc, "<tr><td>%s</td><td align=\"right\">(%d)</td></tr>\n",
                  terrain_name_translation(pterrain->irrigation_result),
                  pterrain->irrigation_time);
        }
        if (pterrain->mining_result == pterrain) {
          fprintf(doc, "<tr><td>+%d P</td><td align=\"right\">(%d)</td></tr>\n",
                  pterrain->mining_shield_incr, pterrain->mining_time);
        } else if (pterrain->mining_result == T_NONE) {
          fprintf(doc, "<tr><td>%s</td></tr>\n", _("impossible"));
        } else {
          fprintf(doc, "<tr><td>%s</td><td align=\"right\">(%d)</td></tr>\n",
                  terrain_name_translation(pterrain->mining_result),
                  pterrain->mining_time);
        }
        fprintf(doc, "<tr><td>+%d T</td><td align=\"right\">(%d)</td></tr>\n",
                pterrain->road_trade_incr, pterrain->road_time);

        if (pterrain->transform_result) {
          fprintf(doc, "<tr><td>%s</td><td align=\"right\">(%d)</td></tr>\n</table></td>\n",
                  terrain_name_translation(pterrain->transform_result),
                  pterrain->transform_time);
        } else {
          fprintf(doc, "<tr><td>-</td><td align=\"right\">(-)</td></tr>\n</table></td>\n");
        }

        fprintf(doc, "<td align=\"center\">%d / %d / %d</td></tr>\n\n",
                pterrain->rail_time,
                pterrain->clean_pollution_time, pterrain->clean_fallout_time);
      } terrain_type_iterate_end;
      fprintf(doc, "</table>\n");

      break;

    case MANUAL_BUILDINGS:
    case MANUAL_WONDERS:
      if (manuals == MANUAL_BUILDINGS) {
        fprintf(doc, _("<h1>Freeciv %s buildings help</h1>\n\n"), VERSION_STRING);
      } else {
        fprintf(doc, _("<h1>Freeciv %s wonders help</h1>\n\n"), VERSION_STRING);
      }

      fprintf(doc, "<table>\n<tr bgcolor=#9bc3d1><th colspan=2>%s</th>"
                   "<th>%s<br/>%s</th><th>%s<br/>%s</th><th>%s</th></tr>\n\n",
              _("Name"), _("Cost"), _("Upkeep"),
              _("Requirement"), _("Obsolete by"), _("More info"));

      improvement_iterate(pimprove) {
        char buf[64000];

        if (!valid_improvement(pimprove)
         || is_great_wonder(pimprove) == (manuals == MANUAL_BUILDINGS)) {
          continue;
        }

        helptext_building(buf, sizeof(buf), NULL, NULL, pimprove);

        fprintf(doc, "<tr><td>" IMAGE_BEGIN "%s" IMAGE_END "</td><td>%s</td>\n"
                     "<td align=\"center\"><b>%d</b><br/>%d</td>\n<td>",
                pimprove->graphic_str,
                improvement_name_translation(pimprove),
                pimprove->build_cost,
                pimprove->upkeep);

        requirement_vector_iterate(&pimprove->reqs, req) {
          char text[512];
          fprintf(doc, "%s<br/>",
                  VUT_NONE != req->source.kind
                  ? universal_name_translation(&req->source, text, sizeof(text))
                  : _("None"));
        } requirement_vector_iterate_end;

        fprintf(doc, "<em>%s</em></td>\n",
                valid_advance(pimprove->obsolete_by)
                ? advance_name_translation(pimprove->obsolete_by)
                : _("None"));
        fprintf(doc, "<td>%s</td>\n</tr>\n\n", buf);
      } improvement_iterate_end;
      break;

    case MANUAL_COUNT:
      break;

    } /* switch */

    fprintf(doc, TAIL);
    fclose(doc);
  } /* manuals */

  return TRUE;
}

int main(int argc, char **argv)
{
  int inx;
  bool showhelp = FALSE;
  bool showvers = FALSE;
  char *option = NULL;

  init_nls();
  init_character_encodings(FC_DEFAULT_DATA_ENCODING, FALSE);

  /* Initialize game with default values */
  game_init();

  /* parse command-line arguments... */
  inx = 1;
  while (inx < argc) {
    if ((option = get_option_malloc("--ruleset", argv, &inx, argc))) {
      sz_strlcpy(game.server.rulesetdir, option);
      free(option);
    } else if (is_option("--help", argv[inx])) {
      showhelp = TRUE;
      break;
    } else if (is_option("--version", argv[inx])) {
      showvers = TRUE;
    }
    inx++;
  }

  if (showvers && !showhelp) {
    fc_fprintf(stderr, "%s \n", freeciv_name_version());
    exit(EXIT_SUCCESS);
  } else if (showhelp) {
    fc_fprintf(stderr,
         _("Usage: %s [option ...]\nValid options are:\n"), argv[0]);
    fc_fprintf(stderr,
         _("  -r, --ruleset RULESET  Make manual for RULESET\n"));
    fc_fprintf(stderr,
         _("  -h, --help             Print a summary of the options\n"));
    fc_fprintf(stderr,
         _("  -v, --version          Print the version number\n"));
    /* TRANS: No full stop after the URL, could cause confusion. */
    fc_fprintf(stderr, _("Report bugs at %s\n"), BUG_URL);
    exit(EXIT_SUCCESS);
  }

  manual_command();

  return 0;
}
