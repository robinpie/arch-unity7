/*****************************************************************************
 *
 * frame - Touch Frame Library
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

#ifndef FRAME_TOUCH_H_
#define FRAME_TOUCH_H_

#include <stdint.h>

#include <map>
#include <memory>

#include "oif/frame.h"
#include "property.h"
#include "typedefs.h"

struct UFTouch_ {
  virtual ~UFTouch_() {}
};

struct UFBackendTouch_ {
  UFBackendTouch_(oif::frame::UFTouch* touch)
    : shared_ptr(touch) {}

  UFBackendTouch_(oif::frame::SharedUFTouch &shared_touch) {
    shared_ptr.swap(shared_touch);
  }

  oif::frame::UFTouch* GetModifiableTouch();

  oif::frame::SharedUFTouch shared_ptr;
};

namespace oif {
namespace frame {

class UFTouch : public UFTouch_, public Property<UFTouchProperty> {
 public:
  UFTouch();
  UFTouch(UFTouchState state, UFTouchId id, float x, float y,
           uint64_t time);
  UFTouch(const UFTouch& touch, UFTouchState new_state);

  void SetId(UFTouchId id) { id_ = id; }
  UFTouchId id() const { return id_; }

  void SetState(UFTouchState state) { state_ = state; }
  UFTouchState state() const { return state_; }

  void SetValue(UFAxisType type, float value);
  UFStatus GetValue(UFAxisType type, float* value) const;

  UFTouch& operator=(const UFTouch&) = delete;

 private:
  UFTouchId id_;
  UFTouchState state_;
  std::map<UFAxisType, float> values_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_TOUCH_H_
