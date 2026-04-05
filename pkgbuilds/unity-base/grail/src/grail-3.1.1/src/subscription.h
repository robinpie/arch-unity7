/*****************************************************************************
 *
 * grail - Multitouch Gesture Recognition Library
 *
 * Copyright (C) 2011-2012,2016 Canonical Ltd.
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

#ifndef GRAIL_SUBSCRIPTION_H_
#define GRAIL_SUBSCRIPTION_H_

#include "oif/grail.h"
#include "forward.h"

struct UGSubscription_ {};

namespace oif {
namespace grail {

class UGSubscription : public UGSubscription_ {
 public:

  struct Limit {
    uint64_t timeout;
    float threshold;
  };

  UGSubscription();

  bool IsValid() const;
  UGStatus SetProperty(UGSubscriptionProperty property, const void* value);
  UGStatus GetProperty(UGSubscriptionProperty property, void* value) const;

  UFDevice device() const { return device_; }
  UFWindowId window_id() const { return window_id_; }
  UGGestureTypeMask mask() const { return mask_; }
  unsigned int touches_start() const { return touches_start_; }
  unsigned int touches_min() const { return touches_min_; }
  unsigned int touches_max() const { return touches_max_; }
  const Limit& drag() const { return drag_; }
  const Limit& pinch() const { return pinch_; }
  const Limit& rotate() const { return rotate_; }
  const Limit& tap() const { return tap_; }
  bool atomic() const { return atomic_; }

  UGSubscription(const UGSubscription&) = delete;
  UGSubscription& operator=(const UGSubscription&) = delete;

 private:
  UFDevice device_;
  UFWindowId window_id_;
  UGGestureTypeMask mask_;
  unsigned int touches_start_;
  unsigned int touches_min_;
  unsigned int touches_max_;
  Limit drag_;
  Limit pinch_;
  Limit rotate_;
  Limit tap_;
  bool atomic_;
};

} // namespace grail
} // namespace oif

#endif // GRAIL_SUBSCRIPTION_H_
