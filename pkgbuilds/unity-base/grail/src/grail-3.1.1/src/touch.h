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

#ifndef GRAIL_TOUCH_H_
#define GRAIL_TOUCH_H_

#include <oif/frame.h>

namespace oif {
namespace grail {

class Touch {
 public:
  Touch(UFTouch touch, UFDevice device, UFWindowId window_id);
  ~Touch();

  void Update(UFTouch touch);
  void Accept();

  UFTouchId id() const { return id_; }
  uint64_t start_time() const { return start_time_; }
  bool accepted() const { return accepted_; }
  bool pending_end() const { return pending_end_; }
  void set_pending_end(bool pending_end) { pending_end_ = pending_end; }
  bool owned() const { return owned_; }
  void set_owned(bool owned) { owned_ = owned; }
  bool ended() const { return ended_; }
  void set_ended(bool ended) { ended_ = ended; }

  Touch(const Touch&) = delete;
  Touch& operator=(const Touch&) = delete;

 private:
  UFTouchId id_;
  uint64_t start_time_;
  UFDevice device_;
  UFWindowId window_id_;
  bool accepted_;
  bool pending_end_;
  bool owned_;
  bool ended_;
};

} // namespace grail
} // namespace oif

#endif // GRAIL_TOUCH_H_
