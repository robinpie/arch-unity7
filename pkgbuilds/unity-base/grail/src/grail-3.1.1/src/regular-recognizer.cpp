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

#include "config.h"
#include "regular-recognizer.h"

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

/**
 * @internal
 * Create a new regular recognizer for a given device and window
 */
RegularRecognizer::RegularRecognizer(UGHandle* handle, const UFDevice device,
                                     UFWindowId window_id)
    : Recognizer(handle, device, window_id) {
}

/**
 * @internal
 * Process a frame event
 */
void RegularRecognizer::ProcessFrameEvent(const UFEvent event) {
  LOG(Dbg) << "new event " << event << " with time "
           << frame_event_get_time(event) << "\n";
  RejectOverdueGesturesAndTouches(frame_event_get_time(event));
  ProcessTouches(event);
  ProcessEvent(event);
  CheckConstructionFinished(frame_event_get_time(event));
}

/**
 * @internal
 * Process all touches present in the given frame event.
 *
 * Add new touches to new_touches_ and free_touches_ maps, update the grail
 * touch state for existing touches, and remove touches from maps when they
 * physically end.
 *
 * When a touch begins, attempt to match it to all gesture subscriptions.
 */
void RegularRecognizer::ProcessTouches(const UFEvent event) {
  UFFrame frame;
  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get frame from event\n";
    return;
  }

  /* Process all the touches that began in this frame */
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
        INSERT_TOUCH(grail_touch, free_touches_);

        for (auto gesture : accepted_gestures_) {
          ExpandGestureIfPossible(grail_touch, gesture);
        }

        for (auto it = unaccepted_gestures_.begin();
             it != unaccepted_gestures_.end();
             ) {
          const SharedGesture& gesture = *it++;
          ExpandGestureIfPossible(grail_touch, gesture);
        }

        /* Attempt to match new gestures for active subscriptions */
        MatchOneTouchGestures(grail_touch);
        MatchTwoTouchGestures(grail_touch);
        MatchThreeTouchGestures(grail_touch);
        MatchFourTouchGestures(grail_touch);
        MatchFiveTouchGestures(grail_touch);
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

          if (grail_touch->pending_end() || grail_touch->ended())
            ERASE_TOUCH(grail_touch->id(), free_touches_);
        }
        break;
      }
    }
  }
}

/**
 * @internal
 * If a gesture may add a touch without crossing the maximum for the
 * subscription, create a new unaccepted gesture with the new touch.
 * Otherwise, do nothing. New gestures may still begin elsewhere.
 */
void RegularRecognizer::ExpandGestureIfPossible(
    const SharedTouch& touch,
    const SharedGesture& gesture) {

  UGSubscription* subscription = gesture->subscription();
  if (gesture->current_touches().size() < subscription->touches_max()
        && !gesture->ContainsTouch(touch)
        && !gesture->IsPhysicallyEnded()) {
    TouchMap map(gesture->current_touches());
    map[touch->id()] = touch;
    Gesture* new_gesture = new Gesture(gesture.get(), map);
    LOG(Dbg) << "touch " << touch->id()
             << " has been added to gesture " << gesture->id()
             << " to create new gesture " << new_gesture->id() << "\n";
    unaccepted_gestures_.insert(SharedGesture(new_gesture));
    LOG(Dbg) << "gesture " << new_gesture
             << " has been added to unaccepted gestures\n";
  }
}

/**
 * @internal
 * Attempt to match the given touch against one touch subscriptions
 */
void RegularRecognizer::MatchOneTouchGestures(const SharedTouch& touch) {
  for (UGSubscription* subscription : subscriptions_[0]) {
    TouchMap map;
    map[touch->id()] = touch;

    assert(!UnacceptedGestureExists(subscription, map));

    Gesture* gesture = new Gesture(this, subscription, map,
                                   touch->start_time());
    unaccepted_gestures_.insert(SharedGesture(gesture));

    LOG(Dbg) << "New tentative gesture " << gesture->id()
             << " matched subscription " << subscription << " with mask "
             << subscription->mask() << " for touch " << touch->id() << "\n";
  }
}

/**
 * @internal
 * Attempt to match the given touch against two touch subscriptions
 */
void RegularRecognizer::MatchTwoTouchGestures(const SharedTouch& touch) {
  for (UGSubscription* subscription : subscriptions_[1]) {
    for (const auto& pair : free_touches_) {
      const SharedTouch& other = pair.second;
      if (other->id() == touch->id())
        continue;

      /* All touches in a gesture must begin within a composition timeframe */
      uint64_t min_start_time = touch->start_time();
      if (other->start_time() < min_start_time && !other->accepted())
        min_start_time = other->start_time();

      if (touch->start_time() - min_start_time < COMPOSITION_TIME) {
        TouchMap map;
        map[touch->id()] = touch;
        map[other->id()] = other;

        assert(!UnacceptedGestureExists(subscription, map));

        Gesture* gesture = new Gesture(this, subscription, map,
                                       touch->start_time());
        unaccepted_gestures_.insert(SharedGesture(gesture));

        LOG(Dbg) << "New tentative gesture " << gesture->id()
                 << " matched subscription " << subscription << " with mask "
                 << subscription->mask() << " for touches " << touch->id()
                 << ", " << other->id() << "\n";
      }
    }
  }
}

/**
 * @internal
 * Attempt to match the given touch against three touch subscriptions
 */
void RegularRecognizer::MatchThreeTouchGestures(const SharedTouch& touch) {
  for (UGSubscription* subscription : subscriptions_[2]) {
    for (const auto& pair_1 : free_touches_) {
      const SharedTouch& other_1 = pair_1.second;
      if (other_1->id() == touch->id())
        continue;

      for (const auto& pair_2 : free_touches_) {
        const SharedTouch& other_2 = pair_2.second;
        if (other_2->id() <= other_1->id() || other_2->id() == touch->id())
          continue;

        /* All touches in a gesture must begin within a composition timeframe */
        uint64_t min_start_time = touch->start_time();
        if (other_1->start_time() < min_start_time && !other_1->accepted())
          min_start_time = other_1->start_time();
        if (other_2->start_time() < min_start_time && !other_2->accepted())
          min_start_time = other_2->start_time();

        if (touch->start_time() - min_start_time < COMPOSITION_TIME) {
          TouchMap map;
          map[touch->id()] = touch;
          map[other_1->id()] = other_1;
          map[other_2->id()] = other_2;

          assert(!UnacceptedGestureExists(subscription, map));

          Gesture* gesture = new Gesture(this, subscription, map,
                                         touch->start_time());
          unaccepted_gestures_.insert(SharedGesture(gesture));

          LOG(Dbg) << "New tentative gesture " << gesture->id()
                   << " matched subscription " << subscription << " with mask "
                   << subscription->mask() << " for touches " << touch->id()
                   << ", " << other_1->id() << ", " << other_2->id() << "\n";
        }
      }
    }
  }
}

/**
 * @internal
 * Attempt to match the given touch against four touch subscriptions
 */
void RegularRecognizer::MatchFourTouchGestures(const SharedTouch& touch) {
  for (UGSubscription* subscription : subscriptions_[3]) {
    for (const auto& pair_1 : free_touches_) {
      const SharedTouch& other_1 = pair_1.second;
      if (other_1->id() == touch->id())
        continue;

      for (const auto& pair_2 : free_touches_) {
        const SharedTouch& other_2 = pair_2.second;
        if (other_2->id() <= other_1->id() || other_2->id() == touch->id())
          continue;

        for (const auto& pair_3 : free_touches_) {
          const SharedTouch& other_3 = pair_3.second;
          if (other_3->id() <= other_2->id() || other_3->id() == touch->id())
            continue;

          /* All touches in a gesture must begin within a composition
           * timeframe */
          uint64_t min_start_time = touch->start_time();
          if (other_1->start_time() < min_start_time && !other_1->accepted())
            min_start_time = other_1->start_time();
          if (other_2->start_time() < min_start_time && !other_2->accepted())
            min_start_time = other_2->start_time();
          if (other_3->start_time() < min_start_time && !other_3->accepted())
            min_start_time = other_3->start_time();

          if (touch->start_time() - min_start_time < COMPOSITION_TIME) {
            TouchMap map;
            map[touch->id()] = touch;
            map[other_1->id()] = other_1;
            map[other_2->id()] = other_2;
            map[other_3->id()] = other_3;

            assert(!UnacceptedGestureExists(subscription, map));

            Gesture* gesture = new Gesture(this, subscription, map,
                                           touch->start_time());
            unaccepted_gestures_.insert(SharedGesture(gesture));

            LOG(Dbg) << "New tentative gesture " << gesture->id()
                     << " matched subscription " << subscription
                     << " with mask " << subscription->mask() << " for touches "
                     << touch->id() << ", " << other_1->id() << ", "
                     << other_2->id() << ", " << other_3->id() << "\n";
          }
        }
      }
    }
  }
}

/**
 * @internal
 * Attempt to match the given touch against five touch subscriptions
 */
void RegularRecognizer::MatchFiveTouchGestures(const SharedTouch& touch) {
  for (UGSubscription* subscription : subscriptions_[4]) {
    for (const auto& pair_1 : free_touches_) {
      const SharedTouch& other_1 = pair_1.second;
      if (other_1->id() == touch->id())
        continue;

      for (const auto& pair_2 : free_touches_) {
        const SharedTouch& other_2 = pair_2.second;
        if (other_2->id() <= other_1->id() || other_2->id() == touch->id())
          continue;

        for (const auto& pair_3 : free_touches_) {
          const SharedTouch& other_3 = pair_3.second;
          if (other_3->id() <= other_2->id() || other_3->id() == touch->id())
            continue;

          for (const auto& pair_4 : free_touches_) {
            const SharedTouch& other_4 = pair_4.second;
            if (other_4->id() <= other_3->id() || other_4->id() == touch->id())
              continue;

            /* All touches in a gesture must begin within a composition
             * timeframe */
            uint64_t min_start_time = touch->start_time();
            if (other_1->start_time() < min_start_time && !other_1->accepted())
              min_start_time = other_1->start_time();
            if (other_2->start_time() < min_start_time && !other_2->accepted())
              min_start_time = other_2->start_time();
            if (other_3->start_time() < min_start_time && !other_3->accepted())
              min_start_time = other_3->start_time();
            if (other_4->start_time() < min_start_time && !other_4->accepted())
              min_start_time = other_4->start_time();

            if (touch->start_time() - min_start_time < COMPOSITION_TIME) {
              TouchMap map;
              map[touch->id()] = touch;
              map[other_1->id()] = other_1;
              map[other_2->id()] = other_2;
              map[other_3->id()] = other_3;
              map[other_4->id()] = other_4;

              assert(!UnacceptedGestureExists(subscription, map));

              Gesture* gesture = new Gesture(this, subscription, map,
                                             touch->start_time());
              unaccepted_gestures_.insert(SharedGesture(gesture));

              LOG(Dbg) << "New tentative gesture " << gesture->id()
                       << " matched subscription " << subscription
                       << " with mask " << subscription->mask()
                       << " for touches " << touch->id() << ", "
                       << other_1->id() << ", " << other_2->id() << ", "
                       << other_3->id() << ", " << other_4->id() << "\n";
            }
          }
        }
      }
    }
  }
}


/**
 * @internal
 * Returns whether an unaccepted gesture with the given characteristics already exists
 */
bool RegularRecognizer::UnacceptedGestureExists(UGSubscription* subscription,
    TouchMap &touch_map)
{
  for (auto gesture : unaccepted_gestures_) {
    if (subscription != gesture->subscription())
      continue;

    if (gesture->current_touches().Equals(touch_map))
      return true;
  }

  return false;
}

} // namespace grail
} // namespace oif
