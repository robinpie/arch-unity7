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

#include "handle.h"

#include <errno.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <stdexcept>

#include "event.h"

namespace oif {
namespace frame {

UFHandle::UFHandle() : event_fd_(-1), event_queue_() {
  event_fd_ = eventfd(0, EFD_NONBLOCK);
  if (event_fd_ == -1) {
    fprintf(stderr, "Error: failed to create eventfd instance\n");
    throw std::runtime_error("Failed to create eventfd instance");
  }
}

void UFHandle::EnqueueEvent(UFEvent* event) {
  static const uint64_t num = 1;

  event_queue_.push(event);
  if (write(event_fd_, &num, sizeof(num)) != sizeof(num))
    fprintf(stderr, "Warning: failed to update eventfd instance\n");
}

UFStatus UFHandle::GetEvent(::UFEvent* event) {
  /* Clear event fd (see eventfd(2) man page) */
  uint64_t buf;
  if (read(event_fd_, &buf, sizeof(buf)) != 8 && errno != EAGAIN)
    fprintf(stderr, "Warning: failed to read data from event fd\n");

  if (event_queue_.empty())
    return UFStatusErrorNoEvent;

  *event = event_queue_.front();
  event_queue_.pop();

  return UFStatusSuccess;
}

UFHandle::~UFHandle() {
  while (!event_queue_.empty()) {
    event_queue_.front()->Unref();
    event_queue_.pop();
  }
}

} // namespace frame
} // namespace oif

extern "C" {

int frame_get_fd(UFHandle handle) {
  return static_cast<const oif::frame::UFHandle*>(handle)->event_fd();
}

UFStatus frame_get_event(UFHandle handle, UFEvent *event) {
  return static_cast<oif::frame::UFHandle*>(handle)->GetEvent(event);
}

} // extern "C"
