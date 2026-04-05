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

#include "recording.h"

#include <stdexcept>

oif::evemu::Recording::Recording(const Device& device, const char* file)
    : device_(device),
      file_(fopen(file, "r")) {
  if (!file_)
    throw std::runtime_error("Failed to open evemu recording");
}

void oif::evemu::Recording::Play() const {
  rewind(file_);
  if (evemu_play(file_, device_.fd()) != 0)
    throw std::runtime_error("Failed to play evemu recording");
}

oif::evemu::Recording::~Recording() {
  fclose(file_);
}
