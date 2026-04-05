#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Checks that when a gesture gets accepted, all the gesture events from
  overlapping gestures are removed from the queue. That ensures the client
  won't get events about gestures that no longer exist in the backend
  (GRAIL), i.e., it won't get outdated, invalid, events.

  Regression test for https://bugs.launchpad.net/geis/+bug/1001365

  Steps:
   - Disable atomic gestures rules.
   - Subscribe for 2-touches Touch gestures.
   - Move 4 simultaneous touch points on a touchscreen. (feed all XInput2
     events in a single burst, so that the corresponding geis events are made
     and queued before processing by the geis client)
   - Accept two non-overlapping gestures already on Gesture Begin
   - Check that you get gesture updates only for the two accepted gestures.
 */

class OutdatedEventsOverlappingGestures : public Geis2GrailBackendBase
{
 protected:
  OutdatedEventsOverlappingGestures() : _subscription(nullptr) {}

  void SendXInput2Events();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventGestureBegin(GeisEvent event);
  virtual void OnEventGestureUpdate(GeisEvent event);

  int GetGestureIdInEvent(GeisEvent event);

  GeisSubscription _subscription;

  std::set<int> _accepted_gestures;
};

void OutdatedEventsOverlappingGestures::SendXInput2Events()
{
  /* event type, touch id, X and Y  */
  SendTouchEvent(XI_TouchBegin, 1, 10.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 2, 20.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 3, 30.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 4, 40.0f, 10.0f);

  xmock_server_time += 5;

  /* touch id  */
  SendTouchOwnershipEvent(1);
  SendTouchOwnershipEvent(2);
  SendTouchOwnershipEvent(3);
  SendTouchOwnershipEvent(4);

  xmock_server_time += 5;

  SendTouchEvent(XI_TouchUpdate, 1, 10.0f, 20.0f);
  SendTouchEvent(XI_TouchUpdate, 2, 20.0f, 20.0f);
  SendTouchEvent(XI_TouchUpdate, 3, 30.0f, 20.0f);
  SendTouchEvent(XI_TouchUpdate, 4, 40.0f, 20.0f);
}

void OutdatedEventsOverlappingGestures::OnEventInitComplete(GeisEvent event)
{
  _subscription = CreateFilteredSubscription(
      "My 2-touches Touch", 2, GEIS_GESTURE_TOUCH);
  ASSERT_NE(nullptr, _subscription);

  SendXInput2Events();
}

int OutdatedEventsOverlappingGestures::GetGestureIdInEvent(GeisEvent event)
{
  /* We expect only one group with one gesture frame.
     Multiple groups with several gesture frames is just not supported or used.
     That is, one GeisEvent is sent for each possible gesture instead of a single
     GeisEvent with all possible gestures (arranged in groups). */

  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet group_set =
    static_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));

  EXPECT_EQ(1, geis_groupset_group_count(group_set));

  GeisGroup group = geis_groupset_group(group_set, 0);

  EXPECT_EQ(1, geis_group_frame_count(group));

  GeisFrame frame = geis_group_frame(group, 0);

  return geis_frame_id(frame);
}

void OutdatedEventsOverlappingGestures::OnEventGestureBegin(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  TouchSet touch_set(static_cast<GeisTouchSet>(geis_attr_value_to_pointer(attr)));

  ASSERT_EQ(2, touch_set.size());

  /* I want to accept pairs from XI touch ids (1,2) and (3,4) */
  if ((touch_set.contains(10.0f, 10.0f) && touch_set.contains(20.0f, 10.0f))
      ||
      (touch_set.contains(30.0f, 10.0f) && touch_set.contains(40.0f, 10.0f)))
  {
    AcceptGestureInEvent(event);
    _accepted_gestures.insert(GetGestureIdInEvent(event));
  }
  else
  {
    RejectGestureInEvent(event);
  }
}

void OutdatedEventsOverlappingGestures::OnEventGestureUpdate(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  TouchSet touch_set(static_cast<GeisTouchSet>(geis_attr_value_to_pointer(attr)));

  ASSERT_EQ(2, touch_set.size());

  /* We expect updates only from the gestures that we have previously accepted */
  if (_accepted_gestures.find(GetGestureIdInEvent(event)) == _accepted_gestures.end())
  {
    /* It's neither (1,2) nor (3,4). */
    FAIL() << "Got an update from an unexpected touch pair";
  }
}

TEST_F(OutdatedEventsOverlappingGestures, Test)
{
  CreateXMockTouchScreenDevice();

  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                       GEIS_INIT_NO_ATOMIC_GESTURES,
                       nullptr);
  ASSERT_NE(nullptr, _geis);

  Run();

  ASSERT_EQ(2, _accepted_gestures.size());

  if (_subscription)
    geis_subscription_delete(_subscription);
  geis_delete(_geis);

  DestroyXMockDevices();
}
