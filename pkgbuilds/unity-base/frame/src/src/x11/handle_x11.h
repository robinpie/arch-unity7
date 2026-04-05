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

#ifndef FRAME_HANDLE_X11_H_
#define FRAME_HANDLE_X11_H_

#include <stdint.h>

#include <map>
#include <memory>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "handle.h"
#include "typedefs.h"

namespace oif {
namespace frame {

class UFHandleX11 : public UFHandle {
 public:
  explicit UFHandleX11(Display *dsp);
  ~UFHandleX11();

  UFStatus ProcessEvent(XGenericEventCookie* xcookie);

  UFHandleX11(const UFHandleX11&) = delete;
  UFHandleX11& operator=(const UFHandleX11&) = delete;

 private:
  void AddDevice(const XIDeviceInfo& info, uint64_t time);
  void HandleHierarchyEvent(const XIHierarchyEvent* event);
  void HandleDeviceEvent(const XIDeviceEvent* event);
  void HandleOwnershipEvent(const XITouchOwnershipEvent* event);

  Display *display_;
  int xi2_opcode_;
  std::map<int, SharedUFDevice> devices_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_HANDLE_X11_H_
