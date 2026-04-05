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

#include "x11/window_x11.h"

#include <stdio.h>

#include "oif/frame_x11.h"
#include "frame.h"
#include "touch.h"
#include "value.h"
#include "x11/device_x11.h"

namespace oif {
namespace frame {

WindowX11::WindowX11(::Window window, const SharedUFDevice& device,
                     Display* display)
    : window_(window),
      display_(display),
      device_(static_cast<UFDeviceX11*>(device.get())),
      unaccepted_unrejected_touches_() {
}

namespace {

void CopyOldValue(const UFFrame* frame, UFTouch* touch, UFAxisType type) {
  float value;
  UFStatus status;

  status = frame->GetPreviousTouchValue(touch, type, &value);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Warning: failed to get previous touch value\n");
    return;
  }

  touch->SetValue(type, value);
}

} // namespace

bool WindowX11::HandleDeviceEvent(const XIDeviceEvent* event,
                                  SharedUFFrame* frame) {
  UFTouchState state;
  switch (event->evtype) {
    case XI_TouchBegin:
      state = UFTouchStateBegin;
      break;

    case XI_TouchUpdate:
      state = UFTouchStateUpdate;
      break;

    case XI_TouchEnd:
      state = UFTouchStateEnd;
      break;

    default:
      return false;
  }

  *frame = SharedUFFrame(new UFFrame(current_frame_));

  const Value* value = new Value(frame_x11_create_window_id(window_));
  (*frame)->InsertProperty(UFFramePropertyWindowId, value);
  value = new Value(device_->shared_from_this());
  (*frame)->InsertProperty(UFFramePropertyDevice, value);

  UFTouchId touch_id = frame_x11_create_touch_id(event->detail);
  std::shared_ptr<UFTouch> touch(new UFTouch(state, touch_id, event->event_x,
                                             event->event_y, event->time));

  if (event->evtype == XI_TouchBegin) {
    value = new Value(false);
    touch->InsertProperty(UFTouchPropertyOwned, value);
  } else {
    bool owned = current_frame_->IsTouchOwned(touch_id);
    value = new Value(owned);
    touch->InsertProperty(UFTouchPropertyOwned, value);
  }

  value = new Value(event->flags & XITouchPendingEnd);
  touch->InsertProperty(UFTouchPropertyPendingEnd, value);

  int i = 0; /* Current XI axis number */
  int j = 0; /* Current XI valuator value offset in event */
  int k = 0; /* Number of handled frame axes */
  for (;
       i < event->valuators.mask_len * 8 &&
       k < static_cast<int>(device_->axis_map().size());
       ++i) {
    const auto& axis = device_->axis_map().find(i);

    if (axis == device_->axis_map().cend()) {
      /* We don't care about this axis, but we may need to skip its value */
      if (XIMaskIsSet(event->valuators.mask, i))
        ++j;

      continue;
    }

    UFAxisType type = axis->second;
    if (XIMaskIsSet(event->valuators.mask, i))
      touch->SetValue(type, event->valuators.values[j++]);
    else
      CopyOldValue(frame->get(), touch.get(), type);

    ++k;
  }

  for (; k < static_cast<int>(device_->axis_map().size()); ++i) {
    const auto& axis = device_->axis_map().find(i);

    if (axis == device_->axis_map().cend())
      continue;

    UFAxisType type = axis->second;
    CopyOldValue(frame->get(), touch.get(), type);
    ++k;
  }

  (*frame)->UpdateTouch(touch);

  current_frame_ = *frame;

  return true;
}

bool WindowX11::HandleOwnershipEvent(const XITouchOwnershipEvent* event,
                                     SharedUFFrame* frame) {
  *frame = SharedUFFrame(new UFFrame(current_frame_));

  const Value* value = new Value(frame_x11_create_window_id(window_));
  (*frame)->InsertProperty(UFFramePropertyWindowId, value);
  value = new Value(device_->shared_from_this());
  (*frame)->InsertProperty(UFFramePropertyDevice, value);

  UFTouch* touch = (*frame)->CopyTouch(event->touchid, UFTouchStateUpdate);
  if (!touch)
    return false;

  value = new Value(static_cast<uint64_t>(event->time));
  touch->InsertProperty(UFTouchPropertyTime, value);
  value = new Value(true);
  touch->InsertProperty(UFTouchPropertyOwned, value);

  (*frame)->UpdateTouch(SharedUFTouch(touch));

  current_frame_ = *frame;

  unaccepted_unrejected_touches_.insert(event->touchid);

  return true;
}

UFStatus WindowX11::AcceptTouch(UFTouchId touch_id) {
  if (XIAllowTouchEvents(display_, device_->master_id(), touch_id, window_,
                         XIAcceptTouch))
    return UFStatusErrorGeneric;

  /* Flush output buffer so touches are actually accepted ASAP. The server
   * can't perform pointer emulation while the currently emulated touch is
   * still potentially active for pointer emulation. */
  XFlush(display_);

  unaccepted_unrejected_touches_.erase(touch_id);

  return UFStatusSuccess;
}

UFStatus WindowX11::RejectTouch(UFTouchId touch_id) {
  if (XIAllowTouchEvents(display_, device_->master_id(), touch_id, window_,
                         XIRejectTouch))
    return UFStatusErrorGeneric;

  /* Flush output buffer so touches are actually rejected ASAP. The server
   * can't perform pointer emulation while the currently emulated touch is
   * still potentially active for pointer emulation. */
  XFlush(display_);

  unaccepted_unrejected_touches_.erase(touch_id);

  return UFStatusSuccess;
}

bool WindowX11::IsContextEnded() const {
  return unaccepted_unrejected_touches_.size() == 0 &&
         current_frame_->IsEnded();
}

} // namespace frame
} // namespace oif
