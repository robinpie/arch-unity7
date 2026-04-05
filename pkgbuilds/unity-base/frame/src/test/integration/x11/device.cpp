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

#include <memory>
#include <stdexcept>

#include "x11/fixture.h"
#include "oif/frame_x11.h"

using namespace oif::frame::x11::testing;

class FrameDeviceTest : public Test {
 public:
  FrameDeviceTest() : pass_(false), device_(NULL) {}

  FrameDeviceTest(const FrameDeviceTest&) = delete;
  FrameDeviceTest& operator=(const FrameDeviceTest&) = delete;

 protected:
  virtual void ProcessFrameEvents();
  void CheckDevice(UFDevice device);

  bool pass_;
  UFDevice device_;
};

void FrameDeviceTest::ProcessFrameEvents() {
  UFEvent event;

  UFStatus status;
  while ((status = frame_get_event(handle(), &event)) == UFStatusSuccess) {
    switch (frame_event_get_type(event)) {
      case UFEventTypeDeviceRemoved:
      case UFEventTypeDeviceAdded: {
        UFDevice device;
        ASSERT_EQ(UFStatusSuccess,
                  frame_event_get_property(event, UFEventPropertyDevice,
                                           &device));

        ASSERT_NE(nullptr, device);

        if (frame_event_get_type(event) == UFEventTypeDeviceAdded)
          CheckDevice(device);
        else if (device == device_)
          pass_ = true;

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

void FrameDeviceTest::CheckDevice(UFDevice device) {
  char* name;
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_property(device, UFDevicePropertyName, &name));

  if (strcmp("N-Trig-MultiTouch-Virtual-Device", name)) {
    std::cout << "Skipping unknown device \"" << name << "\"\n";
    return;
  }

  EXPECT_EQ(nullptr, device_) << "Saw NTrig touchscreen virtual device twice";

  device_ = device;

  EXPECT_STREQ("N-Trig-MultiTouch-Virtual-Device", name);

  int direct;
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_property(device, UFDevicePropertyDirect, &direct));
  EXPECT_TRUE(direct);

  int independent;
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_property(device, UFDevicePropertyIndependent,
                                      &independent));
  EXPECT_FALSE(independent);

  int semi_mt;
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_property(device, UFDevicePropertySemiMT,
                                      &semi_mt));
  EXPECT_FALSE(semi_mt);

  int max_touches;
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_property(device, UFDevicePropertyMaxTouches,
                                      &max_touches));
  EXPECT_EQ(5, max_touches);

  int num_axes = frame_device_get_num_axes(device);
  EXPECT_EQ(5, num_axes);

  UFAxis axis;
  UFAxis by_type;

  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_index(device, 0, &axis));
  EXPECT_EQ(UFAxisTypeX, frame_axis_get_type(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_minimum(axis));
  EXPECT_FLOAT_EQ(9600, frame_axis_get_maximum(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_resolution(axis));
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_type(device, UFAxisTypeX, &by_type));
  EXPECT_EQ(axis, by_type);

  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_index(device, 1, &axis));
  EXPECT_EQ(UFAxisTypeY, frame_axis_get_type(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_minimum(axis));
  EXPECT_FLOAT_EQ(7200, frame_axis_get_maximum(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_resolution(axis));
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_type(device, UFAxisTypeY, &by_type));
  EXPECT_EQ(axis, by_type);

  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_index(device, 2, &axis));
  EXPECT_EQ(UFAxisTypeTouchMajor, frame_axis_get_type(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_minimum(axis));
  EXPECT_FLOAT_EQ(9600, frame_axis_get_maximum(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_resolution(axis));
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_type(device, UFAxisTypeTouchMajor,
                                          &by_type));
  EXPECT_EQ(axis, by_type);

  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_index(device, 3, &axis));
  EXPECT_EQ(UFAxisTypeTouchMinor, frame_axis_get_type(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_minimum(axis));
  EXPECT_FLOAT_EQ(7200, frame_axis_get_maximum(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_resolution(axis));
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_type(device, UFAxisTypeTouchMinor,
                                          &by_type));
  EXPECT_EQ(axis, by_type);

  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_index(device, 4, &axis));
  EXPECT_EQ(UFAxisTypeOrientation, frame_axis_get_type(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_minimum(axis));
  EXPECT_FLOAT_EQ(1, frame_axis_get_maximum(axis));
  EXPECT_FLOAT_EQ(0, frame_axis_get_resolution(axis));
  EXPECT_EQ(UFStatusSuccess,
            frame_device_get_axis_by_type(device, UFAxisTypeOrientation,
                                          &by_type));
  EXPECT_EQ(axis, by_type);

  EXPECT_EQ(UFStatusErrorInvalidAxis,
            frame_device_get_axis_by_index(device, 5, &axis));

  EXPECT_EQ(UFStatusErrorInvalidAxis,
            frame_device_get_axis_by_type(device, UFAxisTypeDistance, &axis));
}

TEST_F(FrameDeviceTest, Device) {
  xorg::testing::evemu::Device* device;
  
  device = new xorg::testing::evemu::Device("recordings/ntrig-dell-xt2.prop");

  std::unique_ptr<xorg::testing::evemu::Device> unique_device(device);

  PumpEvents();

  ASSERT_NE(nullptr, device_) << "Failed to receive device add event for NTrig "
                                 "touchscreen";

  unique_device.reset();

  PumpEvents();

  EXPECT_TRUE(pass_) << "Failed to receive device remove event for NTrig "
                        "touchscreen";
}
