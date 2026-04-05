/**
 * @file geis_logging.h
 *
 * Copyright 2010 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static const char *prefix_format  = "GEIS(%s)-%s:%d ";
static const char *debug_marker   = "debug";
static const char *warning_marker = "warning";
static const char *error_marker   = "error";


static int
reporting_level()
{
  char *level = getenv("GEIS_DEBUG");
  if (level)
  {
    return atoi(level);
  }
  return 0;
}

static int
level_is_enabled(int desired_level)
{
  static int level = -1;
  if (level < 0)
    level = reporting_level();
  return level >= desired_level;
}


int
_geis_message(int level, const char* function, int line, const char *format, ...)
{
  int count = 0;
  if (level_is_enabled(level))
  {
    const char *marker = NULL;
    switch (level)
    {
      case _GEIS_LOG_LEVEL_DEBUG:
	marker = debug_marker;
	break;
      case _GEIS_LOG_LEVEL_WARNING:
	marker = warning_marker;
	break;
      default:
	marker = error_marker;
	break;
    }

    fprintf(stderr, prefix_format, marker, function, line);

    va_list ap;
    va_start(ap, format);
    count = vfprintf(stderr, format, ap);
    va_end(ap);

    fprintf(stderr, "\n");
  }
  return count;
}


