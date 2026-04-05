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

#include "touch.h"

#include <assert.h>

#include "frame.h"

#include <oif/frame_backend.h>

oif::frame::UFTouch* UFBackendTouch_::GetModifiableTouch() {
  if (shared_ptr.unique()) {
    return static_cast<oif::frame::UFTouch*>(shared_ptr.get());
  } else {
    /* Make a hard-copy. We don't want other holders of that UFTouch (like frames
       from previous but still existing events) to get the changes about to be
       made through this UFBackendTouch. */
    oif::frame::UFTouch *old_touch =
      static_cast<oif::frame::UFTouch*>(shared_ptr.get());
    oif::frame::UFTouch *new_touch = new oif::frame::UFTouch(*old_touch);
    shared_ptr.reset(new_touch);
    return new_touch;
  }
}

namespace oif {
namespace frame {

UFTouch::UFTouch()
  : state_(UFTouchStateBegin) {
  const Value* value;

  value = new Value(state_);
  InsertProperty(UFTouchPropertyState, value);
}

UFTouch::UFTouch(UFTouchState state, UFTouchId id, float x, float y,
                 uint64_t time)
    : id_(id),
      state_(state),
      values_() {
  const Value* value;

  value = new Value(state);
  InsertProperty(UFTouchPropertyState, value);

  value = new Value(id);
  InsertProperty(UFTouchPropertyId, value);

  value = new Value(x);
  InsertProperty(UFTouchPropertyWindowX, value);

  value = new Value(y);
  InsertProperty(UFTouchPropertyWindowY, value);

  value = new Value(time);
  InsertProperty(UFTouchPropertyTime, value);

  if (state == UFTouchStateBegin) {
    value = new Value(time);
    InsertProperty(UFTouchPropertyStartTime, value);
  }
}

UFTouch::UFTouch(const UFTouch& touch, UFTouchState new_state)
    : Property(touch),
      id_(touch.id_),
      state_(new_state),
      values_(touch.values_) {
  const Value* value = new Value(new_state);
  InsertProperty(UFTouchPropertyState, value);
}

void UFTouch::SetValue(UFAxisType type, float value) {
  values_[type] = value;
}

UFStatus UFTouch::GetValue(UFAxisType type, float* value) const {
  auto it = values_.find(type);
  if (it == values_.end())
    return UFStatusErrorInvalidAxis;

  *value = it->second;

  return UFStatusSuccess;
}

} // namespace frame
} // namespace oif

extern "C" {

FRAME_PUBLIC
UFStatus frame_touch_get_property_uint64_(UFTouch touch,
                                          UFTouchProperty property,
                                          uint64_t *value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetProperty(property, value);
}

FRAME_PUBLIC
UFStatus frame_touch_get_property_state_(UFTouch touch,
                                         UFTouchProperty property,
                                         UFTouchState *value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetProperty(property, value);
}

FRAME_PUBLIC
UFStatus frame_touch_get_property_float_(UFTouch touch,
                                         UFTouchProperty property,
                                         float *value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetProperty(property, value);
}

FRAME_PUBLIC
UFStatus frame_touch_get_property_int_(UFTouch touch, UFTouchProperty property,
                                       int *value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetProperty(property, value);
}

#undef frame_touch_get_property /* Override C11 generic selections macro */
FRAME_PUBLIC
UFStatus frame_touch_get_property(UFTouch touch, UFTouchProperty property,
                                  void* value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetProperty(property, value);
}

UFStatus frame_touch_get_value(UFTouch touch, UFAxisType type, float* value) {
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  return uftouch->GetValue(type, value);
}

UFTouchId frame_touch_get_id(UFTouch touch) {
  UFTouchId touch_id;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyId, &touch_id);
  assert(status == UFStatusSuccess);
  return touch_id;
}

UFTouchState frame_touch_get_state(UFTouch touch) {
  UFTouchState state;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyState, &state);
  assert(status == UFStatusSuccess);
  return state;
}

float frame_touch_get_window_x(UFTouch touch) {
  float x;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyWindowX, &x);
  assert(status == UFStatusSuccess);
  return x;
}

float frame_touch_get_window_y(UFTouch touch) {
  float y;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyWindowY, &y);
  assert(status == UFStatusSuccess);
  return y;
}

float frame_touch_get_device_x(UFTouch touch) {
  float x;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetValue(UFAxisTypeX, &x);
  assert(status == UFStatusSuccess);
  return x;
}

float frame_touch_get_device_y(UFTouch touch) {
  float y;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetValue(UFAxisTypeY, &y);
  assert(status == UFStatusSuccess);
  return y;
}

uint64_t frame_touch_get_time(UFTouch touch) {
  uint64_t time;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyTime, &time);
  assert(status == UFStatusSuccess);
  return time;
}

uint64_t frame_touch_get_start_time(UFTouch touch) {
  uint64_t start_time;
  const oif::frame::UFTouch* uftouch =
      static_cast<const oif::frame::UFTouch*>(touch);
  UFStatus status = uftouch->GetProperty(UFTouchPropertyStartTime, &start_time);
  assert(status == UFStatusSuccess);
  return start_time;
}

UFBackendTouch frame_backend_touch_new()
{
  return new UFBackendTouch_(new oif::frame::UFTouch);
}

UFTouch frame_backend_touch_get_touch(UFBackendTouch touch)
{
  return touch->shared_ptr.get();
}

void frame_backend_touch_set_id(UFBackendTouch touch_backend, UFTouchId id)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyId, new oif::frame::Value(id));
  touch->SetId(id);
}

void frame_backend_touch_set_ended(UFBackendTouch touch_backend)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyState, new oif::frame::Value(UFTouchStateEnd));
  touch->SetState(UFTouchStateEnd);
}

void frame_backend_touch_set_window_pos(UFBackendTouch touch_backend, float x, float y)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyWindowX, new oif::frame::Value(x));
  touch->InsertProperty(UFTouchPropertyWindowY, new oif::frame::Value(y));
}

void frame_backend_touch_set_time(UFBackendTouch touch_backend, uint64_t time)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyTime, new oif::frame::Value(time));
}

void frame_backend_touch_set_start_time(UFBackendTouch touch_backend,
                                        uint64_t start_time)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyStartTime, new oif::frame::Value(start_time));
}

void frame_backend_touch_set_owned(UFBackendTouch touch_backend, int owned)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyOwned, new oif::frame::Value(owned));
}

void frame_backend_touch_set_pending_end(UFBackendTouch touch_backend, int pending_end)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->InsertProperty(UFTouchPropertyPendingEnd, new oif::frame::Value(pending_end));
}

void frame_backend_touch_set_value(UFBackendTouch touch_backend,
                                   UFAxisType type, float value)
{
  oif::frame::UFTouch *touch = touch_backend->GetModifiableTouch();

  touch->SetValue(type, value);
}

} // extern "C"
