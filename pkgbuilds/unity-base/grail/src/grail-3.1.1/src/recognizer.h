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

#ifndef GRAIL_RECOGNIZER_H_
#define GRAIL_RECOGNIZER_H_

#include <list>
#include <map>
#include <memory>
#include <set>

#include <oif/frame.h>

#include "oif/grail.h"
#include "forward.h"
#include "subscription.h"

#define INSERT_TOUCH(touch, map) \
  { \
    (map)[(touch)->id()] = (touch); \
    LOG(Dbg) << "touch " << (touch)->id() << " has been added to " #map "\n"; \
  }

#define ERASE_TOUCH(touch_id, map) \
  { \
    LOG(Dbg) << "touch " << touch_id << " has been erased from " #map "\n"; \
    (map).erase(touch_id); \
  }

/* OBS: it avoids the "expensive" ToString() call when debug output is not
   wanted. */
#define CLEAR_TOUCHES(map) \
  { \
    if (oif::grail::Logger::instance().level() <= oif::grail::Logger::Dbg) \
      if ((map).size() > 0) \
        LOG(Dbg) << "touch(es) " << (map).ToString() \
                 << " have been erased from " #map "\n"; \
    (map).clear(); \
  }

namespace oif {
namespace grail {

class Recognizer {
 public:
  Recognizer(UGHandle* handle, const UFDevice device, UFWindowId window);
  virtual ~Recognizer();

  virtual bool atomic() const = 0;
  virtual void ProcessFrameEvent(const UFEvent event) = 0;

  unsigned int num_subscriptions() const {return num_subscriptions_;}

  UGStatus ActivateSubscription(UGSubscription* subscription);
  void DeactivateSubscription(UGSubscription* subscription);
  virtual void UpdateTime(uint64_t time);
  void RejectOverdueGesturesAndTouches(uint64_t time);
  virtual uint64_t NextTimeout();
  UGStatus AcceptGesture(unsigned int id);
  UGStatus RejectGesture(unsigned int id);

  UGHandle* handle() const { return handle_; }
  UFDevice device() const { return device_; }
  UFWindowId window_id() const { return window_id_; }
  float device_x_res() const { return device_x_res_; }
  float device_y_res() const { return device_y_res_; }
  bool device_direct() const { return device_direct_; }

  Recognizer(const Recognizer&) = delete;
  Recognizer& operator=(const Recognizer&) = delete;

 protected:
  static const uint64_t COMPOSITION_TIME; /* in milliseconds */

  UGHandle* const handle_;
  const UFDevice device_;
  const UFWindowId window_id_;
  float device_x_res_; /* Units of pixel/mm */
  float device_y_res_;
  bool device_direct_;
  bool atomic_;
  std::set<UGSubscription*> subscriptions_[5];
  std::set<SharedGesture> unaccepted_gestures_;
  std::set<SharedGesture> accepted_gestures_;
  TouchMap free_touches_;

  unsigned int num_subscriptions_;

  void CheckConstructionFinished(uint64_t time);
  void AcceptGesture(SharedGesture gesture);
  void RejectGesture(SharedGesture gesture);
  void ProcessEvent(const UFEvent& event);
};

} // namespace grail
} // namespace oif
#endif // GRAIL_RECOGNIZER_H_
