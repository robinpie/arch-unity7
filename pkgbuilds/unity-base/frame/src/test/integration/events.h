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

#ifndef FRAME_TEST_EVENTS_H_
#define FRAME_TEST_EVENTS_H_

#include <map>
#include <vector>

#include "oif/frame.h"

namespace oif {
namespace frame {
namespace testing {

union Value {
  UFTouchState state;
  float floating;
  int boolean;
};

typedef std::pair<UFTouchProperty, Value> Property;
typedef std::map<UFTouchProperty, Value> Properties;
typedef std::pair<UFAxisType, float> AxisValue;
typedef std::map<UFAxisType, float> AxisValues;
typedef std::pair<Properties, AxisValues> Touch;
typedef std::vector<Touch> Touches;
typedef std::vector<Touches> Events;

const Value NewValue(UFTouchState state);
const Value NewValue(float floating);
const Value NewValue(int boolean);

bool IsEqual(const Touch& a, const Touch& b);

} // namespace testing
} // namespace frame
} // namespace oif

#endif // FRAME_TEST_EVENTS_H_
