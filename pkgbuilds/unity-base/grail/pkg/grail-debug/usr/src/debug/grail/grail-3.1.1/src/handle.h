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

#ifndef GRAIL_HANDLE_H_
#define GRAIL_HANDLE_H_

#include <list>
#include <map>
#include <memory>

#include <oif/frame.h>

#include "oif/grail.h"
#include "forward.h"

struct UGHandle_ {};

namespace oif {
namespace grail {

class UGHandle : public UGHandle_ {
 public:
  UGHandle();
  ~UGHandle();

  int event_fd() const { return event_fd_; }
  Recognizer *CreateRecognizerForSubscription(UGSubscription* subscription);
  UGStatus ActivateSubscription(UGSubscription* subscription);
  void DeactivateSubscription(UGSubscription* subscription);
  unsigned int NewGestureID(Recognizer* recognizer);
  void ProcessFrameEvent(UFEvent event);
  void UpdateTime(uint64_t time);
  uint64_t NextTimeout() const;
  void EnqueueEvent(oif::grail::UGEvent*);
  void RemoveGestureFromEventQueue(unsigned int id);
  UGStatus GetEvent(::UGEvent* event);
  UGStatus AcceptGesture(unsigned int id);
  UGStatus RejectGesture(unsigned int id);

  UGHandle(const UGHandle&) = delete;
  UGHandle& operator=(const UGHandle&) = delete;

 private:
  typedef std::unique_ptr<Recognizer> UniqueRecognizer;

  int event_fd_;
  unsigned int next_id_;
  std::map<UFDevice, std::map<UFWindowId, UniqueRecognizer>> recognizers_;
  std::map<unsigned int, Recognizer*> gestures_;
  std::list<oif::grail::UGEvent*> event_queue_;
};

} // namespace grail
} // namespace oif
#endif // GRAIL_HANDLE_H_
