/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2012 Canonical Ltd.
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

#include "atomic-recognizer.h"

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

namespace {
const uint64_t MAX_TOUCHES_FOR_GESTURES = 5;
} // namespace

namespace oif {
namespace grail {

/**
 * @internal
 * Create a new atomic recognizer for a given device and window
 */
AtomicRecognizer::AtomicRecognizer(UGHandle* handle, const UFDevice device, UFWindowId window)
    : Recognizer(handle, device, window) {
}

/**
 * @internal
 * Process a frame event
 */
void AtomicRecognizer::ProcessFrameEvent(const UFEvent event) {
  LOG(Dbg) << "new event " << event << " with time "
           << frame_event_get_time(event) << "\n";

  uint64_t event_time = frame_event_get_time(event);

  RejectOverdueGesturesAndTouches(event_time);
  ProcessTouches(event);
  if (new_touches_.size() > 0) {
    /* process all new touches at once to avoid the premature initiation of
       gestures for less touches than what the event brings */
    MatchSubscriptionsForNewTouches();
  }
  ProcessEvent(event);
  CheckConstructionFinished(event_time);
  FindGesturesToAccept(event_time);
}

void AtomicRecognizer::UpdateTime(uint64_t time) {
  Recognizer::UpdateTime(time);
  FindGesturesToAccept(time);
}

/**
 * @internal
 * Perform tasks necessary for when new touches occur and there is an existing
 * accepted gesture
 *
 * If a gesture may add touches without crossing the maximum for the
 * subscription, add the touches to the gesture and accept them. Otherwise, end
 * the gesture and add the current gesture touches to the free touches list.
 */
void AtomicRecognizer::HandleNewTouchesForAcceptedGesture(
    const SharedGesture& gesture) {
  UGSubscription* subscription = gesture->subscription();
  if (gesture->current_touches().size() + new_touches_.size() <=
      subscription->touches_max()) {
    gesture->AddTouches(new_touches_);
    LOG(Dbg) << "new_touches_ have been added to atomic gesture "
             << gesture->id() << "\n";

    for (const auto& pair : new_touches_) {
      const SharedTouch& touch = pair.second;
      touch->Accept();
      LOG(Dbg) << "touch " << touch->id()
               << " has been accepted because it has been added to an atomic "
                  "gesture\n";

      ERASE_TOUCH(touch->id(), free_touches_);
    }
    CLEAR_TOUCHES(new_touches_);
  } else {
    for (const auto& pair : gesture->current_touches()) {
      const SharedTouch& touch = pair.second;
      INSERT_TOUCH(touch, free_touches_);
    }
    gesture->End();
    LOG(Dbg) << "ended active atomic gesture " << gesture->id()
             << " because " << new_touches_.size()
             << " new touch(es) began and the max touches has been reached\n";
    accepted_gestures_.erase(gesture);
  }
}

/**
 * @internal
 * Perform tasks necessary for when new touches occur and there is an existing
 * unaccepted gesture
 *
 * If a gesture may receive the new touches without crossing the maximum for
 * the subscription, add the touches to the gesture. Otherwise, cancel the
 * gesture and add the current gesture touches to the free touches list.
 */
void AtomicRecognizer::HandleNewTouchesForUnacceptedGesture(
    const SharedGesture& gesture) {
  UGSubscription* subscription = gesture->subscription();
  if (gesture->current_touches().size() + new_touches_.size() <=
      subscription->touches_max()) {
    for (const auto& pair : new_touches_) {
      const SharedTouch& touch = pair.second;
      gesture->AddTouch(touch);
      LOG(Dbg) << "touch " << touch->id() << " has been added to atomic "
               << "gesture " << gesture->id() << "\n";
    }
  } else {
    for (const auto& pair : gesture->current_touches()) {
      const SharedTouch& touch = pair.second;
      INSERT_TOUCH(touch, free_touches_);
    }
    gesture->Cancel();
    LOG(Dbg) << "canceled inactive atomic gesture " << gesture->id()
             << " because a new touch began and the max touches has been "
             << "reached\n";
    unaccepted_gestures_.erase(gesture);
  }
}

/**
 * @internal
 * Process all touches present in the given frame event.
 *
 * Add new touches to new_touches_ and free_touches_ maps, update the grail
 * touch state for existing touches, and remove touches from maps when they
 * physically end.
 */
void AtomicRecognizer::ProcessTouches(const UFEvent event) {
  UFFrame frame;
  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get frame from event\n";
    return;
  }

  unsigned int num_touches = frame_frame_get_num_touches(frame);
  for (unsigned int i = 0; i < num_touches; ++i) {
    UFTouch touch;
    status = frame_frame_get_touch_by_index(frame, i, &touch);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch from frame\n";
      continue;
    }

    switch (frame_touch_get_state(touch)) {
      case UFTouchStateBegin: {
        SharedTouch grail_touch(new Touch(touch, device_, window_id_));

        LOG(Dbg) << "touch " << grail_touch->id() << " began with start time "
                 << grail_touch->start_time() << "\n";

        INSERT_TOUCH(grail_touch, all_touches_);
        INSERT_TOUCH(grail_touch, new_touches_);
        break;
      }

      case UFTouchStateUpdate:
      case UFTouchStateEnd: {
        auto it = all_touches_.find(frame_touch_get_id(touch));
        if (it != all_touches_.end()) {
          if (it->second.expired()) {
            ERASE_TOUCH(it->first, all_touches_);
            break;
          }

          const SharedTouch& grail_touch = it->second.lock();

          grail_touch->Update(touch);

          if (grail_touch->ended())
            ERASE_TOUCH(grail_touch->id(), all_touches_);

          if (grail_touch->pending_end() || grail_touch->ended()) {
            ERASE_TOUCH(grail_touch->id(), new_touches_);
            ERASE_TOUCH(grail_touch->id(), free_touches_);
          }
        }
        break;
      }
    }
  }
}

/**
 * @internal
 * Consume the new touches.
 * Check if any new atomic gestures should begin because of the new touches
 * that came.
 */
void AtomicRecognizer::MatchSubscriptionsForNewTouches() {
  /* Check if any subscriptions are active before doing any processing */
  if (num_subscriptions_ == 0)
    return;

  /* The new touches can now be used */
  for (const auto& pair : new_touches_) {
    const SharedTouch& touch = pair.second;
    INSERT_TOUCH(touch, free_touches_);
  }

  /* HandleNewTouchForAcceptedGesture may erase the gesture from
   * accepted_gestures_, so we can't use range-based for loops */
  for (auto it = accepted_gestures_.begin();
      it != accepted_gestures_.end();
      ) {
    const SharedGesture& gesture = *it++;
    HandleNewTouchesForAcceptedGesture(gesture);
  }

  if (new_touches_.size() == 0) {
    /* they've all been consumed by the accepted gesture. */
    return;
  }

  /* HandleNewTouchForUnacceptedGesture may erase the gesture from
   * accepted_gestures_, so we can't use range-based for loops */
  for (auto it = unaccepted_gestures_.begin();
      it != unaccepted_gestures_.end();
      ) {
    const SharedGesture& gesture = *it++;
    HandleNewTouchesForUnacceptedGesture(gesture);
  }

  MatchGestures();
  CLEAR_TOUCHES(new_touches_);
}

void AtomicRecognizer::MatchGestures() {
  if (free_touches_.size() == 0
      || free_touches_.size() > MAX_TOUCHES_FOR_GESTURES)
    return;

  uint64_t min_start_time = std::numeric_limits<uint64_t>::max();
  uint64_t max_start_time = 0;
  for (const auto& pair : free_touches_) {
    const SharedTouch& touch = pair.second;

    if (touch->accepted())
      continue;

    if (touch->start_time() < min_start_time)
      min_start_time = touch->start_time();

    if (touch->start_time() > max_start_time)
      max_start_time = touch->start_time();
  }

  /* All touches in a gesture must begin within a composition timeframe */
  if ((max_start_time - min_start_time) >= COMPOSITION_TIME)
    return;

  for (UGSubscription* subscription : subscriptions_[free_touches_.size()-1]) {
    Gesture* gesture = new Gesture(this, subscription, free_touches_,
                                   max_start_time);

    /* hold slice events until we accept the gesture */
    gesture->set_keep_slices(true);

    unaccepted_gestures_.insert(SharedGesture(gesture));

    LOG(Dbg) << "New tentative gesture " << gesture->id()
             << " matched subscription " << subscription << " with mask "
             << subscription->mask() << " for touches "
             << free_touches_.ToString() << "\n";
  }
}

void AtomicRecognizer::FindGesturesToAccept(uint64_t event_time)
{
  int64_t delta_time;
  for (auto it = unaccepted_gestures_.begin();
       it != unaccepted_gestures_.end();
       ) {
    const SharedGesture& gesture = *it++;

    delta_time = event_time - gesture->start_time();

      /* Atomic gestures must be accepted if they meet the subscription
         criteria.
         Wait a bit until accepting them to avoid premature gestures that
         will immediately get cancelled due to the apparition of a new touch
         point on the following events.
         e.g. like when a user puts four fingers on a touch screen but
         the corresponding touch points come in separate events (because fingers
         don't land precisely in sync and/or frame doesn't process their
         arrivals in the very same event). We should generate only the "final"
         4-touches' gesture and not the intermediates 2-touches and 3-touches
         gestures.
       */
    if (gesture->IsActive() && delta_time > 0
        && (uint64_t)delta_time >= COMPOSITION_TIME) {

      gesture->set_keep_slices(false);

      LOG(Dbg) << "accepting active atomic gesture " << gesture->id() << "\n";
      AcceptGesture(gesture);
    }
  }
}

uint64_t AtomicRecognizer::NextTimeout() {
  uint64_t timeout = Recognizer::NextTimeout();

  /* Check for any active gestures that are kept back due to waiting for the
   * composition time to pass before flushing the queue */
  for (const SharedGesture& gesture : unaccepted_gestures_) {
    if (!gesture->IsActive())
      continue;

    uint64_t new_timeout = gesture->start_time() + COMPOSITION_TIME;
    if (new_timeout < timeout)
      timeout = new_timeout;
  }

  return timeout;
}

} // namespace grail
} // namespace oif
