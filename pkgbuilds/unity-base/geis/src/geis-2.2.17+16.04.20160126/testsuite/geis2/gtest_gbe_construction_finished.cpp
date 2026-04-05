#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Check that GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED
  is properly filled.
 */

class ConstructionFinishedProperty : public Geis2GrailBackendBase
{
 protected:
  ConstructionFinishedProperty() : _subscription(nullptr),
    _got_update_with_construction_finished(false) {}

  void SendFirstBatchOfXInput2Events();
  void SendSecondBatchOfXInput2Events();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventGestureBegin(GeisEvent event);
  virtual void OnEventGestureUpdate(GeisEvent event);

  GeisSubscription _subscription;
  bool _got_update_with_construction_finished;
};

void ConstructionFinishedProperty::SendFirstBatchOfXInput2Events()
{
  xmock_server_time = 0;

  SendTouchEvent(XI_TouchBegin, 0, 123.0f, 456.0f);
  SendTouchEvent(XI_TouchBegin, 1, 222.0f, 456.0f);

  xmock_server_time = 5;

  SendTouchOwnershipEvent(0);
  SendTouchOwnershipEvent(1);

  SendTouchEvent(XI_TouchUpdate, 0, 123.0f, 466.0f);
  SendTouchEvent(XI_TouchUpdate, 1, 222.0f, 466.0f);
}

void ConstructionFinishedProperty::SendSecondBatchOfXInput2Events()
{
  /* Go past composition time (i.e. time window where new touches can be grouped
     with existing ones to make new gestures) */
  xmock_server_time = 70;

  SendTouchEvent(XI_TouchUpdate, 0, 123.0f, 466.0f);
  SendTouchEvent(XI_TouchUpdate, 1, 222.0f, 466.0f);

  xmock_server_time = 80;

  SendTouchEvent(XI_TouchEnd, 0, 123.0f, 476.0f);
  SendTouchEvent(XI_TouchEnd, 1, 222.0f, 476.0f);
}

void ConstructionFinishedProperty::OnEventInitComplete(GeisEvent event)
{
  _subscription = CreateFilteredSubscription(
      "My 2-touches Touch", 2, GEIS_GESTURE_TOUCH);
  ASSERT_NE(nullptr, _subscription);
}

void ConstructionFinishedProperty::OnEventGestureBegin(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event,
      GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED);

  GeisBoolean construction_finished = geis_attr_value_to_boolean(attr);

  ASSERT_EQ(GEIS_FALSE, construction_finished);
}

void ConstructionFinishedProperty::OnEventGestureUpdate(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event,
      GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED);

  GeisBoolean construction_finished = geis_attr_value_to_boolean(attr);

  GeisInteger timestamp;
  GetGestureTimestampInEvent(&timestamp, event);

  if (timestamp < 60)
  {
    /* below composition time. Construction cannot be finished just yet */
    ASSERT_EQ(GEIS_FALSE, construction_finished);
  }
  else
  {
    /* we're past composition time and therefore grail will
       send a slice with "construction finished" set to true.
       geis must follow suit. */
    ASSERT_EQ(GEIS_TRUE, construction_finished);
    _got_update_with_construction_finished = true;
  }
}

TEST_F(ConstructionFinishedProperty, Test)
{
  CreateXMockTouchScreenDevice();

  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                       GEIS_INIT_NO_ATOMIC_GESTURES,
                       nullptr);
  ASSERT_NE(nullptr, _geis);

  /* geis emits event stating that its initialization has finished */
  Run();

  /* begin XInput touches */
  SendFirstBatchOfXInput2Events();

  /* geis should receive an XAlarm event. geis updates grail time to 60.
     grail touches timeout (they get beyond composition time) which
     causes construction to finish.
   */
  Run();

  /* end XInput touches  */
  SendSecondBatchOfXInput2Events();

  /* gesture ends  */
  Run();

  ASSERT_TRUE(_got_update_with_construction_finished);

  if (_subscription)
    geis_subscription_delete(_subscription);
  geis_delete(_geis);

  DestroyXMockDevices();
}
