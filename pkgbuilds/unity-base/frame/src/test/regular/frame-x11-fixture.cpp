#include "frame-x11-fixture.h"
#include "x11_mocks.h"

FrameX11Fixture::FrameX11Fixture()
  : frame_handle(nullptr),
    _xevent_serial_number(1)
{
}

void FrameX11Fixture::SetUp()
{
  xmock_touch_acceptance.clear();
}

void FrameX11Fixture::TearDown()
{
  ASSERT_EQ(nullptr, frame_handle);
}

void FrameX11Fixture::CreateXMockTouchScreenDevice()
{
  xmock_devices_count = 1;
  xmock_devices = (XIDeviceInfo*) calloc(xmock_devices_count,
                                         sizeof(XIDeviceInfo));

  XITouchClassInfo *touch_info = (XITouchClassInfo*) malloc(sizeof(XITouchClassInfo));
  touch_info->type = XITouchClass;
  touch_info->sourceid = 0;
  touch_info->mode = XIDirectTouch;
  touch_info->num_touches = 5;

  XIValuatorClassInfo *x_axis_info = (XIValuatorClassInfo*) malloc(sizeof(XIValuatorClassInfo));
  x_axis_info->type = XIValuatorClass;
  x_axis_info->sourceid = 0;
  x_axis_info->number = 0; /* identifies it as being the X axis */
  x_axis_info->min = -500.0;
  x_axis_info->max = 500.0;
  x_axis_info->resolution = 3000; /* counts/meter */

  XIValuatorClassInfo *y_axis_info = (XIValuatorClassInfo*) malloc(sizeof(XIValuatorClassInfo));
  y_axis_info->type = XIValuatorClass;
  y_axis_info->sourceid = 0;
  y_axis_info->number = 1; /* identifies it as being the Y axis */
  y_axis_info->min = -500.0;
  y_axis_info->max = 500.0;
  y_axis_info->resolution = 3000;

  XIAnyClassInfo **classes = (XIAnyClassInfo**) malloc(sizeof(XIAnyClassInfo*)*3);
  classes[0] = (XIAnyClassInfo*) touch_info;
  classes[1] = (XIAnyClassInfo*) x_axis_info;
  classes[2] = (XIAnyClassInfo*) y_axis_info;

  xmock_devices[0].deviceid = 0;
  xmock_devices[0].name = const_cast<char *>("Fake Touch Screen");
  xmock_devices[0].use = XISlavePointer;
  xmock_devices[0].attachment = 1;
  xmock_devices[0].enabled = True;
  xmock_devices[0].num_classes = 3;
  xmock_devices[0].classes = classes;
}

void FrameX11Fixture::DestroyXMockDevices()
{
  for (int i = 0; i < xmock_devices_count; ++i)
  {
    for (int j = 0; j < xmock_devices[i].num_classes; ++j)
      free(xmock_devices[i].classes[j]);
    free(xmock_devices[i].classes);
  }
  free(xmock_devices);
}

void FrameX11Fixture::SendTouchEvent(
    int event_type, int touch_id, float x, float y)
{
  UFStatus status;
  XGenericEventCookie xcookie;
  XIDeviceEvent *device_event = 0;

  device_event = (XIDeviceEvent*)calloc(1, sizeof(XIDeviceEvent));
  device_event->serial = _xevent_serial_number++;
  device_event->display = xmock_display;
  device_event->extension = xmock_xi2_opcode;
  device_event->evtype = event_type;
  device_event->time = xmock_server_time;
  device_event->deviceid = 0;
  device_event->sourceid = device_event->deviceid;
  device_event->detail = touch_id;
  device_event->root = DefaultRootWindow(xmock_display);
  device_event->event = DefaultRootWindow(xmock_display);
  device_event->child = 0;
  device_event->root_x = x;
  device_event->root_y = y;
  device_event->event_x = device_event->root_x;
  device_event->event_y = device_event->root_y;
  device_event->valuators.mask_len = 2;
  device_event->valuators.mask = (unsigned char*) malloc(2);
  XISetMask(device_event->valuators.mask, 0);
  XISetMask(device_event->valuators.mask, 1);
  device_event->valuators.values = (double*) malloc(sizeof(double)*2);
  device_event->valuators.values[0] = 0; /* just change the coordinate system */
  device_event->valuators.values[1] = 0;
  xcookie.extension = xmock_xi2_opcode;
  xcookie.evtype = event_type;
  xcookie.data = device_event;

  status = frame_x11_process_event(frame_handle, &xcookie);
  ASSERT_EQ(UFStatusSuccess, status);

  free(device_event->valuators.mask);
  free(device_event->valuators.values);
  free(device_event);
}

void FrameX11Fixture::SendTouchOwnershipEvent(int touch_id)
{
  UFStatus status;
  XGenericEventCookie xcookie;
  XITouchOwnershipEvent *ownership_event = 0;

  ownership_event = (XITouchOwnershipEvent*)calloc(1, sizeof(XITouchOwnershipEvent));
  ownership_event->type = GenericEvent;
  ownership_event->serial = _xevent_serial_number++;
  ownership_event->display = xmock_display;
  ownership_event->extension = xmock_xi2_opcode;
  ownership_event->evtype = XI_TouchOwnership;
  ownership_event->time = xmock_server_time;
  ownership_event->deviceid = 0;
  ownership_event->sourceid = ownership_event->deviceid;
  ownership_event->touchid = touch_id;
  ownership_event->root = DefaultRootWindow(xmock_display);
  ownership_event->event = DefaultRootWindow(xmock_display);
  ownership_event->child = 0;
  xcookie.extension = xmock_xi2_opcode;
  xcookie.evtype = XI_TouchOwnership;
  xcookie.data = ownership_event;

  status = frame_x11_process_event(frame_handle, &xcookie);
  ASSERT_EQ(UFStatusSuccess, status);

  free(ownership_event);
}

void FrameX11Fixture::FetchDeviceAddedEvent(UFDevice *device)
{
  UFEvent event;
  UFStatus status;

  status = frame_get_event(frame_handle, &event);
  ASSERT_EQ(UFStatusSuccess, status);

  ASSERT_EQ(UFEventTypeDeviceAdded, frame_event_get_type(event));

  status = frame_event_get_property(event, UFEventPropertyDevice, device);
  ASSERT_EQ(UFStatusSuccess, status);

  frame_event_unref(event);
}

void FrameX11Fixture::AssertNoMoreEvents()
{
  UFEvent event;
  UFStatus status;

  status = frame_get_event(frame_handle, &event);
  ASSERT_EQ(UFStatusErrorNoEvent, status);
}
