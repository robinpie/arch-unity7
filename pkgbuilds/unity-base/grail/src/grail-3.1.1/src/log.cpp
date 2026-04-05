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

#include "log.h"

#include <cstdlib>

namespace oif {
namespace grail {

namespace {

Logger logger;

} // namespace

Logger::Logger() : level_(0), null_ostream_(&null_buf_) {
  const char* string = getenv("GRAIL_DEBUG");
  if (string) {
    int tmp = atoi(string);
    if (tmp >= Dbg && tmp <= Err)
      level_ = tmp;
  }
}

std::ostream& Logger::Log(Level level) {
  if (level < logger.level_)
    return logger.null_ostream_;

  switch (level) {
    case Dbg:
      std::clog << "GRAIL DEBUG ";
      return std::clog;
    case Warn:
      std::clog << "GRAIL WARNING ";
      return std::clog;
    case Err:
      std::cerr << "GRAIL ERROR ";
      return std::cerr;
    default:
      return logger.null_ostream_;
  }
}

Logger& Logger::instance() {
  return logger;
}

} // namespace grail
} // namespace oif
