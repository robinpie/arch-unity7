/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2010-2012 Canonical Ltd.
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

#include "subscription.h"

#include <cmath>

namespace oif {
namespace grail {

UGSubscription::UGSubscription()
    : device_(0),
      window_id_(0),
      mask_(0),
      touches_start_(2),
      touches_min_(0),
      touches_max_(0),
      drag_({300, 0.0026}), /* Units are in meters */
      pinch_({300, 1.1}), /* Different from grail v1, ratio instead of delta */
      rotate_({500, 2 * M_PI / 50}),
      tap_({300, 0.0026}),
      atomic_(false) {
}

bool UGSubscription::IsValid() const {
  if (!device_ ||
      !window_id_ ||
      mask_ == 0 ||
      (touches_max_ && touches_start_ > touches_max_) ||
      (touches_min_ && touches_start_ < touches_min_))
    return false;

  return true;
}


UGStatus UGSubscription::SetProperty(UGSubscriptionProperty property,
                                     const void* value) {
  switch (property) {
    case UGSubscriptionPropertyDevice: {
      UFDevice device = *reinterpret_cast<const UFDevice*>(value);
      if (!device)
        return UGStatusErrorInvalidValue;
      device_ = device;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyWindow: {
      UFWindowId window_id = *reinterpret_cast<const UFWindowId*>(value);
      if (!window_id)
        return UGStatusErrorInvalidValue;
      window_id_ = window_id;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyMask: {
      UGGestureTypeMask mask =
          *reinterpret_cast<const UGGestureTypeMask*>(value);
      if (mask == 0)
        return UGStatusErrorInvalidValue;
      mask_ = mask;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyTouchesStart: {
      unsigned int touches = *reinterpret_cast<const unsigned int*>(value);
      if (touches == 0 || touches > 5)
        return UGStatusErrorInvalidValue;
      touches_start_ = touches;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyTouchesMinimum: {
      unsigned int touches = *reinterpret_cast<const unsigned int*>(value);
      if (touches == 0 || touches > 5)
        return UGStatusErrorInvalidValue;
      touches_min_ = touches;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyTouchesMaximum: {
      unsigned int touches = *reinterpret_cast<const unsigned int*>(value);
      if (touches == 0 || touches > 5)
        return UGStatusErrorInvalidValue;
      touches_max_ = touches;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyDragTimeout: {
      drag_.timeout = *reinterpret_cast<const uint64_t*>(value);
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyDragThreshold: {
      float threshold = *reinterpret_cast<const float*>(value);
      if (threshold < 0)
        return UGStatusErrorInvalidValue;
      drag_.threshold = threshold;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyPinchTimeout: {
      pinch_.timeout = *reinterpret_cast<const uint64_t*>(value);
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyPinchThreshold: {
      float threshold = *reinterpret_cast<const float*>(value);
      if (threshold < 0)
        return UGStatusErrorInvalidValue;
      pinch_.threshold = threshold;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyRotateTimeout: {
      rotate_.timeout = *reinterpret_cast<const uint64_t*>(value);
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyRotateThreshold: {
      float threshold = *reinterpret_cast<const float*>(value);
      if (threshold < 0)
        return UGStatusErrorInvalidValue;
      rotate_.threshold = threshold;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyTapTimeout: {
      tap_.timeout = *reinterpret_cast<const uint64_t*>(value);
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyTapThreshold: {
      float threshold = *reinterpret_cast<const float*>(value);
      if (threshold < 0)
        return UGStatusErrorInvalidValue;
      tap_.threshold = threshold;
      return UGStatusSuccess;
    }

    case UGSubscriptionPropertyAtomicGestures: {
      atomic_ = *reinterpret_cast<const int*>(value);
      return UGStatusSuccess;
    }
  }

  return UGStatusErrorUnknownProperty;
}

UGStatus UGSubscription::GetProperty(UGSubscriptionProperty property,
                                     void* value) const {
  switch (property) {
    case UGSubscriptionPropertyDevice:
      *reinterpret_cast<UFDevice*>(value) = device_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyWindow:
      *reinterpret_cast<UFWindowId*>(value) = window_id_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyMask:
      *reinterpret_cast<UGGestureTypeMask*>(value) = mask_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyTouchesStart:
      *reinterpret_cast<unsigned int*>(value) = touches_start_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyTouchesMinimum:
      *reinterpret_cast<unsigned int*>(value) = touches_min_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyTouchesMaximum:
      *reinterpret_cast<unsigned int*>(value) = touches_max_;
      return UGStatusSuccess;

    case UGSubscriptionPropertyDragTimeout:
      *reinterpret_cast<uint64_t*>(value) = drag_.timeout;
      return UGStatusSuccess;

    case UGSubscriptionPropertyDragThreshold:
      *reinterpret_cast<float*>(value) = drag_.threshold;
      return UGStatusSuccess;

    case UGSubscriptionPropertyPinchTimeout:
      *reinterpret_cast<uint64_t*>(value) = pinch_.timeout;
      return UGStatusSuccess;

    case UGSubscriptionPropertyPinchThreshold:
      *reinterpret_cast<float*>(value) = pinch_.threshold;
      return UGStatusSuccess;

    case UGSubscriptionPropertyRotateTimeout:
      *reinterpret_cast<uint64_t*>(value) = rotate_.timeout;
      return UGStatusSuccess;

    case UGSubscriptionPropertyRotateThreshold:
      *reinterpret_cast<float*>(value) = rotate_.threshold;
      return UGStatusSuccess;

    case UGSubscriptionPropertyTapTimeout:
      *reinterpret_cast<uint64_t*>(value) = tap_.timeout;
      return UGStatusSuccess;

    case UGSubscriptionPropertyTapThreshold:
      *reinterpret_cast<float*>(value) = tap_.threshold;
      return UGStatusSuccess;

    case UGSubscriptionPropertyAtomicGestures:
      *reinterpret_cast<int*>(value) = atomic_;
      return UGStatusSuccess;
  }

  return UGStatusErrorUnknownProperty;
}

} // namespace grail
} // namespace oif

UGStatus grail_subscription_new(UGSubscription* subscription) {
  *subscription = new oif::grail::UGSubscription;
  return UGStatusSuccess;
}

void grail_subscription_delete(UGSubscription subscription) {
  delete (static_cast<oif::grail::UGSubscription*>(subscription));
}

UGStatus grail_subscription_set_property(UGSubscription subscription,
                                         UGSubscriptionProperty property,
                                         const void* value) {
  return (static_cast<oif::grail::UGSubscription*>(subscription)->SetProperty(
      property, value));
}

UGStatus grail_subscription_get_property(UGSubscription subscription,
                                         UGSubscriptionProperty property,
                                         void* value) {
  return (static_cast<oif::grail::UGSubscription*>(subscription)->GetProperty(
      property, value));
}
