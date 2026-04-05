#include "grail-fixture.h"

/*
  Checks that once a touch whose end is pending gets finally owned its slices
  are sent to the client.

  Regression test for https://bugs.launchpad.net/grail/+bug/1026962
 */
TEST_F(GrailTest, PendingEndImpedesGestures)
{
  UGStatus status;
  fake_window_id = 321;

  status = grail_new(&grail_handle);
  ASSERT_EQ(UGStatusSuccess, status);

  SendDeviceAddedEvent(time);

  UGSubscription sub_3touch =
    CreateSubscription(3, UGGestureTypeTouch | UGGestureTypeDrag | UGGestureTypePinch,
                       fake_window_id);
  ASSERT_NE(nullptr, sub_3touch);

  time = 13688369;
  BeginTouchWindowCoords(1 /* touch id */, 30.0f /* x */, 0.0f /* y */);

  time = 13688369;
  BeginTouchWindowCoords(2, 40.0f, 0.0f);
  time = 13688372;
  GiveTouchOwnership(2);

  time = 13688369;
  BeginTouchWindowCoords(3, 50.0f, 0.0f);
  time = 13688373;
  GiveTouchOwnership(3);

  time = 13688430;
  grail_update_time(grail_handle, time);

  /* Perform a 4 fingers drag. */
  for (int i=1; i<=4; ++i)
  {
    time += 100;
    UFBackendFrame frame = frame_backend_frame_create_next(previous_frame_);
    frame_backend_frame_set_device(frame, device_);
    frame_backend_frame_set_window_id(frame, fake_window_id);
    SetTouchWindowCoords(frame, 1, 30.0f, i*10.0f);
    SetTouchWindowCoords(frame, 2, 40.0f, i*10.0f);
    SetTouchWindowCoords(frame, 3, 50.0f, i*10.0f);
    SendFrameEvent(time, frame);
  }

  time = 13688877;
  SendTouchPendingEnd(1);

  ProcessGrailEvents();
  // no gesture slices yet since not all touches are owned
  ASSERT_EQ(0, grail_gestures.size());

  time = 13688878;
  GiveTouchOwnership(1);

  EndTouch(1);

  ProcessGrailEvents();
  // Gesture slices should have been sent to the client by now
  ASSERT_EQ(1, grail_gestures.size());
  // Gesture must have ended since it cannot continue with only two touches.
  ASSERT_EQ(UGGestureStateEnd, grail_gestures.front().state);

  EndTouch(2);

  time = 13688880;
  EndTouch(3);

  ProcessGrailEvents();

  grail_subscription_delete(sub_3touch);
}
