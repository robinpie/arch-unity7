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

#ifndef GRAIL_FORWARD_H_
#define GRAIL_FORWARD_H_

#include <map>
#include <memory>
#include <set>
#include <sstream>

#include <oif/frame.h>

namespace oif {
namespace grail {

class UGEvent;
typedef std::shared_ptr<UGEvent> SharedUGEvent;

class Gesture;
typedef std::shared_ptr<Gesture> SharedGesture;

class UGHandle;
typedef std::shared_ptr<UGHandle> SharedUGHandle;

class Recognizer;
typedef std::shared_ptr<Recognizer> SharedRecognizer;

class Touch;
typedef std::shared_ptr<Touch> SharedTouch;

class UGSlice;
typedef std::shared_ptr<UGSlice> SharedUGSlice;

class UGSubscription;
typedef std::shared_ptr<UGSubscription> SharedUGSubscription;

typedef std::set<UFTouchId> TouchSet;

class TouchMap : public std::map<UFTouchId, SharedTouch> {

public:
  std::string ToString() const {
    std::ostringstream stream;
    auto it = begin();
    if (it != end())
      stream << (it++)->first;
    while (it != end())
      stream << ", " << (it++)->first;
    return stream.str();
  }

  bool Contains(const TouchMap &other_map) const {
    for (auto it : other_map) {
      if (find(it.first) == end())
        return false;
    }
    return true;
  }

  bool Equals(const TouchMap &other_map) const {
    if (size() == other_map.size())
      return Contains(other_map);
    else
      return false;
  }
};

} // namespace grail
} // namespace oif

#define MUST_SUCCEED(status)\
  if (status != UGStatusSuccess) {\
    fprintf(stderr, "Fatal failure at %s, %s:%d\n", __func__, __FILE__, __LINE__);\
    abort();\
  }

#endif // GRAIL_FORWARD_H_
