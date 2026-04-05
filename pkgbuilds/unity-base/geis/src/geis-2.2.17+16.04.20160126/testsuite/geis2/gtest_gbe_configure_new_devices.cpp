#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Regression test for bug LP#1045785
  https://bugs.launchpad.net/geis/+bug/1045785

  Steps to reproduce the issue:
    1 - create a subscription, set some configuration options
        (geis_subscription_set_configuration) and activate it.
    2 - connect a new multitouch device to your computer (e.g. pair an Apple
        Magic Trackpad)

  Expected outcome:
    Gestures from that new multitouch device follow the configurations set by
    the subscription.
 */

class ConfigureNewDevice : public Geis2GrailBackendBase
{
 protected:
  ConfigureNewDevice();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventClassAvailable(GeisEvent event);
  virtual void OnEventGestureBegin(GeisEvent event);

  void CreateSubscription();
  void SendXInput2Events();

  /* signals that a new device has been added */
  void SendXIDeviceAddedEvent();

  GeisSubscription _subscription;
  GeisGestureClass _drag_class;
};

ConfigureNewDevice::ConfigureNewDevice()
    : _subscription(nullptr),
      _drag_class(nullptr)
{
}

void ConfigureNewDevice::OnEventInitComplete(GeisEvent event)
{
  CreateSubscription();

  /* The new device comes only after the subscription has been created and
     configured */
  CreateXMockTouchScreenDevice();
  SendXIDeviceAddedEvent();

  SendXInput2Events();
}

void ConfigureNewDevice::OnEventClassAvailable(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
  GeisGestureClass gesture_class =
    reinterpret_cast<GeisGestureClass>(geis_attr_value_to_pointer(attr));

  if (strcmp(geis_gesture_class_name(gesture_class), GEIS_GESTURE_DRAG) == 0)
  {
    _drag_class = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
}

void ConfigureNewDevice::OnEventGestureBegin(GeisEvent event)
{
  /* There should be no drag gesture since we set a huge drag threshold. */

  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet group_set =
    reinterpret_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));
  GeisGroup group = geis_groupset_group(group_set, 0);
  GeisFrame frame = geis_group_frame(group, 0);

  ASSERT_NE(nullptr, _drag_class);
  GeisBoolean is_drag = geis_frame_is_class(frame, _drag_class);
  ASSERT_FALSE(is_drag);
}

void ConfigureNewDevice::CreateSubscription()
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisFilter filter = nullptr;

  _subscription = geis_subscription_new(_geis, "2-fingers drag",
      GEIS_SUBSCRIPTION_NONE);

  filter = geis_filter_new(_geis, "filter");
  ASSERT_NE(nullptr, filter);

  status = geis_filter_add_term(filter,
      GEIS_FILTER_CLASS,
      GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_DRAG,
      GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, 2,
      nullptr);
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);

  status = geis_filter_add_term(filter,
      GEIS_FILTER_REGION,
      GEIS_REGION_ATTRIBUTE_WINDOWID, GEIS_FILTER_OP_EQ,
      DefaultRootWindow(xmock_display),
      nullptr);
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);

  status = geis_subscription_add_filter(_subscription, filter);
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);

  /* Set a huge threshold (in meters) so that no drag can be recognized */
  GeisFloat drag_threshold = 1000.0f;
  status = geis_subscription_set_configuration(_subscription,
      GEIS_CONFIG_DRAG_THRESHOLD, &drag_threshold);
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);

  status = geis_subscription_activate(_subscription);
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);
}

void ConfigureNewDevice::SendXInput2Events()
{
  /* Emulate a simple 2 fingers drag */

  /* event type, touch id, X and Y  */
  SendTouchEvent(XI_TouchBegin, 1, 10.0f, 10.0f);
  SendTouchEvent(XI_TouchBegin, 2, 20.0f, 10.0f);

  xmock_server_time += 2;

  /* touch id  */
  SendTouchOwnershipEvent(1);
  SendTouchOwnershipEvent(2);

  xmock_server_time += 20;

  SendTouchEvent(XI_TouchUpdate, 1, 10.0f, 25.0f);
  SendTouchEvent(XI_TouchUpdate, 2, 20.0f, 25.0f);

  xmock_server_time += 20;

  SendTouchEvent(XI_TouchUpdate, 1, 10.0f, 35.0f);
  SendTouchEvent(XI_TouchUpdate, 2, 20.0f, 35.0f);

  xmock_server_time += 20;

  SendTouchEvent(XI_TouchEnd, 1, 10.0f, 50.0f);
  SendTouchEvent(XI_TouchEnd, 2, 20.0f, 50.0f);
}

void ConfigureNewDevice::SendXIDeviceAddedEvent()
{
  XEvent event;
  XGenericEventCookie *xcookie = 0;
  XIHierarchyEvent *hierarchy_event;
  XIHierarchyInfo *info;

  info = (XIHierarchyInfo*)calloc(1, sizeof(XIHierarchyInfo));
  info->deviceid = xmock_devices[0].deviceid;
  info->enabled = True;
  info->flags = XISlaveAdded;

  hierarchy_event = (XIHierarchyEvent*)calloc(1, sizeof(XIHierarchyEvent));
  hierarchy_event->type = GenericEvent;
  hierarchy_event->serial = _xevent_serial_number++;
  hierarchy_event->display = xmock_display;
  hierarchy_event->extension = xmock_xi2_opcode;
  hierarchy_event->evtype = XI_HierarchyChanged;
  hierarchy_event->time = xmock_server_time;
  hierarchy_event->flags = XISlaveAdded;
  hierarchy_event->num_info = 1;
  hierarchy_event->info = info;

  event.type = GenericEvent;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_HierarchyChanged;
  xcookie->data = hierarchy_event;
  xmock_add_to_event_queue(&event);
}

TEST_F(ConfigureNewDevice, Test)
{
  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                   GEIS_INIT_NO_ATOMIC_GESTURES,
                   nullptr);
  ASSERT_NE(nullptr, _geis);

  Run();

  if (_subscription)
    geis_subscription_delete(_subscription);

  if (_drag_class)
    geis_gesture_class_unref(_drag_class);

  geis_delete(_geis);

  DestroyXMockDevices();
}
