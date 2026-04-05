/**
 * @file geis_grail_xsync.c
 * @brief Handles XSync lib usage for the GEIS grail backend
 */
/*
 * Copyright 2012 Canonical Ltd.
 *
 * This file is part of GEIS.
 *
 * GEIS is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * GEIS is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with GEIS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "geis_grail_xsync.h"

#include "geis_bag.h"
#include "geis_logging.h"
#include "geis/geis.h"
#include <X11/extensions/sync.h>
#include <string.h>

struct GeisGrailAlarm {
  XSyncAlarm xsync_alarm;
  uint64_t timeout;
};

struct GeisGrailXSync
{
  Display *display;
  XSyncCounter   server_time_counter;
  int xsync_event_base;
  GeisBag alarms; /* List of GeisGrailAlarm, describing existing alarms */
};

GeisGrailXSync
geis_grail_xsync_new(Display *display)
{
  GeisGrailXSync self = malloc(sizeof(struct GeisGrailXSync));
  if (!self)
  {
    geis_error("failed to allocate new GeisGrailXSync");
    goto return_failure;
  }

  self->display = display;
  self->xsync_event_base = -1;

  self->alarms = geis_bag_new(sizeof(struct GeisGrailAlarm), 4, 4.0f);
  if (!self->alarms)
  {
    geis_error("failed to create GeisGrailXSync.alarms bag");
    goto return_failure;
  }

  /* check for the presence of the XSync extension */
  int error_base;
  if (XSyncQueryExtension(self->display, &self->xsync_event_base, &error_base) != True)
  {
    geis_warning("XSync extension is not available");
    goto return_failure;
  }

  /* initialize the XSync extension */
  int major;
  int minor;
  if (XSyncInitialize(self->display, &major, &minor) != True)
  {
    geis_warning("failed to initialize XSync extension");
    goto return_failure;
  }

  /* find the server-time counter */
  int num_system_counters;
  XSyncSystemCounter *counters;
  counters = XSyncListSystemCounters(self->display, &num_system_counters);
  GeisBoolean found_counter = GEIS_FALSE;
  for (int i = 0; i < num_system_counters; ++i)
  {
    if (0 == strcmp(counters[i].name, "SERVERTIME"))
    {
      self->server_time_counter = counters[i].counter;
      found_counter = GEIS_TRUE;
      break;
    }
  }
  XSyncFreeSystemCounterList(counters);
  if (!found_counter)
    geis_warning("couldn't find SERVERTIME XSyncCounter");

  return self;

return_failure:
  if (self->alarms)
    geis_bag_delete(self->alarms);

  if (self)
    free(self);

  return NULL;
}

void
geis_grail_xsync_delete(GeisGrailXSync self)
{
  geis_bag_delete(self->alarms);
  free(self);
}

/**
 * Creates and returns an XSyncAlarm configured with the given timeout or
 * None on failure
 */
static XSyncAlarm
_geis_grail_xsync_create_alarm(GeisGrailXSync self, uint64_t timeout)
{
  XSyncAlarmAttributes alarm_attributes;
  alarm_attributes.trigger.counter = self->server_time_counter;
  alarm_attributes.trigger.value_type = XSyncAbsolute;
  XSyncIntsToValue(&alarm_attributes.trigger.wait_value, timeout & 0xffffffff,
                   timeout & 0xffffffff00000000);
  alarm_attributes.trigger.test_type = XSyncPositiveComparison;
  alarm_attributes.events = True;

  return XSyncCreateAlarm(self->display,
                          XSyncCACounter | XSyncCAValueType | XSyncCAValue
                          | XSyncCATestType | XSyncCAEvents,
                          &alarm_attributes);
}

/**
 * Returns GEIS_TRUE if an XSyncAlarm exists for the given timeout or
 * GEIS_FALSE otherwise
 */
static GeisBoolean
_geis_grail_xsync_alarm_exists_for_timeout(GeisGrailXSync self, uint64_t timeout)
{
  for (GeisSize i = 0, alarms_count = geis_bag_count(self->alarms);
       i < alarms_count; ++i)
  {
    struct GeisGrailAlarm *alarm = geis_bag_at(self->alarms, i);
    if (alarm->timeout == timeout)
      return GEIS_TRUE;
  }
  return GEIS_FALSE;
}

void
geis_grail_xsync_set_timeout(GeisGrailXSync self, uint64_t timeout)
{
  if (_geis_grail_xsync_alarm_exists_for_timeout(self, timeout))
    return;

  struct GeisGrailAlarm alarm;

  alarm.xsync_alarm = _geis_grail_xsync_create_alarm(self, timeout);
  if (alarm.xsync_alarm == None)
  {
    geis_error("failed to create an XSync alarm.");
    return;
  }
  /* make sure xserver creates the alarm now, otherwise it could in theory
     happen only after the desired timeout period */
  XFlush(self->display); 

  alarm.timeout = timeout;

  geis_bag_append(self->alarms, &alarm);
}

/**
 * If the given XSyncAlarm exists, it's destroyed, removed from the
 * list of alarms and the function returns GEIS_TRUE.
 *
 * If the given XSyncAlarm doesn't exist, the function returns GEIS_FALSE.
 */
static GeisBoolean
_geis_grail_xsync_destroy_alarm(GeisGrailXSync self, XSyncAlarm xsync_alarm)
{
  for (GeisSize i = 0, alarms_count = geis_bag_count(self->alarms);
       i < alarms_count; ++i)
  {
    struct GeisGrailAlarm *alarm = geis_bag_at(self->alarms, i);
    if (alarm->xsync_alarm == xsync_alarm)
    {
      if (XSyncDestroyAlarm(self->display, xsync_alarm) == False)
        geis_error("failed to destroy XSync alarm");

      geis_bag_remove(self->alarms, i);
      return GEIS_TRUE;
    }
  }
  return GEIS_FALSE;
}

GeisBoolean
geis_grail_xsync_is_timeout(GeisGrailXSync self, const XEvent *event)
{
  if (event->type != (self->xsync_event_base + XSyncAlarmNotify))
    return GEIS_FALSE;

  const XSyncAlarmNotifyEvent *alarm_notify =
      (const XSyncAlarmNotifyEvent *) event;

  /* If the destruction isn't successful it means that this alarm has already
     been destroyed due to a previous XSyncAlarmNotifyEvent. Therefore its
     corresponding timeout has already happened and we shouldn't confirm it
     again. */
  return _geis_grail_xsync_destroy_alarm(self, alarm_notify->alarm);
}

uint64_t
geis_grail_xsync_get_server_time(const XEvent *event)
{
  const XSyncAlarmNotifyEvent *alarm_notify =
      (const XSyncAlarmNotifyEvent *) event;

  XSyncValue time = alarm_notify->counter_value;

  return (uint64_t)XSyncValueHigh32(time) << 32 | XSyncValueLow32(time);
}
