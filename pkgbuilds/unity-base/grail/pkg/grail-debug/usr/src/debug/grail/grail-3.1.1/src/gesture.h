/*****************************************************************************
 *
 * grail - Multitouch Gesture Recognition Library
 *
 * Copyright (C) 2011-2012 Canonical Ltd.
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

#ifndef GRAIL_GESTURE_H_
#define GRAIL_GESTURE_H_

#include <map>
#include <memory>
#include <queue>
#include <set>

#include <oif/frame.h>

#include "oif/grail.h"
#include "forward.h"

namespace oif {
namespace grail {

class Gesture : public std::enable_shared_from_this<Gesture> {
 public:
  Gesture(Recognizer* recognizer, UGSubscription* subscription,
          TouchMap& touches, uint64_t start_time);
  Gesture(const Gesture* gesture, TouchMap& touches);
  ~Gesture();

  void AddTouch(const SharedTouch& touch);
  void AddTouches(TouchMap touches);
  bool ContainsTouch(const SharedTouch& touch) const;
  void Update(UFEvent event, TouchSet& touches);
  bool IsActive() const;
  bool IsConstructionFinished() const;
  bool IsPhysicallyEnded() const;
  void FinishConstruction();
  uint64_t Timeout() const;
  float AngleForTouch(UFTouchId touch_id) const;
  void SetAngleForTouch(UFTouchId touch_id, float angle);
  void Cancel();
  void End();

  const unsigned int id() const { return id_; }
  const Recognizer& recognizer() const { return *recognizer_; }
  UGSubscription* subscription() const { return subscription_; }
  const TouchMap& current_touches() const { return current_touches_; }
  const TouchMap& all_touches() const { return all_touches_; }
  const uint64_t start_time() const { return start_time_; }
  bool canceled() const { return canceled_; }
  bool ended() const { return ended_; }

  Gesture(const Gesture&) = delete;
  Gesture& operator=(const Gesture&) = delete;

  void set_keep_slices(bool keep_slices);

 private:
  void CheckOwned();
  void FlushSlices();

  Recognizer* recognizer_;
  unsigned int id_;
  UGSubscription* subscription_;
  TouchMap current_touches_; /**< Current touches of the gesture */
  TouchMap all_touches_; /**< All previous and current touches of the gesture */
  uint64_t start_time_;
  bool owned_;
  bool not_owned_;
  UGGestureTypeMask recognized_;
  std::queue<SharedUGSlice> slices_;
  SharedUGSlice last_slice_;
  bool canceled_;
  bool ended_;
  std::map<UFTouchId, float> angles_;
  bool keep_slices_;
};

} // namespace grail
} // namespace oif
#endif // GRAIL_GESTURE_H_
