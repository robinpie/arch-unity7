/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2012 Canonical Ltd.
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

#include "x11/fixture.h"

#include <X11/extensions/XInput2.h>

#include "oif/frame.h"
#include "oif/frame_x11.h"

void oif::grail::x11::testing::Test::SetUp() {
  ASSERT_NO_FATAL_FAILURE(xorg::testing::Test::SetUp());

  int xi_major = 2;
  int xi_minor = 2;
  ASSERT_EQ(Success, XIQueryVersion(Display(), &xi_major, &xi_minor));
  ASSERT_GE(xi_major, 2);
  ASSERT_GE(xi_minor, 2);

  Window root = DefaultRootWindow(Display());

  XIEventMask mask;
  mask.mask_len = XIMaskLen(XI_LASTEVENT);
  mask.mask = reinterpret_cast<unsigned char*>(calloc(mask.mask_len,
                                                      sizeof(char)));

  mask.deviceid = XIAllDevices;
  XISetMask(mask.mask, XI_HierarchyChanged);
  ASSERT_EQ(Success, XISelectEvents(Display(), root, &mask, 1));
  XIClearMask(mask.mask, XI_HierarchyChanged);

  mask.deviceid = XIAllMasterDevices;
  XISetMask(mask.mask, XI_TouchBegin);
  XISetMask(mask.mask, XI_TouchUpdate);
  XISetMask(mask.mask, XI_TouchEnd);
  XISetMask(mask.mask, XI_TouchOwnership);

  XIGrabModifiers mods = { static_cast<int>(XIAnyModifier), 0 };

  ASSERT_EQ(Success, XIGrabTouchBegin(Display(), XIAllMasterDevices, root, 0,
                                      &mask, 1, &mods));

  ASSERT_EQ(XIGrabSuccess, mods.status);

  free(mask.mask);

  Status sync_status;
  int event_base;
  int error_base;
  sync_status = XSyncQueryExtension(Display(), &event_base, &error_base);
  ASSERT_TRUE(sync_status);

  int major_version;
  int minor_version;
  sync_status = XSyncInitialize(Display(), &major_version, &minor_version);
  ASSERT_TRUE(sync_status);

  XSyncSystemCounter* counters;
  int num_system_counters;
  bool found = false;
  counters = XSyncListSystemCounters(Display(), &num_system_counters);
  for (int i = 0; i < num_system_counters; ++i) {
    if (strcmp(counters[i].name, "SERVERTIME") == 0) {
      server_time_counter_ = counters[i].counter;
      found = true;
      break;
    }
  }
  XSyncFreeSystemCounterList(counters);
  ASSERT_TRUE(found);

  XSyncAlarmAttributes attrs;
  attrs.trigger.counter = server_time_counter_;
  attrs.trigger.value_type = XSyncAbsolute;
  attrs.trigger.test_type = XSyncPositiveComparison;
  attrs.delta = { 0, 0 };
  attrs.events = True;

  alarm_ = XSyncCreateAlarm(Display(),
                            XSyncCACounter | XSyncCAValueType |
                            XSyncCATestType | XSyncCAEvents | XSyncCADelta,
                            &attrs);
  ASSERT_NE(None, alarm_) << "failed to create XSync alarm";

  UFStatus frame_status = frame_x11_new(Display(), &frame_handle_);
  ASSERT_EQ(UFStatusSuccess, frame_status);

  UGStatus grail_status = grail_new(&grail_handle_);
  ASSERT_EQ(UGStatusSuccess, grail_status);
  ASSERT_NE(grail_handle_, nullptr);
}

void oif::grail::x11::testing::Test::PumpEvents(uint64_t timeout) {
  fd_set set;
  FD_ZERO(&set);

  int display_fd = ConnectionNumber(Display());
  int frame_fd = frame_get_fd(frame_handle_);
  int grail_fd = grail_get_fd(grail_handle_);

  int nfds;
  if (display_fd > frame_fd && display_fd > grail_fd)
    nfds = display_fd + 1;
  else if (frame_fd > grail_fd)
    nfds = frame_fd + 1;
  else
    nfds = grail_fd + 1;

  while (true) {
    XSync(Display(), 0);

    if (!XPending(Display())) {
      FD_SET(display_fd, &set);
      FD_SET(frame_fd, &set);
      FD_SET(grail_fd, &set);

      SetX11Timeout();

      struct timeval timeval = {
        static_cast<time_t>(timeout / 1000),
        static_cast<time_t>(timeout % 1000)
      };

      int ret;
      if (timeout)
        ret = select(nfds, &set, NULL, NULL, &timeval);
      else
        ret = select(nfds, &set, NULL, NULL, NULL);

      ASSERT_GE(ret, 0) << "Failed to select on FDs";

      if (ret == 0)
        return;
    }

    if (XPending(Display()) || FD_ISSET(display_fd, &set)) {
      while (XPending(Display())) {
        XEvent event;

        XNextEvent(Display(), &event);

        if (event.type != GenericEvent) {
          /* If it's not an XI 2 event, it's probably a timer event */
          UpdateTime(reinterpret_cast<XSyncAlarmNotifyEvent&>(event));
          continue;
        }

        XGenericEventCookie* xcookie = &event.xcookie;
        ASSERT_NE(0, XGetEventData(Display(), xcookie)) <<
            "Failed to get X generic event data";

        if (FilterXIEvent(xcookie)) {
          XFreeEventData(Display(), xcookie);
          continue;
        }

        ASSERT_EQ(UFStatusSuccess,
                  frame_x11_process_event(frame_handle_, xcookie)) <<
            "Failed to process X event";

        XFreeEventData(Display(), xcookie);
      }
    }

    if (FD_ISSET(frame_fd, &set))
      ProcessFrameEvents();

    if (FD_ISSET(grail_fd, &set))
      ProcessGrailEvents();
  }
}

bool oif::grail::x11::testing::Test::FilterXIEvent(
    const XGenericEventCookie* xcookie) {
  return false;
}

void oif::grail::x11::testing::Test::UpdateTime(
    const XSyncAlarmNotifyEvent& event) {
  /* Process any outstanding frames first */
  ProcessFrameEvents();

  XSyncValue time = event.counter_value;
  uint64_t server_time =
      (uint64_t)XSyncValueHigh32(time) << 32 | XSyncValueLow32(time);
  EXPECT_GT(server_time, 0);

  grail_update_time(grail_handle(), server_time);
}

void oif::grail::x11::testing::Test::SetX11Timeout() {
  uint64_t timeout = grail_next_timeout(grail_handle());
  if (timeout > 0) {
    XSyncAlarmAttributes attrs;

    XSyncIntsToValue(&attrs.trigger.wait_value, timeout & 0xffffffff,
                     timeout & 0xffffffff00000000);

    if (!XSyncChangeAlarm(Display(), alarm_, XSyncCAValue, &attrs))
      fprintf(stderr, "Warning: failed to set XSync alarm\n");

    XFlush(Display());
  }
}

void oif::grail::x11::testing::Test::TearDown() {
  XSyncDestroyAlarm(Display(), alarm_);
  grail_delete(grail_handle_);
  frame_x11_delete(frame_handle_);
  xorg::testing::Test::TearDown();
}
