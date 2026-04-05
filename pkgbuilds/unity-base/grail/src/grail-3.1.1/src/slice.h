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

#ifndef GRAIL_SLICE_H_
#define GRAIL_SLICE_H_

#include <memory>
#include <set>

#include "oif/grail.h"
#include "forward.h"

struct UGSlice_ {};

namespace oif {
namespace grail {

class UGSlice : public UGSlice_ {
 public:
  /** @internal Create a new slice for a new gesture */
  UGSlice(Gesture& gesture, UFEvent event, const TouchMap& touches,
          UGGestureTypeMask recognized);
  /** @internal Create a new copy of an existing slice */
  UGSlice(const SharedUGSlice& prev, bool end = false);
  /** @internal Create a new slice by updating an existing slice */
  UGSlice(const SharedUGSlice& prev, Gesture &gesture,
          UFEvent event, const TouchMap& touches);

  ~UGSlice();

  void CheckGestureEnd();
  UGGestureTypeMask CheckRecognition(const Gesture& gesture);
  UGStatus GetTouchId(unsigned int index, UFTouchId* touch_id) const;
  UGStatus GetProperty(UGSliceProperty property, void* value) const;

  uint64_t time() const { return time_; }
  const TouchMap& touches() { return touches_; }
  const UFFrame& frame() { return frame_; }
  UGGestureState state() const { return state_; }
  bool physically_ended() const { return physically_ended_; }
  void set_state(UGGestureState state) { state_ = state; }
  bool construction_finished() const { return construction_finished_; }
  void set_construction_finished() { construction_finished_ = true; }

  UGSlice(const UGSlice&) = delete;
  UGSlice& operator=(const UGSlice&) = delete;

 private:
  unsigned int id_;
  const UFEvent event_;
  UFFrame frame_;
  TouchMap touches_;
  uint64_t time_;
  UGGestureState state_;
  bool physically_ended_;
  float original_center_x_;
  float original_center_y_;
  float original_radius_;
  float original_angle_;
  float radius_;
  float angle_;
  float transform_[3][3];
  float cumulative_transform_[3][3];
  float center_of_rotation_x_;
  float center_of_rotation_y_;
  UGGestureTypeMask recognized_;
  bool construction_finished_;
  bool touch_count_changed_;
  UGSubscription *subscription_;

  void GetValues(Gesture &gesture, const TouchMap& touches, float* x, float* y,
                 float* radius, float* angle, bool init);
  void SetTransforms(Gesture &gesture);
  void SetCenterOfRotation();
  float CumulativeDrag2(float device_x_res, float device_y_res) const;
  float CumulativePinch() const;
};

} // namespace grail
} // namespace oif

#endif // GRAIL_SLICE_H_
