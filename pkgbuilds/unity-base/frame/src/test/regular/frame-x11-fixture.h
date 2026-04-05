#ifndef GTEST_FRAME_X11_FIXTURE_H
#define GTEST_FRAME_X11_FIXTURE_H

#include <gtest/gtest.h>
#include "oif/frame.h"
#include "oif/frame_x11.h"

class FrameX11Fixture : public ::testing::Test
{
 protected:
  FrameX11Fixture();

  virtual void SetUp();
  virtual void TearDown();

  void CreateXMockTouchScreenDevice();
  void DestroyXMockDevices();

  void SendTouchEvent(int event_type, int touch_id, float x, float y);
  void SendTouchOwnershipEvent(int touch_id);
  
  void FetchDeviceAddedEvent(UFDevice *device);
  void AssertNoMoreEvents();

  UFHandle frame_handle;
 private:
  /* holds the serial number to be used on the next synthetic XEvent */
  int _xevent_serial_number;
};

#endif
