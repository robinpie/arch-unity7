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

#ifndef FRAME_PROPERTY_H_
#define FRAME_PROPERTY_H_

#include <map>

#include "value.h"

namespace oif {
namespace frame {

template<typename T>
class Property {
 public:
  Property() : properties_() {}
  virtual ~Property() {}

  explicit Property(const Property& property) : properties_() {
    for (const auto& pair : property.properties_) {
      T property = pair.first;
      Value* value = new Value(*pair.second);
      properties_[property] = std::move(UniqueValue(value));
    }
  }

  void InsertProperty(T property, const Value* value) {
    properties_.erase(property);
    properties_[property] = std::move(UniqueValue(value));
  }

  template<typename D>
  UFStatus GetProperty(T property, D* data) const {
    auto it = properties_.find(property);
    if (it == properties_.end())
      return UFStatusErrorUnknownProperty;

    it->second->GetValue(data);

    return UFStatusSuccess;
  }

  Property& operator=(const Property&) = delete;

 private:
  typedef std::unique_ptr<const Value> UniqueValue;

  std::map<T, UniqueValue> properties_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_PROPERTY_H_
