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

#ifndef GRAIL_EVENT_H_
#define GRAIL_EVENT_H_

#include <cstdint>
#include <memory>

#include "oif/grail.h"
#include "forward.h"

struct UGEvent_ {};

namespace oif {
namespace grail {

class UGEvent : public UGEvent_ {
 public:
  UGEvent(const SharedUGSlice& slice);

  void Ref();
  void Unref();

  UGStatus GetProperty(UGEventProperty property, void* value) const;

  UGEvent(const UGEvent&) = delete;
  UGEvent& operator=(const UGEvent&) = delete;

 private:
  const SharedUGSlice slice_;
  const uint64_t time_;
  int referenceCount_;
};

} // namespace grail
} // namespace oif

#endif // GRAIL_EVENT_H
