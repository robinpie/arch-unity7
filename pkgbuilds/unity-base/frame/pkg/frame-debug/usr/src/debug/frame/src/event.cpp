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

#include <assert.h>

#include "event.h"
#include "frame.h"
#include "device.h"

#include <oif/frame_backend.h>

namespace oif {
namespace frame {

UFEvent::UFEvent()
  : ref_count_(1) {
}

UFEvent::UFEvent(UFEventType type, const Value* data, uint64_t time)
    : ref_count_(1) {
  const Value* value;

  value = new Value(type);
  InsertProperty(UFEventPropertyType, value);

  switch (type) {
    case UFEventTypeDeviceAdded:
    case UFEventTypeDeviceRemoved:
      InsertProperty(UFEventPropertyDevice, data);
      break;

    case UFEventTypeFrame:
      InsertProperty(UFEventPropertyFrame, data);
      break;
  }

  value = new Value(time);
  InsertProperty(UFEventPropertyTime, value);
}

void UFEvent::Ref() {
  ++ref_count_;
}

void UFEvent::Unref() {
  --ref_count_;
  if (ref_count_ == 0)
    delete this;
}

UFEvent::~UFEvent() {
  ::UFFrame frame;

  if (GetProperty(UFEventPropertyFrame, &frame) == UFStatusSuccess)
    static_cast<oif::frame::UFFrame*>(frame)->ReleasePreviousFrame();
}

} // namespace frame
} // namespace oif

extern "C" {

UFEvent frame_event_new() {
  return new oif::frame::UFEvent;
}

void frame_event_ref(UFEvent event) {
  static_cast<oif::frame::UFEvent*>(event)->Ref();
}

void frame_event_unref(UFEvent event) {
  static_cast<oif::frame::UFEvent*>(event)->Unref();
}

FRAME_PUBLIC
UFStatus frame_event_get_property_type_(UFEvent event, UFEventProperty property,
                                        UFEventType *value) {
  return static_cast<const oif::frame::UFEvent*>(event)->GetProperty(
      property,
      value);
}

FRAME_PUBLIC
UFStatus frame_event_get_property_device_(UFEvent event,
                                          UFEventProperty property,
                                          UFDevice *value) {
  return static_cast<const oif::frame::UFEvent*>(event)->GetProperty(
      property,
      value);
}

FRAME_PUBLIC
UFStatus frame_event_get_property_frame_(UFEvent event,
                                         UFEventProperty property,
                                         UFFrame *value) {
  return static_cast<const oif::frame::UFEvent*>(event)->GetProperty(
      property,
      value);
}

FRAME_PUBLIC
UFStatus frame_event_get_property_uint64_(UFEvent event,
                                          UFEventProperty property,
                                          uint64_t *value) {
  return static_cast<const oif::frame::UFEvent*>(event)->GetProperty(
      property,
      value);
}

#undef frame_event_get_property /* Override C11 generic selections macro */
FRAME_PUBLIC
UFStatus frame_event_get_property(UFEvent event, UFEventProperty property,
                                  void *value) {
  return static_cast<const oif::frame::UFEvent*>(event)->GetProperty(
      property,
      value);
}

UFEventType frame_event_get_type(UFEvent event) {
  UFEventType type;
  const oif::frame::UFEvent* ufevent =
      static_cast<const oif::frame::UFEvent*>(event);
  UFStatus status = ufevent->GetProperty(UFEventPropertyType, &type);
  assert(status == UFStatusSuccess);
  return type;
}

uint64_t frame_event_get_time(UFEvent event) {
  uint64_t time;
  const oif::frame::UFEvent* ufevent =
      static_cast<const oif::frame::UFEvent*>(event);
  UFStatus status = ufevent->GetProperty(UFEventPropertyTime, &time);
  assert(status == UFStatusSuccess);
  return time;
}

void frame_event_set_type(UFEvent event, UFEventType type)
{
  static_cast<oif::frame::UFEvent*>(event)->
    InsertProperty(UFEventPropertyType, new oif::frame::Value(type));
}

void frame_event_set_device(UFEvent event, UFBackendDevice device)
{
  static_cast<oif::frame::UFEvent*>(event)->
    InsertProperty(UFEventPropertyDevice,
                   new oif::frame::Value(device->shared_ptr));
}

void frame_event_set_frame(UFEvent event, UFBackendFrame frame)
{
  static_cast<oif::frame::UFEvent*>(event)->
    InsertProperty(UFEventPropertyFrame,
                   new oif::frame::Value(frame->shared_ptr));
}

void frame_event_set_time(UFEvent event, uint64_t time)
{
  static_cast<oif::frame::UFEvent*>(event)->
    InsertProperty(UFEventPropertyTime, new oif::frame::Value(time));
}

} // extern "C"
