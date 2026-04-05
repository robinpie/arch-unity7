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

#ifndef FRAME_VALUE_H_
#define FRAME_VALUE_H_

#include <stdint.h>

#include <memory>

#include "oif/frame.h"
#include "typedefs.h"

#if __SIZEOF_LONG__ != __SIZEOF_LONG_LONG__
#define HAVE_LONG_UNSIGNED_VALUE
#endif

namespace oif {
namespace frame {

class Value {
 public:
  explicit Value(bool value);
  explicit Value(int value);
  explicit Value(unsigned int value);
  explicit Value(float value);
#ifdef HAVE_LONG_UNSIGNED_VALUE
  explicit Value(long unsigned int value);
#endif // HAVE_LONG_UNSIGNED_VALUE
  explicit Value(uint64_t value);
  explicit Value(const char* value);
  explicit Value(const SharedUFDevice& device);
  explicit Value(const SharedUFFrame& frame);
  explicit Value(UFEventType value);
  explicit Value(UFTouchState value);
  explicit Value(const Value& value);
  ~Value();

  void GetValue(void* data) const;
  void GetValue(char** data) const;
  void GetValue(int* data) const;
  void GetValue(unsigned int* data) const;
  void GetValue(UFEventType* data) const;
  void GetValue(::UFDevice* data) const;
  void GetValue(::UFFrame* data) const;
  void GetValue(uint64_t* data) const;
  void GetValue(UFTouchState* data) const;
  void GetValue(float* data) const;

  Value& operator=(const Value&) = delete;

 private:
  const enum {
    kBool,
    kInt,
    kUnsignedInt,
    kFloat,
#ifdef HAVE_LONG_UNSIGNED_VALUE
    kLongUnsignedInt,
#endif // HAVE_LONG_UNSIGNED_VALUE
    kuint64_t,
    kString,
    kSharedDevice,
    kSharedFrame,
    kEventType,
    kTouchState,
  } type_;

  union {
    const int bool_;
    const int int_;
    const unsigned int unsigned_int_;
    const float float_;
#ifdef HAVE_LONG_UNSIGNED_VALUE
    const long unsigned int long_unsigned_int_;
#endif // HAVE_LONG_UNSIGNED_VALUE
    const uint64_t uint64_t_;
    char* string_;
    SharedUFDevice* device_;
    SharedUFFrame* frame_;
    UFEventType event_type_;
    UFTouchState touch_state_;
    uint64_t const any_; /* Used to set any value */
  };
};

} // namespace frame
} // namespace oif

#endif // FRAME_VALUE_H_
