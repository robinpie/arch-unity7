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
#include "recognizer.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>

#include <oif/frame.h>

#include "handle.h"
#include "gesture.h"
#include "log.h"
#include "touch.h"

namespace oif {
namespace grail {

const uint64_t Recognizer::COMPOSITION_TIME = 60;

/**
 * @internal
 * Create a new recognizer for a given device and window
 */
Recognizer::Recognizer(UGHandle* handle, const UFDevice device,
                       UFWindowId window_id)
    : handle_(handle),
      device_(device),
      window_id_(window_id),
      device_direct_(true),
      num_subscriptions_(0) {
  /* Save direct property for gesture processing */
  UFStatus status = frame_device_get_property(device, UFDevicePropertyDirect,
                                              &device_direct_);
  if (status != UFStatusSuccess)
    LOG(Warn) << "failed to get direct property value for device\n";

  /* Save X and Y axis resolutions for gesture processing */
  if (device_direct_) {
    device_x_res_ = frame_device_get_window_resolution_x(device);
    device_y_res_ = frame_device_get_window_resolution_y(device);

    /* If resolution is not available, assume 96 dpi and convert to meters */
    if (device_x_res_ <= 0)
      device_x_res_ = 3780;
    if (device_y_res_ <= 0)
      device_y_res_ = 3780;
  } else {
    device_x_res_ = 46000; /* Default to resolution of Apple Magic Trackpad */
    device_y_res_ = 45000;

    UFAxis axis;
    status = frame_device_get_axis_by_type(device, UFAxisTypeX, &axis);
    if (status != UFStatusSuccess)
      LOG(Warn) << "failed to get X axis from device\n";
    else if (frame_axis_get_resolution(axis) > 0)
      device_x_res_ = frame_axis_get_resolution(axis);

    status = frame_device_get_axis_by_type(device, UFAxisTypeY, &axis);
    if (status != UFStatusSuccess)
      LOG(Warn) << "failed to get Y axis from device\n";
    else if (frame_axis_get_resolution(axis) > 0)
      device_y_res_ = frame_axis_get_resolution(axis);
  }
}

/**
 * @internal
 * Activate a subscription
 */
UGStatus Recognizer::ActivateSubscription(UGSubscription* subscription) {
  /* All the subscriptions must be atomic or non-atomic, mixes break things */
  if (subscription->atomic() != atomic())
      return UGStatusErrorAtomicity;

  /* Save the subscription and update atomicity */
  subscriptions_[subscription->touches_start() - 1].insert(subscription);
  num_subscriptions_++;

  return UGStatusSuccess;
}

/**
 * @internal
 * Deactivate a subscription
 */
void Recognizer::DeactivateSubscription(UGSubscription* subscription) {
  /* Check if subscription is active */
  if (subscriptions_[subscription->touches_start() - 1].find(subscription) ==
      subscriptions_[subscription->touches_start() - 1].cend()) {
    LOG(Warn) << "attempted to deactivate inactive subscription " << subscription
              << "\n";
    return;
  }

  subscriptions_[subscription->touches_start() - 1].erase(subscription);

  /* Cancel any unaccepted gestures */
  for (auto it = unaccepted_gestures_.begin();
       it != unaccepted_gestures_.end();
       ) {
    const SharedGesture& gesture = *it++;
    if (gesture->subscription() == subscription) {
      gesture->Cancel();
      unaccepted_gestures_.erase(gesture);
    }
  }

  /* Cancel any accepted gestures */
  for (auto it = accepted_gestures_.begin(); it != accepted_gestures_.end(); ) {
    const SharedGesture& gesture = *it++;
    if (gesture->subscription() == subscription) {
      gesture->Cancel();
      accepted_gestures_.erase(gesture);
    }
  }

  num_subscriptions_--;
}

namespace {

/**
 * @internal
 * A helper function to get information from a touch frame
 *
 * This function retrieves a set of touches that have been modified.
 */
void GetModifiedTouches(UFFrame frame, TouchSet* modified,
                        uint64_t event_time) {
  unsigned int num_touches = frame_frame_get_num_touches(frame);
  for (unsigned int i = 0; i < num_touches; ++i) {
    UFTouch touch;
    UFStatus status = frame_frame_get_touch_by_index(frame, i, &touch);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch from frame\n";
      continue;
    }

    /* If the touch time is different from the event time, the touch hasn't been
     * modified */
    if (frame_touch_get_time(touch) == event_time) {
      UFTouchId touch_id = frame_touch_get_id(touch);

      modified->insert(touch_id);
    }
  }
}

} // namespace

/**
 * @internal
 * Process a frame event for gesture updates
 */
void Recognizer::ProcessEvent(const UFEvent& event) {
  UFFrame frame;
  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get frame from event\n";
    return;
  }

  TouchSet modified_touches;
  GetModifiedTouches(frame, &modified_touches, frame_event_get_time(event));

  /* For each accepted gesture, update its state */
  for (auto it = accepted_gestures_.begin(); it != accepted_gestures_.end(); ) {
    const SharedGesture& gesture = *it++;
    gesture->Update(event, modified_touches);
    if (gesture->ended()) {
      /* Allow touches of an ended gesture to be part of a new gesture */
      for (const auto& pair : gesture->current_touches()) {
        const SharedTouch& touch = pair.second;

        if (!touch->pending_end() && !touch->ended())
          INSERT_TOUCH(touch, free_touches_);
      }

      LOG(Dbg) << "gesture " << gesture->id()
               << " has been erased from accepted gestures\n";
      accepted_gestures_.erase(gesture);
    }
  }

  /* For each unaccepted gesture, update its state */
  for (auto it = unaccepted_gestures_.begin();
       it != unaccepted_gestures_.end();
       ) {
    const SharedGesture& gesture = *it++;
    gesture->Update(event, modified_touches);
    if (gesture->canceled()) {
      LOG(Dbg) << "rejecting gesture " << gesture->id()
               << " because it has been canceled\n";
      RejectGesture(gesture);
    }
  }
}

/**
 * @internal
 *
 * Mark a gesture as having finished construction if all the touches of the
 * gesture:
 *
 * - Are past the composition time or have ended (i.e. will not be part of any
 *   new gestures)
 * - Are not part of another gesture the client hasn't seen yet (because the
 *   gesture has not crossed the recognition thresholds yet)
 */
void Recognizer::CheckConstructionFinished(uint64_t time) {
  if (atomic())
    return;

  for (const SharedGesture& gesture : unaccepted_gestures_) {
    if (!gesture->IsActive() || gesture->IsConstructionFinished())
      continue;

    for (const auto& pair : gesture->current_touches()) {
      const SharedTouch& touch = pair.second;

      if (time - touch->start_time() < COMPOSITION_TIME)
        goto next_gesture;

      for (const SharedGesture& other_gesture : unaccepted_gestures_) {
        if (other_gesture == gesture)
          continue;

        if (other_gesture->current_touches().find(touch->id()) !=
            other_gesture->current_touches().end() &&
            !other_gesture->IsActive())
          goto next_gesture;
      }
    }

    gesture->FinishConstruction();

next_gesture: ;
  }
}

void Recognizer::RejectOverdueGesturesAndTouches(uint64_t time)
{
  /* Reject gestures that have timed out before being recognized */
  for (auto it = unaccepted_gestures_.begin();
       it != unaccepted_gestures_.end();
       ) {
    const SharedGesture& gesture = *it++;
    uint64_t timeout = gesture->Timeout();
    if (timeout && time - gesture->start_time() > timeout) {
      LOG(Dbg) << "rejecting gesture " << gesture->id()
               << " because it has timed out\n";
      RejectGesture(gesture);
    }
  }

  /* Remove touches older than the gesture composition time from free_touches_
   */
  for (auto it = free_touches_.begin(); it != free_touches_.end(); ) {
    const auto& pair = *it++;
    const SharedTouch& touch = pair.second;

    if (time - touch->start_time() < COMPOSITION_TIME)
      continue;

    LOG(Dbg) << "touch " << touch->id()
             << " has been removed from free_touches_ because it is older than "
                "the gesture composition time (time: " << time
             << ", touch start time: " << touch->start_time() << ")\n";
    ERASE_TOUCH(touch->id(), free_touches_);
  }
}

/**
 * @internal
 * Update the internal time state of the recognizer
 *
 * This function rejects unaccepted gestures that have timed out and rejects
 * touches that have timed out.
 */
void Recognizer::UpdateTime(uint64_t time) {
  LOG(Dbg) << "Updating time to " << time << "\n";

  RejectOverdueGesturesAndTouches(time);
  CheckConstructionFinished(time);
}

/**
 * @internal
 * Determine how long until the client should update the recognizer time state
 */
uint64_t Recognizer::NextTimeout() {
  /* Find the minimum timeout of each unaccepted touch that is not part of a
   * gesture */
  uint64_t min_timeout = std::numeric_limits<uint64_t>::max();
  for (const auto& pair : free_touches_) {
    const SharedTouch& touch = pair.second;

    for (const SharedGesture& gesture : unaccepted_gestures_)
      if (gesture->all_touches().find(touch->id()) !=
          gesture->all_touches().end())
        goto next_touch;

    if (COMPOSITION_TIME + touch->start_time() < min_timeout)
      min_timeout = touch->start_time() + COMPOSITION_TIME;

next_touch: ;
  }

  /* Find the minimum timeout of each unaccepted gesture */
  for (const SharedGesture& gesture : unaccepted_gestures_) {
    uint64_t timeout = gesture->Timeout();
    if (timeout && gesture->start_time() + timeout < min_timeout)
      min_timeout = gesture->start_time() + timeout;
  }

  return min_timeout;
}

/**
 * @internal
 * Accept a gesture, given its id
 */
UGStatus Recognizer::AcceptGesture(unsigned int id) {
  /* Find the gesture by looking up the ID */
  for (const SharedGesture& gesture : unaccepted_gestures_) {
    if (gesture->id() == id) {
      AcceptGesture(gesture);
      return UGStatusSuccess;
    }
  }

  return UGStatusErrorInvalidGesture;
}

/**
 * @internal
 * Accept a gesture
 */
void Recognizer::AcceptGesture(SharedGesture gesture) {

  LOG(Dbg) << "gesture " << gesture->id() << " has been accepted by the "
                                             "client\n";

  for (const auto& pair : gesture->all_touches()) {
    const SharedTouch& touch = pair.second;

    if (!touch->accepted()) {
      touch->Accept();
      LOG(Dbg) << "touch " << touch->id()
               << " has been accepted because it is part of an accepted "
                  "gesture\n";
    } else {
      /* This gesture may be an extension to more touches of an accepted
       * gesture. Cancel the old gesture and remove any gesture events for it
       * from the event queue. Atomic behavior does not allow for gestures to be
       * extended in this way, and gestures may overlap.*/
      if (!atomic()) {
        for (auto it = accepted_gestures_.begin();
             it != accepted_gestures_.end();
             ) {
          const SharedGesture& other_gesture = *it++;
          const TouchMap& other_touches = other_gesture->current_touches();
          if (other_touches.find(touch->id()) != other_touches.end()) {
            LOG(Dbg) << "cancelling gesture " << other_gesture->id()
                     << " because it has a touch (" << touch->id() << ") "
                     << "that is part of a new accepted gesture\n";
            other_gesture->Cancel();
            handle_->RemoveGestureFromEventQueue(other_gesture->id());
            accepted_gestures_.erase(other_gesture);
            LOG(Dbg) << "gesture " << other_gesture->id()
                     << " has been erased from accepted gestures\n";
          }
        }
      }
    }

    /* Remove this touch from the free touches set, it can't be used for new
     * gestures while it is part of an accepted gesture */
    ERASE_TOUCH(touch->id(), free_touches_);
  }

  /* Reject any overlapping unaccepted gestures. Atomic subscriptions may have
   * overlapping gestures due to historical behavior. */
  if (!atomic()) {
    for (auto it = unaccepted_gestures_.begin();
         it != unaccepted_gestures_.end();
         ) {
      const SharedGesture& other_gesture = *it++;
      if (other_gesture == gesture)
        continue;

      for (const auto& pair : other_gesture->all_touches()) {
        const SharedTouch& touch = pair.second;

        if (gesture->all_touches().find(touch->id()) !=
            gesture->all_touches().end()) {
          LOG(Dbg) << "rejecting gesture " << other_gesture->id()
                   << "because it has a touch (" << touch->id()
                   << ") that is part of an accepted gesture\n";
          RejectGesture(other_gesture);
          break;
        }
      }
    }
  }

  /* Check if the gesture has finished construction */
  gesture->FinishConstruction();

  if (!gesture->ended()) {
    accepted_gestures_.insert(gesture);
    LOG(Dbg) << "gesture " << gesture->id()
             << " has been added to accepted gestures\n";
  }

  unaccepted_gestures_.erase(gesture);
  LOG(Dbg) << "gesture " << gesture->id()
           << " has been erased from unaccepted gestures\n";

}

/**
 * @internal
 * Reject a gesture by ID
 */
UGStatus Recognizer::RejectGesture(unsigned int id) {
  for (const SharedGesture& gesture : unaccepted_gestures_) {
    if (gesture->id() == id) {
      RejectGesture(gesture);
      return UGStatusSuccess;
    }
  }

  return UGStatusErrorInvalidGesture;
}

/**
 * @internal
 * Reject a gesture by internal gesture handle
 */
void Recognizer::RejectGesture(SharedGesture gesture) {
  LOG(Dbg) << "cancelling gesture " << gesture->id()
           << " because it has been rejected\n";
  gesture->Cancel();
  handle_->RemoveGestureFromEventQueue(gesture->id());
  unaccepted_gestures_.erase(gesture);
  LOG(Dbg) << "gesture " << gesture->id()
           << " has been erased from unaccepted gestures\n";
}

Recognizer::~Recognizer() {
  for (const SharedGesture& gesture : unaccepted_gestures_)
    gesture->Cancel();

  for (const SharedGesture& gesture : accepted_gestures_)
    gesture->Cancel();
}

} // namespace grail
} // namespace oif
