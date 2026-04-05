#include "grail-fixture.h"

void GrailTest::SendFrameEvent(uint64_t time, UFBackendFrame frame)
{
  UFEvent event = frame_event_new();
  frame_event_set_type(event, UFEventTypeFrame);
  frame_event_set_frame(event, frame);
  frame_event_set_time(event, time);

  grail_process_frame_event(grail_handle, event);
  frame_event_unref(event);
  event = nullptr;

  if (previous_frame_)
    frame_backend_frame_delete(previous_frame_);
  previous_frame_ = frame;
}

void GrailTest::SendDeviceAddedEvent(uint64_t time)
{
  UFEvent event = frame_event_new();
  frame_event_set_type(event, UFEventTypeDeviceAdded);
  frame_event_set_time(event, time);
  frame_event_set_device(event, device_);

  grail_process_frame_event(grail_handle, event);
  frame_event_unref(event);
  event = nullptr;
}

void GrailTest::CreateFakeDevice()
{
  device_ = frame_backend_device_new();

  frame_backend_device_add_axis(device_,
                                UFAxisTypeX,
                                0, 10000, 3764.70);

  frame_backend_device_add_axis(device_,
                                UFAxisTypeY,
                                0, 10000, 3764.70);

  frame_backend_device_set_direct(device_, 1);

  /* pixels/m */
  frame_backend_device_set_window_resolution(device_, 3764.70, 3764.70);
}

UGSubscription GrailTest::CreateSubscription(
    unsigned int num_touches, UGGestureTypeMask gesture_mask,
    UFWindowId window_id)
{
  UGStatus status;
  UGSubscription subscription = nullptr;
  UFDevice uf_device = frame_backend_device_get_device(device_);

  status = grail_subscription_new(&subscription);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to create subscription";
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyDevice,
                                           &uf_device);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set device subscription";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyWindow,
                                           &window_id);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set subscription window";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesStart,
                                           &num_touches);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set subscription start touches";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMaximum,
                                           &num_touches);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set subscription start touches";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
                                           UGSubscriptionPropertyTouchesMinimum,
                                           &num_touches);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set subscription min touches";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_set_property(subscription,
      UGSubscriptionPropertyMask, &gesture_mask);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to set subscription mask";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  status = grail_subscription_activate(grail_handle, subscription);
  if (status != UGStatusSuccess)
  {
    ADD_FAILURE() << "failed to activate subscription";
    grail_subscription_delete(subscription);
    return nullptr;
  }

  return subscription;
}

void GrailTest::BeginTouch(int touch_id)
{
  BeginTouchWindowCoords(touch_id, touch_id * 10.0f, 0.0f);
}

namespace
{
void IncrementFrameActiveTouches(UFBackendFrame frame_backend)
{
  UFFrame frame = frame_backend_frame_get_frame(frame_backend);
  unsigned int active_touches;
  frame_frame_get_property(frame, UFFramePropertyActiveTouches, &active_touches);
  frame_backend_frame_set_active_touches(frame_backend, active_touches + 1);
}

} // anonymous namespace

void GrailTest::BeginTouchWindowCoords(int touch_id, float window_x, float window_y)
{
  UFBackendTouch touch = frame_backend_touch_new();
  frame_backend_touch_set_id(touch, touch_id);
  frame_backend_touch_set_time(touch, time);
  frame_backend_touch_set_start_time(touch, time);
  frame_backend_touch_set_window_pos(touch, window_x, window_y);
  frame_backend_touch_set_owned(touch, 0);
  frame_backend_touch_set_pending_end(touch, 0);

  UFBackendFrame frame;
  if (previous_frame_)
    frame = frame_backend_frame_create_next(previous_frame_);
  else
    frame = frame_backend_frame_new();

  frame_backend_frame_set_device(frame, device_);

  frame_backend_frame_give_touch(frame, &touch);
  IncrementFrameActiveTouches(frame);
  frame_backend_frame_set_window_id(frame, fake_window_id);

  SendFrameEvent(time, frame);
}

void GrailTest::GiveTouchOwnership(int touch_id)
{
  std::function<void(UFBackendTouch)> update_func =
    [](UFBackendTouch touch)
    {
      frame_backend_touch_set_owned(touch, 1);
    };

  UpdateTouch(touch_id, update_func);
}

void GrailTest::UpdateTouch(int touch_id,
                            std::function< void(UFBackendTouch) >& update_func)
{
  assert(previous_frame_); // error would be in the test itself

  UFBackendFrame frame = frame_backend_frame_create_next(previous_frame_);
  frame_backend_frame_set_device(frame, device_);
  frame_backend_frame_set_window_id(frame, fake_window_id);

  UFStatus status;
  UFBackendTouch touch;
  status = frame_backend_frame_borrow_touch_by_id(frame, touch_id, &touch);
  ASSERT_EQ(UFStatusSuccess, status);

  update_func(touch);
  frame_backend_touch_set_time(touch, time);

  status = frame_backend_frame_give_touch(frame, &touch);
  ASSERT_EQ(UFStatusSuccess, status);

  SendFrameEvent(time, frame);
}

void GrailTest::SetTouchWindowCoords(UFBackendFrame frame,
                                     int touch_id, float window_x, float window_y)
{
  UFStatus status;
  UFBackendTouch touch;
  status = frame_backend_frame_borrow_touch_by_id(frame, touch_id, &touch);
  ASSERT_EQ(UFStatusSuccess, status);

  frame_backend_touch_set_window_pos(touch, window_x, window_y);

  status = frame_backend_frame_give_touch(frame, &touch);
  ASSERT_EQ(UFStatusSuccess, status);
}

void GrailTest::SendTouchPendingEnd(int touch_id)
{
  std::function<void(UFBackendTouch)> update_func =
    [](UFBackendTouch touch)
    {
      frame_backend_touch_set_pending_end(touch, 1);
    };

  UpdateTouch(touch_id, update_func);
}

void GrailTest::EndTouch(int touch_id)
{
  std::function<void(UFBackendTouch)> update_func =
    [](UFBackendTouch touch)
    {
      frame_backend_touch_set_ended(touch);
    };

  UpdateTouch(touch_id, update_func);
}

void GrailTest::ProcessGrailEvents()
{
  UGEvent grail_event;
  UGStatus get_event_status;

  do
  {
    get_event_status = grail_get_event(grail_handle, &grail_event);
    if (get_event_status != UGStatusSuccess)
      continue;

    switch (grail_event_get_type(grail_event))
    {
      case UGEventTypeSlice:
        {
          UGStatus status;
          UGSlice slice;
          status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
          ASSERT_EQ(UGStatusSuccess, status);
          ProcessSlice(slice);
        }
        break;
      default:
        FAIL(); // we are only expecting slice events
    }

    grail_event_unref(grail_event);
  }
  while (get_event_status == UGStatusSuccess);

  ASSERT_EQ(UGStatusErrorNoEvent, get_event_status);
}

void GrailTest::ProcessSlice(UGSlice slice)
{
  switch (grail_slice_get_state(slice))
  {
    case UGGestureStateBegin:
      AddNewGesture(slice);
      break;
    case UGGestureStateUpdate:
      UpdateGesture(slice);
      break;
    case UGGestureStateEnd:
      EndGesture(slice);
      break;
    default:
      FAIL();
  }
}

GrailGesture *GrailTest::GestureWithId(unsigned int id)
{
  GrailGesture *gesture = nullptr;

  for (GrailGesture &other_gesture : grail_gestures)
  {
    if (id == other_gesture.id)
    {
      gesture = &other_gesture;
      break;
    }
  }

  return gesture;
}

void GrailTest::AddNewGesture(UGSlice slice)
{
  unsigned int gesture_id;
  UGStatus status =  grail_slice_get_property(slice, UGSlicePropertyId, &gesture_id);
  ASSERT_EQ(UGStatusSuccess, status);

  // there should be no gesture with this id
  ASSERT_EQ(nullptr, GestureWithId(gesture_id));

  GrailGesture gesture;
  gesture.id = gesture_id;

  unsigned int num_touches;
  status =  grail_slice_get_property(slice, UGSlicePropertyNumTouches, &num_touches);
  ASSERT_EQ(UGStatusSuccess, status);
  for (unsigned int i = 0; i < num_touches; ++i)
  {
    UFTouchId touch_id;
    status = grail_slice_get_touch_id(slice, i, &touch_id);
    ASSERT_EQ(UGStatusSuccess, status);
    gesture.touches.insert(touch_id);
  }

  gesture.construction_finished = grail_slice_get_construction_finished(slice);
  gesture.state = UGGestureStateBegin;

  grail_gestures.push_back(gesture);
}

void GrailTest::UpdateGesture(UGSlice slice)
{
  GrailGesture *gesture = nullptr;
  unsigned int gesture_id;

  UGStatus status = grail_slice_get_property(slice, UGSlicePropertyId, &gesture_id);
  ASSERT_EQ(UGStatusSuccess, status);

  gesture = GestureWithId(gesture_id);

  // the gesture must already exist
  ASSERT_NE(nullptr, gesture);

  if (grail_slice_get_construction_finished(slice))
  {
    gesture->construction_finished = true;
  }
  else
  {
    // can't go back from true to false
    ASSERT_FALSE(gesture->construction_finished);
  }

  ASSERT_TRUE(gesture->state == UGGestureStateBegin
           || gesture->state == UGGestureStateUpdate);
  gesture->state = UGGestureStateUpdate;
}

void GrailTest::EndGesture(UGSlice slice)
{
  GrailGesture *gesture = nullptr;
  unsigned int gesture_id;

  UGStatus status = grail_slice_get_property(slice, UGSlicePropertyId, &gesture_id);
  ASSERT_EQ(UGStatusSuccess, status);

  gesture = GestureWithId(gesture_id);

  // the gesture must already exist
  ASSERT_NE(nullptr, gesture);

  gesture->construction_finished = grail_slice_get_construction_finished(slice);
  ASSERT_TRUE(gesture->construction_finished);

  ASSERT_TRUE(gesture->state == UGGestureStateBegin
           || gesture->state == UGGestureStateUpdate);
  gesture->state = UGGestureStateEnd;
}

void GrailTest::PrintPendingGestures()
{
  UGEvent grail_event;
  UGStatus get_event_status;

  do
  {
    get_event_status = grail_get_event(grail_handle, &grail_event);
    if (get_event_status != UGStatusSuccess)
      continue;

    switch (grail_event_get_type(grail_event))
    {
      case UGEventTypeSlice:
        {
          UGStatus status;
          UGSlice slice;
          status = grail_event_get_property(grail_event, UGEventPropertySlice, &slice);
          ASSERT_EQ(UGStatusSuccess, status);
          PrintSlice(slice);
        }
        break;
      default:
        std::cout << "other event\n";
    }

    grail_event_unref(grail_event);
  }
  while (get_event_status == UGStatusSuccess);

  ASSERT_EQ(UGStatusErrorNoEvent, get_event_status);
}

void GrailTest::PrintSlice(UGSlice slice)
{
  UGStatus status;
  unsigned int num_touches;
  unsigned int gesture_id;

  std::cout << "slice";

  status =  grail_slice_get_property(slice, UGSlicePropertyId, &gesture_id);
  ASSERT_EQ(UGStatusSuccess, status);
  std::cout << " id=" << gesture_id;

  switch (grail_slice_get_state(slice))
  {
    case UGGestureStateBegin:
      std::cout << " begin";
      break;
    case UGGestureStateUpdate:
      std::cout << " update";
      break;
    case UGGestureStateEnd:
      std::cout << " end";
      break;
    default:
      FAIL();
  }

  status =  grail_slice_get_property(slice, UGSlicePropertyNumTouches, &num_touches);
  ASSERT_EQ(UGStatusSuccess, status);
  std::cout << " touches=";
  for (unsigned int i = 0; i < num_touches; ++i)
  {
    if (i != 0)
      std::cout << ",";

    UFTouchId touch_id;
    status = grail_slice_get_touch_id(slice, i, &touch_id);
    ASSERT_EQ(UGStatusSuccess, status);
    std::cout << touch_id;
  }


  if (grail_slice_get_construction_finished(slice))
    std::cout << " finished";
  else
    std::cout << " unfinished";

  std::cout << "\n";
}
