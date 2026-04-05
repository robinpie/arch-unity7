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

#include "handle.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <limits>
#include <stdexcept>

#include <oif/frame.h>

#include "atomic-recognizer.h"
#include "event.h"
#include "log.h"
#include "regular-recognizer.h"
#include "subscription.h"

namespace oif {
namespace grail {

UGHandle::UGHandle()
    : event_fd_(eventfd(0, EFD_NONBLOCK)),
      next_id_(0) {
  if (event_fd_ == -1) {
    throw std::runtime_error("Error: failed to create eventfd instance\n");
  }
}

Recognizer *UGHandle::CreateRecognizerForSubscription(
    UGSubscription* subscription) {
  Recognizer* recognizer;

  if (subscription->atomic())
    recognizer = new AtomicRecognizer(this,
        subscription->device(),
        subscription->window_id());
  else
    recognizer = new RegularRecognizer(this,
        subscription->device(),
        subscription->window_id());

  recognizers_[subscription->device()][subscription->window_id()] =
    std::move(UniqueRecognizer(recognizer));

  return recognizer;
}

UGStatus UGHandle::ActivateSubscription(UGSubscription* subscription) {
  Recognizer* recognizer;

  if (!subscription->IsValid())
    return UGStatusErrorInvalidSubscription;

  const auto& it =
      recognizers_[subscription->device()].find(subscription->window_id());
  if (it != recognizers_[subscription->device()].end()) {
    recognizer = it->second.get();

    if (recognizer->atomic() != subscription->atomic()
        && recognizer->num_subscriptions() == 0) {
      /* Fix the mismatch */
      recognizer = CreateRecognizerForSubscription(subscription);
    }

  } else {
    recognizer = CreateRecognizerForSubscription(subscription);
  }

  return recognizer->ActivateSubscription(subscription);
}

void UGHandle::DeactivateSubscription(UGSubscription* subscription) {
  recognizers_[subscription->device()].erase(subscription->window_id());
}

namespace {

bool GetDeviceFromEvent(const UFEvent event, UFDevice* device) {
  UFStatus status = frame_event_get_property(event, UFEventPropertyDevice,
                                             device);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get device from event\n";
    return false;
  }

  return true;
}

bool GetDeviceAndWindowFromEvent(const UFEvent event, UFDevice* device,
                                 UFWindowId* window_id) {
  UFFrame frame;
  UFStatus status = frame_event_get_property(event, UFEventPropertyFrame,
                                             &frame);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get frame from event\n";
    return false;
  }

  *device = frame_frame_get_device(frame);
  *window_id = frame_frame_get_window_id(frame);

  return true;
}

void RejectFrame(const UFEvent event, const UFDevice device,
                 UFWindowId window_id) {
  UFFrame frame;
  UFStatus status;
  unsigned int num_touches;
  unsigned int i;

  status = frame_event_get_property(event, UFEventPropertyFrame, &frame);
  if (status != UFStatusSuccess) {
    LOG(Warn) << "failed to get frame from event\n";
    return;
  }

  num_touches = frame_frame_get_num_touches(frame);

  for (i = 0; i < num_touches; ++i) {
    UFTouch touch;

    status = frame_frame_get_touch_by_index(frame, i, &touch);
    if (status != UFStatusSuccess) {
      LOG(Warn) << "failed to get touch from frame by index\n";
      continue;
    }

    if (frame_touch_get_state(touch) != UFTouchStateBegin)
      continue;

    frame_reject_touch(device, window_id, frame_touch_get_id(touch));
  }
}

} // namespace

void UGHandle::ProcessFrameEvent(const UFEvent event) {
  switch (frame_event_get_type(event)) {
    case UFEventTypeDeviceAdded: {
      UFDevice device;
      if (!GetDeviceFromEvent(event, &device))
        return;

      recognizers_.insert(
          std::make_pair(device, std::map<UFWindowId, UniqueRecognizer>()));
      break;
    }

    case UFEventTypeDeviceRemoved: {
      UFDevice device;
      if (!GetDeviceFromEvent(event, &device))
        return;
      recognizers_.erase(device);
      break;
    }

    case UFEventTypeFrame: {
      UFDevice device;
      UFWindowId window_id;
      if (!GetDeviceAndWindowFromEvent(event, &device, &window_id))
        return;

      if (recognizers_[device].find(window_id) != recognizers_[device].end())
        recognizers_[device][window_id]->ProcessFrameEvent(event);
      else
        RejectFrame(event, device, window_id);

      break;
    }
  }
}

void UGHandle::UpdateTime(uint64_t time) {
  LOG(Dbg) << "client updating time to " << time << "\n";
  for (auto& pair : recognizers_)
    for (auto& pair2 : pair.second)
      pair2.second->UpdateTime(time);
}

unsigned int UGHandle::NewGestureID(Recognizer* recognizer) {
  unsigned int id = next_id_++;
  gestures_[id] = recognizer;
  return id;
}

void UGHandle::EnqueueEvent(oif::grail::UGEvent* event) {
  static const uint64_t num = 1;

  event_queue_.push_back(event);
  if (write(event_fd_, &num, sizeof(num)) != sizeof(num))
    LOG(Warn) << "failed to update eventfd instance\n";
}

void UGHandle::RemoveGestureFromEventQueue(unsigned int id) {
  for (auto it = event_queue_.begin(); it != event_queue_.end(); ) {
    oif::grail::UGEvent* event = *it++;
    if (grail_event_get_type(event) != UGEventTypeSlice)
      continue;

    ::UGSlice slice;
    UGStatus status = grail_event_get_property(event, UGEventPropertySlice,
                                               &slice);
    MUST_SUCCEED(status);

    if (grail_slice_get_id(slice) == id) {
      event_queue_.remove(event);
      event->Unref();
    }
  }

  LOG(Dbg) << "removed gesture " << id << " events from queue\n";
}

UGStatus UGHandle::GetEvent(::UGEvent* event) {
  /* Clear event fd (see eventfd(2) man page) */
  uint64_t buf;
  if (read(event_fd_, &buf, sizeof(buf)) != 8 && errno != EAGAIN)
    LOG(Warn) << "failed to read data from event fd\n";

  if (event_queue_.empty())
    return UGStatusErrorNoEvent;

  *event = event_queue_.front();
  event_queue_.pop_front();

  return UGStatusSuccess;
}

uint64_t UGHandle::NextTimeout() const {
  uint64_t min_timeout = std::numeric_limits<uint64_t>::max();
  for (auto& pair : recognizers_) {
    for (auto& pair2 : pair.second) {
      uint64_t timeout = pair2.second->NextTimeout();
      if (timeout < min_timeout)
        min_timeout = timeout;
    }
  }

  return min_timeout != std::numeric_limits<uint64_t>::max() ? min_timeout : 0;
}

UGStatus UGHandle::AcceptGesture(unsigned int id) {
  auto it = gestures_.find(id);
  if (it == gestures_.end())
    return UGStatusErrorInvalidGesture;

  return it->second->AcceptGesture(id);
}

UGStatus UGHandle::RejectGesture(unsigned int id) {
  auto it = gestures_.find(id);
  if (it == gestures_.end())
    return UGStatusErrorInvalidGesture;

  LOG(Dbg) << "rejecting gesture " << id << " because of client request\n";
  return it->second->RejectGesture(id);
}

UGHandle::~UGHandle() {
  while (!event_queue_.empty()) {
    event_queue_.front()->Unref();
    event_queue_.pop_front();
  }
}

} // namespace grail
} // namespace oif

extern "C" {

UGStatus grail_new(UGHandle* handle) {
  try {
    *handle = new oif::grail::UGHandle;
  } catch(const std::exception&) {
    return UGStatusErrorResources;
  }

  return UGStatusSuccess;
}

void grail_delete(UGHandle handle) {
  delete static_cast<oif::grail::UGHandle*>(handle);
}

int grail_get_fd(UGHandle handle) {
  return static_cast<oif::grail::UGHandle*>(handle)->event_fd();
}

UGStatus grail_subscription_activate(UGHandle handle,
                                     const UGSubscription subscription) {
  return static_cast<oif::grail::UGHandle*>(handle)->ActivateSubscription(
      static_cast<oif::grail::UGSubscription*>(subscription));
}

void grail_subscription_deactivate(UGHandle handle,
                                   const UGSubscription subscription) {
  return static_cast<oif::grail::UGHandle*>(handle)->DeactivateSubscription(
      static_cast<oif::grail::UGSubscription*>(subscription));
}

void grail_process_frame_event(UGHandle handle, const UFEvent event) {
  try {
    static_cast<oif::grail::UGHandle*>(handle)->ProcessFrameEvent(event);
  } catch (const std::exception&) {
  }
}

UGStatus grail_get_event(UGHandle handle, UGEvent *event) {
  return static_cast<oif::grail::UGHandle*>(handle)->GetEvent(event);
}

void grail_update_time(UGHandle handle, uint64_t time) {
  static_cast<oif::grail::UGHandle*>(handle)->UpdateTime(time);
}

uint64_t grail_next_timeout(UGHandle handle) {
  return static_cast<oif::grail::UGHandle*>(handle)->NextTimeout();
}

UGStatus grail_accept_gesture(UGHandle handle, unsigned int id) {
  return static_cast<oif::grail::UGHandle*>(handle)->AcceptGesture(id);
}

UGStatus grail_reject_gesture(UGHandle handle, unsigned int id) {
  return static_cast<oif::grail::UGHandle*>(handle)->RejectGesture(id);
}

} // extern "C"
