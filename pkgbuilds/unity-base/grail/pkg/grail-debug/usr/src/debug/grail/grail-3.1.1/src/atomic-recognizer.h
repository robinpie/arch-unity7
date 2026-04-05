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

#ifndef GRAIL_ATOMIC_RECOGNIZER_H_
#define GRAIL_ATOMIC_RECOGNIZER_H_

#include "recognizer.h"

namespace oif {
namespace grail {

class AtomicRecognizer : public Recognizer {
 public:
  AtomicRecognizer(UGHandle* handle, const UFDevice device, UFWindowId window);

  virtual bool atomic() const {return true;}
  virtual void ProcessFrameEvent(const UFEvent event);
  virtual void UpdateTime(uint64_t time);

 private:
  void HandleNewTouchesForAcceptedGesture(const SharedGesture& gesture);
  void HandleNewTouchesForUnacceptedGesture(const SharedGesture& gesture);
  void ProcessTouches(const UFEvent event);
  void MatchSubscriptionsForNewTouches();
  void MatchGestures();
  void FindGesturesToAccept(uint64_t event_time);
  virtual uint64_t NextTimeout();

  std::map<UFTouchId, std::weak_ptr<Touch>> all_touches_;

  /* Touches that have begun but not yet been matched against subscriptions
     (for the creation of new gestures) or used to update existing gestures. */
  TouchMap new_touches_;
};

} // namespace grail
} // namespace oif

#endif // GRAIL_ATOMIC_RECOGNIZER_H_
