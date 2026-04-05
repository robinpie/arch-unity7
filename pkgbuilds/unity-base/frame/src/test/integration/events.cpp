/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "events.h"

#include <stdexcept>

namespace oif {
namespace frame {
namespace testing {

const Value NewValue(UFTouchState state) {
  Value value;
  value.state = state;
  return value;
}

const Value NewValue(float floating) {
  Value value;
  value.floating = floating;
  return value;
}

const Value NewValue(int boolean) {
  Value value;
  value.boolean = boolean;
  return value;
}

bool IsEqual(const oif::frame::testing::Touch& a,
             const oif::frame::testing::Touch& b) {
  /* We rely on the fact that values are stored in maps, so if the values are
   * equal they will also be stored in the same order. */
  for (auto property_a = a.first.cbegin(), property_b = b.first.cbegin();
       property_a != a.first.cend() && property_b != b.first.cend();
       ++property_a, ++property_b) {
    if (property_a->first != property_b->first)
      return false;

    switch (property_a->first) {
      case UFTouchPropertyState:
        if (property_a->second.state != property_b->second.state)
          return false;
        break;
      case UFTouchPropertyWindowX:
      case UFTouchPropertyWindowY:
        if (property_a->second.floating != property_b->second.floating)
          return false;
        break;
      case UFTouchPropertyOwned:
      case UFTouchPropertyPendingEnd:
        if ((property_a->second.boolean && !property_b->second.boolean) ||
            (!property_a->second.boolean && property_b->second.boolean))
          return false;
        break;
      default:
        throw std::runtime_error("Unknown property to check for equality");
    }
  }

  for (auto value_a = a.second.cbegin(), value_b = b.second.cbegin();
       value_a != a.second.cend() && value_b != b.second.cend();
       ++value_a, ++value_b)
    if (value_a->first != value_b->first || value_a->second != value_b->second)
      return false;

  return true;
}

} // namespace testing
} // namespace frame
} // namespace oif
