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
 * @file Drag Threshold Test
 *
 * This test plays two different one-touch drags. The first drag is below the
 * threshold required for recognizing a drag gesture, and the second drag is
 * above the threshold.
 */

#include <map>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include "device.h"
#include "recording.h"
#include "x11/fixture.h"
#include "oif/frame_x11.h"

using namespace oif::grail::x11::testing;

class DragThreshold : public Test {
 public:
  DragThreshold() : device_(NULL), expect_drag_(false) {}

 protected:
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();
  void Subscribe();
  void CheckSlice(UGSlice);

  UFDevice device_;
  bool expect_drag_;
  UGSubscription subscription_;
};

void DragThreshold::ProcessFrameEvents() {
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

void DragThreshold::ProcessGrailEvents() {
  UGEvent event;

  UGStatus status;
  while ((status = grail_get_event(grail_handle(), &event)) == UGStatusSuccess) {
    ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(event));

    UGSlice slice;
    status = grail_event_get_property(event, UGEventPropertySlice, &slice);
    ASSERT_EQ(UGStatusSuccess, status);

    CheckSlice(slice);

    grail_event_unref(event);
  }

  EXPECT_EQ(UGStatusErrorNoEvent, status);
}

void DragThreshold::Subscribe() {
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

void DragThreshold::CheckSlice(UGSlice slice) {
  if (grail_slice_get_state(slice) != UGGestureStateEnd)
    return;

  if (expect_drag_)
    EXPECT_TRUE(grail_slice_get_recognized(slice) & UGGestureTypeDrag);
  else
    EXPECT_FALSE(grail_slice_get_recognized(slice) & UGGestureTypeDrag);
}

TEST_F(DragThreshold, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/ntrig_dell_xt2/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(nullptr, device_) << "X server failed to initialize touchscreen";

  expect_drag_ = false;

  oif::evemu::Recording bad_drag(
      device,
      TEST_ROOT_DIR "recordings/ntrig_dell_xt2/drag_below_thresh.record");

  bad_drag.Play();

  PumpEvents();

  expect_drag_ = true;

  oif::evemu::Recording good_drag(
      device,
      TEST_ROOT_DIR "recordings/ntrig_dell_xt2/drag_above_thresh.record");

  good_drag.Play();

  PumpEvents();

  grail_subscription_deactivate(grail_handle(), subscription_);
  grail_subscription_delete(subscription_);
}
