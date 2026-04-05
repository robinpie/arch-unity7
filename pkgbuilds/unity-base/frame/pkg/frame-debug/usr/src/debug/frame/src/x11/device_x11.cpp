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

#include "x11/device_x11.h"

#include <stdio.h>
#include <string.h>

#include <stdexcept>

#include <xorg/xserver-properties.h>

#include "oif/frame_x11.h"
#include "axis.h"
#include "value.h"
#include "x11/window_x11.h"

namespace oif {
namespace frame {

namespace {

UFAxisType UFAxisTypeForLabel(Display* display, Atom label) {
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_TOUCH_MAJOR,
                              True))
    return UFAxisTypeTouchMajor;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_TOUCH_MINOR,
                              True))
    return UFAxisTypeTouchMinor;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_WIDTH_MAJOR,
                              True))
    return UFAxisTypeWidthMajor;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_WIDTH_MINOR,
                              True))
    return UFAxisTypeWidthMinor;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_ORIENTATION,
                              True))
    return UFAxisTypeOrientation;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_TOOL_TYPE, True))
    return UFAxisTypeTool;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_BLOB_ID, True))
    return UFAxisTypeBlobId;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_TRACKING_ID,
                              True))
    return UFAxisTypeTrackingId;
  if (label == XInternAtom(display, AXIS_LABEL_PROP_ABS_MT_PRESSURE, True))
    return UFAxisTypePressure;

  throw std::domain_error("Not a multitouch axis");
}

} // namespace

UFDeviceX11::UFDeviceX11(Display* display, const XIDeviceInfo& info)
    : display_(display),
      master_id_(info.attachment),
      axis_map_(),
      windows_() {
  const Value* value;

  value = new Value(info.name);
  InsertProperty(UFDevicePropertyName, value);

  for (int i = 0; i < info.num_classes; ++i) {
    switch (info.classes[i]->type) {
      case XITouchClass: {
        XITouchClassInfo* touch_info =
          reinterpret_cast<XITouchClassInfo*>(info.classes[i]);

        value = new Value(touch_info->mode == XIDirectTouch);
        InsertProperty(UFDevicePropertyDirect, value);

        value = new Value(false);
        InsertProperty(UFDevicePropertyIndependent, value);

        value = new Value(false);
        InsertProperty(UFDevicePropertySemiMT, value);

        value = new Value(touch_info->num_touches);
        InsertProperty(UFDevicePropertyMaxTouches, value);
        break;
      }

      case XIValuatorClass: {
        XIValuatorClassInfo* valuator_info =
          reinterpret_cast<XIValuatorClassInfo*>(info.classes[i]);

        UFAxisType type;

        /* X and Y axes are always 0 and 1 in X11. Due to historical reasons,
         * the labels of these axes may not be consistent across input modules.
         * Hard code them here instead. */
        if (valuator_info->number == 0)
          type = UFAxisTypeX;
        else if (valuator_info->number == 1)
          type = UFAxisTypeY;
        else {
          try {
            type = UFAxisTypeForLabel(display, valuator_info->label);
          } catch (std::domain_error &error) {
            break;
          }
        }

        UFAxis_* axis = new UFAxis(type, valuator_info->min, valuator_info->max,
                                   valuator_info->resolution);

        axes_[type] =
            std::move(UniqueUFAxis(static_cast<oif::frame::UFAxis*>(axis)));

        axis_map_[valuator_info->number] = type;

        break;
      }

      default:
        break;
    }
  }

  /* X11 doesn't provide us the real physical resolution of the display */
  value = new Value(0.f);
  InsertProperty(UFDevicePropertyWindowResolutionX, value);
  value = new Value(0.f);
  InsertProperty(UFDevicePropertyWindowResolutionY, value);
}

bool UFDeviceX11::HandleDeviceEvent(const XIDeviceEvent* event,
                                    SharedUFFrame* frame) {
  WindowX11* window;
  if (windows_.find(event->event) != windows_.end()) {
    SharedWindow& shared_window = windows_[event->event];
    window = static_cast<WindowX11*>(shared_window.get());
  } else {
    window = new WindowX11(event->event, shared_from_this(), display_);
    windows_[event->event] = SharedWindow(window);
  }

  if (!window->HandleDeviceEvent(event, frame))
    return false;

  if (window->IsContextEnded()) {
    window->ReleaseFrames();
    windows_.erase(event->event);
  }

  return true;
}

bool UFDeviceX11::HandleOwnershipEvent(const XITouchOwnershipEvent* event,
                                       SharedUFFrame* frame) {
  /* We don't really know if the ownership event is for a particular window
   * when using the Ubuntu prototype multitouch X server. We just take the first
   * one for algorithmic completeness, and assume the frame client will work
   * around this issue. */
  for (auto& pair : windows_) {
    WindowX11* window = static_cast<WindowX11*>(pair.second.get());
    if (!window->IsTouchOwned(event->touchid))
      return window->HandleOwnershipEvent(event, frame);
  }

  return false;
}

UFStatus UFDeviceX11::AcceptTouch(UFWindowId window_id,
                                  UFTouchId touch_id) {
  ::Window x11_window_id = frame_x11_get_window_id(window_id);
  auto it = windows_.find(x11_window_id);
  if (it != windows_.end()) {
    WindowX11* window = static_cast<WindowX11*>(it->second.get());
    return window->AcceptTouch(touch_id);
  }

  return UFStatusErrorInvalidTouch;
}

UFStatus UFDeviceX11::RejectTouch(UFWindowId window_id,
                                  UFTouchId touch_id) {
  ::Window x11_window_id = frame_x11_get_window_id(window_id);
  auto it = windows_.find(x11_window_id);
  if (it != windows_.end()) {
    WindowX11* window = static_cast<WindowX11*>(it->second.get());
    return window->RejectTouch(touch_id);
  }

  return UFStatusErrorInvalidTouch;
}

void UFDeviceX11::ReleaseFrames() {
  for (auto& pair : windows_)
    pair.second->ReleaseFrames();
  windows_.clear();
}

} // namespace frame
} // namespace oif

extern "C" {

UFStatus frame_x11_accept_touch(UFDevice device, UFWindowId window_id,
                                UFTouchId touch_id) {
  oif::frame::UFDeviceX11* device_x11 =
      static_cast<oif::frame::UFDeviceX11*>(device);
  return device_x11->AcceptTouch(window_id, touch_id);
}

UFStatus frame_x11_reject_touch(UFDevice device, UFWindowId window_id,
                                UFTouchId touch_id) {
  oif::frame::UFDeviceX11* device_x11 =
      static_cast<oif::frame::UFDeviceX11*>(device);
  return device_x11->RejectTouch(window_id, touch_id);
}

} // extern "C"
