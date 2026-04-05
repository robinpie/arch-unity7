/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by
 *             Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

#include "zeitgeist-timerange.h"

/**
 * SECTION:zeitgeist-timestamp
 * @short_description: Convenience functions for dealing with timestamps and dates
 * @include: zeitgeist.h
 *
 * A suite of convenience functions for dealing with timestamps and dates.
 *
 * Zeitgeist timestamps are represented as #gint64<!-- -->s with the number
 * of milliseconds since the Unix Epoch. 
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-timestamp.h"

/**
 * zeitgeist_timestamp_from_timeval:
 * @tv: A #GTimeVal instance
 *
 * Returns: A #gint64 with the total number of milliseconds since the Unix Epoch
 */
gint64
zeitgeist_timestamp_from_timeval (GTimeVal *tv)
{
  g_return_val_if_fail (tv != NULL, -1);

  return ((gint64)tv->tv_sec)*G_GINT64_CONSTANT(1000) +
         ((gint64)tv->tv_usec)/G_GINT64_CONSTANT(1000);
}

/**
 * zeitgeist_timestamp_to_timeval:
 * @timestamp: A #gint64 with a number of milliseconds since the Unix Epoch
 * @tv: A #GTimeVal instance to store the result in
 *
 * Write a Zeitgeist timestamp to a #GTimeVal instance. Note that Zeitgeist
 * uses only a millisecond resolution for where #GTimeVal uses a microsecond
 * resolution, meaning that the lower three digits of @tv.tv_usec will
 * be 0.
 * 
 * Returns: Nothing. The result is stored in @tv
 */
void
zeitgeist_timestamp_to_timeval (gint64 timestamp, GTimeVal *tv)
{
  g_return_if_fail (tv != NULL);

  tv->tv_sec = timestamp / G_GINT64_CONSTANT(1000);
  tv->tv_usec = (timestamp % G_GINT64_CONSTANT(1000)) * G_GINT64_CONSTANT(1000);
}
        
/**
 * zeitgeist_timestamp_for_now:
 *
 * Get the timestamp for the current system time
 *
 * Returns: The timestamp for the current system time. Ie. the number of
 *          milliseconds since the Unix Epoch
 */
gint64
zeitgeist_timestamp_for_now (void)
{
  GTimeVal            tv;

  g_get_current_time (&tv);
  return zeitgeist_timestamp_from_timeval (&tv);
}

/**
 * zeitgeist_timestamp_from_iso8601:
 * @datetime: A string containing an iso8601 conforming datetime specification
 *
 * Parse a timestamp from a ISO8601 encoded string.
 *
 * Returns: The timestamp represented by @datetime, or -1 on failure to parse
 *          @datetime
 */
gint64
zeitgeist_timestamp_from_iso8601 (const gchar *datetime)
{
  GTimeVal            tv;
  
  g_return_val_if_fail (datetime != NULL, -1);
  
  if (g_time_val_from_iso8601 (datetime, &tv))
    return zeitgeist_timestamp_from_timeval (&tv);
  else
    return -1;
}

/**
 * zeitgeist_timestamp_to_iso8601:
 * @timestamp: A Zeitgeist timestamp in ms since the Epoch
 *
 * Convert a timestamp to a human readable ISO8601 format
 *
 * Returns: A newly allocated string containing the ISO8601 format of
 *          @timestamp. Free with g_free().
 */
gchar*
zeitgeist_timestamp_to_iso8601 (gint64 timestamp)
{
  GTimeVal            tv;
  
  zeitgeist_timestamp_to_timeval (timestamp, &tv);
  return g_time_val_to_iso8601 (&tv);
}

/**
 * zeitgeist_timestamp_from_date:
 * @date: A #GDate to convert
 *
 * Convert a #GDate to a Zeitgeist timestamp.
 *
 * Returns: @date as a timestamp in milliseconds since the Epoch. The timestamp
 *          is guaranteed to be rounded off to the midnight of the given date.
 */
gint64
zeitgeist_timestamp_from_date (GDate *date)
{
  g_return_val_if_fail (date != NULL, -1);
  
  gint64 julian = g_date_get_julian (date);
  
  return zeitgeist_timestamp_prev_midnight (
                julian*ZEITGEIST_TIMESTAMP_DAY - 1969*ZEITGEIST_TIMESTAMP_YEAR);
}

/**
 * zeitgeist_timestamp_from_dmy:
 * @day: The day of the month represented as a #GDateDay
 * @month: The month of the year represented as a #GDateMonth
 * @year: The year represented as a #GDateYear
 *
 * Convert a day, month, year tuple into a Zeitgeist timestamp
 *
 * Returns: Input data as a timestamp in milliseconds since the Epoch or
 *          -1 in case the provided data does not constitute a valid date.
 *          If the date is valid the returned timestamp is guaranteed to be
 *          rounded off to the midnight of the given date
 */
gint64
zeitgeist_timestamp_from_dmy (GDateDay day,
                              GDateMonth month,
                              GDateYear year)
{
  GDate date = { 0 };
  
  g_return_val_if_fail (g_date_valid_dmy (day, month, year), -1);
  
  g_date_set_dmy (&date, day, month, year);
  return zeitgeist_timestamp_from_date (&date);
}

/**
 * zeitgeist_timestamp_to_date:
 * @timestamp: The Zeitgeist timestamp to convert to a #GDate
 * @date: The place to store the result in. Must be non-%NULL
 *
 * Write a timestamp to a #GDate structure
 *
 * Returns: Nothing. The return value is written to @date
 */
void
zeitgeist_timestamp_to_date (gint64 timestamp,
                             GDate *date)
{
  GTimeVal            tv;
  
  g_return_if_fail (date != NULL);
  
  zeitgeist_timestamp_to_timeval (timestamp, &tv);
  g_date_set_time_val (date, &tv);
}

/**
 * zeitgeist_timestamp_next_midnight:
 * @timestamp: The Zeitgeist timestamp to find the next midnight for
 *
 * Calculate the timestamp for the next midnight after @timestamp
 *
 * Returns: Returns the timestamp for the first midnight after @timestamp.
 *          If @timestamp is already midnight (down to the millisecond) this
 *          method will return the value for the next midnight. In other words
 *          you can call this method recursively in order to iterate, forwards
 *          in time, over midnights.
 */
gint64
zeitgeist_timestamp_next_midnight (gint64       timestamp)
{
  gint remainder = timestamp % ZEITGEIST_TIMESTAMP_DAY;
  
  if (remainder == 0)
    return timestamp + ZEITGEIST_TIMESTAMP_DAY;
  else
    return (timestamp - remainder) + ZEITGEIST_TIMESTAMP_DAY;
}

/**
 * zeitgeist_timestamp_prev_midnight:
 * @timestamp: The Zeitgeist timestamp to find the previous midnight for
 *
 * Calculate the timestamp for the midnight just before @timestamp
 *
 * Returns: Returns the timestamp for the first midnight just before @timestamp.
 *          If @timestamp is already midnight (down to the millisecond) this
 *          method will return the value for the midnight before. In other words
 *          you can call this method recursively in order to iterate, backwards
 *          in time, over midnights.
 */
gint64
zeitgeist_timestamp_prev_midnight (gint64       timestamp)
{
  gint remainder = timestamp % ZEITGEIST_TIMESTAMP_DAY;
  
  if (remainder == 0)
    return timestamp - ZEITGEIST_TIMESTAMP_DAY;
  else
    return (timestamp - remainder);
}
