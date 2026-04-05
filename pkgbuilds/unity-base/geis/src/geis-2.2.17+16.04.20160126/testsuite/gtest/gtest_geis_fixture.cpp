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
#include "gtest_geis_fixture.h"

#include <stdexcept>
#include <sys/select.h>
#include <sys/time.h>
#include <X11/extensions/XInput2.h>


GTestGeisFixture::
GTestGeisFixture()
: geis_(nullptr),
  geis_is_initialized_(false),
  geis_dispatch_loop_timeout_(1),
  geis_event_handler_(nullptr),
  geis_event_loop_stop_(false)
{
}


void GTestGeisFixture::
SetUp()
{
  // Chain up the static class heirarchy.
  ASSERT_NO_FATAL_FAILURE(xorg::testing::Test::SetUp());

  // Verify that whatever X server is in use supports the required XInput
  // version.
  int xi_major = 2;
  int xi_minor = 2;
  ASSERT_EQ(Success, XIQueryVersion(Display(), &xi_major, &xi_minor));
  ASSERT_GE(xi_major, 2);
  ASSERT_GE(xi_minor, 2);

  setup_geis();
  EXPECT_TRUE(NULL != geis_) << "could not create GEIS instance";

  add_event_callback(geis_fd(),
                     std::bind(&GTestGeisFixture::geis_event_callback, this));
}

void GTestGeisFixture::
TearDown()
{
  geis_delete(geis_);
  xorg::testing::Test::TearDown();
}

void GTestGeisFixture::
add_event_callback(int fd, EventCallback event_callback)
{
  event_callbacks_[fd] = event_callback;
}

int GTestGeisFixture::
geis_fd()
{
  GeisInteger fd;
  geis_get_configuration(geis_, GEIS_CONFIGURATION_FD, &fd);
  return fd;
}


void GTestGeisFixture::
setup_geis()
{
  geis_ = geis_new(GEIS_INIT_SYNCHRONOUS_START,
                   GEIS_INIT_NO_ATOMIC_GESTURES,
                   NULL);
}


bool GTestGeisFixture::
geis_is_initialized() const
{
  return geis_is_initialized_;
}


void GTestGeisFixture::
pre_event_handler(GeisEvent event)
{
}


void GTestGeisFixture::
set_geis_event_handler(GeisEventHandler geis_event_handler)
{
  geis_event_handler_ = geis_event_handler;
}


void GTestGeisFixture::
post_event_handler(GeisEvent event)
{
  if (geis_event_type(event) == GEIS_EVENT_GESTURE_END)
  {
    geis_dispatch_stop(true);
  }
}


void GTestGeisFixture::
geis_dispatch_stop(bool do_stop)
{
  geis_event_loop_stop_ = do_stop;
}


void GTestGeisFixture::
geis_event_callback()
{
  ASSERT_NE(geis_event_handler_, nullptr) << "no geis event handler installed";

  GeisStatus dstatus = geis_dispatch_events(geis_);
  ASSERT_TRUE(dstatus == GEIS_STATUS_CONTINUE || dstatus == GEIS_STATUS_SUCCESS);

  GeisEvent  event;
  for (GeisStatus estatus = geis_next_event(geis_, &event);
       estatus == GEIS_STATUS_CONTINUE || estatus == GEIS_STATUS_SUCCESS;
       estatus = geis_next_event(geis_, &event))
  {
    if (geis_event_type(event) == GEIS_EVENT_INIT_COMPLETE)
    {
      geis_is_initialized_ = true;
    }

    pre_event_handler(event);
    geis_event_handler_(geis_, event);
    post_event_handler(event);
    geis_event_delete(event);
    if (geis_event_loop_stop_)
      return;
  }
}


void GTestGeisFixture::
geis_dispatch_loop()
{
  while (true)
  {
    if (geis_event_loop_stop_)
      break;

    int max_fd = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    for (auto h: event_callbacks_)
    {
      FD_SET(h.first, &read_fds);
      max_fd = std::max(max_fd, h.first);
    }

    timeval tmo = { geis_dispatch_loop_timeout_, 0 };
    int sstat = select(max_fd + 1, &read_fds, NULL, NULL, &tmo);
    ASSERT_GT(sstat, -1) << "error in select";
    if (sstat == 0)
      break;

    for (auto h: event_callbacks_)
    {
      if (FD_ISSET(h.first, &read_fds))
      {
        h.second();
      }
    }
  }
}


