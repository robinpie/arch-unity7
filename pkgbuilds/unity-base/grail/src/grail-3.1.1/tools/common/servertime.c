/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "common/servertime.h"

#include <stdio.h>
#include <string.h>

XSyncAlarm create_alarm(Display *display) {
  int event_base;
  int error_base;
  int major_version;
  int minor_version;
  XSyncSystemCounter *counters;
  int num_system_counters;
  XSyncCounter counter = -1;
  XSyncAlarmAttributes attrs;
  XSyncAlarm alarm;
  int i;

  if (XSyncQueryExtension(display, &event_base, &error_base) != True) {
    fprintf(stderr, "Warning: failed to query sync extension\n");
    return -1;
  }

  if (XSyncInitialize(display, &major_version, &minor_version) != True) {
    fprintf(stderr, "Warning: failed to initialize sync extension\n");
    return -1;
  }

  counters = XSyncListSystemCounters(display, &num_system_counters);
  for (i = 0; i < num_system_counters; ++i) {
    if (strcmp(counters[i].name, "SERVERTIME") == 0) {
      counter = counters[i].counter;
      break;
    }
  }

  XSyncFreeSystemCounterList(counters);

  if (i == num_system_counters) {
    fprintf(stderr, "Warning: failed to find SERVERTIME counter\n");
    return None;
  }

  attrs.trigger.counter = counter;
  attrs.trigger.value_type = XSyncAbsolute;
  attrs.trigger.test_type = XSyncPositiveComparison;
  attrs.delta.hi = 0;
  attrs.delta.lo = 0;
  attrs.events = True;

  alarm = XSyncCreateAlarm(display,
                           XSyncCACounter | XSyncCAValueType | XSyncCATestType |
                           XSyncCAEvents | XSyncCADelta, &attrs);
  if (alarm == None)
    fprintf(stderr, "Warning: failed to create XSync alarm\n");

  return alarm;
}

void destroy_alarm(Display *display, XSyncAlarm alarm) {
  if (alarm != None)
    XSyncDestroyAlarm(display, alarm);
}

void update_time(UGHandle handle, XSyncAlarmNotifyEvent *event) {
  XSyncValue value = event->counter_value;
  uint64_t grail_time = (uint64_t)value.hi << 32 | value.lo;

  grail_update_time(handle, grail_time);
}

void set_timeout(UGHandle handle, Display *display, XSyncAlarm alarm) {
  if (alarm == None)
    return;

  uint64_t timeout = grail_next_timeout(handle);
  if (timeout) {
    XSyncAlarmAttributes attrs;

    XSyncIntsToValue(&attrs.trigger.wait_value, timeout & 0xffffffff,
                     timeout & 0xffffffff00000000);

    if (!XSyncChangeAlarm(display, alarm, XSyncCAValue, &attrs))
      fprintf(stderr, "Warning: failed to set XSync alarm\n");

    XFlush(display);
  }
}
