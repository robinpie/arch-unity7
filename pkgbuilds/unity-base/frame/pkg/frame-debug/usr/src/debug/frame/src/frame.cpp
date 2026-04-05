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

#include "frame.h"

#include <assert.h>
#include <stdio.h>

#include "device.h"
#include "touch.h"

#include <oif/frame_backend.h>

namespace oif {
namespace frame {

UFFrame::UFFrame(const SharedUFFrame& prev)
    : prev_(prev),
      touches_array_(),
      touches_map_() {
  for (const SharedUFTouch& prev_touch : prev_->touches_array_) {
    if (prev_touch->state() == UFTouchStateBegin) {
      touches_map_[prev_touch->id()] = touches_array_.size();
      SharedUFTouch touch(prev->CopyTouch(prev_touch->id(),
                                           UFTouchStateUpdate));
      touches_array_.push_back(touch);
    } else if (prev_touch->state() == UFTouchStateUpdate) {
      touches_map_[prev_touch->id()] = touches_array_.size();
      touches_array_.push_back(prev_touch);
    }
  }

  const Value* value;
  value = new Value(static_cast<unsigned int>(touches_map_.size()));
  InsertProperty(UFFramePropertyNumTouches, value);
  value = new Value(static_cast<unsigned int>(touches_map_.size()));
  InsertProperty(UFFramePropertyActiveTouches, value);
}

UFTouch* UFFrame::CopyTouch(UFTouchId touchid, UFTouchState new_state) const {
  auto map_it = touches_map_.find(touchid);
  if (map_it == touches_map_.end()) {
    fprintf(stderr, "Failed to copy non-existent touch\n");
    return NULL;
  }

  return new UFTouch(*touches_array_[map_it->second].get(), new_state);
}

bool UFFrame::IsTouchOwned(UFTouchId touchid) {
  auto map_it = touches_map_.find(touchid);
  if (map_it == touches_map_.end())
    return false;

  SharedUFTouch& touch = touches_array_[map_it->second];

  int owned;
  UFStatus status = touch->GetProperty(UFTouchPropertyOwned, &owned);
  if (status != UFStatusSuccess)
    /* If the window server doesn't support touch ownership, then all touches
     * are implicitly owned. */
    return true;

  return owned;
}

namespace {

void CopyStartTime(const SharedUFTouch& src,
                   const SharedUFTouch& dst) {
  uint64_t start_time = 0;
  UFStatus status = src->GetProperty(UFTouchPropertyStartTime, &start_time);
  assert(status == UFStatusSuccess);
  const Value* value = new Value(start_time);
  dst->InsertProperty(UFTouchPropertyStartTime, value);
}

} // namespace

UFStatus UFFrame::GiveTouch(SharedUFTouch& touch) {
  auto map_it = touches_map_.find(touch->id());
  if (map_it != touches_map_.end()) {
    /* A loan is being returned */

    SharedUFTouch &our_touch = touches_array_[map_it->second];

    /* we shouldn't be referencing to any UFTouch here.
       If we are, it's a programming error by the API user. */
    if (our_touch.get() != NULL) {
      return UFStatusErrorTouchIdExists;
    }

    /* take back our UFtouch */
    our_touch.swap(touch);
  } else {
    touches_map_[touch->id()] = touches_array_.size();
    touches_array_.push_back(touch);
    touch.reset();
  }

  return UFStatusSuccess;
}

void UFFrame::UpdateTouch(const SharedUFTouch& touch) {
  auto map_it = touches_map_.find(touch->id());

  switch (touch->state()) {
    case UFTouchStateBegin:
      if (map_it != touches_map_.end()) {
        CopyStartTime(touches_array_[map_it->second], touch);
        touches_array_[map_it->second] = touch;
      } else {
        touches_map_[touch->id()] = touches_array_.size();
        touches_array_.push_back(touch);
      }
      break;

    case UFTouchStateUpdate: {
    case UFTouchStateEnd:
      if (map_it == touches_map_.end()) {
        fprintf(stderr, "Warning: ignoring update for unknown touch %ju\n",
                touch->id());
        break;
      }

      CopyStartTime(touches_array_[map_it->second], touch);
      touches_array_[map_it->second] = touch;
      break;
    }
  }
}

bool UFFrame::IsEnded() const {
  for (const SharedUFTouch& touch : touches_array_)
    if (touch->state() != UFTouchStateEnd)
      return false;

  return true;
}

UFStatus UFFrame::GetPreviousTouchProperty(const UFTouch* touch,
                                            UFTouchProperty property,
                                            void* value) const {
  if (!prev_)
    return UFStatusErrorInvalidTouch;

  auto it = prev_->touches_map_.find(touch->id());
  if (it == prev_->touches_map_.end())
    return UFStatusErrorInvalidTouch;

  return prev_->touches_array_[it->second]->GetProperty(property, value);
}

UFStatus UFFrame::GetPreviousTouchValue(const UFTouch* touch, UFAxisType type,
                                         float* value) const {
  if (!prev_)
    return UFStatusErrorInvalidTouch;

  auto it = prev_->touches_map_.find(touch->id());
  if (it == prev_->touches_map_.end())
    return UFStatusErrorInvalidTouch;

  return prev_->touches_array_[it->second]->GetValue(type, value);
}

UFStatus UFFrame::GetTouchByIndex(unsigned int index, ::UFTouch* touch) const {
  if (index >= touches_array_.size())
    return UFStatusErrorInvalidTouch;

  *touch = touches_array_[index].get();
  return UFStatusSuccess;
}

SharedUFTouch* UFFrame::GetSharedTouchById(UFTouchId touch_id) {
  auto it = touches_map_.find(touch_id);
  if (it == touches_map_.end())
    return nullptr;
  else
    return &(touches_array_[it->second]);
}

UFStatus UFFrame::GetTouchById(UFTouchId touch_id, ::UFTouch* touch) const {
  auto it = touches_map_.find(touch_id);
  if (it == touches_map_.end())
    return UFStatusErrorInvalidTouch;

  *touch = touches_array_[it->second].get();

  return UFStatusSuccess;
}

void UFFrame::ReleasePreviousFrame() {
  prev_.reset();
}

} // namespace frame
} // namespace oif

extern "C" {

FRAME_PUBLIC
UFStatus frame_frame_get_property_device_(UFFrame frame,
                                          UFFrameProperty property,
                                          UFDevice *value) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetProperty(property, value);
}

FRAME_PUBLIC
UFStatus frame_frame_get_property_uint64_(UFFrame frame,
                                          UFFrameProperty property,
                                          uint64_t *value) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetProperty(property, value);
}

FRAME_PUBLIC
UFStatus frame_frame_get_property_unsigned_int_(UFFrame frame,
                                                UFFrameProperty property,
                                                unsigned int *value) {
  const oif::frame::UFFrame* ufframe =
    static_cast<const oif::frame::UFFrame*>(frame);

  if (property == UFFramePropertyNumTouches) {
    *value = ufframe->GetNumTouches();
    return UFStatusSuccess;
  } else if (property == UFFramePropertyActiveTouches) {
    UFStatus status = ufframe->GetProperty(property, value);
    if (status != UFStatusSuccess) {
      *value = ufframe->GetNumTouches();
      status = UFStatusSuccess;
    }
    return status;
  } else {
    return ufframe->GetProperty(property, value);
  }
}

#undef frame_frame_get_property /* Override C11 generic selections macro */
FRAME_PUBLIC
UFStatus frame_frame_get_property(UFFrame frame, UFFrameProperty property,
                                  void *value) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);

  if (property == UFFramePropertyNumTouches) {
    *reinterpret_cast<unsigned int *>(value) = ufframe->GetNumTouches();
    return UFStatusSuccess;
  } else if (property == UFFramePropertyActiveTouches) {
    UFStatus status = ufframe->GetProperty(property, value);
    if (status != UFStatusSuccess) {
      *reinterpret_cast<unsigned int *>(value) = ufframe->GetNumTouches();
      status = UFStatusSuccess;
    }
    return status;
  } else {
    return ufframe->GetProperty(property, value);
  }
}

UFStatus frame_frame_get_touch_by_index(UFFrame frame, unsigned int index,
                                        UFTouch *touch) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetTouchByIndex(index, touch);
}

UFStatus frame_frame_get_touch_by_id(UFFrame frame, UFTouchId touch_id,
                                     UFTouch *touch) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetTouchById(touch_id, touch);
}

UFStatus frame_frame_get_previous_touch_property(UFFrame frame, UFTouch touch,
                                                 UFTouchProperty property,
                                                 void *value) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetPreviousTouchProperty(
      static_cast<const oif::frame::UFTouch*>(touch),
      property,
      value);
}

UFStatus frame_frame_get_previous_touch_value(UFFrame frame, UFTouch touch,
                                              UFAxisType type, float *value) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetPreviousTouchValue(
      static_cast<const oif::frame::UFTouch*>(touch),
      type,
      value);
}

UFWindowId frame_frame_get_window_id(UFFrame frame) {
  UFWindowId window_id;
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  UFStatus status = ufframe->GetProperty(UFFramePropertyWindowId, &window_id);
  assert(status == UFStatusSuccess);
  return window_id;
}

uint32_t frame_frame_get_num_touches(UFFrame frame) {
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  return ufframe->GetNumTouches();
}

UFDevice frame_frame_get_device(UFFrame frame) {
  UFDevice device;
  const oif::frame::UFFrame* ufframe =
      static_cast<const oif::frame::UFFrame*>(frame);
  UFStatus status = ufframe->GetProperty(UFFramePropertyDevice, &device);
  assert(status == UFStatusSuccess);
  return device;
}

UFBackendFrame frame_backend_frame_new()
{
  return new UFBackendFrame_(new oif::frame::UFFrame);
}

UFBackendFrame frame_backend_frame_create_next(UFBackendFrame frame)
{
  return new UFBackendFrame_(new oif::frame::UFFrame(frame->shared_ptr));
}

UFFrame frame_backend_frame_get_frame(UFBackendFrame frame)
{
  return frame->shared_ptr.get();
}

UFStatus frame_backend_frame_borrow_touch_by_id(UFBackendFrame frame,
                                                UFTouchId id,
                                                UFBackendTouch *touch)
{
  oif::frame::UFFrame* ufframe =
    static_cast<oif::frame::UFFrame*>(frame->shared_ptr.get());

  oif::frame::SharedUFTouch *shared_touch = ufframe->GetSharedTouchById(id);
  if (shared_touch) {
    *touch = new UFBackendTouch_(*shared_touch);
    return UFStatusSuccess;
  } else {
    return UFStatusErrorInvalidTouch;
  }
}

void frame_backend_frame_set_device(UFBackendFrame frame, UFBackendDevice device)
{
  static_cast<oif::frame::UFFrame*>(frame->shared_ptr.get())->
    InsertProperty(UFFramePropertyDevice,
                   new oif::frame::Value(device->shared_ptr));
}

void frame_backend_frame_set_window_id(UFBackendFrame frame, UFWindowId window_id)
{
  static_cast<oif::frame::UFFrame*>(frame->shared_ptr.get())->
    InsertProperty(UFFramePropertyWindowId,
                   new oif::frame::Value(window_id));
}

void frame_backend_frame_set_active_touches(UFBackendFrame frame, unsigned int active_touches)
{
  static_cast<oif::frame::UFFrame*>(frame->shared_ptr.get())->
    InsertProperty(UFFramePropertyActiveTouches,
                   new oif::frame::Value(active_touches));
}

UFStatus frame_backend_frame_give_touch(UFBackendFrame frame, UFBackendTouch *touch)
{
  UFStatus status;

  /* the touch backend must be carrying a UFtouch */
  assert((*touch)->shared_ptr);

  status = static_cast<oif::frame::UFFrame*>(frame->shared_ptr.get())->
    GiveTouch((*touch)->shared_ptr);

  /* frame must have taken the touch */
  assert(!(*touch)->shared_ptr);

  delete (*touch);
  *touch = nullptr;

  return status;
}

void frame_backend_frame_delete(UFBackendFrame frame)
{
  delete frame;
}

} // extern "C"
