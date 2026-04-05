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
 * @file Atomic Timout Test
 *
 * This test plays a two-touch tap and checks that the appropriate events are
 * generated for a two-touch atomic tap subscription.
 *
 * Regression test for lp:949855 and lp:957437.
 */

#include <cmath>
#include <map>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include "device.h"
#include "events.h"
#include "recording.h"
#include "x11/fixture.h"
#include "oif/frame_x11.h"

using namespace oif::grail::x11::testing;

class AtomicTap : public Test {
 public:
  AtomicTap() : device_(NULL), step_(0), subscription_(NULL) {}

 protected:
  static oif::grail::testing::Events events_;

  virtual void ProcessFrameEvents();
  virtual void ProcessGrailEvents();
  void Subscribe();
  void CheckSlice(UGSlice);

  UFDevice device_;
  unsigned int step_;
  UGSubscription subscription_;
};

void AtomicTap::ProcessFrameEvents() {
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

void AtomicTap::ProcessGrailEvents() {
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

void AtomicTap::Subscribe() {
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

  const int atomic = true;
  status = grail_subscription_set_property(subscription_,
                                           UGSubscriptionPropertyAtomicGestures,
                                           &atomic);

  status = grail_subscription_activate(grail_handle(), subscription_);
  ASSERT_EQ(UGStatusSuccess, status);
}

void AtomicTap::CheckSlice(UGSlice slice) {
  ASSERT_LT(step_, events_.size()) << "Received too many frame events";

  /* Ensure we got a device addition event first */
  if (step_ == 0)
    EXPECT_NE(nullptr, device_);

  if (!events_[step_].skip) {
    EXPECT_EQ(events_[step_].id, grail_slice_get_id(slice)) << "step " << step_;
    EXPECT_EQ(events_[step_].state, grail_slice_get_state(slice)) << "step "
                                                                  << step_;
    EXPECT_EQ(events_[step_].recognized, grail_slice_get_recognized(slice))
        << "step " << step_;
    EXPECT_EQ(events_[step_].num_touches, grail_slice_get_num_touches(slice))
        << "step " << step_;
  }

  ++step_;
}

TEST_F(AtomicTap, Recording) {
  oif::evemu::Device device(TEST_ROOT_DIR "recordings/ntrig_dell_xt2/device.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(nullptr, device_) << "X server failed to initialize touchscreen";

  oif::evemu::Recording recording(device,
                                     TEST_ROOT_DIR "recordings/ntrig_dell_xt2/2_tap.record");

  recording.Play();

  PumpEvents();

  EXPECT_EQ(events_.size(), step_) << "Failed to receive all frame events for "
                                      "touchscreen";

  grail_subscription_deactivate(grail_handle(), subscription_);
  grail_subscription_delete(subscription_);
}

/* Construct the expected events */
namespace {

using namespace oif::grail::testing;

const Events ConstructEvents() {
  Events events;

  {
    Slice slice = {
      false, /* skip */
      0, /* UGSlicePropertyID */
      UGGestureStateBegin, /* UGSlicePropertyState */
      0, /* UGSlicePropertyRecognized */
      2, /* UGSlicePropertyNumTouches */
      0, /* UGSlicePropertyOriginalCenterX */
      0, /* UGSlicePropertyOriginalCenterY */
      0, /* UGSlicePropertyOriginalRadius */
      {}, /* UGSlicePropertyTransform */
      {}, /* UGSlicePropertyCumulativeTransform */
      0, /* UGSlicePropertyCenterOfRotationX */
      0, /* UGSlicePropertyCenterOfRotationY */
      false, /* UGSlicePropertyConstructionFinished */
    };
    events.push_back(slice);
  }

  {
    Slice slice = {
      false, /* skip */
      0, /* UGSlicePropertyID */
      UGGestureStateUpdate, /* UGSlicePropertyState */
      0, /* UGSlicePropertyRecognized */
      2, /* UGSlicePropertyNumTouches */
      0, /* UGSlicePropertyOriginalCenterX */
      0, /* UGSlicePropertyOriginalCenterY */
      0, /* UGSlicePropertyOriginalRadius */
      {}, /* UGSlicePropertyTransform */
      {}, /* UGSlicePropertyCumulativeTransform */
      0, /* UGSlicePropertyCenterOfRotationX */
      0, /* UGSlicePropertyCenterOfRotationY */
      false, /* UGSlicePropertyConstructionFinished */
    };
    events.push_back(slice);
  }

  {
    Slice slice = {
      false, /* skip */
      0, /* UGSlicePropertyID */
      UGGestureStateUpdate, /* UGSlicePropertyState */
      UGGestureTypeTap, /* UGSlicePropertyRecognized */
      2, /* UGSlicePropertyNumTouches */
      0, /* UGSlicePropertyOriginalCenterX */
      0, /* UGSlicePropertyOriginalCenterY */
      0, /* UGSlicePropertyOriginalRadius */
      {}, /* UGSlicePropertyTransform */
      {}, /* UGSlicePropertyCumulativeTransform */
      0, /* UGSlicePropertyCenterOfRotationX */
      0, /* UGSlicePropertyCenterOfRotationY */
      false, /* UGSlicePropertyConstructionFinished */
    };
    events.push_back(slice);
  }

  {
    Slice slice = {
      false, /* skip */
      0, /* UGSlicePropertyID */
      UGGestureStateEnd, /* UGSlicePropertyState */
      UGGestureTypeTap, /* UGSlicePropertyRecognized */
      2, /* UGSlicePropertyNumTouches */
      0, /* UGSlicePropertyOriginalCenterX */
      0, /* UGSlicePropertyOriginalCenterY */
      0, /* UGSlicePropertyOriginalRadius */
      {}, /* UGSlicePropertyTransform */
      {}, /* UGSlicePropertyCumulativeTransform */
      0, /* UGSlicePropertyCenterOfRotationX */
      0, /* UGSlicePropertyCenterOfRotationY */
      true, /* UGSlicePropertyConstructionFinished */
    };
    events.push_back(slice);
  }

  return events;
}

} // namespace

/* Now initialize the static test fixture member */
oif::grail::testing::Events AtomicTap::events_(ConstructEvents());
