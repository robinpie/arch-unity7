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

#ifndef FRAME_TEST_FIXTURE_H_
#define FRAME_TEST_FIXTURE_H_

#include <xorg/gtest/xorg-gtest.h>

#include "oif/frame.h"

namespace oif {
namespace frame {
namespace x11 {
namespace testing {

class Test : public xorg::testing::Test {
 public:
  Test() : handle_(NULL) {}

  virtual void SetUp();
  virtual void TearDown();

  Test(const Test&) = delete;
  Test& operator=(const Test&) = delete;

 protected:
  void PumpEvents(uint64_t timeout = 1000);
  virtual void ProcessFrameEvents() {};

  UFHandle handle() const { return handle_; }

 private:
  UFHandle handle_;
};

} // namespace testing
} // namespace x11
} // namespace frame
} // namespace oif

#endif // FRAME_TEST_FIXTURE_H_
