#include "frame-x11-fixture.h"
#include "x11_mocks.h"

class X11TouchAcceptance : public FrameX11Fixture
{
};

/*
 Calls frame_accept_touch and check if the corresponding X11 call is made
 */
TEST_F(X11TouchAcceptance, Accept)
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

  UFWindowId frame_window_id = frame_x11_create_window_id(DefaultRootWindow(display));
  status = frame_accept_touch(device, frame_window_id, 1);
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

/*
 Calls frame_reject_touch and check if the corresponding X11 call is made
 */
TEST_F(X11TouchAcceptance, Reject)
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

  UFWindowId frame_window_id = frame_x11_create_window_id(DefaultRootWindow(display));
  status = frame_reject_touch(device, frame_window_id, 1);
  ASSERT_EQ(UFStatusSuccess, status);

  ASSERT_EQ(XIRejectTouch,
            xmock_get_touch_acceptance(xmock_devices[0].attachment /* device id */,
                                       1 /* touch id */,
                                       DefaultRootWindow(display)));

  frame_x11_delete(frame_handle);
  frame_handle = nullptr;

  XCloseDisplay(display);

  DestroyXMockDevices();
}
