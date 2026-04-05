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

#ifndef GRAIL_TEST_RECORDING_H_
#define GRAIL_TEST_RECORDING_H_

#include <stdio.h>

extern "C" {

#include <evemu.h>

} // extern "C"

#include "device.h"

namespace oif {
namespace evemu {

class Recording {
 public:
  Recording(const Device& device, const char* file);
  ~Recording();

  void Play() const;
  
 private:
  const Device& device_;
  FILE* const file_;
};

} // namespace evemu
} // namespace oif

#endif // GRAIL_TEST_RECORDING_H_
