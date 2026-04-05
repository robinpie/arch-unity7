/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published by
 * the Free Software Foundation.
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

#include <X11/extensions/XInput2.h>
#include <oif/frame.h>
#include <oif/frame_x11.h>

#include "common/device.h"
#include "common/frame.h"

static void accept_touches(UFHandle handle, UFEvent event) {
  UFFrame frame;
  UFDevice device;
  UFWindowId window_id;
  unsigned int num_touches;
  UFStatus status;
  int i;

  status = frame_event_get_property(event, UFEventPropertyFrame, &frame);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Warning: failed to get frame from event\n");
    return;
  }

  device = frame_frame_get_device(frame);
  num_touches = frame_frame_get_num_touches(frame);

  window_id = frame_frame_get_window_id(frame);

  for (i = 0; i < num_touches; ++i) {
    UFTouch touch;
    UFTouchId touch_id;

    status = frame_frame_get_touch_by_index(frame, i, &touch);
    if (status != UFStatusSuccess) {
      fprintf(stderr, "Warning: failed to get touch from frame by index\n");
      continue;
    }

    if (frame_touch_get_state(touch) != UFTouchStateBegin)
      continue;

    touch_id = frame_touch_get_id(touch);
    if (frame_x11_accept_touch(device, window_id, touch_id) != UFStatusSuccess)
      fprintf(stderr, "Warning: failed to accept touch\n");
  }
}

static void handle_frame_events(UFHandle handle) {
  UFEvent event;

  while (frame_get_event(handle, &event) == UFStatusSuccess) {
    switch (frame_event_get_type(event)) {
      case UFEventTypeDeviceAdded:
        print_device_added(handle, event);
        break;

      case UFEventTypeDeviceRemoved:
        print_device_removed(handle, event);
        break;

      case UFEventTypeFrame:
        print_frame(handle, event);
        accept_touches(handle, event);
        break;
    }

    frame_event_unref(event);
  }
}

static int quit = 0;
static void sigint_handler(__attribute__((unused)) int signum) {
  quit = 1;
}

int main(int argc, const char* argv[]) {
  Display *display;
  Window root;
  Window win;
  XIEventMask mask;
  XIGrabModifiers mods = { XIAnyModifier, 0 };
  UFStatus status;
  UFHandle frame_handle;
  int xi_major = 2;
  int xi_minor = 2;
  int frame_fd;
  int nfds;

  signal(SIGINT, sigint_handler);

  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "Error: failed to open connection to X server\n");
    return -1;
  }

  if (XIQueryVersion(display, &xi_major, &xi_minor) != Success) {
    fprintf(stderr, "Error: failed to query XInput version\n");
    return -1;
  }

  if (xi_major < 2 || xi_minor < 2) {
    fprintf(stderr, "Error: XInput version of server is too old (%d.%d)\n",
            xi_major, xi_minor);
    return -1;
  }

  root = DefaultRootWindow(display);

  if (argc == 1) {
    win = XCreateSimpleWindow(display, root,
            0, 0, 400, 400, 0,
            BlackPixel(display, 0),
            WhitePixel(display, 0));
  } else if (argc == 2) {
    char *end;
    win = strtoul(argv[1], &end, 0);
    if (*end != '\0') {
      fprintf(stderr, "Invalid window ID\n");
      return -1;
    }
  } else {
    fprintf(stderr, "Usage: %s [window ID]\n", argv[0]);
    return -1;
  }

  XMapWindow(display, win);
  XFlush(display);

  mask.deviceid = XIAllMasterDevices;
  mask.mask_len = XIMaskLen(XI_LASTEVENT);
  mask.mask = calloc(mask.mask_len, sizeof(char));

  XISetMask(mask.mask, XI_TouchBegin);
  XISetMask(mask.mask, XI_TouchUpdate);
  XISetMask(mask.mask, XI_TouchEnd);
  XISetMask(mask.mask, XI_TouchOwnership);
  XISetMask(mask.mask, XI_HierarchyChanged);

  XIGrabTouchBegin(display, XIAllMasterDevices, win, 0, &mask, 1, &mods);

  free(mask.mask);

  status = frame_x11_new(display, &frame_handle);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Error: failed to create frame instance\n");
    return -1;
  }

  frame_fd = frame_get_fd(frame_handle);

  nfds = ConnectionNumber(display) > frame_fd ? ConnectionNumber(display) + 1 :
                                                frame_fd + 1;

  while (!quit) {
    fd_set set;

    FD_ZERO(&set);

    XSync(display, 0);

    if (!XPending(display)) {
      int ret;

      FD_SET(ConnectionNumber(display), &set);
      FD_SET(frame_fd, &set);
      ret = select(nfds, &set, NULL, NULL, NULL);
      if (ret < 1) {
        if (quit)
          break;

        fprintf(stderr, "Warning: select returned with no fds\n");
        continue;
      }
    }

    if (XPending(display) || FD_ISSET(ConnectionNumber(display), &set)) {
      while (XPending(display)) {
        XEvent event;

        XNextEvent(display, &event);

        if (event.type != GenericEvent)
          continue;

        XGenericEventCookie *xcookie = &event.xcookie;
        if (!XGetEventData(display, xcookie)) {
          fprintf(stderr, "Warning: failed to get X generic event data\n");
          continue;
        }

        status = frame_x11_process_event(frame_handle, xcookie);
        if (status != UFStatusSuccess)
          fprintf(stderr, "Warning: failed to inject X event\n");

        XFreeEventData(display, xcookie);
      }
    }

    if (FD_ISSET(frame_fd, &set))
      handle_frame_events(frame_handle);
  }

  frame_x11_delete(frame_handle);
  XDestroyWindow(display, win);
  XCloseDisplay(display);

  return 0;
}
