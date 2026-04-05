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

#ifndef GRAIL_LOG_H_
#define GRAIL_LOG_H_

#include <iostream>

namespace oif {
namespace grail {

class NullStreamBuf : public std::streambuf {
 protected:
  virtual int overflow(int c) { return traits_type::not_eof(c); }
};

class Logger {
 public:
  enum Level {
    Dbg = -1,
    Warn = 0,
    Err = 1,
  };

  Logger();

  static std::ostream& Log(Level level);

  static Logger& instance();

  Level level() const {return static_cast<Level>(level_);};

 private:
  int level_;
  NullStreamBuf null_buf_;
  std::ostream null_ostream_;
};

} // namespace grail
} // namespace oif

#define LOG(level) oif::grail::Logger::Log(oif::grail::Logger::level) \
    << "(" << __FILE__ << ":" << __func__ << ":" << __LINE__ << "): "

#endif // GRAIL_LOG_H_
