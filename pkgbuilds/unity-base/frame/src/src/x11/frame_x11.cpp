/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2011-12 Canonical Ltd.
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

#include <stdlib.h>

#include <X11/Xlib.h>

#include "oif/frame.h"
#include "oif/frame_x11.h"
#include "x11/handle_x11.h"

extern "C" {

UFStatus frame_x11_new(Display* display, ::UFHandle* handle) {
  oif::frame::UFHandleX11** handle_x11 =
      reinterpret_cast<oif::frame::UFHandleX11**>(handle);

  try {
    *handle_x11 = new oif::frame::UFHandleX11(display);
    return UFStatusSuccess;
  } catch (const std::exception& exception) {
    *handle_x11 = NULL;
    return UFStatusErrorResources;
  }
}

UFStatus frame_x11_process_event(UFHandle handle,
                                 XGenericEventCookie* xcookie) {
  oif::frame::UFHandleX11* handle_x11 =
      static_cast<oif::frame::UFHandleX11*>(handle);
  return handle_x11->ProcessEvent(xcookie);
}

void frame_x11_delete(UFHandle handle) {
  oif::frame::UFHandleX11* handle_x11 =
      static_cast<oif::frame::UFHandleX11*>(handle);
  delete handle_x11;
}

Window frame_x11_get_window_id(UFWindowId window_id) {
  return static_cast<Window>(window_id);
}

UFWindowId frame_x11_create_window_id(Window id) {
  return static_cast<UFWindowId>(id);
}

unsigned int frame_x11_get_touch_id(UFTouchId touch_id) {
  return static_cast<unsigned int>(touch_id);
}

UFTouchId frame_x11_create_touch_id(unsigned int id) {
  return static_cast<UFTouchId>(id);
}

} // extern "C"
