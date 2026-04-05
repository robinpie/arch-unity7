#include "grail-fixture.h"

class Intermittent3TouchTest : public GrailTest
{
};

/*
  Simulate the following situation:
     The user lays 3 fingers over a touchscreen or trackpad
     and the device momentarily (just for a couple of milliseconds)
     loses contact with one of the fingers.
  That will cause the touch for that finger to end and a new touch
  to be created for that same finger.

  Regression test for bug https://bugs.launchpad.net/grail/+bug/1023397
 */
TEST_F(Intermittent3TouchTest, Test)
{
  UGStatus status;
  fake_window_id = 321;

  status = grail_new(&grail_handle);
  ASSERT_EQ(UGStatusSuccess, status);

  SendDeviceAddedEvent(time);

  UGSubscription subscription =
    CreateSubscription(3, UGGestureTypeTouch, fake_window_id);
  if (!subscription) return;

  time = 20531903;
  BeginTouch(18 /*touch id*/);
  time = 20531905;
  GiveTouchOwnership(18);

  time = 20531909;
  BeginTouch(19);
  time = 20531910;
  GiveTouchOwnership(19);

  time = 20531909;
  BeginTouch(20);
  time = 20531912;
  GiveTouchOwnership(20);

  ProcessGrailEvents();
  /*
    Expected outcome:
     gesture(id=0, touches={18,19,20}) begins
  */
  ASSERT_EQ(1, grail_gestures.size());
  ASSERT_EQ(3, grail_gestures.front().touches.size());

  time = 20531961;
  EndTouch(18);

  time = 20531967;
  BeginTouch(21);
  time = 20531969;
  GiveTouchOwnership(21);

  ProcessGrailEvents();
  /*
   Expected outcome:
    gesture(id=0, touches={18,19,20}) ends
    gesture(id=1, touches={19,20,21}) begins
   Actual outcome for the bug found:
    gesture(id=0, touches={18,19,20}) ends
    gesture(id=1, touches={19,20,21}) begins
    gesture(id=2, touches={19,20,21}) begins
  */
  ASSERT_EQ(UGGestureStateEnd, GestureWithId(0)->state);
  ASSERT_EQ(2, grail_gestures.size());
  GrailGesture *gesture = GestureWithId(1);
  ASSERT_EQ(3, gesture->touches.size());
  ASSERT_TRUE(gesture->HasTouch(19));
  ASSERT_TRUE(gesture->HasTouch(20));
  ASSERT_TRUE(gesture->HasTouch(21));

  grail_subscription_delete(subscription);
}
