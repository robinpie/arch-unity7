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

#include "common/frame.h"

#include <stdio.h>

#include <X11/X.h>

#include <oif/frame_x11.h>

#include "common/touch.h"

void print_frame(UFHandle handle, UFEvent event) {
  UFFrame frame;
  UFDevice device;
  UFStatus status;
  Window window;
  char *string = NULL;
  int num_touches = 0;
  int i;

  printf("Frame received:\n");

  printf("  Time: %ju ms\n", frame_event_get_time(event));

  status = frame_event_get_property(event, UFEventPropertyFrame, &frame);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Error: failed to get frame from frame event\n");
    return;
  }

  window = frame_x11_get_window_id(frame_frame_get_window_id(frame));
  printf("  Window: 0x%lx\n", window);

  device = frame_frame_get_device(frame);

  status = frame_device_get_property(device, UFDevicePropertyName, &string);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get device name from device\n");
  else
    printf("  Device: %s\n", string);

  num_touches = frame_frame_get_num_touches(frame);
  printf("  Number of touches: %d\n", num_touches);

  for (i = 0; i < num_touches; ++i) {
    UFTouch touch;

    printf("  Touch %d:\n", i);

    status = frame_frame_get_touch_by_index(frame, i, &touch);
    if (status != UFStatusSuccess) {
      fprintf(stderr, "Error: failed to get touch %d from frame\n", i);
      continue;
    }

    print_touch(touch, frame, device, window);
  }

  printf("\n");
}
