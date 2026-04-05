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

#ifndef FRAME_DEVICE_H_
#define FRAME_DEVICE_H_

#include <map>
#include <memory>

#include "oif/frame.h"
#include "axis.h"
#include "typedefs.h"

struct UFDevice_ {
  virtual ~UFDevice_() {};
};

struct UFBackendDevice_ {
  UFBackendDevice_(oif::frame::UFDevice* device)
    : shared_ptr(device) {}

  oif::frame::SharedUFDevice shared_ptr;
};

namespace oif {
namespace frame {

class UFDevice : public UFDevice_, public Property<UFDeviceProperty> {
 public:
  UFDevice() : axes_() {}

  UFStatus GetAxisByIndex(unsigned int index, ::UFAxis* axis) const;
  UFStatus GetAxisByType(UFAxisType type, ::UFAxis* axis) const;

  UFDevice(const UFDevice&) = delete;
  UFDevice& operator=(const UFDevice&) = delete;

  typedef std::unique_ptr<UFAxis> UniqueUFAxis;

  std::map<UFAxisType, UniqueUFAxis> axes_;

  virtual UFStatus AcceptTouch(UFWindowId window_id, UFTouchId touch_id);
  virtual UFStatus RejectTouch(UFWindowId window_id, UFTouchId touch_id);
};

} // namespace frame
} // namespace oif

#endif // FRAME_DEVICE_H_
