#include "gtest_grail_backend.h"
#include "x11_mocks.h"

Touch::Touch(GeisTouch geis_touch)
{
  GeisAttr attr = geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_X);
  x = geis_attr_value_to_float(attr);

  attr = geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_Y);
  y = geis_attr_value_to_float(attr);

  attr = geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_ID);
  id = geis_attr_value_to_integer(attr);
}

std::ostream& operator<<(std::ostream& os, const Touch& touch)
{
  std::cout << "(id=" << touch.id << ",x=" << touch.x << ",y=" << touch.y << ")";
  return os;
}

TouchSet::TouchSet(GeisTouchSet geis_touch_set)
{
  int count = geis_touchset_touch_count(geis_touch_set);
  for (int i = 0; i < count; ++i)
    push_back(Touch(geis_touchset_touch(geis_touch_set, i)));
}

bool TouchSet::contains(float x, float y)
{
  for (auto touch : *this)
  {
    if (touch.x == x && touch.y == y)
      return true;
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const TouchSet& touch_set)
{
  bool first = true;
  std::cout << "{";
  for (auto touch : touch_set)
  {
    if (first)
      first = false;
    else
      std::cout << ", ";

    std::cout << touch;
  }
  std::cout << "}";

  return os;
}

GeisSubscription Geis2GrailBackendBase::CreateFilteredSubscription(
    GeisString name, GeisSize num_touches, GeisString gesture_class)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisFilter filter = nullptr;
  GeisSubscription sub = nullptr;

  sub = geis_subscription_new(_geis, name, GEIS_SUBSCRIPTION_NONE);

  filter = geis_filter_new(_geis, "filter");
  if (filter == nullptr) {ADD_FAILURE(); return nullptr;}

  status = geis_filter_add_term(filter,
      GEIS_FILTER_CLASS,
      GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, gesture_class,
      GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches,
      nullptr);
  if (status != GEIS_STATUS_SUCCESS) {ADD_FAILURE(); return nullptr;}

  status = geis_filter_add_term(filter,
      GEIS_FILTER_REGION,
      GEIS_REGION_ATTRIBUTE_WINDOWID, GEIS_FILTER_OP_EQ,
      DefaultRootWindow(xmock_display),
      nullptr);
  if (status != GEIS_STATUS_SUCCESS) {ADD_FAILURE(); return nullptr;}

  status = geis_subscription_add_filter(sub, filter);
  if (status != GEIS_STATUS_SUCCESS) {ADD_FAILURE(); return nullptr;}

  status = geis_subscription_activate(sub);
  if (status != GEIS_STATUS_SUCCESS) {ADD_FAILURE(); return nullptr;}

  return sub;
}

void Geis2GrailBackendBase::CreateXMockTouchScreenDevice()
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

void Geis2GrailBackendBase::DestroyXMockDevices()
{
  for (int i = 0; i < xmock_devices_count; ++i)
  {
    for (int j = 0; j < xmock_devices[i].num_classes; ++j)
      free(xmock_devices[i].classes[j]);
    free(xmock_devices[i].classes);
  }
  free(xmock_devices);
}

bool Geis2GrailBackendBase::DispatchAndProcessEvents()
{
  bool got_events = false;
  GeisEvent event;
  GeisStatus status;

  status = geis_dispatch_events(_geis);
  if (status != GEIS_STATUS_SUCCESS
      && status != GEIS_STATUS_CONTINUE) {ADD_FAILURE(); return false;}

  status = geis_next_event(_geis, &event);
  if (status != GEIS_STATUS_SUCCESS
      && status != GEIS_STATUS_CONTINUE
      && status != GEIS_STATUS_EMPTY)
    {ADD_FAILURE(); return false;}

  if (status == GEIS_STATUS_SUCCESS || status == GEIS_STATUS_CONTINUE)
    got_events = true;

  while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_INIT_COMPLETE:
        OnEventInitComplete(event);
        break;
      case GEIS_EVENT_CLASS_AVAILABLE:
        OnEventClassAvailable(event);
        break;
      case GEIS_EVENT_GESTURE_BEGIN:
        OnEventGestureBegin(event);
        break;
      case GEIS_EVENT_GESTURE_UPDATE:
        OnEventGestureUpdate(event);
        break;
      case GEIS_EVENT_GESTURE_END:
        OnEventGestureEnd(event);
        break;
      default:
        break;
    }
    geis_event_delete(event);
    status = geis_next_event(_geis, &event);
  }

  return got_events;
}

void Geis2GrailBackendBase::Run()
{
  bool got_events;
  do
  {
    got_events = DispatchAndProcessEvents();
  } while (got_events);
}

void Geis2GrailBackendBase::SendTouchEvent(
    int event_type, int touch_id, float x, float y)
{
  XEvent event;
  XGenericEventCookie *xcookie = 0;
  XIDeviceEvent *device_event = 0;

  event.type = GenericEvent;
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
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = event_type;
  xcookie->data = device_event;
  xmock_add_to_event_queue(&event);
}

void Geis2GrailBackendBase::SendTouchOwnershipEvent(int touch_id)
{
  XEvent event;
  XGenericEventCookie *xcookie = 0;
  XITouchOwnershipEvent *ownership_event = 0;

  event.type = GenericEvent;
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
  xcookie = &event.xcookie;
  xcookie->extension = xmock_xi2_opcode;
  xcookie->evtype = XI_TouchOwnership;
  xcookie->data = ownership_event;
  xmock_add_to_event_queue(&event);
}

void Geis2GrailBackendBase::AcceptRejectGestureInEvent(
    GeisEvent event, bool accept)
{
  /* We expect only one group with one gesture frame.
     Multiple groups with several gesture frames is just not supported or used.
     That is, one GeisEvent is sent for each possible gesture instead of a single
     GeisEvent with all possible gestures (arranged in groups). */

  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet group_set =
    static_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));

  ASSERT_EQ(1, geis_groupset_group_count(group_set));

  GeisGroup group = geis_groupset_group(group_set, 0);

  ASSERT_EQ(1, geis_group_frame_count(group));

  GeisFrame frame = geis_group_frame(group, 0);

  GeisStatus status;
  if (accept)
  {
    status = geis_gesture_accept(_geis, group, geis_frame_id(frame));
  }
  else
  {
    status = geis_gesture_reject(_geis, group, geis_frame_id(frame));
  }
  ASSERT_EQ(GEIS_STATUS_SUCCESS, status);
}

void Geis2GrailBackendBase::AcceptGestureInEvent(GeisEvent event)
{
  AcceptRejectGestureInEvent(event, true);
}

void Geis2GrailBackendBase::RejectGestureInEvent(GeisEvent event)
{
  AcceptRejectGestureInEvent(event, false);
}

void Geis2GrailBackendBase::GetGestureTimestampInEvent(GeisInteger *timestamp,
                                                       GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet group_set =
    static_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));

  ASSERT_EQ(1, geis_groupset_group_count(group_set));

  GeisGroup group = geis_groupset_group(group_set, 0);

  ASSERT_EQ(1, geis_group_frame_count(group));

  GeisFrame frame = geis_group_frame(group, 0);

  attr = geis_frame_attr_by_name(frame, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP);
  ASSERT_NE(nullptr, attr);

  *timestamp =  geis_attr_value_to_integer(attr);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
