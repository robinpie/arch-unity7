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

/**
 * @internal
 * @file Tap Touch Accept Test
 *
 * This test plays a one touch tap recording, then checks that the tap was
 * recognized and the touch was not erroneously rejected and replayed to a
 * subwindow selecting for touch events.
 *
 * This is a regression test for lp:960438.
 */

#include <map>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include <X11/extensions/XInput2.h>

#include "device.h"
#include "recording.h"
#include "x11/fixture.h"
#include "oif/frame_x11.h"

using namespace oif::grail::x11::testing;

class TapTouchAccept : public Test {
 public:
  TapTouchAccept() : device_(NULL), saw_tap_(false), subscription_(NULL) {}

 protected:
  virtual void SetUp();
  virtual bool FilterXIEvent(const XGenericEventCookie* xcookie);
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();
  void Subscribe();

  UFDevice device_;
  Window subwin_;
  bool saw_tap_;
  UGSubscription subscription_;
};

void TapTouchAccept::SetUp() {
  ASSERT_NO_FATAL_FAILURE(oif::grail::x11::testing::Test::SetUp());

  Window root = DefaultRootWindow(Display());

  unsigned int width;
  unsigned int height;
  {
    Window root_return;
    int x;
    int y;
    unsigned int border_width;
    unsigned int depth;
    ASSERT_NE(0, XGetGeometry(Display(), root, &root_return, &x, &y, &width,
                              &height, &border_width, &depth));
  }

  subwin_ = XCreateSimpleWindow(Display(), root, 0, 0, width, height, 0, 0, 0);
  ASSERT_NE(subwin_, None);

  ASSERT_NE(0, XMapWindow(Display(), subwin_));

  XIEventMask mask;
  mask.mask_len = XIMaskLen(XI_LASTEVENT);
  mask.mask = reinterpret_cast<unsigned char*>(calloc(mask.mask_len,
                                                      sizeof(char)));

  mask.deviceid = XIAllMasterDevices;
  XISetMask(mask.mask, XI_TouchBegin);
  XISetMask(mask.mask, XI_TouchUpdate);
  XISetMask(mask.mask, XI_TouchEnd);

  ASSERT_EQ(Success, XISelectEvents(Display(), subwin_, &mask, 1));

  free(mask.mask);
}

bool TapTouchAccept::FilterXIEvent(const XGenericEventCookie* xcookie) {
  if (xcookie->evtype == XI_TouchBegin) {
    const XIDeviceEvent* device_event =
      reinterpret_cast<const XIDeviceEvent*>(xcookie->data);

    if (device_event->event == subwin_) {
      ADD_FAILURE() << "saw touch selection event";
      return true;
    }
  }

  return false;
}

void TapTouchAccept::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(frame_handle(), &event)) == UFStatusSuccess) {
    grail_process_frame_event(grail_handle(), event);

    if (frame_event_get_type(event) == UFEventTypeDeviceAdded) {
      UFDevice device;
      ASSERT_EQ(UFStatusSuccess,
                frame_event_get_property(event, UFEventPropertyDevice,
                                         &device));

      const char* name;
      ASSERT_EQ(UFStatusSuccess,
                frame_device_get_property(device, UFDevicePropertyName, &name));
      if (strcmp(name, "N-Trig MultiTouch (Virtual Test Device)") == 0) {
        EXPECT_EQ(NULL, device_);
        device_ = device;
        Subscribe();
      }
    }

    frame_event_unref(event);
  }

  EXPECT_EQ(UFStatusErrorNoEvent, status);
}

void TapTouchAccept::ProcessGrailEvents() {
  UGEvent event;

  UGStatus status;
  while ((status = grail_get_event(grail_handle(), &event)) == UGStatusSuccess) {
    ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(event));

    UGSlice slice;
    status = grail_event_get_property(event, UGEventPropertySlice, &slice);
    ASSERT_EQ(UGStatusSuccess, status);

    if (!saw_tap_ && grail_slice_get_recognized(slice) & UGGestureTypeTap) {
      saw_tap_ = true;
      EXPECT_EQ(UGStatusSuccess,
                grail_accept_gesture(grail_handle(),
                                     grail_slice_get_id(slice)));
    }

    grail_event_unref(event);
  }

  EXPECT_EQ(UGStatusErrorNoEvent, status);
}

void TapTouchAccept::Subscribe() {
  UGStatus status = grail_subscription_new(&subscription_);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyDevice,
                                           &device_);
  ASSERT_EQ(UGStatusSuccess, status);

  const UFWindowId window_id =
      frame_x11_create_window_id(DefaultRootWindow(Display()));
  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  ASSERT_EQ(UGStatusSuccess, status);

  const UGGestureTypeMask mask = UGGestureTypeTap;
  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyMask,
                                           &mask);
  ASSERT_EQ(UGStatusSuccess, status);

  const unsigned int touches = 1;
  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyTouchesStart,
                                           &touches);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &touches);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_activate(grail_handle(), subscription_);
  ASSERT_EQ(UGStatusSuccess, status);
}

TEST_F(TapTouchAccept, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/ntrig_dell_xt2/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(nullptr, device_) << "X server failed to initialize touchscreen";

  oif::evemu::Recording recording(device,
                                     TEST_ROOT_DIR "recordings/ntrig_dell_xt2/1_tap.record");

  recording.Play();

  PumpEvents();

  EXPECT_TRUE(saw_tap_);

  grail_subscription_deactivate(grail_handle(), subscription_);
  grail_subscription_delete(subscription_);
}
