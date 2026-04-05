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

#include "gesture.h"

#include <cstdio>
#include <stdexcept>

#include "oif/grail.h"

#include "event.h"
#include "handle.h"
#include "log.h"
#include "recognizer.h"
#include "slice.h"
#include "touch.h"

namespace oif {
namespace grail {

/**
 * @internal
 * Creates a new gesture
 */
Gesture::Gesture(Recognizer* recognizer, UGSubscription* subscription,
                 TouchMap& touches, uint64_t start_time)
    : recognizer_(recognizer),
      id_(recognizer_->handle()->NewGestureID(recognizer_)),
      subscription_(subscription),
      current_touches_(touches),
      all_touches_(touches),
      start_time_(start_time),
      owned_(false),
      not_owned_(false),
      recognized_(0),
      canceled_(false),
      ended_(false),
      keep_slices_(false) {
}

/**
 * @internal
 * Copies an existing gesture, but with the passed in touch set
 */
Gesture::Gesture(const Gesture* other_gesture, TouchMap& touches)
    : recognizer_(other_gesture->recognizer_),
      id_(recognizer_->handle()->NewGestureID(recognizer_)),
      subscription_(other_gesture->subscription_),
      current_touches_(touches),
      all_touches_(touches),
      start_time_(other_gesture->start_time_),
      owned_(false),
      not_owned_(false),
      recognized_(other_gesture->recognized_),
      canceled_(false),
      ended_(false) {
}

/**
 * @internal
 * Add a new touch to an existing gesture
 */
void Gesture::AddTouch(const SharedTouch& touch) {
  current_touches_[touch->id()] = touch;
  all_touches_[touch->id()] = touch;
}

/**
 * @internal
 * Add a new set of touches to an existing gesture.
 */
void Gesture::AddTouches(TouchMap touches) {
  current_touches_.insert(touches.begin(), touches.end());
  all_touches_.insert(touches.begin(), touches.end());
}

/**
 * @internal
 * Returns whether that gesture contains the given touch.
 */
bool Gesture::ContainsTouch(const SharedTouch& touch) const {
  return current_touches_.find(touch->id()) != current_touches_.end();
}

/**
 * @internal
 * Update a gesture with the passed in frame event and list of modified touches
 */
void Gesture::Update(UFEvent frame_event, TouchSet& modified_touches) {

  if (ended_) {
    /* Now the only thing that can happen is receiving ownership */
    if (!owned_ && !not_owned_) {
      CheckOwned();
      if (IsActive())
        FlushSlices();
    }
    return;
  }

  TouchSet ended_touches;
  bool touch_found = false;
  for (UFTouchId touch_id : modified_touches) {
    const auto& it = all_touches_.find(touch_id);

    /* If the touch is not part of this gesture, skip it */
    if (it != all_touches_.end())
      touch_found = true;
    else
      continue;

    const SharedTouch& touch = it->second;

    if (touch->ended() && !touch->owned()) {
      /* If we receive a touch end before we own it, someone higher in the
       * stack accepted it. We need to cancel any handling of the touch. */
      canceled_ = true;
      break;
    }

    if (touch->pending_end() || touch->ended())
      ended_touches.insert(touch->id());
  }

  /* If the frame doesn't include any info about the touches in the gesture, or
   * the gesture has been canceled, bail. */
  if (!touch_found || canceled_)
    return;

  /* Is this the first slice of the gesture? */
  if (!last_slice_) {
    UGSlice* slice;
    /* Create gesture begin slice */
    slice = new UGSlice(*this, frame_event, current_touches_,
                        recognized_);
    last_slice_ = SharedUGSlice(slice);
    slices_.push(last_slice_);
  } else if (!IsPhysicallyEnded()) {
    UGSlice* slice;
    /* Create gesture update or end slice */
    slice = new UGSlice(last_slice_, *this, frame_event, current_touches_);
    last_slice_ = SharedUGSlice(slice);
    slices_.push(last_slice_);
  }

  for (UFTouchId touch_id : ended_touches)
    current_touches_.erase(touch_id);

  CheckOwned();

  /* Check for any new subscription matches */
  if (recognized_ != subscription_->mask()) {
    recognized_ |= last_slice_->CheckRecognition(*this);
  }

  /* Send slice events to client if gesture is active */
  if (IsActive())
    FlushSlices();

  /* If the gesture ended and nothing was recognized or all the gesture slices
   * have been sent, end the gesture */
  if (IsPhysicallyEnded() &&
      (!recognized_ || IsConstructionFinished()))
    End();

  if (IsPhysicallyEnded() && !recognized_)
    canceled_ = true;
}

bool Gesture::IsPhysicallyEnded() const {
  return (ended_ || (last_slice_ && last_slice_->physically_ended()));
}

/**
 * @internal
 * Check if the gesture is active
 *
 * A gesture is active if a subscribed gesture has been matched and we own all
 * the touches in the gesture.
 */
bool Gesture::IsActive() const {
  return recognized_ && owned_;
}

/**
 * @internal
 * Check if construction is finished for the set of touches in the gesture
 *
 * See UGSlicePropertyConstructionFinished for details.
 */
bool Gesture::IsConstructionFinished() const {
  /* last_slice_ must either be valid or the gesture has ended, in which case
   * construction is considered finished. */
  return (!last_slice_ || last_slice_->construction_finished());
}

/**
 * @internal
 * Perform processing necessary for a gesture that has finished construction
 */
void Gesture::FinishConstruction() {
  if (!IsActive()) {
    LOG(Warn) << "attempted to finish construction of an inactive gesture\n";
    return;
  }

  /* If the gesture is already construction finished, bail */
  if (IsConstructionFinished())
    return;

  /* Create a new gesture slice to tell the client that construction has
   * finished */
  UGSlice* slice = new UGSlice(last_slice_, IsPhysicallyEnded());
  if (!slice)
    return;

  slice->set_construction_finished();

  last_slice_ = SharedUGSlice(slice);
  slices_.push(last_slice_);

  /* If construction has finished, we know we can send events to the client */
  FlushSlices();

  /* If we construction has finished on an ended gesture, we're done with it */
  if (IsPhysicallyEnded()) {
    ended_ = true;
    last_slice_.reset();
    LOG(Dbg) << "gesture " << id_ << " has ended\n";
  }
}

/**
 * @internal
 * Check that all the touches in the gesture are owned
 */
void Gesture::CheckOwned() {
  if (owned_ || not_owned_)
    return;

  for (const auto& pair : all_touches_) {
    const SharedTouch& touch = pair.second;

    if (touch->owned()) {
      continue;
    } else if (touch->ended()) {
      LOG(Warn) << "failed to get ownership property from touch, gesture "
                << id_ << " marked as not owned\n";
      not_owned_ = true;
      return;
    } else {
      return;
    }
  }

  LOG(Dbg) << "all touches owned, marking gesture " << id_ << " as owned\n";
  owned_ = true;
  return;
}

/**
 * @internal
 * Flush all pending gesture slices to the client as grail events */
void Gesture::FlushSlices() {
  if (keep_slices_)
    return;

  while (!slices_.empty()) {
    UGEvent* event = new UGEvent(slices_.front());
    recognizer_->handle()->EnqueueEvent(event);
    slices_.pop();
  }
}

/**
 * @internal
 * Check subscription timeouts
 */
uint64_t Gesture::Timeout() const {
  if (recognized_)
    return 0;

  uint64_t time = 0;
  if ((subscription_->mask() & UGGestureTypeDrag) &&
      subscription_->drag().timeout > time)
    time = subscription_->drag().timeout;
  if ((subscription_->mask() & UGGestureTypePinch) &&
      subscription_->pinch().timeout > time)
    time = subscription_->pinch().timeout;
  if ((subscription_->mask() & UGGestureTypeRotate) &&
      subscription_->rotate().timeout > time)
    time = subscription_->rotate().timeout;
  if ((subscription_->mask() & UGGestureTypeTap) &&
      subscription_->tap().timeout > time)
    time = subscription_->tap().timeout;

  return time;
}

/**
 * @internal
 * Get the current angle about the centroid of the gesture for a given touch */
float Gesture::AngleForTouch(UFTouchId touch_id) const {
  auto it = angles_.find(touch_id);
  if (it != angles_.end())
    return it->second;

  return 0;
}

/**
 * @internal
 * Set the angle about the centroid of the gesture for a given touch */
void Gesture::SetAngleForTouch(UFTouchId touch_id, float angle) {
  angles_[touch_id] = angle;
}

/**
 * @internal
 * Cancel the gesture
 */
void Gesture::Cancel() {
  while (!slices_.empty())
    slices_.pop();
  last_slice_.reset();
  ended_ = true;
}

/**
 * @internal
 * End an active gesture */
void Gesture::End() {
  /* Create a new gesture slice in the end state */
  UGSlice* slice = new UGSlice(last_slice_, true);
  slices_.push(SharedUGSlice(slice));
  FlushSlices();
  last_slice_.reset();
  ended_ = true;
  current_touches_.clear();
  all_touches_.clear();
  LOG(Dbg) << "gesture " << id_ << " has ended\n";
}

/**
 * @internal
 * If true, FlushSlices() will have no effect. The Gesture will keep all its
 * slices until this property is disabled again, which will cause all pending
 * slices to be flushed.
 *
 * By default, this property is false. */
void Gesture::set_keep_slices(bool keep_slices)
{
  if (keep_slices_ && !keep_slices) {
    keep_slices_ = keep_slices;
    FlushSlices();
  } else {
    keep_slices_ = keep_slices;
  }
}

Gesture::~Gesture()
{
  LOG(Dbg) << "deleting gesture " << id_ << "\n";
}

} // namespace grail
} // namespace oif
