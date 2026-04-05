#include "grail-fixture.h"

/*
  Checks that a gesture finishes its construction after enough time has passed.
  Even if no touch updates come within that time.

  In other words, checks that a grail_update_time() call can trigger
  the completion of the construction of a gesture.
 */
TEST_F(GrailTest, StillGestureFinishesConstruction)
{
  UGStatus status;
  UFWindowId fake_window_id = 321;
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
  GiveTouchOwnership(1); // ownership will trigger delivery of grail events.

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

  // a lot of time has passed
  time += 100;
  grail_update_time(grail_handle, time);

  // An update event should come and its "construction finished" property should be true

  status = grail_get_event(grail_handle, &grail_event);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGEventTypeSlice, grail_event_get_type(grail_event));

  status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
  ASSERT_EQ(UGStatusSuccess, status);

  ASSERT_EQ(UGGestureStateUpdate, grail_slice_get_state(slice));

  ASSERT_TRUE(grail_slice_get_construction_finished(slice));

  grail_event_unref(grail_event);

  grail_subscription_delete(subscription);
}
