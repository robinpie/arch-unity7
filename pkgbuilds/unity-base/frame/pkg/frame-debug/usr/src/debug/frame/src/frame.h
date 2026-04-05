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

#ifndef FRAME_FRAME_H_
#define FRAME_FRAME_H_

#include <map>
#include <memory>
#include <vector>

#include "oif/frame.h"
#include "property.h"
#include "typedefs.h"

struct UFFrame_ {
  virtual ~UFFrame_() {}
};

struct UFBackendFrame_ {
  UFBackendFrame_(oif::frame::UFFrame* frame)
    : shared_ptr(frame) {}

  oif::frame::SharedUFFrame shared_ptr;
};

namespace oif {
namespace frame {

class UFFrame : public UFFrame_, public Property<UFFrameProperty> {
 public:
  UFFrame() : prev_(), touches_array_(), touches_map_() {}
  UFFrame(const SharedUFFrame& prev);

  UFTouch* CopyTouch(UFTouchId touchid, UFTouchState new_state) const;
  bool IsTouchOwned(UFTouchId touchid);
  UFStatus GiveTouch(SharedUFTouch& touch);
  void UpdateTouch(const SharedUFTouch& touch);
  bool IsEnded() const;
  unsigned int GetNumTouches() const { return touches_array_.size(); }
  UFStatus GetPreviousTouchValue(const UFTouch* touch, UFAxisType type,
                                 float* value) const;
  UFStatus GetPreviousTouchProperty(const UFTouch* touch,
                                    UFTouchProperty property, void* value) const;
  UFStatus GetTouchByIndex(unsigned int index, ::UFTouch* touch) const;
  SharedUFTouch* GetSharedTouchById(UFTouchId touch_id);
  UFStatus GetTouchById(UFTouchId touch_id, ::UFTouch* touch) const;
  void ReleasePreviousFrame();

  UFFrame(const UFFrame&) = delete;
  UFFrame& operator=(const UFFrame&) = delete;

 private:
  SharedUFFrame prev_;
  std::vector<SharedUFTouch> touches_array_;
  std::map<UFTouchId, unsigned int> touches_map_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_FRAME_H_
