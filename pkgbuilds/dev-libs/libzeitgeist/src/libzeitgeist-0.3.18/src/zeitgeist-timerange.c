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
 * SECTION:zeitgeist-timerange
 * @short_description: Immutable representation of an interval in time, marked by a beginning and an end
 * @include: zeitgeist.h
 *
 * #ZeitgeistTimeRange is a light, immutable, encapsulation of an interval
 * in time, marked by a beginning and an end.
 *
 * Time ranges are sub classes of #GInitiallyUnowned which means that
 * you may pass them directly to methods provided by the Zeitgeist library
 * and forget about them. The callee will own the floating reference.
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-timerange.h"
#include "zeitgeist-timestamp.h"

G_DEFINE_TYPE (ZeitgeistTimeRange, zeitgeist_time_range, G_TYPE_INITIALLY_UNOWNED);
#define ZEITGEIST_TIME_RANGE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_TIME_RANGE, ZeitgeistTimeRangePrivate))

typedef struct
{
  gint64 start;
  gint64 end;
} ZeitgeistTimeRangePrivate;

static void
zeitgeist_time_range_init (ZeitgeistTimeRange *object)
{
  
}

static void
zeitgeist_time_range_finalize (GObject *object)
{
  /*ZeitgeistTimeRange *time_range = ZEITGEIST_TIME_RANGE (object);
  ZeitgeistTimeRangePrivate *priv;
  
  priv = ZEITGEIST_TIME_RANGE_GET_PRIVATE (time_range);*/
  
  G_OBJECT_CLASS (zeitgeist_time_range_parent_class)->finalize (object); 
}

static void
zeitgeist_time_range_class_init (ZeitgeistTimeRangeClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  
  /*object_class->finalize = zeitgeist_time_range_finalize;*/

  g_type_class_add_private (object_class, sizeof (ZeitgeistTimeRangePrivate));
}

/**
 * zeitgeist_time_range_new:
 * @start_msec: Starting timestamp in number of milliseconds since the Unix Epoch
 * @end_msec: Ending timestamp in number of milliseconds since the Unix Epoch
 *
 * Returns: A newly allocated #ZeitgeistTimeRange. Free with g_object_unref().
 */
ZeitgeistTimeRange*
zeitgeist_time_range_new (gint64 start_msec,
                          gint64 end_msec)
{
  ZeitgeistTimeRange        *time_range;
  ZeitgeistTimeRangePrivate *priv;

  time_range = (ZeitgeistTimeRange*) g_object_new (ZEITGEIST_TYPE_TIME_RANGE,
                                                   NULL);

  priv = ZEITGEIST_TIME_RANGE_GET_PRIVATE (time_range);
  priv->start = start_msec;
  priv->end = end_msec;

  return time_range;
}

/**
 * zeitgeist_time_range_new_from_variant:
 * @time_range: A #GVariant with signature
 *              #ZEITGEIST_TIME_RANGE_VARIANT_SIGNATURE. If @time_range is
 *              a floating reference this method will assume ownership of it
 *
 * Create a #ZeitgeistTimeRange from a #GVariant
 *
 * Returns: A floating reference to a newly created #ZeitgeistTimeRange
 */
ZeitgeistTimeRange*
zeitgeist_time_range_new_from_variant  (GVariant *time_range)
{
  gint64 start, end;

  g_return_val_if_fail (time_range != NULL, NULL);

  g_variant_ref_sink (time_range);
  g_variant_get (time_range, ZEITGEIST_TIME_RANGE_VARIANT_SIGNATURE,
                 &start, &end);
  g_variant_unref (time_range);

  return zeitgeist_time_range_new (start, end);
}

/**
 * zeitgeist_time_range_new_from_now
 *
 * Returns: A new time range starting from the moment of invocation to the end
 *          of time
 */
ZeitgeistTimeRange*
zeitgeist_time_range_new_from_now (void)
{
  return zeitgeist_time_range_new (zeitgeist_timestamp_for_now (),
                                   G_MAXINT64);
}

/**
 * zeitgeist_time_range_new_to_now
 *
 * Returns: A new time range starting from the beginning of the Unix Epoch
 *          ending at the moment of invocation
 */
ZeitgeistTimeRange*
zeitgeist_time_range_new_to_now (void)
{
  return zeitgeist_time_range_new (0, zeitgeist_timestamp_for_now ());
}

/**
 * zeitgeist_time_range_new_anytime
 *
 * Returns: A new time range starting from the beginning of the Unix Epoch
 *          stretching to the end of time
 */
ZeitgeistTimeRange*
zeitgeist_time_range_new_anytime (void)
{
  return zeitgeist_time_range_new (0, G_MAXINT64);
}

/**
 * zeitgeist_time_range_get_start:
 * @time_range: The time range to inspect
 *
 * Returns: The starting timestamp of the time range in miiliseconds since the
 *          Unix Epoch
 */
gint64
zeitgeist_time_range_get_start (ZeitgeistTimeRange *time_range)
{
  ZeitgeistTimeRangePrivate *priv;

  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), 0);

  priv = ZEITGEIST_TIME_RANGE_GET_PRIVATE (time_range);
  return priv->start;
}

/**
 * zeitgeist_time_range_get_end:
 * @time_range: The time range to inspect
 *
 * Returns: The ending timestamp of the time range in miiliseconds since the
 *          Unix Epoch
 */
gint64
zeitgeist_time_range_get_end (ZeitgeistTimeRange *time_range)
{
  ZeitgeistTimeRangePrivate *priv;

  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), 0);

  priv = ZEITGEIST_TIME_RANGE_GET_PRIVATE (time_range);
  return priv->end;
}

/**
 * zeitgeist_time_range_get_start_iso8601:
 * @time_range: The time range to extract the start timestamp from
 *
 * Converts the a timestamp to a human readable format. Eg. 1970-01-01T00:00:00Z
 * or 2010-04-09T07:24:08.082000Z. Note that the timezone of the formatted
 * string will always be UTC disregarding locale.
 *
 * Returns: A newly allocated string containing the timestamp formatted
 *          according to ISO-8601. Free with g_free().
 */
gchar*
zeitgeist_time_range_get_start_iso8601 (ZeitgeistTimeRange *time_range)
{
  GTimeVal tv;

  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), NULL);

  zeitgeist_timestamp_to_timeval (zeitgeist_time_range_get_start (time_range),
                                  &tv);
  return g_time_val_to_iso8601 (&tv);
}

/**
 * zeitgeist_time_range_get_end_iso8601:
 * @time_range: The time range to extract the end timestamp from
 *
 * Converts the a timestamp to a human readable format. Eg. 1970-01-01T00:00:00Z
 * or 2010-04-09T07:24:08.082000Z. Note that the timezone of the formatted
 * string will always be UTC disregarding locale.
 *
 * Returns: A newly allocated string containing the timestamp formatted
 *          according to ISO-8601. Free with g_free().
 */
gchar*
zeitgeist_time_range_get_end_iso8601 (ZeitgeistTimeRange *time_range)
{
  GTimeVal tv;

  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), NULL);

  zeitgeist_timestamp_to_timeval (zeitgeist_time_range_get_end (time_range),
                                  &tv);
  return g_time_val_to_iso8601 (&tv);
}

/**
 * zeitgeist_time_range_to_variant:
 * @time_range: A #ZeitgeistTimeRange. If the reference is floating this method
 *              will assume ownership of it
 *
 * Returns: A floating reference to a #GVariant with signature
 *          #ZEITGEIST_TIME_RANGE_VARIANT_SIGNATURE
 */
GVariant*
zeitgeist_time_range_to_variant  (ZeitgeistTimeRange *time_range)
{
  g_return_val_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range), NULL);

  return g_variant_new (ZEITGEIST_TIME_RANGE_VARIANT_SIGNATURE,
                        zeitgeist_time_range_get_start (time_range),
                        zeitgeist_time_range_get_end (time_range));
}
