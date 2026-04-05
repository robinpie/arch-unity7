/*****************************************************************************
 *
 * grail - Multitouch Gesture Recognition Library
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

#include "event.h"

#include <cassert>

#include "slice.h"

namespace oif {
namespace grail {

UGEvent::UGEvent(const SharedUGSlice& slice)
    : slice_(slice),
      time_(slice->time()),
      referenceCount_(1) {
}

void UGEvent::Ref() {
  referenceCount_++;
}

void UGEvent::Unref() {
  referenceCount_--;
  if(referenceCount_ == 0)
    delete this;
}

UGStatus UGEvent::GetProperty(UGEventProperty property, void* data) const {
  switch (property) {
    case UGEventPropertyType:
      *reinterpret_cast<UGEventType*>(data) = UGEventTypeSlice;
      return UGStatusSuccess;

    case UGEventPropertySlice:
      *reinterpret_cast<UGSlice**>(data) = slice_.get();
      return UGStatusSuccess;

    case UGEventPropertyTime:
      *reinterpret_cast<uint64_t*>(data) = time_;
      return UGStatusSuccess;
  }

  return UGStatusErrorUnknownProperty;
}

} // namespace grail
} // namespace oif

extern "C" {

void grail_event_ref(UGEvent event) {
  static_cast<oif::grail::UGEvent*>(event)->Ref();
}

void grail_event_unref(UGEvent event) {
  static_cast<oif::grail::UGEvent*>(event)->Unref();
}

UGStatus grail_event_get_property(const UGEvent event, UGEventProperty property,
                                  void* data) {
  return static_cast<const oif::grail::UGEvent*>(event)->GetProperty(property,
                                                                        data);
}

UGEventType grail_event_get_type(const UGEvent event) {
  UGEventType type;
  UGStatus status =
      static_cast<const oif::grail::UGEvent*>(event)->GetProperty(
          UGEventPropertyType, &type);
  MUST_SUCCEED(status);
  return type;
}

uint64_t grail_event_get_time(const UGEvent event) {
  uint64_t time;
  UGStatus status =
      static_cast<const oif::grail::UGEvent*>(event)->GetProperty(
          UGEventPropertyTime, &time);
  MUST_SUCCEED(status);
  return time;
}

} // extern "C"
