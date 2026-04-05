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

#include "window.h"

#include <assert.h>

#include "frame.h"

namespace oif {
namespace frame {

Window::Window() : current_frame_(new UFFrame) {
}

bool Window::IsTouchOwned(UFTouchId touchid) const {
  return current_frame_->IsTouchOwned(touchid);
}

void Window::ReleaseFrames() {
  current_frame_ = SharedUFFrame(new UFFrame);
}

bool Window::IsContextEnded() const {
  return current_frame_->IsEnded();
}

} // namespace frame
} // namespace oif
