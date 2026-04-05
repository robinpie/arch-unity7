#include "frame-x11-fixture.h"
#include "x11_mocks.h"

class AcceptEndedTouch : public FrameX11Fixture
{
};

/*
  Regression test for https://bugs.launchpad.net/bugs/1025297

  The bug is as follows:

  frame lib fails to forward the accept/reject command to xserver for an owned
  touch if it has already ended.

  Steps to reproduce the issue:
    1 - touch 1 begins and gets ownership
    2 - touch 1 ends
    3 - touch 2 begins and gets ownership
    4 - frame client accepts touch 1

  Expected outcome:
    frame lib accepts the touch on the XInput backend.

  Outcome of the bug:
    Nothing happens. frame lib never accepts that touch.
 */
TEST_F(AcceptEndedTouch, Test)
{
  UFStatus status;
  UFDevice device;

  xmock_server_time = 1234;

  CreateXMockTouchScreenDevice();

  Display *display = XOpenDisplay(NULL);

  status = frame_x11_new(display, &frame_handle);
  ASSERT_EQ(UFStatusSuccess, status);

  FetchDeviceAddedEvent(&device);
  AssertNoMoreEvents();

  SendTouchEvent(XI_TouchBegin, 1, 10.0f, 10.0f);
  SendTouchOwnershipEvent(1);

  xmock_server_time += 10;

  SendTouchEvent(XI_TouchEnd, 1, 10.0f, 20.0f);

  xmock_server_time += 3;

  SendTouchEvent(XI_TouchBegin, 2, 10.0f, 21.0f);
  SendTouchOwnershipEvent(2);

  UFWindowId frame_window_id = frame_x11_create_window_id(DefaultRootWindow(display));
  status = frame_x11_accept_touch(device, frame_window_id, 1);
  ASSERT_EQ(UFStatusSuccess, status);

  ASSERT_EQ(XIAcceptTouch,
            xmock_get_touch_acceptance(xmock_devices[0].attachment /* device id */,
                                       1 /* touch id */,
                                       DefaultRootWindow(display)));

  frame_x11_delete(frame_handle);
  frame_handle = nullptr;

  XCloseDisplay(display);

  DestroyXMockDevices();
}
