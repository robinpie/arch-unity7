/*****************************************************************************
 *
 * grail - Gesture Recognition And Instantiation Library
 *
 * Copyright (C) 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU General Public License as published
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

#ifndef GRAIL_TEST_FIXTURE_H_
#define GRAIL_TEST_FIXTURE_H_

/* GTest must be included before Xlib.h */
#include <xorg/gtest/xorg-gtest.h>

#include <X11/Xlib.h>
#include <X11/extensions/sync.h>

#include "oif/frame.h"
#include "oif/grail.h"

namespace oif {
namespace grail {
namespace x11 {
namespace testing {

class Test : public xorg::testing::Test {
 public:
  virtual void SetUp();
  virtual void TearDown();

 protected:
  void PumpEvents(uint64_t timeout = 1000);
  virtual bool FilterXIEvent(const XGenericEventCookie* xcookie);
  virtual void ProcessFrameEvents() {};
  virtual void ProcessGrailEvents() {};

  UFHandle frame_handle() const { return frame_handle_; }
  UGHandle grail_handle() const { return grail_handle_; }

 private:
  void UpdateTime(const XSyncAlarmNotifyEvent&);
  void SetX11Timeout();

  UFHandle frame_handle_;
  UGHandle grail_handle_;
  XSyncCounter server_time_counter_;
  XSyncAlarm alarm_;
};

} // namespace testing
} // namespace x11
} // namespace grail
} // namespace oif

#endif // GRAIL_TEST_FIXTURE_H_
