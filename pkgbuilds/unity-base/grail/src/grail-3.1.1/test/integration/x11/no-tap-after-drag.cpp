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
 * @file "No Tap after a Drag" Test
 *
 * This test plays a four-touches' drag and checks that no Tap gesture gets
 * recognized. Two subscriptions are used: A 4-touches drag and and a
 * 4-touches tap. Also two test runs are done: one using atomic gestures rules and
 * another using regular gestures rules.
 *
 * It's a regression test for https://bugs.launchpad.net/grail/+bug/944901
 */

#include <set>
#include <gtest/gtest.h>
#include <oif/frame_x11.h>

// evemu wrappers
#include "device.h"
#include "recording.h"

#include "x11/fixture.h"

class NoTapAfterDrag : public oif::grail::x11::testing::Test {
 public:
  NoTapAfterDrag() : device_(nullptr), use_atomic_gestures_(0)  {}
  void SetupEvEmuDeviceAndRun();
 protected:
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();

  // Holds the device we are interested in getting input from.
  // More specifically, the fake one we will create via evemu.
  UFDevice device_;

  // Value to be set on the property UGSubscriptionPropertyAtomicGestures
  // for the subscriptions created
  int use_atomic_gestures_;

  std::set<UGSubscription> subscriptions_;

 private:
  void ProcessFrameEventDeviceAdded(UFEvent event);
  void CreateSubscriptions();
  void CreateSubscription(unsigned int num_touches,
                          UGGestureTypeMask gesture_mask);
  void CheckSlice(UGSlice slice);
};

void NoTapAfterDrag::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(frame_handle(), &event)) == UFStatusSuccess) {
    grail_process_frame_event(grail_handle(), event);

    if (frame_event_get_type(event) == UFEventTypeDeviceAdded) {
      ProcessFrameEventDeviceAdded(event);
    }

    frame_event_unref(event);
  }

  EXPECT_EQ(status, UFStatusErrorNoEvent);
}

void NoTapAfterDrag::ProcessFrameEventDeviceAdded(UFEvent event)
{
  UFStatus status;
  UFDevice device;
  const char* name;

  status = frame_event_get_property(event, UFEventPropertyDevice, &device);
  ASSERT_EQ(status, UFStatusSuccess);

  status = frame_device_get_property(device, UFDevicePropertyName, &name);
  ASSERT_EQ(status, UFStatusSuccess);

  if (strcmp(name, "Apple Magic Trackpad (Virtual Test Device)") == 0) {
    EXPECT_EQ(device_, nullptr);
    device_ = device;
    CreateSubscriptions();
  }
}

void NoTapAfterDrag::CreateSubscriptions() {
  CreateSubscription(4, UGGestureTypeDrag);
  CreateSubscription(4, UGGestureTypeTap);
}

void NoTapAfterDrag::CreateSubscription(unsigned int num_touches,
                                        UGGestureTypeMask gesture_mask) {
  UGSubscription subscription;
  UGStatus status;

  status = grail_subscription_new(&subscription);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyDevice,
                                           &device_);
  ASSERT_EQ(status, UFStatusSuccess);

  UFWindowId window_id =
      frame_x11_create_window_id(DefaultRootWindow(Display()));
  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesStart,
                                           &num_touches);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMaximum,
                                           &num_touches);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &num_touches);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyMask,
                                           &gesture_mask);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyAtomicGestures,
                                           &use_atomic_gestures_);
  ASSERT_EQ(status, UFStatusSuccess);

  status = grail_subscription_activate(grail_handle(), subscription);
  ASSERT_EQ(status, UFStatusSuccess);

  subscriptions_.insert(subscription);
}

void NoTapAfterDrag::ProcessGrailEvents() {
  UGEvent event;

  UGStatus status;
  while ((status = grail_get_event(grail_handle(), &event)) == UGStatusSuccess) {
    ASSERT_EQ(grail_event_get_type(event), UGEventTypeSlice);

    UGSlice slice;
    status = grail_event_get_property(event, UGEventPropertySlice, &slice);
    ASSERT_EQ(status, UGStatusSuccess);

    CheckSlice(slice);

    grail_event_unref(event);
  }

  EXPECT_EQ(status, UGStatusErrorNoEvent);
}

void NoTapAfterDrag::CheckSlice(UGSlice slice) {
  UGGestureTypeMask mask;

  mask = grail_slice_get_recognized(slice);

  // The main point of this test:
  // Check that a tap gesture never gets recognized.
  EXPECT_EQ(mask & UGGestureTypeTap, 0)
    << "A bobus tap gesture has been recognized out of an actual drag gesture!";

  if (!use_atomic_gestures_ &&
      grail_slice_get_state(slice) == UGGestureStateEnd)
    grail_accept_gesture(grail_handle(), grail_slice_get_id(slice));
}

void NoTapAfterDrag::SetupEvEmuDeviceAndRun() {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/apple_magic_trackpad/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(device_, nullptr) << "X server failed to initialize trackpad";

  oif::evemu::Recording recording(
      device,
      TEST_ROOT_DIR "recordings/apple_magic_trackpad/4_drag.record");

  recording.Play();

  PumpEvents();

  for (UGSubscription subscription : subscriptions_) {
    grail_subscription_deactivate(grail_handle(), subscription);
    grail_subscription_delete(subscription);
  }
}

TEST_F(NoTapAfterDrag, UsingAtomicGestures) {

  use_atomic_gestures_ = 1;
  SetupEvEmuDeviceAndRun();
}

TEST_F(NoTapAfterDrag, Regular) {
  use_atomic_gestures_ = 0;
  SetupEvEmuDeviceAndRun();
}
