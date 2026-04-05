#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Regression test for bug https://bugs.launchpad.net/geis/+bug/1015775

  Checks that the following situation doesn't lead to a crash:
    - Geis processed the end event of a grail gesture
    - Geis has a number of geis events pending delivery to application
    - Application accepts that ended grail gesture

  Cause:
    - While checking the queue of pending events for events that should
      no longer be delivered due to the gesture acceptance, geis would
      try to use a struct with information from the ended grail gesture
      that also no longer exists.
 */

class AcceptEndedGesture : public Geis2GrailBackendBase
{
 protected:
  AcceptEndedGesture() :  _subscription(nullptr) {}

  void SendXInput2Events();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventGestureBegin(GeisEvent event);

  GeisSubscription _subscription;
};

void AcceptEndedGesture::SendXInput2Events()
{
  /* args: event type, touch id, X and Y  */
  SendTouchEvent(XI_TouchBegin, 1, 10.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 2, 20.0f, 10.0f);

  xmock_server_time += 5;

  /* args: touch id  */
  SendTouchOwnershipEvent(1);
  SendTouchOwnershipEvent(2);

  xmock_server_time += 5;

  SendTouchEvent(XI_TouchUpdate, 1, 10.0f, 20.0f);
  SendTouchEvent(XI_TouchUpdate, 2, 20.0f, 20.0f);

  xmock_server_time += 5;

  SendTouchEvent(XI_TouchEnd, 1, 10.0f, 30.0f);
  SendTouchEvent(XI_TouchEnd, 2, 20.0f, 30.0f);
}

void AcceptEndedGesture::OnEventInitComplete(GeisEvent event)
{
  _subscription = CreateFilteredSubscription(
      "My 2-touches Touch", 2, GEIS_GESTURE_TOUCH);
  ASSERT_NE(nullptr, _subscription);

  SendXInput2Events();
}

void AcceptEndedGesture::OnEventGestureBegin(GeisEvent event)
{
  AcceptGestureInEvent(event);
}

TEST_F(AcceptEndedGesture, Test)
{
  CreateXMockTouchScreenDevice();

  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                       GEIS_INIT_NO_ATOMIC_GESTURES,
                       nullptr);
  ASSERT_NE(nullptr, _geis);

  Run();

  if (_subscription)
    geis_subscription_delete(_subscription);
  geis_delete(_geis);

  DestroyXMockDevices();
}
