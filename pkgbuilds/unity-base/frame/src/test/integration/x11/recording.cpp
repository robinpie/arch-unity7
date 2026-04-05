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

#include <map>

#include "events.h"
#include "x11/fixture.h"
#include "oif/frame_x11.h"

using namespace oif::frame::x11::testing;

class FrameRecordingTest : public Test {
 public:
  FrameRecordingTest()
      : device_(NULL),
        step_(0),
        touch_map_(),
        saved_event_(NULL),
        touch_start_times_(),
        touch_times_() {}

  FrameRecordingTest(const FrameRecordingTest&) = delete;
  FrameRecordingTest& operator=(const FrameRecordingTest&) = delete;

 protected:
  static const oif::frame::testing::Events events_;

  virtual void ProcessFrameEvents();
  void CheckFrame(UFFrame frame, uint64_t event_time);
  void CheckTouch(UFFrame frame, unsigned int touch_index, uint64_t event_time);
  void CheckProperty(const oif::frame::testing::Property& property,
                     UFTouch touch, UFFrame frame, int touch_index,
                     bool check_prev);
  void CheckAxisValue(const oif::frame::testing::AxisValue& axis_value,
                      UFTouch touch, UFFrame frame, int touch_index,
                      bool check_prev);

  UFDevice device_;
  unsigned int step_;
  std::map<UFTouchId, unsigned int> touch_map_;
  UFEvent saved_event_; /* Holds a ref'd event to double check at the end */
  std::map<UFTouchId, uint64_t> touch_start_times_;
  std::map<UFTouchId, uint64_t> touch_times_;
};

void FrameRecordingTest::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(handle(), &event)) == UFStatusSuccess) {
    switch (frame_event_get_type(event)) {
      case UFEventTypeFrame: {
        UFFrame frame;
        ASSERT_EQ(UFStatusSuccess,
                  frame_event_get_property(event, UFEventPropertyFrame,
                                           &frame));

        if (step_ == 0) {
          frame_event_ref(event);
          saved_event_ = event;
        }

        CheckFrame(frame, frame_event_get_time(event));
        break;
      }
      case UFEventTypeDeviceAdded: {
        UFDevice device;
        ASSERT_EQ(UFStatusSuccess,
                  frame_event_get_property(event, UFEventPropertyDevice,
                                           &device));

        const char* name;
        ASSERT_EQ(UFStatusSuccess,
                  frame_device_get_property(device, UFDevicePropertyName,
                                            &name));
        if (strcmp(name, "N-Trig-MultiTouch-Virtual-Device") == 0) {
          EXPECT_EQ(NULL, device_);
          device_ = device;
        }
        break;
      }
      default:
        break;
    }

    frame_event_unref(event);
  }

  EXPECT_EQ(UFStatusErrorNoEvent, status);
}

void FrameRecordingTest::CheckFrame(UFFrame frame, uint64_t event_time) {
  ASSERT_LT(step_, events_.size()) << "Received too many frame events";

  /* Ensure we got a device addition event first */
  EXPECT_NE(nullptr, device_);

  EXPECT_EQ(device_, frame_frame_get_device(frame));

  UFWindowId window_id = frame_frame_get_window_id(frame);
  EXPECT_EQ(DefaultRootWindow(Display()), frame_x11_get_window_id(window_id));

  /* The number of active touches should match the number of provided touches
   * for an N-Trig display */
  unsigned int active_touches;
  EXPECT_EQ(UFStatusSuccess,
            frame_frame_get_property(frame, UFFramePropertyActiveTouches,
                                     &active_touches));
  EXPECT_EQ(frame_frame_get_num_touches(frame), active_touches);

  /* Check that the number of touches matches the expected amount for this step
   */
  ASSERT_EQ(events_[step_].size(), active_touches);

  /* Check each touch */
  for (unsigned int touch_index = 0;
       touch_index < events_[step_].size();
       ++touch_index)
    CheckTouch(frame, touch_index, event_time);

  /* Update touch map if a touch ended in the previous frame event. This allows
   * us to properly check the previous property and axis values. */
  if (step_ > 0) {
    for (const auto& pair : touch_map_) {
      const oif::frame::testing::Touch* touch;
      const oif::frame::testing::Properties* properties;
      UFTouchState state;

      /* Check if the size of the touch map is at most the expected size for
       * this step. If so, a touch may have begun. */
      if (events_[step_].size() >= touch_map_.size()) {
        touch = &events_[step_][pair.second];
        properties = &touch->first;

        state = properties->find(UFTouchPropertyState)->second.state;

        /* If touch is new in current step, skip */
        if (state == UFTouchStateBegin)
          continue;
      }

      touch = &events_[step_ - 1][pair.second];
      properties = &touch->first;

      state = properties->find(UFTouchPropertyState)->second.state;

      if (state == UFTouchStateEnd) { /* If touch ended in previous step */
        /* Recalculate slot values for touches that come after this touch */
        for (auto& pair2 : touch_map_)
          if (pair2.second > pair.second)
            pair2.second--;

        /* Remove ended touch from touch map */
        touch_map_.erase(pair.first);

        break; /* In the X11 implementation, only one touch ends at a time */
      }
    }
  }

  /* Check for out of range touch indexes and IDs */
  UFTouch touch;
  EXPECT_EQ(UFStatusErrorInvalidTouch,
            frame_frame_get_touch_by_index(frame, active_touches, &touch));
  EXPECT_EQ(UFStatusErrorInvalidTouch,
            frame_frame_get_touch_by_id(frame, 83492, &touch));

  /* All touches will end after step 18, accept or reject them */
  if (step_ == 18) {
    ASSERT_EQ(4, touch_map_.size());

    UFWindowId window_id =
        frame_x11_create_window_id(DefaultRootWindow(Display()));

    int i = 0;
    for (const auto& pair : touch_map_) {
      if (i++ % 2)
        frame_x11_reject_touch(device_, window_id, pair.first);
      else
        frame_x11_accept_touch(device_, window_id, pair.first);
    }
  }

  ++step_;
}

void FrameRecordingTest::CheckTouch(
    UFFrame frame,
    unsigned int touch_index,
    uint64_t event_time) {
  UFTouch touch;

  ASSERT_EQ(UFStatusSuccess,
            frame_frame_get_touch_by_index(frame, touch_index, &touch));

  UFTouchId id = frame_touch_get_id(touch);
  const oif::frame::testing::Touch& expected = events_[step_][touch_index];

  /* Check each property */
  for (const auto& property : expected.first)
    CheckProperty(property, touch, frame, touch_index, false);

  /* Check each axis value */
  for (const auto& axis_value : expected.second)
    CheckAxisValue(axis_value, touch, frame, touch_index, false);

  if (frame_touch_get_state(touch) == UFTouchStateBegin) {
    /* Update touch map for new touch */
    touch_map_[id] = touch_index;

    /* Ensure that the touch times match the current event time */
    EXPECT_EQ(event_time, frame_touch_get_start_time(touch));
    EXPECT_EQ(event_time, frame_touch_get_time(touch));

    /* Update touch times for this touch */
    touch_start_times_[id] = frame_touch_get_start_time(touch);
    touch_times_[id] = frame_touch_get_time(touch);
  } else {

    /* Check that the touch start time is still correct */
    EXPECT_EQ(touch_start_times_[id], frame_touch_get_start_time(touch))
        << "Step " << step_ << ", Touch " << id;

    int slot = touch_map_[id];
    const oif::frame::testing::Touch& prev_expected =
        events_[step_ - 1][slot];

    if (oif::frame::testing::IsEqual(expected, prev_expected)) {
      EXPECT_EQ(touch_times_[id], frame_touch_get_time(touch))
          << "Step " << step_ << ", Touch " << id;
    } else {
      EXPECT_EQ(event_time, frame_touch_get_time(touch))
          << "Step " << step_ << ", Touch " << id;
      touch_times_[id] = frame_touch_get_time(touch);
    }

    /* Check each property's previous value */
    for (const auto& property : prev_expected.first)
      CheckProperty(property, touch, frame, touch_index, true);

    /* Check each axis' previous value */
    for (const auto& axis_value : prev_expected.second)
      CheckAxisValue(axis_value, touch, frame, touch_index, true);
  }

  UFTouch check_touch;
  EXPECT_EQ(UFStatusSuccess,
            frame_frame_get_touch_by_id(frame, id, &check_touch));
  EXPECT_EQ(touch, check_touch);
}

void FrameRecordingTest::CheckProperty(
    const oif::frame::testing::Property& property,
    UFTouch touch,
    UFFrame frame,
    int touch_index,
    bool check_prev) {
  switch(property.first) {
    case UFTouchPropertyState:
      UFTouchState state;
      if (!check_prev)
        ASSERT_EQ(UFStatusSuccess,
                  frame_touch_get_property(touch, property.first,
                                           &state));
      else
        ASSERT_EQ(UFStatusSuccess,
                  frame_frame_get_previous_touch_property(frame, touch,
                                                          property.first,
                                                          &state));
      EXPECT_EQ(property.second.state, state) << "Step " << step_ << ", Touch "
                                              << touch_index << ", Property " 
                                              << property.first << ", Prev "
                                              << check_prev;
      break;

    case UFTouchPropertyWindowX:
    case UFTouchPropertyWindowY:
      float floating;
      if (!check_prev) {
        if (property.first == UFTouchPropertyWindowX)
          floating = frame_touch_get_window_x(touch);
        else
          floating = frame_touch_get_window_y(touch);
      } else {
        ASSERT_EQ(UFStatusSuccess,
                  frame_frame_get_previous_touch_property(frame, touch,
                                                          property.first,
                                                          &floating));
      }
      EXPECT_FLOAT_EQ(property.second.floating, floating) << "Step " << step_
                                                          << ", Touch "
                                                          << touch_index
                                                          << ", Property "
                                                          << property.first
                                                          << ", Prev "
                                                          << check_prev;
      break;

    case UFTouchPropertyOwned:
    case UFTouchPropertyPendingEnd:
      int boolean;
      if (!check_prev)
        ASSERT_EQ(UFStatusSuccess,
                  frame_touch_get_property(touch, property.first,
                                           &boolean));
      else
        ASSERT_EQ(UFStatusSuccess,
                  frame_frame_get_previous_touch_property(frame, touch,
                                                          property.first,
                                                          &boolean));
      if (property.second.boolean)
        EXPECT_TRUE(boolean) << "Step " << step_ << ", Touch " << touch_index
                             << ", Property " << property.first << ", Prev "
                             << check_prev;
      else
        EXPECT_FALSE(boolean) << "Step " << step_ << ", Touch " << touch_index
                              << ", Property " << property.first << ", Prev "
                              << check_prev;
      break;

    default:
      ADD_FAILURE() << "Unimplemented touch property check for "
                    << property.first;
      break;
  }
}

void FrameRecordingTest::CheckAxisValue(
    const oif::frame::testing::AxisValue& axis_value,
    UFTouch touch,
    UFFrame frame,
    int touch_index,
    bool check_prev) {
  float floating;

  if (!check_prev) {
    switch (axis_value.first) {
      case UFAxisTypeX:
        floating = frame_touch_get_device_x(touch);
        break;
      case UFAxisTypeY:
        floating = frame_touch_get_device_y(touch);
        break;
      default:
        ASSERT_EQ(UFStatusSuccess,
                  frame_touch_get_value(touch, axis_value.first, &floating))
            << "Step " << step_ << ", Touch " << touch_index << ", Axis "
            << axis_value.first;
        break;
    }
  } else {
    ASSERT_EQ(UFStatusSuccess,
              frame_frame_get_previous_touch_value(frame, touch,
                                                   axis_value.first,
                                                   &floating));
  }

  EXPECT_FLOAT_EQ(axis_value.second, floating) << "Step " << step_ << ", Touch "
                                               << touch_index << ", Axis "
                                               << axis_value.first << ", Prev "
                                               << check_prev;
}

TEST_F(FrameRecordingTest, Recording) {
  xorg::testing::evemu::Device device("recordings/ntrig-dell-xt2.prop");

  /* Pump once to ensure the X server has initialized the device */
  PumpEvents();
  ASSERT_NE(nullptr, device_) << "X server failed to initialize touchscreen";

  device.Play("recordings/ntrig-dell-xt2.event");

  PumpEvents();

  EXPECT_EQ(events_.size(), step_) << "Failed to receive all frame events for "
                                      "touchscreen";

  ASSERT_NE(nullptr, saved_event_);

  UFFrame frame;
  ASSERT_EQ(UFStatusSuccess,
            frame_event_get_property(saved_event_, UFEventPropertyFrame,
                                     &frame));

  step_ = 0;
  CheckFrame(frame, frame_event_get_time(saved_event_));

  frame_event_unref(saved_event_);
  saved_event_ = NULL;
}

/* Construct the expected events */
namespace {

using namespace oif::frame::testing;

const Touch ConstructTouch(UFTouchState state, float window_x, float window_y,
                           bool owned, bool pending_end, float x, float y,
                           float touch_major, float touch_minor,
                           float orientation) {
  Properties properties;
  AxisValues values;

  properties[UFTouchPropertyState] = NewValue(state);
  properties[UFTouchPropertyWindowX] = NewValue(window_x);
  properties[UFTouchPropertyWindowY] = NewValue(window_y);
  properties[UFTouchPropertyOwned] = NewValue(owned);
  properties[UFTouchPropertyPendingEnd] = NewValue(pending_end);
  values[UFAxisTypeX] = x;
  values[UFAxisTypeY] = y;
  values[UFAxisTypeTouchMajor] = touch_major;
  values[UFAxisTypeTouchMinor] = touch_minor;
  values[UFAxisTypeOrientation] = orientation;

  return Touch(properties, values);
}

const Events ConstructEvents() {
  Events events;
  Touches touches;

  touches.push_back(ConstructTouch(UFTouchStateBegin, 790.506653, 498.880005,
                                   false, false, 7411, 4677, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateBegin, 785.173340, 351.040009,
                                   false, false, 7361, 3291, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 785.173340, 351.040009,
                                   true, false, 7361, 3291, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 785.173340, 351.040009,
                                   true, false, 7361, 3291, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateBegin, 630.613342, 158.186661,
                                   false, false, 5912, 1483, 540, 462, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 785.173340, 351.040009,
                                   true, false, 7361, 3291, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 462, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 462, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 462, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 385, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 385, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 385, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 1));
  touches.push_back(ConstructTouch(UFTouchStateBegin, 729.280029, 284.693329,
                                   false, false, 6837, 2669, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 1));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 351.040009,
                                   true, false, 7371, 3291, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 349.973328,
                                   true, false, 7371, 3281, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 462, 360, 1));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 349.973328,
                                   true, false, 7371, 3281, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 404, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 436, 257, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 349.973328,
                                   true, false, 7371, 3281, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 380, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 436, 257, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateEnd, 790.506653, 498.880005,
                                   true, false, 7411, 4677, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 786.239990, 349.973328,
                                   true, false, 7371, 3281, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 380, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 436, 257, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateEnd, 786.239990, 349.973328,
                                   true, false, 7371, 3281, 436, 360, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 380, 0));
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 436, 257, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateUpdate, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 380, 0));
  touches.push_back(ConstructTouch(UFTouchStateEnd, 729.280029, 284.693329,
                                   true, false, 6837, 2669, 436, 257, 0));
  events.push_back(touches);

  touches.clear();
  touches.push_back(ConstructTouch(UFTouchStateEnd, 630.613342, 158.186661,
                                   true, false, 5912, 1483, 540, 380, 0));
  events.push_back(touches);

  return events;
}

} // namespace

/* Now initialize the static test fixture member */
const oif::frame::testing::Events
    FrameRecordingTest::events_(ConstructEvents());
