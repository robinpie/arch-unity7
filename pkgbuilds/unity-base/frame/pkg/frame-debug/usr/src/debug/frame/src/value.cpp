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

#include "value.h"

#include <stdlib.h>
#include <string.h>

#include <stdexcept>

#include "device.h"
#include "frame.h"

namespace oif {
namespace frame {

Value::Value(bool value) : type_(kBool), bool_(value) {
}

Value::Value(int value) : type_(kInt), int_(value) {
}

Value::Value(unsigned int value) : type_(kUnsignedInt), unsigned_int_(value) {
}

Value::Value(float value) : type_(kFloat), float_(value) {
}

#ifdef HAVE_LONG_UNSIGNED_VALUE
Value::Value(long unsigned int value)
    : type_(kLongUnsignedInt),
      long_unsigned_int_(value) {
}
#endif // HAVE_LONG_UNSIGNED_VALUE

Value::Value(uint64_t value) : type_(kuint64_t), uint64_t_(value) {
}

Value::Value(const char* value) : type_(kString), string_(strdup(value)) {
}

Value::Value(const SharedUFDevice& device)
    : type_(kSharedDevice),
      device_(new SharedUFDevice(device)) {
}

Value::Value(const SharedUFFrame& frame)
    : type_(kSharedFrame),
      frame_(new SharedUFFrame(frame)) {
}

Value::Value(UFEventType value) : type_(kEventType), event_type_(value) {
}

Value::Value(UFTouchState value) : type_(kTouchState), touch_state_(value) {
}

Value::Value(const Value& value)
    : type_(value.type_),
      any_(value.any_) {
  switch (type_) {
    case kString:
      string_ = strdup(value.string_);
      break;

    case kSharedDevice:
      device_ = new SharedUFDevice(*value.device_);
      break;

    case kSharedFrame:
      frame_ = new SharedUFFrame(*value.frame_);
      break;

    default:
      break;
  }
}

void Value::GetValue(void* data) const {
  switch (type_) {
    case kBool:
      *reinterpret_cast<int*>(data) = bool_;
      break;

    case kInt:
      *reinterpret_cast<int*>(data) = int_;
      break;

    case kUnsignedInt:
      *reinterpret_cast<unsigned int*>(data) = unsigned_int_;
      break;

    case kFloat:
      *reinterpret_cast<float*>(data) = float_;
      break;

#ifdef HAVE_LONG_UNSIGNED_VALUE
    case kLongUnsignedInt:
      *reinterpret_cast<long unsigned int*>(data) = long_unsigned_int_;
      break;
#endif // HAVE_LONG_UNSIGNED_VALUE

    case kuint64_t:
      *reinterpret_cast<uint64_t*>(data) = uint64_t_;
      break;

    case kString:
      *reinterpret_cast<const char**>(data) = string_;
      break;

    case kSharedDevice:
      *reinterpret_cast< ::UFDevice*>(data) = device_->get();
      break;

    case kSharedFrame:
      *reinterpret_cast< ::UFFrame*>(data) = frame_->get();
      break;

    case kEventType:
      *reinterpret_cast<UFEventType*>(data) = event_type_;
      break;

    case kTouchState:
      *reinterpret_cast<UFTouchState*>(data) = touch_state_;
      break;
  }
}

void Value::GetValue(char** value) const {
  if (type_ == kString)
    *value = string_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(int* value) const {
  if (type_ == kInt)
    *value = int_;
  else if (type_ == kBool)
    *value = bool_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(unsigned int* value) const {
  if (type_ == kUnsignedInt)
    *value = unsigned_int_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(UFEventType* value) const {
  if (type_ == kEventType)
    *value = event_type_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(::UFDevice* value) const {
  if (type_ == kSharedDevice)
    *value = device_->get();
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(::UFFrame* value) const {
  if (type_ == kSharedFrame)
    *value = frame_->get();
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(uint64_t* value) const {
  if (type_ == kuint64_t)
    *value = uint64_t_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(UFTouchState* value) const {
  if (type_ == kTouchState)
    *value = touch_state_;
  else
    throw std::runtime_error("Bad property value type");
}

void Value::GetValue(float* value) const {
  if (type_ == kFloat)
    *value = float_;
  else
    throw std::runtime_error("Bad property value type");
}

Value::~Value() {
  switch (type_) {
    case kString:
      free(const_cast<char*>(string_));
      break;

    case kSharedDevice:
      delete device_;
      break;

    case kSharedFrame:
      delete frame_;
      break;

    default:
      break;
  }
}

} // namespace frame
} // namespace oif
