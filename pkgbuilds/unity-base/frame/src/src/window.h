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

#ifndef FRAME_WINDOW_H_
#define FRAME_WINDOW_H_

#include <map>
#include <memory>

#include "oif/frame.h"
#include "property.h"
#include "typedefs.h"

namespace oif{
namespace frame {

class Window {
 public:
  Window();
  virtual ~Window() {};

  bool IsTouchOwned(UFTouchId touchid) const;
  void ReleaseFrames();
  virtual bool IsContextEnded() const;

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

 protected:
  SharedUFFrame current_frame_;
};

} // namespace frame
} // namespace oif

#endif // FRAME_WINDOW_H_
