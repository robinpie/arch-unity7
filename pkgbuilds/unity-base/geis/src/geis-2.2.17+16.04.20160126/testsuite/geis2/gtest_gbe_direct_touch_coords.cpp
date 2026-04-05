#include "gtest_grail_backend.h"
#include "x11_mocks.h"

/*
  Check that when a direct device (e.g. a touchscreen) is being used, the
  position of a GeisTouch will be in window coordinates instead of input
  device coordinates.

  This means that the x an y values of a GeisTouch should come from
  XIDeviceEvent::event_x and XIDeviceEvent::event_y and not from
  XIDeviceEvent::valuators.values[0] and XIDeviceEvent::valuators.values[1],
  like it's the case for indirect devices (such as trackpads).

  Regression test for https://bugs.launchpad.net/bugs/984069
 */

class Geis2GrailBackend : public Geis2GrailBackendBase
{
 protected:
  Geis2GrailBackend() : _subscription(nullptr) {}

  void SendXInput2Events();

  virtual void OnEventInitComplete(GeisEvent event);
  virtual void OnEventGestureBegin(GeisEvent event);

  GeisSubscription _subscription;
};

void Geis2GrailBackend::SendXInput2Events()
{
  XEvent event;
  XGenericEventCookie *xcookie = 0;
  XIDeviceEvent *device_event = 0;
  XITouchOwnershipEvent *ownership_event = 0;
  int serial = 0;

  event.type = GenericEvent;
  device_event = (XIDeviceEvent*)calloc(1, sizeof(XIDeviceEvent));
  device_event->serial = serial++;
  device_event->display = xmock_display;
  device_event->extension = xmock_xi2_opcode;
  device_event->evtype = XI_TouchBegin;
  device_event->time = xmock_server_time;
  device_event->deviceid = 0;
  /* The source device that originally generated the event. */
  device_event->sourceid = device_event->deviceid;
  device_event->detail = 0; /* touch id */
  device_event->root = DefaultRootWindow(xmock_display);
  device_event->event = DefaultRootWindow(xmock_display);
  device_event->child = 0;
  device_event->root_x = 123.0f;
  device_event->root_y = 456.0f;
  device_event->event_x = device_event->root_x;
  device_event->event_y = device_event->root_y;
  device_event->valuators.mask_len = 2; /* two bytes */
  device_event->valuators.mask = (unsigned char*) malloc(2);
  XISetMask(device_event->valuators.mask, 0); /* X axis is present */
  XISetMask(device_event->valuators.mask, 1); /* Y axis is present */
  device_event->valuators.values = (double*) malloc(sizeof(double)*2);
  device_event->valuators.values[0] = -200.0;
  device_event->valuators.values[1] = -100.0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchBegin;
  xcookie->data = device_event;
  xmock_add_to_event_queue(&event);

  event.type = GenericEvent;
  device_event = (XIDeviceEvent*)calloc(1, sizeof(XIDeviceEvent));
  device_event->serial = serial++;
  device_event->display = xmock_display;
  device_event->extension = xmock_xi2_opcode;
  device_event->evtype = XI_TouchBegin;
  device_event->time = xmock_server_time;
  device_event->deviceid = 0;
  device_event->sourceid = device_event->deviceid;
  device_event->detail = 1; /* touch id */
  device_event->root = DefaultRootWindow(xmock_display);
  device_event->event = DefaultRootWindow(xmock_display);
  device_event->child = 0;
  device_event->root_x = 222.0f;
  device_event->root_y = 456.0f;
  device_event->event_x = device_event->root_x;
  device_event->event_y = device_event->root_y;
  device_event->valuators.mask_len = 2;
  device_event->valuators.mask = (unsigned char*) malloc(2);
  XISetMask(device_event->valuators.mask, 0);
  XISetMask(device_event->valuators.mask, 1);
  device_event->valuators.values = (double*) malloc(sizeof(double)*2);
  device_event->valuators.values[0] = -150.0;
  device_event->valuators.values[1] = -100.0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchBegin;
  xcookie->data = device_event;
  xmock_add_to_event_queue(&event);

  xmock_server_time += 5;

  event.type = GenericEvent;
  ownership_event = (XITouchOwnershipEvent*)calloc(1, sizeof(XITouchOwnershipEvent));
  ownership_event->type = GenericEvent;
  ownership_event->serial = serial++;
  ownership_event->display = xmock_display;
  ownership_event->extension = xmock_xi2_opcode;
  ownership_event->evtype = XI_TouchOwnership;
  ownership_event->time = xmock_server_time;
  ownership_event->deviceid = 0;
  ownership_event->sourceid = ownership_event->deviceid;
  ownership_event->touchid = 0;
  ownership_event->root = DefaultRootWindow(xmock_display);
  ownership_event->event = DefaultRootWindow(xmock_display);
  ownership_event->child = 0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchOwnership;
  xcookie->data = ownership_event;
  xmock_add_to_event_queue(&event);

  event.type = GenericEvent;
  ownership_event = (XITouchOwnershipEvent*)calloc(1, sizeof(XITouchOwnershipEvent));
  ownership_event->type = GenericEvent;
  ownership_event->serial = serial++;
  ownership_event->display = xmock_display;
  ownership_event->extension = xmock_xi2_opcode;
  ownership_event->evtype = XI_TouchOwnership;
  ownership_event->time = xmock_server_time;
  ownership_event->deviceid = 0;
  ownership_event->sourceid = ownership_event->deviceid;
  ownership_event->touchid = 1;
  ownership_event->root = DefaultRootWindow(xmock_display);
  ownership_event->event = DefaultRootWindow(xmock_display);
  ownership_event->child = 0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchOwnership;
  xcookie->data = ownership_event;
  xmock_add_to_event_queue(&event);

  event.type = GenericEvent;
  device_event = (XIDeviceEvent*)calloc(1, sizeof(XIDeviceEvent));
  device_event->serial = serial++;
  device_event->display = xmock_display;
  device_event->extension = xmock_xi2_opcode;
  device_event->evtype = XI_TouchUpdate;
  device_event->time = xmock_server_time;
  device_event->deviceid = 0;
  device_event->sourceid = device_event->deviceid;
  device_event->detail = 0;
  device_event->root = DefaultRootWindow(xmock_display);
  device_event->event = DefaultRootWindow(xmock_display);
  device_event->child = 0;
  device_event->root_x = 123.0f;
  device_event->root_y = 466.0f;
  device_event->event_x = device_event->root_x;
  device_event->event_y = device_event->root_y;
  device_event->valuators.mask_len = 2; /* two bytes */
  device_event->valuators.mask = (unsigned char*) malloc(2);
  XISetMask(device_event->valuators.mask, 0);
  XISetMask(device_event->valuators.mask, 1);
  device_event->valuators.values = (double*) malloc(sizeof(double)*2);
  device_event->valuators.values[0] = -200.0;
  device_event->valuators.values[1] = -90.0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchUpdate;
  xcookie->data = device_event;
  xmock_add_to_event_queue(&event);

  event.type = GenericEvent;
  device_event = (XIDeviceEvent*)calloc(1, sizeof(XIDeviceEvent));
  device_event->serial = serial++;
  device_event->display = xmock_display;
  device_event->extension = xmock_xi2_opcode;
  device_event->evtype = XI_TouchUpdate;
  device_event->time = xmock_server_time;
  device_event->deviceid = 0;
  device_event->sourceid = device_event->deviceid;
  device_event->detail = 1; /* touch id */
  device_event->root = DefaultRootWindow(xmock_display);
  device_event->event = DefaultRootWindow(xmock_display);
  device_event->child = 0;
  device_event->root_x = 222.0f;
  device_event->root_y = 466.0f;
  device_event->event_x = device_event->root_x;
  device_event->event_y = device_event->root_y;
  device_event->valuators.mask_len = 2;
  device_event->valuators.mask = (unsigned char*) malloc(2);
  XISetMask(device_event->valuators.mask, 0);
  XISetMask(device_event->valuators.mask, 1);
  device_event->valuators.values = (double*) malloc(sizeof(double)*2);
  device_event->valuators.values[0] = -150.0;
  device_event->valuators.values[1] = -90.0;
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchUpdate;
  xcookie->data = device_event;
  xmock_add_to_event_queue(&event);
}

void Geis2GrailBackend::OnEventInitComplete(GeisEvent event)
{
  _subscription = CreateFilteredSubscription(
      "My 2-touches Touch", 2, GEIS_GESTURE_TOUCH);
  ASSERT_NE(nullptr, _subscription);

  SendXInput2Events();
}

void Geis2GrailBackend::OnEventGestureBegin(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  GeisTouchSet geis_touch_set = (GeisTouchSet) geis_attr_value_to_pointer(attr);

  TouchSet touch_set(geis_touch_set);

  /* check that there are two touch points and that they are in window coordinates
     (instead of input device coordinates) */
  ASSERT_EQ(2, touch_set.size());
  ASSERT_TRUE(touch_set.contains(123.0f, 456.0f));
  ASSERT_TRUE(touch_set.contains(222.0f, 456.0f));
}

TEST_F(Geis2GrailBackend, DirectDeviceTouchCoords)
{
  CreateXMockTouchScreenDevice();

  _geis = geis_new(GEIS_INIT_GRAIL_BACKEND,
                       nullptr);
  ASSERT_NE(nullptr, _geis);

  Run();

  if (_subscription)
    geis_subscription_delete(_subscription);
  geis_delete(_geis);

  DestroyXMockDevices();
}
