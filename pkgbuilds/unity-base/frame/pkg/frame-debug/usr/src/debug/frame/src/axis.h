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

#ifndef FRAME_AXIS_H_
#define FRAME_AXIS_H_

#include "oif/frame.h"
#include "property.h"

struct UFAxis_ {
  virtual ~UFAxis_() {}
};

namespace oif {
namespace frame {

class UFAxis : public UFAxis_ {
 public:
  UFAxis(UFAxisType type, float min, float max, float resolution);

  UFAxisType type() const { return type_; }
  float min() const { return min_; }
  float max() const { return max_; }
  float resolution() const { return resolution_; }

  UFAxis(const UFAxis&) = delete;
  UFAxis& operator=(const UFAxis&) = delete;

 private:
  const UFAxisType type_;
  const float min_;
  const float max_;
  const float resolution_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_AXIS_H_
