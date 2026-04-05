#include "grail-fixture.h"

class SingleTouchGestureTest : public GrailTest
{
};

/*
 Regression test for bug https://bugs.launchpad.net/grail/+bug/1020315
 */
TEST_F(SingleTouchGestureTest, QuickTapEndsWithConstructionFinished)
{
  UGStatus status;
  fake_window_id = 321;
  uint64_t time = 1234;

  status = grail_new(&grail_handle);
  ASSERT_EQ(UGStatusSuccess, status);

  SendDeviceAddedEvent(time);

  UGSubscription subscription =
    CreateSubscription(1, UGGestureTypeTouch, fake_window_id);
  if (!subscription) return;

  time += 10;
  BeginTouch(1);

  time += 10;
  GiveTouchOwnership(1);

  // There should now be two grail events, corresponding to the frame
  // event that were sent, waiting to be processed.

  // Check first event. A gesture begin.
  UGEvent grail_event;
  status = grail_get_event(grail_handle, &grail_event);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(grail_event));

  UGSlice slice;
  status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGGestureStateBegin, grail_slice_get_state(slice));
  ASSERT_FALSE(grail_slice_get_construction_finished(slice));

  grail_event_unref(grail_event);

  // Check the second event. A gesture update.
  status = grail_get_event(grail_handle, &grail_event);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(grail_event));

  status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGGestureStateUpdate, grail_slice_get_state(slice));
  ASSERT_FALSE(grail_slice_get_construction_finished(slice));

  grail_event_unref(grail_event);

  // now end the touch

  time += 10;
  EndTouch(1);

  // An end event should come and its "construction finished" property should be true

  UGStatus get_event_status;
  do
  {
    get_event_status = grail_get_event(grail_handle, &grail_event);
    if (get_event_status != UGStatusSuccess)
      break;

    ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(grail_event));

    status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
    ASSERT_EQ(UGStatusSuccess, status);

    // We ignore any intermediate updates
    if (grail_slice_get_state(slice) == UGGestureStateUpdate)
      continue;

    ASSERT_EQ(UGGestureStateEnd, grail_slice_get_state(slice));
    ASSERT_TRUE(grail_slice_get_construction_finished(slice));

    grail_event_unref(grail_event);
  } while (get_event_status == UGStatusSuccess);
  ASSERT_EQ(UGStatusErrorNoEvent, get_event_status);

  grail_subscription_delete(subscription);
}
