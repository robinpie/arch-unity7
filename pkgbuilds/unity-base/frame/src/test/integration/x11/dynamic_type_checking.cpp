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

/* This test checks for proper dynamic type checking in property getters.
 * frame_device_get_property() is used as the guinea pig. It is not exhaustive,
 * but should let us know if the machinery is working.
 *
 * This test is a big no-op if C generic selections are unavailable.
 */

#include <memory>

#include "x11/fixture.h"

using namespace oif::frame::x11::testing;

class DynamicTypeTest : public Test {
 public:
  DynamicTypeTest() : pass_(false) {}

 protected:
  virtual void ProcessFrameEvents();
  void CheckDevice(UFDevice device);

  bool pass_;
};

void DynamicTypeTest::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(handle(), &event)) == UFStatusSuccess) {
    switch (frame_event_get_type(event)) {
      case UFEventTypeDeviceAdded: {
        UFDevice device;
        ASSERT_EQ(UFStatusSuccess,
                  frame_event_get_property(event, UFEventPropertyDevice,
                                           &device));

        ASSERT_NE(nullptr, device);

        CheckDevice(device);
        break;
      }
      default:
        FAIL() << "Received spurious frame event";
        break;
    }

    frame_event_unref(event);
  }

  EXPECT_EQ(UFStatusErrorNoEvent, status);
}

void DynamicTypeTest::CheckDevice(UFDevice device) {
  char* string;
  int integer;
  unsigned int unsigned_integer;

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyName, &integer));

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyName,
                                      &unsigned_integer));

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyDirect, &string));

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyDirect,
                                      &unsigned_integer));

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyNumAxes,
                                      &string));

  EXPECT_EQ(UFStatusErrorInvalidType,
            frame_device_get_property(device, UFDevicePropertyNumAxes,
                                      &integer));

  pass_ = true;
}

TEST_F(DynamicTypeTest, Device) {
#ifdef __has_extension
#if __has_extension(c_generic_selections)

  xorg::testing::evemu::Device device("recordings/ntrig-dell-xt2.prop");
  
  PumpEvents();

  ASSERT_TRUE(pass_) << "Failed to receive device add event for NTrig touchscreen";

#endif // __has_extension(c_generic_selections)
#endif // __has_extension
}
