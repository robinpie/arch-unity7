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

#ifndef FRAME_TYPEDEFS_H_
#define FRAME_TYPEDEFS_H_

namespace oif {
namespace frame {

class UFDevice;
class UFFrame;
class UFTouch;
class Window;

typedef std::shared_ptr<UFDevice> SharedUFDevice;
typedef std::shared_ptr<UFFrame> SharedUFFrame;
typedef std::shared_ptr<UFTouch> SharedUFTouch;
typedef std::shared_ptr<Window> SharedWindow;

} // namespace frame
} // namespace oif

#endif // FRAME_TYPEDEFS_H_
