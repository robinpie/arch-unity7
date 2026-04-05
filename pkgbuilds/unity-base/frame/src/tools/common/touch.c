/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
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

#include "common/touch.h"

#include <stdio.h>

#include <oif/frame_x11.h>

#include "common/device.h"

void print_touch(UFTouch touch, UFFrame frame, UFDevice device, Window window) {
  UFStatus status;
  UFTouchState prev_state;
  int num_axes = 0;
  int prev_valid = 0;
  unsigned long long time;
  float floating;
  int owned;
  int pending_end;
  int j;

  printf("    ID: %u\n", frame_x11_get_touch_id(frame_touch_get_id(touch)));

  switch (frame_touch_get_state(touch)) {
    case UFTouchStateBegin:
      printf("    State: Begin");
      prev_valid = 0;
      break;

    case UFTouchStateUpdate:
      printf("    State: Update");
      prev_valid = 1;
      break;

    case UFTouchStateEnd:
      printf("    State: End");
      prev_valid = 1;
      break;
  }

  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyState,
                                                     &prev_state);
    if (status != UFStatusSuccess)
      fprintf(stderr, "\nError: failed to get previous touch state\n");
    else {
      switch (prev_state) {
        case UFTouchStateBegin:
          printf(" (Prev: Begin)\n");
          break;

        case UFTouchStateUpdate:
          printf(" (Prev: Update)\n");
          break;

        case UFTouchStateEnd:
          printf(" (Prev: End)\n");
          break;
      }
    }
  } else {
    printf("\n");
  }

  printf("    Time: %ju", frame_touch_get_time(touch));
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyTime,
                                                     &time);
    if (status != UFStatusSuccess)
      fprintf(stderr, "\nError: failed to get previous touch time\n");
    else
      printf(" (Prev: %llu)\n", time);
  } else {
    printf("\n");
  }

  printf("    Start Time: %ju", frame_touch_get_start_time(touch));
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyStartTime,
                                                     &time);
    if (status != UFStatusSuccess)
      fprintf(stderr, "\nError: failed to get previous touch start time\n");
    else
      printf(" (Prev: %llu)\n", time);
  } else {
    printf("\n");
  }

  printf("    Window X: %f", frame_touch_get_window_x(touch));
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyWindowX,
                                                     &floating);
    if (status != UFStatusSuccess)
      fprintf(stderr, "\nError: failed to get previous touch window X\n");
    else
      printf(" (Prev: %f)\n", floating);
  } else {
    printf("\n");
  }

  printf("    Window Y: %f", frame_touch_get_window_y(touch));
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyWindowY,
                                                     &floating);
    if (status != UFStatusSuccess)
      fprintf(stderr, "\nError: failed to get previous touch window Y\n");
    else
      printf(" (Prev: %f)\n", floating);
  } else {
    printf("\n");
  }

  status = frame_touch_get_property(touch, UFTouchPropertyOwned, &owned);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get touch ownership property\n");
  else
    printf("    Owned: %s", owned ? "Yes" : "No");
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyOwned,
                                                     &owned);
    if (status != UFStatusSuccess)
      fprintf(stderr,
              "\nError: failed to get previous touch ownership property\n");
    else
      printf(" (Prev: %s)\n", owned ? "Yes" : "No");
  } else {
    printf("\n");
  }

  status = frame_touch_get_property(touch, UFTouchPropertyPendingEnd,
                                    &pending_end);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get touch pending end property\n");
  else
    printf("    Pending End: %s", pending_end ? "Yes" : "No");
  if (prev_valid) {
    status = frame_frame_get_previous_touch_property(frame, touch,
                                                     UFTouchPropertyPendingEnd,
                                                     &pending_end);
    if (status != UFStatusSuccess)
      fprintf(stderr,
              "Error: failed to get previous touch pending end property\n");
    else
      printf(" (Prev: %s)\n", pending_end ? "Yes" : "No");
  } else {
    printf("\n");
  }

  num_axes = frame_device_get_num_axes(device);
  for (j = 0; j < num_axes; ++j) {
    UFAxis axis;
    UFAxisType type;
    const char *name;
    float min;
    float max;
    float res;
    float value;
    float prev_value = 0;

    status = frame_device_get_axis_by_index(device, j, &axis);
    if (status != UFStatusSuccess) {
      fprintf(stderr, "Error: failed to get axis %d\n", j);
      continue;
    }

    get_axis_info(axis, &type, &name, &min, &max, &res);

    status = frame_touch_get_value(touch, type, &value);
    if (status != UFStatusSuccess) {
      fprintf(stderr, "Error: failed to get value for axis %d\n", j);
      continue;
    }

    if (prev_valid) {
      status = frame_frame_get_previous_touch_value(frame, touch, type,
                                                    &prev_value);
      if (status != UFStatusSuccess)
        fprintf(stderr, "Error: failed to get previous value for axis %d\n", j);
    }

    printf("%s", (prev_valid && value != prev_value) ? "  * " : "    ");
    printf("Axis Value: %f (Axis: %s, Min: %f, Max: %f, Res: %f", value, name,
           min, max, res);
    if (prev_valid)
      printf(" Prev: %f", prev_value);
    printf(")\n");
  }
}
