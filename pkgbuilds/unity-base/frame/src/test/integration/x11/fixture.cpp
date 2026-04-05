/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2011 Canonical Ltd.
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

#include "x11/fixture.h"

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "oif/frame.h"
#include "oif/frame_x11.h"

void oif::frame::x11::testing::Test::SetUp() {
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

  free(mask.mask);

  UFStatus status = frame_x11_new(Display(), &handle_);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_TRUE(handle_ != NULL);
}

void oif::frame::x11::testing::Test::PumpEvents(uint64_t timeout) {
  fd_set set;
  FD_ZERO(&set);

  int display_fd = ConnectionNumber(Display());
  int frame_fd = frame_get_fd(handle_);
  int nfds = display_fd > frame_fd ? display_fd + 1: frame_fd + 1;

  XSync(Display(), 0);

  while (true) {
    if (!XPending(Display())) {
      FD_SET(display_fd, &set);
      FD_SET(frame_fd, &set);

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

        if (event.type != GenericEvent)
          continue;

        XGenericEventCookie* xcookie = &event.xcookie;
        ASSERT_NE(0, XGetEventData(Display(), xcookie)) <<
            "Failed to get X generic event data";

        ASSERT_EQ(UFStatusSuccess,
                  frame_x11_process_event(handle_, xcookie)) <<
            "Failed to process X event";

        XFreeEventData(Display(), xcookie);
      }
    }

    if (FD_ISSET(frame_fd, &set)) {
      ProcessFrameEvents();
    }
  }
}

void oif::frame::x11::testing::Test::TearDown() {
  frame_x11_delete(handle_);
  xorg::testing::Test::TearDown();
}
