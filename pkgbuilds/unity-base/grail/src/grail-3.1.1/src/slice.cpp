/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2010-2012 Canonical Ltd.
 *
 * This library is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "config.h"
#include "slice.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include <oif/frame.h>

#include "gesture.h"
#include "log.h"
#include "recognizer.h"
#include "subscription.h"
#include "touch.h"

namespace {

const UGTransform IDENTITY_TRANSFORM = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

} // namespace

namespace oif {
namespace grail {

/**
 * @internal
 * Create a gesture begin slice
 */
UGSlice::UGSlice(Gesture& gesture, UFEvent event,
                 const TouchMap& touches, UGGestureTypeMask recognized)
    : id_(gesture.id()),
      event_(event),
      frame_(NULL),
      touches_(touches),
      time_(frame_event_get_time(event)),
      state_(UGGestureStateBegin),
      physically_ended_(false),
      original_center_x_(0),
      original_center_y_(0),
      original_radius_(0),
      original_angle_(0),
      radius_(0),
      angle_(0),
      center_of_rotation_x_(0),
      center_of_rotation_y_(0),
      recognized_(recognized),
      construction_finished_(false),
      touch_count_changed_(false),
      subscription_(gesture.subscription()) {
  std::memcpy(transform_, IDENTITY_TRANSFORM, sizeof(IDENTITY_TRANSFORM));
  std::memcpy(cumulative_transform_, IDENTITY_TRANSFORM,
              sizeof(IDENTITY_TRANSFORM));

  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame_);
  if (status != UFStatusSuccess)
    throw std::runtime_error("Warning: failed to get frame from event\n");

  /* Hold a reference to the frame event */
  frame_event_ref(event);

  /* Compute initial gesture slice properties */
  GetValues(gesture, touches, &original_center_x_, &original_center_y_,
            &original_radius_, &original_angle_, true);
  radius_ = original_radius_;
  angle_ = original_angle_;
}

/**
 * @internal
 * Copy a gesture slice
 */
UGSlice::UGSlice(const SharedUGSlice& prev, bool end)
    : id_(prev->id_),
      event_(prev->event_),
      frame_(prev->frame_),
      touches_(prev->touches_),
      time_(frame_event_get_time(event_)),
      state_(end ? UGGestureStateEnd : UGGestureStateUpdate),
      physically_ended_(end ? true : prev->physically_ended_),
      original_center_x_(prev->original_center_x_),
      original_center_y_(prev->original_center_y_),
      original_radius_(prev->original_radius_),
      original_angle_(prev->original_angle_),
      radius_(prev->radius_),
      angle_(prev->angle_),
      center_of_rotation_x_(0),
      center_of_rotation_y_(0),
      recognized_(prev->recognized_),
      construction_finished_(prev->construction_finished_),
      touch_count_changed_(false),
      subscription_(prev->subscription_) {
  std::memcpy(transform_, IDENTITY_TRANSFORM, sizeof(IDENTITY_TRANSFORM));
  std::memcpy(cumulative_transform_, prev->cumulative_transform_,
              sizeof(prev->cumulative_transform_));

  UFStatus status = frame_event_get_property(event_, UFEventPropertyFrame,
                                             &frame_);
  if (status != UFStatusSuccess)
    throw std::runtime_error("Warning: failed to copy gesture slice\n");

  /* Hold a reference to the frame event */
  frame_event_ref(event_);
}

/**
 * @internal
 * Create a gesture update or end slice based on new touch frame
 */
UGSlice::UGSlice(const SharedUGSlice& prev, Gesture &gesture,
                 UFEvent event, const TouchMap& touches)
    : id_(prev->id_),
      event_(event ? : NULL),
      frame_(NULL),
      touches_(touches),
      time_(frame_event_get_time(event)),
      state_(UGGestureStateUpdate),
      physically_ended_(prev->physically_ended_),
      original_center_x_(prev->original_center_x_),
      original_center_y_(prev->original_center_y_),
      original_radius_(prev->original_radius_),
      original_angle_(prev->original_angle_),
      radius_(prev->radius_),
      angle_(prev->angle_),
      center_of_rotation_x_(0),
      center_of_rotation_y_(0),
      recognized_(prev->recognized_),
      construction_finished_(prev->construction_finished()),
      touch_count_changed_(touches_.size() != prev->touches_.size()),
      subscription_(prev->subscription_) {
  std::memcpy(transform_, IDENTITY_TRANSFORM, sizeof(IDENTITY_TRANSFORM));

  /* If the number of touches changed, reset transformation */
  if (touch_count_changed_) {
    std::memcpy(cumulative_transform_, IDENTITY_TRANSFORM,
                sizeof(IDENTITY_TRANSFORM));
    original_radius_ = 0;
    original_angle_ = 0;
  } else {
    std::memcpy(cumulative_transform_, prev->cumulative_transform_,
                sizeof(prev->cumulative_transform_));
  }

  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame_);
  if (status != UFStatusSuccess)
    throw std::runtime_error("Warning: failed to get frame from event\n");

  /* Hold a reference to the frame event */
  if (event_)
    frame_event_ref(event_);

  if (touch_count_changed_) {
    /* Compute initial gesture slice properties */
    GetValues(gesture, touches, &original_center_x_, &original_center_y_,
              &original_radius_, &original_angle_, true);
    radius_ = original_radius_;
    angle_ = original_angle_;
    CheckGestureEnd();
  } else {
    /* Compute updated gesture slice properties */
    SetTransforms(gesture);
    SetCenterOfRotation();
    CheckGestureEnd();
  }
}

/**
 * @internal
 * Get gesture slice transformation properties
 */
void UGSlice::GetValues(Gesture &gesture, const TouchMap& touches,
                        float* x, float* y, float* radius, float* angle,
                        bool init) {
  *x = 0;
  *y = 0;

  /* Accumulate X and Y positions */
  for (const auto& pair : touches) {
    const SharedTouch& grail_touch = pair.second;

    UFTouch touch;
    UFStatus status = frame_frame_get_touch_by_id(frame_, grail_touch->id(),
                                                  &touch);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch from frame\n";
      continue;
    }

    if (gesture.recognizer().device_direct()) {
      *x += frame_touch_get_window_x(touch);
      *y += frame_touch_get_window_y(touch);
    } else {
      *x += frame_touch_get_device_x(touch);
      *y += frame_touch_get_device_y(touch);
    }
  }

  /* Calculate centroid of touches */
  *x /= touches.size();
  *y /= touches.size();

  /* Two or more touches are needed for radius and angle calculations */
  if (touches.size() == 1)
    return;

  *radius = 0;
  *angle = 0;
  int num_angles = 0;
  for (const auto& pair : touches) {
    const SharedTouch& grail_touch = pair.second;

    UFTouch touch;
    float cur_x;
    float cur_y;

    UFStatus status = frame_frame_get_touch_by_id(frame_, grail_touch->id(),
                                                  &touch);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch from frame\n";
      continue;
    }

    if (gesture.recognizer().device_direct()) {
      cur_x = frame_touch_get_window_x(touch);
      cur_y = frame_touch_get_window_y(touch);
    } else {
      cur_x = frame_touch_get_device_x(touch);
      cur_y = frame_touch_get_device_y(touch);
    }

    /* Accumulate distance from each point to the centroid */
    *radius += std::sqrt(
        (cur_x - *x) * (cur_x - *x) + (cur_y - *y) * (cur_y - *y));

    /* Calculate the angle around a circle centered at the centroid from a
     * theoretical point at (1, 0) to the current touch */
    float new_angle = std::atan2(cur_y - *y, cur_x - *x);

    if (init) {
      /* If this is a new calculation, accumulate angles */
      *angle += new_angle;
      num_angles++;
    } else if (frame_touch_get_state(touch) != UFTouchStateBegin) {
      /* Update touch angle if the touch has moved */
      float prev_angle = gesture.AngleForTouch(grail_touch->id());

      if (new_angle - prev_angle < -M_PI)
        new_angle += 2 * M_PI;
      else if (new_angle - prev_angle > M_PI)
        new_angle -= 2 * M_PI;

      *angle += new_angle - prev_angle;
      num_angles++;
    }

    /* Save the touch angle in the gesture state */
    gesture.SetAngleForTouch(grail_touch->id(), new_angle);
  }

  /* Calculate the average angle of the touches */
  *radius /= touches.size();
  *angle /= num_angles;
  if (!init)
    *angle += angle_;
}

/**
 * @internal
 * Calculate the new 2 dimensional affine transformation for the slice
 */
void UGSlice::SetTransforms(Gesture &gesture) {
  float center_x;
  float center_y;
  float new_radius = radius_;
  float new_angle = angle_;

  /* Get the transformation values for the updated touches */
  GetValues(gesture, touches_, &center_x, &center_y, &new_radius, &new_angle,
            false);

  if (!touch_count_changed_) {
    /* If the touch count has not changed, calculate new transforms */
    float scale = radius_ ? new_radius / radius_ : 1;

    transform_[0][0] = std::cos(new_angle - angle_) * scale;
    transform_[0][1] = -std::sin(new_angle - angle_) * scale;
    transform_[0][2] =
        center_x - cumulative_transform_[0][2] - original_center_x_;
    transform_[1][0] = -transform_[0][1];
    transform_[1][1] = transform_[0][0];
    transform_[1][2] =
        center_y - cumulative_transform_[1][2] - original_center_y_;

    scale = original_radius_ ? new_radius / original_radius_ : 1;

    cumulative_transform_[0][0] = std::cos(new_angle - original_angle_) * scale;
    cumulative_transform_[0][1] =
        -std::sin(new_angle - original_angle_) * scale;
    cumulative_transform_[0][2] = center_x - original_center_x_;
    cumulative_transform_[1][0] = -cumulative_transform_[0][1];
    cumulative_transform_[1][1] = cumulative_transform_[0][0];
    cumulative_transform_[1][2] = center_y - original_center_y_;
  } else {
    /* If the touch count has changed, the transforms are set appropriately in
     * the slice constructor. Update the transformation state values here. */
    original_radius_ += new_radius - radius_;
    original_angle_ += new_angle - angle_;
    original_center_x_ +=
        center_x - (original_center_x_ + cumulative_transform_[0][2]);
    original_center_y_ +=
        center_y - (original_center_y_ + cumulative_transform_[1][2]);
  }

  /* Save the new radius and angle */
  radius_ = new_radius;
  angle_ = new_angle;
}

/**
 * @internal
 * Determine the center of rotation point.
 *
 * For any given point q that is transformed by a 2D affine transformation
 * matrix T about anchor point P the new point q' may be determined by the
 * following equation:
 *
 * q' = T * (q - P) + P
 *
 * T and P are dependent, so we can modify one and find a new value for the
 * other. We will label the original T and P as T0 and P0, and the new values
 * will be labeled T1 and P1. We can find new values by solving the following
 * equation:
 *
 * q' = T0 * (q - P0) + P0 = T1 * (q - P1) + P1
 *
 * In the calculations below, we use variables for the scalar values
 * that make up T0, P0, T1, and P1:
 *
 * T0: [ a -b c ]  P0: [ x0 ]  T1: [ a -b 0 ]  P1: [ x1 ]
 *     [ b  a d ]      [ y0 ]      [ b  a 0 ]      [ y1 ]
 *     [ 0  0 1 ]      [  0 ]      [ 0  0 1 ]      [  0 ]
 *
 * Note that rotation and scaling are independent of the anchor point, so a and
 * b are equivalent between the transformation matrices.
 *
 * Since we know all the values of T0, P0, and T1, we can calculate the values
 * x1 and y1 in P1.
 */
void UGSlice::SetCenterOfRotation() {
  float a = transform_[0][0];
  float b = transform_[1][0];
  float c = transform_[0][2];
  float d = transform_[0][2];
  float x0 = original_center_x_ + cumulative_transform_[0][2];
  float y0 = original_center_y_ + cumulative_transform_[1][2];
  float x1;
  float y1;

  float div = a*a - 2*a + b*b + 1;

  if (std::fabs(div) < 1e-5)
    return;

  x1 = (a*a*x0 - a*(2*x0+c) + b*b*x0 - b*d + c + x0) / div;
  y1 = (a*a*y0 - a*(2*y0+d) + b*b*y0 + b*c + d + y0) / div;

  center_of_rotation_x_ = x1;
  center_of_rotation_y_ = y1;
}

/**
 * @internal
 * Check if the gesture has ended
 */
void UGSlice::CheckGestureEnd() {
  /* Get number of physically non-ended touches */
  unsigned int num_active_touches = 0;
  for (const auto& pair : touches_) {
    const SharedTouch& touch = pair.second;

    if (!touch->pending_end() && !touch->ended())
      num_active_touches++;
  }

  /* Check if currently active touches is outside range for subscription */
  unsigned int touches_start = subscription_->touches_start();
  unsigned int touches_min = subscription_->touches_min();
  if ((!touches_min && num_active_touches < touches_start) ||
      (touches_min && num_active_touches < touches_min))
    physically_ended_ = true;
}

/**
 * @internal
 * Calculate the square of the cumulative drag of the gesture
 */
float UGSlice::CumulativeDrag2(float device_x_res, float device_y_res) const {

  return std::fabs(cumulative_transform_[0][2] / device_x_res *
                   cumulative_transform_[0][2] / device_x_res +
                   cumulative_transform_[1][2] / device_y_res *
                   cumulative_transform_[1][2] / device_y_res);
}

/**
 * @internal
 * Calculate the cumulative pinch of the gesture.
 */
float UGSlice::CumulativePinch() const {
  float pinch = original_radius_ ? radius_ / original_radius_ : 1;

  return (pinch >= 1 ? pinch : 1 / pinch);
}

/**
 * @internal
 * Check if any subscribed gesture primitives have matched due to this slice
 */
UGGestureTypeMask UGSlice::CheckRecognition(const Gesture& gesture) {
  const UGSubscription &subscription = *subscription_;
  float res_x = gesture.recognizer().device_x_res();
  float res_y = gesture.recognizer().device_y_res();

  /* the cumulative time the gesture has been physically active */
  uint64_t cumulative_time = time_ - gesture.start_time();

  if ((subscription.mask() & UGGestureTypeDrag) &&
      (!subscription.drag().timeout ||
       cumulative_time < subscription.drag().timeout) &&
      CumulativeDrag2(res_x, res_y) > (subscription.drag().threshold *
                                       subscription.drag().threshold))
    recognized_ |= UGGestureTypeDrag;

  if ((subscription.mask() & UGGestureTypePinch) &&
      (!subscription.pinch().timeout ||
       cumulative_time < subscription.pinch().timeout) &&
       CumulativePinch() > subscription.pinch().threshold)
    recognized_ |= UGGestureTypePinch;

  if ((subscription.mask() & UGGestureTypeRotate) &&
      (!subscription.rotate().timeout ||
       cumulative_time < subscription.rotate().timeout) &&
       std::fabs(angle_ - original_angle_) > subscription.rotate().threshold)
    recognized_ |= UGGestureTypeRotate;

  if ((subscription.mask() & UGGestureTypeTap) &&
      cumulative_time < subscription.tap().timeout &&
      CumulativeDrag2(res_x, res_y) < (subscription.tap().threshold *
                                       subscription.tap().threshold) &&
      physically_ended_)
    recognized_ |= UGGestureTypeTap;

  if (subscription.mask() & UGGestureTypeTouch)
    recognized_ |= UGGestureTypeTouch;

  return recognized_;
}

UGStatus UGSlice::GetTouchId(unsigned int index, UFTouchId* touch_id) const {
  if (index >= touches_.size())
    return UGStatusErrorInvalidIndex;

  auto it = touches_.cbegin();
  std::advance(it, index);

  *touch_id = it->first;

  return UGStatusSuccess;
}

UGStatus UGSlice::GetProperty(UGSliceProperty property, void* value) const {
  switch (property) {
    case UGSlicePropertyId:
      *reinterpret_cast<unsigned int*>(value) = id_;
      return UGStatusSuccess;

    case UGSlicePropertyState:
      *reinterpret_cast<UGGestureState*>(value) = state_;
      return UGStatusSuccess;

    case UGSlicePropertySubscription:
      *reinterpret_cast<UGSubscription**>(value) = subscription_;
      return UGStatusSuccess;

    case UGSlicePropertyRecognized:
      *reinterpret_cast<UGGestureTypeMask*>(value) = recognized_;
      return UGStatusSuccess;

    case UGSlicePropertyNumTouches:
      *reinterpret_cast<unsigned int*>(value) = touches_.size();
      return UGStatusSuccess;

    case UGSlicePropertyFrame:
      *reinterpret_cast<UFFrame*>(value) = frame_;
      return UGStatusSuccess;

    case UGSlicePropertyOriginalCenterX:
      *reinterpret_cast<float*>(value) = original_center_x_;
      return UGStatusSuccess;

    case UGSlicePropertyOriginalCenterY:
      *reinterpret_cast<float*>(value) = original_center_y_;
      return UGStatusSuccess;

    case UGSlicePropertyOriginalRadius:
      *reinterpret_cast<float*>(value) = original_radius_;
      return UGStatusSuccess;

    case UGSlicePropertyTransform:
      *reinterpret_cast<UGTransform**>(value) = &transform_;
      return UGStatusSuccess;

    case UGSlicePropertyCumulativeTransform:
      *reinterpret_cast<UGTransform**>(value) = &cumulative_transform_;
      return UGStatusSuccess;

    case UGSlicePropertyCenterOfRotationX:
      *reinterpret_cast<float*>(value) = center_of_rotation_x_;
      return UGStatusSuccess;

    case UGSlicePropertyCenterOfRotationY:
      *reinterpret_cast<float*>(value) = center_of_rotation_y_;
      return UGStatusSuccess;

    case UGSlicePropertyConstructionFinished:
      *reinterpret_cast<int*>(value) = construction_finished_;
      return UGStatusSuccess;
  }

  return UGStatusErrorUnknownProperty;
}

UGSlice::~UGSlice() {
  frame_event_unref(event_);
}

} // namespace grail
} // namespace oif

extern "C" {

UGStatus grail_slice_get_property(const UGSlice slice, UGSliceProperty property,
                                  void* value) {
  return static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(property,
      value);
}

unsigned int grail_slice_get_id(const UGSlice slice) {
  unsigned int id;

  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyId, &id);
  MUST_SUCCEED(status);
  return id;
}

UGGestureState grail_slice_get_state(const UGSlice slice) {
  UGGestureState state;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyState, &state);
  MUST_SUCCEED(status);
  return state;
}

UGGestureTypeMask grail_slice_get_recognized(const UGSlice slice) {
  UGGestureTypeMask mask;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyRecognized, &mask);
  MUST_SUCCEED(status);
  return mask;
}

UGSubscription grail_slice_get_subscription(const UGSlice slice) {
  UGSubscription subscription;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertySubscription, &subscription);
  MUST_SUCCEED(status);
  return subscription;
}

unsigned int grail_slice_get_num_touches(const UGSlice slice) {
  unsigned int num_touches;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyNumTouches, &num_touches);
  MUST_SUCCEED(status);
  return num_touches;
}

UGStatus grail_slice_get_touch_id(const UGSlice slice, unsigned int index,
                                  UFTouchId *touch_id) {
  return static_cast<const oif::grail::UGSlice*>(slice)->GetTouchId(
      index,
      touch_id);
}

float grail_slice_get_original_center_x(const UGSlice slice) {
  float x;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyOriginalCenterX, &x);
  MUST_SUCCEED(status);
  return x;
}

float grail_slice_get_original_center_y(const UGSlice slice) {
  float y;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyOriginalCenterY, &y);
  MUST_SUCCEED(status);
  return y;
}

float grail_slice_get_original_radius(const UGSlice slice) {
  float radius;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyOriginalRadius, &radius);
  MUST_SUCCEED(status);
  return radius;
}

float grail_slice_get_center_of_rotation_x(const UGSlice slice) {
  float x;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyCenterOfRotationX, &x);
  MUST_SUCCEED(status);
  return x;
}

float grail_slice_get_center_of_rotation_y(const UGSlice slice) {
  float y;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyCenterOfRotationY, &y);
  MUST_SUCCEED(status);
  return y;
}

const UGTransform *grail_slice_get_transform(const UGSlice slice) {
  UGTransform *transform;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyTransform, &transform);
  MUST_SUCCEED(status);
  return transform;
}

const UGTransform *grail_slice_get_cumulative_transform(const UGSlice slice) {
  UGTransform *transform;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyCumulativeTransform, &transform);
  MUST_SUCCEED(status);
  return transform;
}

const UFFrame grail_slice_get_frame(const UGSlice slice) {
  UFFrame frame;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyFrame, &frame);
  MUST_SUCCEED(status);
  return frame;
}

int grail_slice_get_construction_finished(const UGSlice slice) {
  int construction_finished;
  UGStatus status =
      static_cast<const oif::grail::UGSlice*>(slice)->GetProperty(
          UGSlicePropertyConstructionFinished,
          &construction_finished);
  MUST_SUCCEED(status);
  return construction_finished;
}

} // extern "C"
