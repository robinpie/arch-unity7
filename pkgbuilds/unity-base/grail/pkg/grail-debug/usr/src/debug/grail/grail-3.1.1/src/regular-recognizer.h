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

#ifndef GRAIL_REGULAR_RECOGNIZER_H_
#define GRAIL_REGULAR_RECOGNIZER_H_

#include "recognizer.h"

namespace oif {
namespace grail {

class RegularRecognizer : public Recognizer {
 public:
  RegularRecognizer(UGHandle* handle, const UFDevice device, UFWindowId window);

  virtual bool atomic() const {return false;}
  virtual void ProcessFrameEvent(const UFEvent event);

 private: 
  void ExpandGestureIfPossible(const SharedTouch& touch,
                               const SharedGesture& gesture);
  void ProcessTouches(const UFEvent event);
  void MatchOneTouchGestures(const SharedTouch& touch);
  void MatchTwoTouchGestures(const SharedTouch& touch);
  void MatchThreeTouchGestures(const SharedTouch& touch);
  void MatchFourTouchGestures(const SharedTouch& touch);
  void MatchFiveTouchGestures(const SharedTouch& touch);
  bool UnacceptedGestureExists(UGSubscription* subscription, TouchMap &touch_map);

  std::map<UFTouchId, std::weak_ptr<Touch>> all_touches_;
};

} // namespace grail
} // namespace oif
#endif // GRAIL_REGULAR_RECOGNIZER_H_
