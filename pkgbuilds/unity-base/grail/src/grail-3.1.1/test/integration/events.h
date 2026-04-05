/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU General Public License as published
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

#ifndef GRAIL_TEST_EVENTS_H_
#define GRAIL_TEST_EVENTS_H_

#include <map>
#include <vector>

#include "oif/grail.h"

namespace oif {
namespace grail {
namespace testing {

struct Slice {
  bool skip;
  unsigned int id;
  UGGestureState state;
  UGGestureTypeMask recognized;
  unsigned int num_touches;
  float original_center_x;
  float original_center_y;
  float original_radius;
  float transform[3][3];
  float cumulative_transform[3][3];
  float center_of_rotation_x;
  float center_of_rotation_y;
  bool construction_finished;
  UGSubscription subscription;
};

typedef std::vector<Slice> Events;

} // namespace testing
} // namespace grail
} // namespace oif

#endif // GRAIL_TEST_EVENTS_H_
