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
 * @file "No Premature Gestures" Test
 *
 * This test plays a four-touches' drag where the four fingers land almost in
 * sync and checks that only 4-touches' slices are generated.
 * Three subscriptions are used:
 *   (A) An atomic two-touches Touch
 *   (B) An atomic three-touches Touch
 *   (C) An atomic four-touches Touch.
 *
 * It's a regression test for https://bugs.launchpad.net/grail/+bug/949916
 *
 * We don't want to see gestures being unnecessarily created (and therefore
 * their slices) for subscriptions (A) and (B) since we already know from the
 * very first events that there are four touches in total, not only two or three.
 *
 * Note that this logic holds only when atomic gesture rules are being used.
 */

#include <set>
#include <gtest/gtest.h>
#include <oif/frame_x11.h>

// evemu wrappers
#include "device.h"
#include "recording.h"

#include "x11/fixture.h"

class NoPrematureGestures : public oif::grail::x11::testing::Test {
 public:
  NoPrematureGestures() : device_(nullptr) {}
 protected:
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();

  // Holds the device we are interested in getting input from.
  // More specifically, the fake one we will create via evemu.
  UFDevice device_;

  std::set<UGSubscription> subscriptions_;

 private:
  void ProcessFrameEventDeviceAdded(UFEvent event);
  void CreateSubscriptions();
  void CreateSubscription(unsigned int num_touches,
                          UGGestureTypeMask gesture_mask);
  void CheckSlice(UGSlice slice);
};

void NoPrematureGestures::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(frame_handle(), &event)) == UFStatusSuccess) {
    grail_process_frame_event(grail_handle(), event);

    if (frame_event_get_type(event) == UFEventTypeDeviceAdded) {
      ProcessFrameEventDeviceAdded(event);
    }

    frame_event_unref(event);
  }

  EXPECT_EQ(UFStatusErrorNoEvent, status);
}

void NoPrematureGestures::ProcessFrameEventDeviceAdded(UFEvent event)
{
  UFStatus status;
  UFDevice device;
  const char* name;

  status = frame_event_get_property(event, UFEventPropertyDevice, &device);
  ASSERT_EQ(UFStatusSuccess, status);

  status = frame_device_get_property(device, UFDevicePropertyName, &name);
  ASSERT_EQ(UFStatusSuccess, status);

  if (strcmp(name, "Apple Magic Trackpad (Virtual Test Device)") == 0) {
    EXPECT_EQ(nullptr, device_);
    device_ = device;
    CreateSubscriptions();
  }
}

void NoPrematureGestures::CreateSubscriptions() {
  CreateSubscription(2, UGGestureTypeTouch);
  CreateSubscription(3, UGGestureTypeTouch);
  CreateSubscription(4, UGGestureTypeTouch);
}

void NoPrematureGestures::CreateSubscription(unsigned int num_touches,
                                        UGGestureTypeMask gesture_mask) {
  UGSubscription subscription;
  UGStatus status;

  status = grail_subscription_new(&subscription);
  ASSERT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyDevice,
                                           &device_);
  ASSERT_EQ(UFStatusSuccess, status);

  UFWindowId window_id =
      frame_x11_create_window_id(DefaultRootWindow(Display()));
  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  ASSERT_EQ(UFStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesStart,
                                           &num_touches);
  ASSERT_EQ(UFStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMaximum,
                                           &num_touches);
  ASSERT_EQ(UFStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &num_touches);
  ASSERT_EQ(UFStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyMask,
                                           &gesture_mask);
  ASSERT_EQ(UFStatusSuccess, status);

  int use_atomic_gestures = 1;
  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyAtomicGestures,
                                           &use_atomic_gestures);
  ASSERT_EQ(UFStatusSuccess, status);

  status = grail_subscription_activate(grail_handle(), subscription);
  ASSERT_EQ(UFStatusSuccess, status);

  subscriptions_.insert(subscription);
}

void NoPrematureGestures::ProcessGrailEvents() {
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

void NoPrematureGestures::CheckSlice(UGSlice slice) {
  UGStatus status;
  unsigned int num_touches;

  status = grail_slice_get_property(slice, UGSlicePropertyNumTouches,
                                    &num_touches);
  ASSERT_EQ(UGStatusSuccess, status);

  // The main point of this test:
  // Check that we don't get any slices from a 2 or 3 touches gesture.
  EXPECT_EQ(4, num_touches)
    << "Got a slice with " << num_touches << " touches from a synced 4-fingers drag.";
}

TEST_F(NoPrematureGestures, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/apple_magic_trackpad/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(device_, nullptr) << "X server failed to initialize trackpad";

  oif::evemu::Recording recording(
      device,
      TEST_ROOT_DIR "recordings/apple_magic_trackpad/synced_4_drag.record");

  recording.Play();

  PumpEvents();

  for (UGSubscription subscription : subscriptions_) {
    grail_subscription_deactivate(grail_handle(), subscription);
    grail_subscription_delete(subscription);
  }
}
