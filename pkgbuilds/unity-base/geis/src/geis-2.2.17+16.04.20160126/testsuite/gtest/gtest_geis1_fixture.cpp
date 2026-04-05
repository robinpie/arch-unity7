/**
 * @file gtest_geis_fixture.cpp
 * @brief A GTest fixture for testing the full stack through GEIS.
 */
/*
 *  Copyright 2012 Canonical Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License version 3, as published 
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranties of 
 *  MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along 
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "gtest_geis1_fixture.h"

#include <unistd.h>
#include <X11/extensions/XInput2.h>


void GTestGeis1Fixture::
SetUp()
{
  // wait for things to settle
  usleep(1000000);

  // Chain up the static class heirarchy, as if the test framework was
  // designed by a Java trainee with some exposure to glib but has never used
  // C++.
  ASSERT_NO_FATAL_FAILURE(xorg::testing::Test::SetUp());

  // Verify that whatever X server is in use supports the required XInput
  // version.
  int xi_major = 2;
  int xi_minor = 2;
  ASSERT_EQ(Success, XIQueryVersion(Display(), &xi_major, &xi_minor));
  ASSERT_GE(xi_major, 2);
  ASSERT_GE(xi_minor, 2);

  uint32_t geis_window_id = static_cast<uint32_t>(DefaultRootWindow(Display()));
  GeisXcbWinInfo xcb_win_info = { NULL, NULL, geis_window_id };
  GeisWinInfo win_info = { GEIS_XCB_FULL_WINDOW, &xcb_win_info };

  ASSERT_EQ(GEIS_STATUS_SUCCESS, geis_init(&win_info, &geis_));

  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_configuration_get_value(geis_, GEIS_CONFIG_UNIX_FD,
                                         &geis_fd_));
}

void GTestGeis1Fixture::
TearDown()
{
  EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_finish(geis_));
  xorg::testing::Test::TearDown(); // NVI ftw.
  usleep(1000000);
}
