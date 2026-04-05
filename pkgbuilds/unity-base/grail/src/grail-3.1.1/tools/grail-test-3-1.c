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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/extensions/XInput2.h>
#include <oif/frame.h>
#include <oif/frame_x11.h>
#include <oif/grail.h>

#include "common/device.h"
#include "common/servertime.h"
#include "common/slice.h"

static const int kNumSubscriptions = 10;

static int subscribe(UGHandle handle, UFDevice device, Window window,
                     UGSubscription *subscriptions) {
  UGStatus status;
  UFWindowId window_id = frame_x11_create_window_id(window);
  const UGGestureTypeMask mask = UGGestureTypeDrag |
                                 UGGestureTypePinch |
                                 UGGestureTypeRotate |
                                 UGGestureTypeTap;
  unsigned int touches;
  int i;

  for (i = 0; i < kNumSubscriptions; ++i)
    if (subscriptions[i] == NULL)
      break;

  if (i == kNumSubscriptions) {
    fprintf(stderr, "Warning: maximum number of devices reached\n");
    return 0;
  }

  status = grail_subscription_new(&subscriptions[i]);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to create subscription\n");
    return 0;
  }

  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyDevice,
                                           &device);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription device\n");
    return 0;
  }

  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription window\n");
    return 0;
  }

  touches = 3;
  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyTouchesStart,
                                           &touches);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription start touches\n");
    return 0;
  }

  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyTouchesMaximum,
                                           &touches);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription start touches\n");
    return 0;
  }

  touches = 1;
  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &touches);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription min touches\n");
    return 0;
  }

  status = grail_subscription_set_property(subscriptions[i],
                                           UGSubscriptionPropertyMask, &mask);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to set subscription mask\n");
    return 0;
  }

  status = grail_subscription_activate(handle, subscriptions[i]);
  if (status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to activate subscription\n");
    return 0;
  }

  return 1;
}

static void process_frame_events(UGHandle grail_handle, UFHandle frame_handle,
                                 Window subscribe_window,
                                 UGSubscription* subscriptions) {
  UFEvent event;

  while (frame_get_event(frame_handle, &event) == UFStatusSuccess) {
    grail_process_frame_event(grail_handle, event);

    switch (frame_event_get_type(event)) {
      case UFEventTypeDeviceAdded: {
        UFDevice device;
        UFStatus status;

        print_device_added(frame_handle, event);
        status = frame_event_get_property(event, UFEventPropertyDevice,
                                          &device);
        if (status != UFStatusSuccess)
          fprintf(stderr, "Error: failed to get device from event\n");
        else
          subscribe(grail_handle, device, subscribe_window, subscriptions);
        break;
      }

      case UFEventTypeDeviceRemoved:
        print_device_removed(frame_handle, event);
        break;

      default:
        break;
    }

    frame_event_unref(event);
  }
}

static void process_slice(UGHandle handle, UGSlice slice, uint64_t time) {
  if (grail_slice_get_state(slice) == UGGestureStateBegin) {
    unsigned int id = grail_slice_get_id(slice);

    printf("Accepting gesture %u\n", id);
    grail_accept_gesture(handle, id);
  }

  print_slice(handle, slice, time);
}

static void process_grail_events(UGHandle handle) {
  UGEvent event;

  while (grail_get_event(handle, &event) == UGStatusSuccess) {
    switch (grail_event_get_type(event)) {
      case UGEventTypeSlice: {
        UGSlice slice;
        UGStatus status;

        status = grail_event_get_property(event, UGEventPropertySlice, &slice);
        if (status != UGStatusSuccess) {
          fprintf(stderr, "Error: failed to get slice from event\n");
          break;
        }
        process_slice(handle, slice, grail_event_get_time(event));
        break;
      }

      default:
        break;
    }

    grail_event_unref(event);
  }
}

int quit = 0;
static void sigint_handler(__attribute__((unused)) int signum) {
  quit = 1;
}

int main(int argc, const char* argv[]) {
  Display *display;
  Window win;
  XIEventMask mask;
  XIGrabModifiers mods = { XIAnyModifier, 0 };
  XSyncAlarm alarm;
  UFStatus frame_status;
  UFHandle frame_handle;
  UGStatus grail_status;
  UGHandle grail_handle;
  UGSubscription subscriptions[kNumSubscriptions];
  int xi_major = 2;
  int xi_minor = 2;
  int frame_fd;
  int grail_fd;
  int nfds;
  int i;
  char *end;

  memset(subscriptions, 0, sizeof(UGSubscription) * kNumSubscriptions);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <Window ID>\n", argv[0]);
    return -1;
  }

  win = strtoul(argv[1], &end, 0);
  if (*end != '\0') {
    fprintf(stderr, "Invalid window ID\n");
    return -1;
  }

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

  nfds = ConnectionNumber(display) + 1;

  alarm = create_alarm(display);

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

  if (mods.status != XIGrabSuccess) {
    fprintf(stderr, "Error: failed to grab touches on window\n");
    return -1;
  }

  frame_status = frame_x11_new(display, &frame_handle);
  if (frame_status != UFStatusSuccess) {
    fprintf(stderr, "Error: failed to create frame instance\n");
    return -1;
  }

  frame_fd = frame_get_fd(frame_handle);

  nfds = frame_fd + 1 > nfds ? frame_fd + 1 : nfds;

  grail_status = grail_new(&grail_handle);
  if (grail_status != UGStatusSuccess) {
    fprintf(stderr, "Error: failed to create grail instance\n");
    frame_x11_delete(frame_handle);
    return -1;
  }

  grail_fd = grail_get_fd(grail_handle);

  nfds = grail_fd + 1 > nfds ? grail_fd + 1 : nfds;

  while (!quit) {
    fd_set set;

    FD_ZERO(&set);

    XSync(display, 0);

    if (!XPending(display)) {
      int ret;

      FD_SET(ConnectionNumber(display), &set);
      FD_SET(frame_fd, &set);
      FD_SET(grail_fd, &set);

      set_timeout(grail_handle, display, alarm);

      ret = select(nfds, &set, NULL, NULL, NULL);

      if (ret < 0) {
        if (quit)
          break;

        perror("Warning: select returned an error");
        continue;
      }
    }

    if (XPending(display) || FD_ISSET(ConnectionNumber(display), &set)) {
      while (XPending(display)) {
        XEvent event;

        XNextEvent(display, &event);

        if (event.type != GenericEvent) {
          /* If it's not an XI 2 event, it's a timer event */
          if (alarm != None) {
            /* Process any outstanding frame events first */
            process_frame_events(grail_handle, frame_handle, win,
                                 subscriptions);
            update_time(grail_handle, (XSyncAlarmNotifyEvent *)&event);
          }
          continue;
        }

        XGenericEventCookie *xcookie = &event.xcookie;
        if (!XGetEventData(display, xcookie)) {
          fprintf(stderr, "Warning: failed to get X generic event data\n");
          continue;
        }

        frame_status = frame_x11_process_event(frame_handle, xcookie);
        if (frame_status != UFStatusSuccess)
          fprintf(stderr, "Warning: failed to inject X event\n");

        XFreeEventData(display, xcookie);
      }
    }

    if (FD_ISSET(frame_fd, &set))
      process_frame_events(grail_handle, frame_handle, win, subscriptions);

    if (FD_ISSET(grail_fd, &set))
      process_grail_events(grail_handle);
  }

  for (i = 0; i < kNumSubscriptions; ++i) {
    if (subscriptions[i]) {
      grail_subscription_deactivate(grail_handle, subscriptions[i]);
      grail_subscription_delete(subscriptions[i]);
    }
  }

  destroy_alarm(display, alarm);
  grail_delete(grail_handle);
  frame_x11_delete(frame_handle);
  XCloseDisplay(display);

  return 0;
}
