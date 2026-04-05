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

#include "axis.h"

#include "value.h"

namespace oif {
namespace frame {

UFAxis::UFAxis(UFAxisType type, float min, float max, float resolution)
    : type_(type), min_(min), max_(max), resolution_(resolution) {
}

} // namespace frame
} // namespace oif

extern "C" {

UFAxisType frame_axis_get_type(UFAxis axis) {
  return static_cast<const oif::frame::UFAxis*>(axis)->type();
}

float frame_axis_get_minimum(UFAxis axis) {
  return static_cast<const oif::frame::UFAxis*>(axis)->min();
}

float frame_axis_get_maximum(UFAxis axis) {
  return static_cast<const oif::frame::UFAxis*>(axis)->max();
}

float frame_axis_get_resolution(UFAxis axis) {
  return static_cast<const oif::frame::UFAxis*>(axis)->resolution();
}

} // extern "C"
