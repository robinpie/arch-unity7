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
 * @file Timeout Test
 *
 * This test plays a recording that begins a touch and ends it two seconds
 * later. The touch does not move. A drag subscription is made with a 300 ms
 * timeout. A fullscreen child window of the root window is created, and touch
 * events are selected on it.
 *
 * When this test is run, a drag gesture should be matched but then rejected by
 * grail because it does not cross the distance threshold within the 300 ms
 * timeout. The touch should be rejected, and the X server should replay the
 * touch to the child window selecting for touch events. The time of the child
 * window touch event should be right after the timeout; however, we play the
 * recording until it finishes, and then process touch events. Since the
 * recording is two seconds long, we wait for up to three seconds from the touch
 * begin to the subwindow receiving the begin event.
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

namespace {

/* Manually specify the drag timeout in case the default changes in the
 * future. */
const uint64_t DRAG_TIMEOUT = 0;

} // namespace

using namespace oif::grail::x11::testing;

class Timeout : public Test {
 public:
  Timeout() : device_(NULL), touch_select_event_seen_(false) {}

 protected:
  virtual void SetUp();
  virtual bool FilterXIEvent(const XGenericEventCookie* xcookie);
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();
  void Subscribe();

  UFDevice device_;
  Window subwin_;
  uint64_t begin_time_;
  bool touch_select_event_seen_;
  UGSubscription subscription_;
};

void Timeout::SetUp() {
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

bool Timeout::FilterXIEvent(const XGenericEventCookie* xcookie) {
  const XIDeviceEvent* device_event =
    reinterpret_cast<const XIDeviceEvent*>(xcookie->data);

  if (xcookie->evtype == XI_TouchBegin) {
    if (device_event->event != subwin_) {
      begin_time_ = device_event->time;
    } else {
      if (!touch_select_event_seen_) {
        EXPECT_GE(device_event->time, begin_time_ + DRAG_TIMEOUT);
        /* Allow 3 s of slack */
        EXPECT_LT(device_event->time, begin_time_ + DRAG_TIMEOUT + 3000);
        touch_select_event_seen_ = true;
      }
      return true;
    }
  } else if (device_event->event == subwin_) {
    return true;
  }

  return false;
}

void Timeout::ProcessFrameEvents() {
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

void Timeout::ProcessGrailEvents() {
  UGEvent event;

  UGStatus status;
  while ((status = grail_get_event(grail_handle(), &event)) == UGStatusSuccess) {
    ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(event));

    UGSlice slice;
    status = grail_event_get_property(event, UGEventPropertySlice, &slice);
    ASSERT_EQ(UGStatusSuccess, status);

    grail_event_unref(event);
  }

  EXPECT_EQ(UGStatusErrorNoEvent, status);
}

void Timeout::Subscribe() {
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

  const UGGestureTypeMask mask = UGGestureTypeDrag;
  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyMask,
                                           &mask);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyDragTimeout,
                                           &DRAG_TIMEOUT);
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

TEST_F(Timeout, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/ntrig_dell_xt2/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(nullptr, device_) << "X server failed to initialize touchscreen";

  oif::evemu::Recording bad_drag(device,
                                    TEST_ROOT_DIR "recordings/ntrig_dell_xt2/timeout.record");

  bad_drag.Play();

  PumpEvents();

  EXPECT_TRUE(touch_select_event_seen_);

  grail_subscription_deactivate(grail_handle(), subscription_);
  grail_subscription_delete(subscription_);
}
