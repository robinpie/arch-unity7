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

#ifndef FRAME_EVENT_H_
#define FRAME_EVENT_H_

#include <stdint.h>

#include "oif/frame.h"
#include "property.h"

struct UFEvent_ {
  virtual ~UFEvent_() {}
};

namespace oif {
namespace frame {

class Value;

class UFEvent : public UFEvent_, public Property<UFEventProperty> {
 public:
  UFEvent();
  UFEvent(UFEventType type, const Value* data, uint64_t time);
  ~UFEvent();

  void Ref();
  void Unref();

  UFEvent(const UFEvent&) = delete;
  UFEvent& operator=(const UFEvent&) = delete;

 private:
  unsigned int ref_count_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_EVENT_H
