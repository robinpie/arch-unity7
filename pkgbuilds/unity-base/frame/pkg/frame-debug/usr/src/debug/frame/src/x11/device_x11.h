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

#ifndef FRAME_DEVICE_X11_H_
#define FRAME_DEVICE_X11_H_

#include <map>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "oif/frame.h"
#include "device.h"
#include "typedefs.h"

namespace oif {
namespace frame {

class UFDeviceX11
    : public UFDevice,
      public std::enable_shared_from_this<UFDeviceX11> {
 public:
  UFDeviceX11(Display* display, const XIDeviceInfo& info);

  int master_id() const { return master_id_; }
  const std::map<int, UFAxisType>& axis_map() const { return axis_map_; }

  bool HandleDeviceEvent(const XIDeviceEvent* event, SharedUFFrame* frame);
  bool HandleOwnershipEvent(const XITouchOwnershipEvent* event,
                            SharedUFFrame* frame);
  virtual UFStatus AcceptTouch(UFWindowId window_id, UFTouchId touch_id);
  virtual UFStatus RejectTouch(UFWindowId window_id, UFTouchId touch_id);
  void ReleaseFrames();

  UFDeviceX11(const UFDeviceX11&) = delete;
  UFDeviceX11& operator=(const UFDeviceX11&) = delete;

 private:
  Display* display_;
  int master_id_;
  std::map<int, UFAxisType> axis_map_;
  std::map< ::Window, SharedWindow> windows_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_DEVICE_X11_H_
