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

#include "touch.h"

#include "log.h"

namespace oif {
namespace grail {

Touch::Touch(UFTouch touch, UFDevice device, UFWindowId window_id)
      : id_(frame_touch_get_id(touch)),
        start_time_(frame_touch_get_start_time(touch)),
        device_(device),
        window_id_(window_id),
        accepted_(false),
        pending_end_(false),
        owned_(false),
        ended_(false) {
  Update(touch);
}

void Touch::Update(UFTouch touch) {
  if (frame_touch_get_state(touch) == UFTouchStateEnd) {
    ended_ = true;
    LOG(Dbg) << "touch " << id_ << " has ended\n";
  }

  UFStatus status;
  int value;

  if (!pending_end_) {
    status = frame_touch_get_property(touch, UFTouchPropertyPendingEnd, &value);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch pending end property\n";
    } else if (value) {
      pending_end_ = true;
      LOG(Dbg) << "touch " << id_ << " is pending end\n";
    }
  }

  if (!owned_) {
    status = frame_touch_get_property(touch, UFTouchPropertyOwned, &value);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch owned property\n";
    } else if (value) {
      owned_ = true;
      LOG(Dbg) << "touch " << id_ << " is owned\n";
    }
  }
}

void Touch::Accept() {
  LOG(Dbg) << "accepting touch " << id_ << "\n";
  if (frame_accept_touch(device_, window_id_, id_) != UFStatusSuccess)
    LOG(Err) << "touch " << id_ << " failed to be accepted\n";

  accepted_ = true;
}

Touch::~Touch() {
  if (!accepted_) {
    LOG(Dbg) << "rejecting touch " << id_ << "\n";
    if (frame_reject_touch(device_, window_id_, id_) != UFStatusSuccess)
      LOG(Err) << "touch " << id_ << " failed to be rejected\n";
  }
}

} // namespace grail
} // namespace oif
