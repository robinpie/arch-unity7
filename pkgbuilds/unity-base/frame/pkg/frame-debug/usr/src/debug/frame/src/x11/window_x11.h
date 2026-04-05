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

#ifndef FRAME_WINDOW_X11_H_
#define FRAME_WINDOW_X11_H_

#include <map>
#include <memory>
#include <set>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "oif/frame.h"
#include "typedefs.h"
#include "window.h"

namespace oif {
namespace frame {

class UFDeviceX11;

class WindowX11 : public Window {
 public:
  WindowX11(::Window window, const SharedUFDevice& device,
            Display* display);
  virtual ~WindowX11() {};

  bool HandleDeviceEvent(const XIDeviceEvent* event, SharedUFFrame* frame);
  bool HandleOwnershipEvent(const XITouchOwnershipEvent* event,
                            SharedUFFrame* frame);
  UFStatus AcceptTouch(UFTouchId touch_id);
  UFStatus RejectTouch(UFTouchId touch_id);
  virtual bool IsContextEnded() const;

  WindowX11(const WindowX11&) = delete;
  WindowX11& operator=(const WindowX11&) = delete;

 private:
  ::Window window_;
  Display* display_;
  UFDeviceX11* device_;
  std::set<UFTouchId> unaccepted_unrejected_touches_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_WINDOW_X11_H_
