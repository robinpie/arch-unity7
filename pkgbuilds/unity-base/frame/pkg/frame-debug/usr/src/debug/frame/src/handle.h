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

#ifndef FRAME_HANDLE_H_
#define FRAME_HANDLE_H_

#include <queue>

#include "oif/frame.h"

struct UFHandle_ {
  virtual ~UFHandle_() {}
};

namespace oif {
namespace frame {

class UFEvent;

class UFHandle : public UFHandle_ {
 public:
  UFHandle();
  ~UFHandle();

  int event_fd() const { return event_fd_; }
  UFStatus GetEvent(::UFEvent* event);
  void ReleaseEvent(UFEvent event);

 protected:
  void EnqueueEvent(UFEvent*);

  UFHandle(const UFHandle&) = delete;
  UFHandle& operator=(const UFHandle&) = delete;

 private:
  int event_fd_;
  std::queue<UFEvent*> event_queue_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_HANDLE_H_
