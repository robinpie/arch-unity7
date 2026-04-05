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
 * @file "Parallel Atomic Gestures" Test
 *
 * This test plays a 3 touch points drag followed by the appearance of a 4th
 * touch point and then all 4 touch points are lifted at once.
 *
 * There are three subscriptions:
 *   (A) an atomic 3-touches Drag
 *   (B) an atomic 3-touches Touch
 *   (C) an atomic 3-touches Pinch
 *
 * What must happen is the following:
 * In the beginning, gesture (B) will be accepted. Then after a short while
 * gesture (A) will also be accepted. Gesture (C) will time out and be
 * rejected. Thus there should be no slice coming from (C) at all.
 * The appearance of the fourth touch point will cause the rejection of
 * the accepted gestures (A) and (B).
 */

#include <future>
#include <gtest/gtest.h>
#include <oif/frame_x11.h>

/* evemu wrappers */
#include "device.h"
#include "recording.h"

#include "slice-checker.h"

#include "x11/fixture.h"

using namespace oif::grail::testing;

#define DELETE_SUBSCRIPTION(sub) \
    grail_subscription_deactivate(grail_handle(), sub); \
    grail_subscription_delete(sub); \
    sub = nullptr;

class ParallelAtomicGestures : public oif::grail::x11::testing::Test {
 public:
  ParallelAtomicGestures();
 protected:
  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();

  /* Holds the device we are interested in getting input from.
     More specifically, the fake one we will create via evemu. */
  UFDevice device_;

  SliceChecker slice_checker_;

  void DeleteSubscriptions();
 private:
  void ProcessFrameEventDeviceAdded(UFEvent event);
  void CreateSubscriptions();
  void ConstructExpectedSlices();
  UGSubscription CreateSubscription(unsigned int num_touches,
                          UGGestureTypeMask gesture_mask);

  UGSubscription drag_sub_;
  UGSubscription touch_sub_;
  UGSubscription pinch_sub_;

  bool no_slices_checked_;
};

ParallelAtomicGestures::ParallelAtomicGestures()
    : device_(nullptr),
      drag_sub_(nullptr),
      touch_sub_(nullptr),
      pinch_sub_(nullptr),
      no_slices_checked_(true) {
}

void ParallelAtomicGestures::ProcessFrameEvents() {
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

void ParallelAtomicGestures::ProcessFrameEventDeviceAdded(UFEvent event)
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
    ConstructExpectedSlices();
  }
}

void ParallelAtomicGestures::CreateSubscriptions() {
  drag_sub_ = CreateSubscription(3, UGGestureTypeDrag);
  ASSERT_NE(drag_sub_, nullptr);

  touch_sub_ = CreateSubscription(3, UGGestureTypeTouch);
  ASSERT_NE(touch_sub_, nullptr);

  pinch_sub_ = CreateSubscription(3, UGGestureTypePinch);
  ASSERT_NE(pinch_sub_, nullptr);
}

void ParallelAtomicGestures::DeleteSubscriptions() {
  DELETE_SUBSCRIPTION(drag_sub_);
  DELETE_SUBSCRIPTION(touch_sub_);
  DELETE_SUBSCRIPTION(pinch_sub_);
}

UGSubscription ParallelAtomicGestures::CreateSubscription(
    unsigned int num_touches,
    UGGestureTypeMask gesture_mask) {
  UGSubscription subscription;
  UGStatus status;

  status = grail_subscription_new(&subscription);
  EXPECT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyDevice,
                                           &device_);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  UFWindowId window_id =
      frame_x11_create_window_id(DefaultRootWindow(Display()));
  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  EXPECT_EQ(UGStatusSuccess, status);

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesStart,
                                           &num_touches);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMaximum,
                                           &num_touches);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &num_touches);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyMask,
                                           &gesture_mask);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  int use_atomic_gestures = 1;
  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyAtomicGestures,
                                           &use_atomic_gestures);
  EXPECT_EQ(UFStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  status = grail_subscription_activate(grail_handle(), subscription);
  EXPECT_EQ(UGStatusSuccess, status);
  if (status != UGStatusSuccess) return 0;

  return subscription;
}

void ParallelAtomicGestures::ProcessGrailEvents() {
  UGEvent event;

  UGStatus status;
  while ((status = grail_get_event(grail_handle(), &event)) == UGStatusSuccess) {
    ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(event));

    UGSlice slice;
    status = grail_event_get_property(event, UGEventPropertySlice, &slice);
    ASSERT_EQ(UGStatusSuccess, status);

    /* Ensure we got a device addition event first */
    if (no_slices_checked_) {
      no_slices_checked_ = false;
      EXPECT_NE(nullptr, device_);
    }
    slice_checker_.CheckSlice(slice);

    grail_event_unref(event);
  }

  EXPECT_EQ(UGStatusErrorNoEvent, status);
}

void ParallelAtomicGestures::ConstructExpectedSlices()
{
  {
    ExpectSlice *state = new ExpectSlice;
    state->expected_slice = {
      UGGestureStateBegin, /* UGSlicePropertyState */
      UGGestureTypeTouch, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      false, /* UGSlicePropertyConstructionFinished */
      touch_sub_ /* grail_slice_get_subscription */
    };
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectSlices *state = new ExpectSlices;
    state->expected_slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeTouch, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      false, /* UGSlicePropertyConstructionFinished */
      touch_sub_ /* grail_slice_get_subscription */
    };
    state->SetAverageCount(10);
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectSlices *state = new ExpectSlices;
    state->expected_slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeTouch, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      true, /* UGSlicePropertyConstructionFinished */
      touch_sub_ /* grail_slice_get_subscription */
    };
    state->SetAverageCount(23);
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectSlice *state = new ExpectSlice;
    state->expected_slice = {
      UGGestureStateBegin, /* UGSlicePropertyState */
      0, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      false, /* UGSlicePropertyConstructionFinished */
      drag_sub_ /* grail_slice_get_subscription */
    };
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectSlices *state = new ExpectSlices;
    state->expected_slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      0, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      false, /* UGSlicePropertyConstructionFinished */
      drag_sub_ /* grail_slice_get_subscription */
    };
    state->SetAverageCount(31);
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectSlices *state = new ExpectSlices;
    state->expected_slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeDrag, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      false, /* UGSlicePropertyConstructionFinished */
      drag_sub_ /* grail_slice_get_subscription */
    };
    state->min_count = 0; /* may or may not happen */
    state->max_count = 1;
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectParallelSlices *state = new ExpectParallelSlices;
    ExpectedSlice slice;

    slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeDrag, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      true, /* UGSlicePropertyConstructionFinished */
      drag_sub_ /* grail_slice_get_subscription */
    };
    state->expected_slices.push_back(slice);

    slice = {
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeTouch, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      true, /* UGSlicePropertyConstructionFinished */
      touch_sub_ /* grail_slice_get_subscription */
    };
    state->expected_slices.push_back(slice);

    state->SetAverageCount(67);
    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }

  {
    ExpectParallelSlices *state = new ExpectParallelSlices;
    ExpectedSlice slice;

    slice = {
      UGGestureStateEnd, /* UGSlicePropertyState */
      UGGestureTypeDrag, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      true, /* UGSlicePropertyConstructionFinished */
      drag_sub_ /* grail_slice_get_subscription */
    };
    state->expected_slices.push_back(slice);

    slice = {
      UGGestureStateEnd, /* UGSlicePropertyState */
      UGGestureTypeTouch, /* UGSlicePropertyRecognized */
      3, /* UGSlicePropertyNumTouches */
      true, /* UGSlicePropertyConstructionFinished */
      touch_sub_ /* grail_slice_get_subscription */
    };
    state->expected_slices.push_back(slice);

    state->min_count = state->max_count = 1;

    slice_checker_.AppendState(std::unique_ptr<SliceCheckerState>(state));
  }
}

TEST_F(ParallelAtomicGestures, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/apple_magic_trackpad/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(device_, nullptr) << "X server failed to initialize trackpad";

  oif::evemu::Recording recording(
      device,
      TEST_ROOT_DIR "recordings/apple_magic_trackpad/3_drag_ended_by_4th_touch.record");

  recording.Play();

  PumpEvents();

  DeleteSubscriptions();

  slice_checker_.CheckAllExpectedSlicesReceived();
}
