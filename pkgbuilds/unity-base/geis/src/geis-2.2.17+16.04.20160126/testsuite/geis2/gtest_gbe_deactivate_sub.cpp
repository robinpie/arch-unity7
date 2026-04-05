#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Checks that a subscription deactivation completes successfully
 */

class SubscriptionDeactivation : public Geis2GrailBackendBase
{
 protected:
  SubscriptionDeactivation() : _subscription(nullptr) {}

  void SendXInput2Events();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventGestureEnd(GeisEvent event);

  GeisSubscription _subscription;
};

void SubscriptionDeactivation::SendXInput2Events()
{
  /* event type, touch id, X and Y  */
  SendTouchEvent(XI_TouchBegin, 1, 10.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 2, 20.0f, 10.0f);

  xmock_server_time += 5;

  /* touch id  */
  SendTouchOwnershipEvent(1);
  SendTouchOwnershipEvent(2);

  xmock_server_time += 5;

  SendTouchEvent(XI_TouchUpdate, 1, 10.0f, 20.0f);
  SendTouchEvent(XI_TouchUpdate, 2, 20.0f, 20.0f);

  xmock_server_time += 5;

  SendTouchEvent(XI_TouchEnd, 1, 10.0f, 30.0f);
  SendTouchEvent(XI_TouchEnd, 2, 20.0f, 30.0f);
}

void SubscriptionDeactivation::OnEventInitComplete(GeisEvent event)
{
  _subscription = CreateFilteredSubscription(
      "My 2-touches Touch", 2, GEIS_GESTURE_TOUCH);
  ASSERT_NE(nullptr, _subscription);

  SendXInput2Events();
}

void SubscriptionDeactivation::OnEventGestureEnd(GeisEvent event)
{
  AcceptGestureInEvent(event);

  GeisStatus status = geis_subscription_deactivate(_subscription);
  // The main point of this test.
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);
}

TEST_F(SubscriptionDeactivation, Test)
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
