/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <gtest/gtest.h>

#include "oif/frame_x11.h"

TEST(FrameTest, IDs) {
  Window x11_window_id = 0x892347;
  UFWindowId frame_window_id = frame_x11_create_window_id(x11_window_id);
  EXPECT_EQ(x11_window_id, frame_x11_get_window_id(frame_window_id));

  unsigned int x11_touch_id = 394;
  UFTouchId frame_touch_id = frame_x11_create_touch_id(x11_touch_id);
  EXPECT_EQ(x11_touch_id, frame_x11_get_touch_id(frame_touch_id));
}
