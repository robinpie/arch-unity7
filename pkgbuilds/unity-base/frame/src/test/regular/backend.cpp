#include <gtest/gtest.h>
#include "oif/frame_backend.h"

TEST(Backend, Touch)
{
  UFStatus status;

  UFBackendTouch touch_backend = frame_backend_touch_new();
  ASSERT_TRUE(touch_backend != nullptr);

  UFTouch touch = frame_backend_touch_get_touch(touch_backend);
  ASSERT_TRUE(touch != nullptr);

  ASSERT_EQ(UFTouchStateBegin, frame_touch_get_state(touch));

  frame_backend_touch_set_id(touch_backend, 123);
  ASSERT_EQ(123, frame_touch_get_id(touch));

  frame_backend_touch_set_ended(touch_backend);
  ASSERT_EQ(UFTouchStateEnd, frame_touch_get_state(touch));

  frame_backend_touch_set_window_pos(touch_backend, 1.2f, 3.4f);
  ASSERT_EQ(1.2f, frame_touch_get_window_x(touch));
  ASSERT_EQ(3.4f, frame_touch_get_window_y(touch));

  frame_backend_touch_set_time(touch_backend, 852);
  ASSERT_EQ(852, frame_touch_get_time(touch));

  frame_backend_touch_set_start_time(touch_backend, 555);
  ASSERT_EQ(555, frame_touch_get_start_time(touch));

  frame_backend_touch_set_owned(touch_backend, 1);
  int owned = 0;
  status = frame_touch_get_property(touch, UFTouchPropertyOwned, &owned);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, owned);

  frame_backend_touch_set_pending_end(touch_backend, 1);
  int pending_end = 0;
  status = frame_touch_get_property(touch, UFTouchPropertyPendingEnd, &pending_end);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, pending_end);

  frame_backend_touch_set_value(touch_backend, UFAxisTypeTouchMajor, 987.6f);
  float touch_major = 0.0f;
  status = frame_touch_get_value(touch, UFAxisTypeTouchMajor, &touch_major);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(987.6f, touch_major);

  /* clean up */
  UFBackendFrame frame = frame_backend_frame_new();
  frame_backend_frame_give_touch(frame, &touch_backend);
  frame_backend_frame_delete(frame);
}

TEST(Backend, Device)
{
  UFStatus status;

  UFBackendDevice device_backend = frame_backend_device_new();
  ASSERT_NE(nullptr, device_backend);

  UFDevice device = frame_backend_device_get_device(device_backend);
  ASSERT_NE(nullptr, device);

  const char *name = nullptr;
  status = frame_device_get_property(device, UFDevicePropertyName, &name);
  ASSERT_EQ(UFStatusErrorUnknownProperty, status);
  frame_backend_device_set_name(device_backend, "Hello World");
  status = frame_device_get_property(device, UFDevicePropertyName, &name);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_STREQ("Hello World", name);

  int direct = 0;
  status = frame_device_get_property(device, UFDevicePropertyDirect, &direct);
  ASSERT_EQ(UFStatusErrorUnknownProperty, status);
  frame_backend_device_set_direct(device_backend, 1);
  status = frame_device_get_property(device, UFDevicePropertyDirect, &direct);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, direct);

  int independent = 0;
  status = frame_device_get_property(device, UFDevicePropertyIndependent, &independent);
  ASSERT_EQ(UFStatusErrorUnknownProperty, status);
  frame_backend_device_set_independent(device_backend, 1);
  status = frame_device_get_property(device, UFDevicePropertyIndependent, &independent);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, independent);

  int semi_mt = 0;
  status = frame_device_get_property(device, UFDevicePropertySemiMT, &semi_mt);
  ASSERT_EQ(UFStatusErrorUnknownProperty, status);
  frame_backend_device_set_semi_mt(device_backend, 1);
  status = frame_device_get_property(device, UFDevicePropertySemiMT, &semi_mt);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, semi_mt);

  unsigned int max_touches = 0;
  status = frame_device_get_property(device, UFDevicePropertyMaxTouches, &max_touches);
  ASSERT_EQ(UFStatusErrorUnknownProperty, status);
  frame_backend_device_set_max_touches(device_backend, 5);
  status = frame_device_get_property(device, UFDevicePropertyMaxTouches, &max_touches);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(5, max_touches);

  frame_backend_device_set_window_resolution(device_backend, 1024.0f, 768.0f);
  ASSERT_EQ(1024.0f, frame_device_get_window_resolution_x(device));
  ASSERT_EQ(768.0f, frame_device_get_window_resolution_y(device));

  unsigned int num_axes = 0;
  status = frame_device_get_property(device, UFDevicePropertyNumAxes, &num_axes);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(0, num_axes);
  frame_backend_device_add_axis(device_backend,
                        UFAxisTypePressure, 1.0f, 1000.0f, 3.5f);
  status = frame_device_get_property(device, UFDevicePropertyNumAxes, &num_axes);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, num_axes);
  UFAxis axis = nullptr;
  status = frame_device_get_axis_by_type(device, UFAxisTypePressure, &axis);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_NE(nullptr, axis);
  ASSERT_EQ(UFAxisTypePressure, frame_axis_get_type(axis));
  ASSERT_EQ(1.0f, frame_axis_get_minimum(axis));
  ASSERT_EQ(1000.0f, frame_axis_get_maximum(axis));
  ASSERT_EQ(3.5f, frame_axis_get_resolution(axis));

  frame_backend_device_delete(device_backend);
}

TEST(Backend, Frame)
{
  UFStatus status;

  UFBackendFrame frame_backend = frame_backend_frame_new();
  ASSERT_NE(nullptr, frame_backend);

  UFFrame frame = frame_backend_frame_get_frame(frame_backend);
  ASSERT_NE(nullptr, frame);

  UFBackendDevice device_backend = frame_backend_device_new();
  UFDevice device = frame_backend_device_get_device(device_backend);

  frame_backend_frame_set_device(frame_backend, device_backend);
  ASSERT_EQ(device, frame_frame_get_device(frame));

  frame_backend_frame_set_window_id(frame_backend, 123);
  ASSERT_EQ(123, frame_frame_get_window_id(frame));


  unsigned int active_touches = 0;
  UFBackendTouch touch_backend = frame_backend_touch_new();
  UFTouch touch = frame_backend_touch_get_touch(touch_backend);
  frame_backend_touch_set_id(touch_backend, 10);
  UFTouch touch_retrieved = nullptr;
  ASSERT_EQ(0, frame_frame_get_num_touches(frame));
  status = frame_frame_get_property(frame, UFFramePropertyActiveTouches, &active_touches);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(0, active_touches);
  status = frame_backend_frame_give_touch(frame_backend, &touch_backend);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(nullptr, touch_backend);
  ASSERT_EQ(1, frame_frame_get_num_touches(frame));
  status = frame_frame_get_property(frame, UFFramePropertyActiveTouches, &active_touches);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(1, active_touches);
  status = frame_frame_get_touch_by_id(frame, 10, &touch_retrieved);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(touch, touch_retrieved);

  frame_backend_frame_set_active_touches(frame_backend, 3);
  status = frame_frame_get_property(frame, UFFramePropertyActiveTouches, &active_touches);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(3, active_touches);

  frame_backend_frame_delete(frame_backend);
  frame_backend_device_delete(device_backend);
}

TEST(Backend, FrameBorrowInexistentTouch)
{
  UFStatus status;

  UFBackendFrame frame_backend = frame_backend_frame_new();

  UFBackendTouch touch_backend;
  status = frame_backend_frame_borrow_touch_by_id(frame_backend, 123, &touch_backend);
  ASSERT_EQ(UFStatusErrorInvalidTouch, status);

  /* clean up */
  frame_backend_frame_delete(frame_backend);
}

TEST(Backend, FrameCreateNext)
{
  UFStatus status;

  /* frame 1  */
  UFBackendTouch touch_backend = frame_backend_touch_new();
  frame_backend_touch_set_id(touch_backend, 1);
  UFTouch frame1_touch = frame_backend_touch_get_touch(touch_backend);

  UFBackendFrame frame1_backend = frame_backend_frame_new();
  status = frame_backend_frame_give_touch(frame1_backend, &touch_backend);
  ASSERT_EQ(UFStatusSuccess, status);

  /* frame 2 */
  UFBackendFrame frame2_backend = frame_backend_frame_create_next(frame1_backend);
  UFFrame frame2 = frame_backend_frame_get_frame(frame2_backend);

  UFTouch frame2_touch;
  status = frame_frame_get_touch_by_id(frame2, 1, &frame2_touch);
  ASSERT_EQ(UFStatusSuccess, status);

  /* frame 3 */
  UFBackendFrame frame3_backend = frame_backend_frame_create_next(frame2_backend);
  UFFrame frame3 = frame_backend_frame_get_frame(frame3_backend);

  UFTouch frame3_touch;
  status = frame_frame_get_touch_by_id(frame3, 1, &frame3_touch);
  ASSERT_EQ(UFStatusSuccess, status);

  /* frame 4 */
  UFBackendFrame frame4_backend = frame_backend_frame_create_next(frame3_backend);

  status = frame_backend_frame_borrow_touch_by_id(frame4_backend, 1, &touch_backend);
  ASSERT_EQ(UFStatusSuccess, status);
  frame_backend_touch_set_ended(touch_backend);
  status = frame_backend_frame_give_touch(frame4_backend, &touch_backend);
  ASSERT_EQ(UFStatusSuccess, status);

  UFFrame frame4 = frame_backend_frame_get_frame(frame4_backend);
  UFTouch frame4_touch;
  status = frame_frame_get_touch_by_id(frame4, 1, &frame4_touch);
  ASSERT_EQ(UFStatusSuccess, status);

  /* frame 5 */
  UFBackendFrame frame5_backend = frame_backend_frame_create_next(frame4_backend);
  UFFrame frame5 = frame_backend_frame_get_frame(frame5_backend);

  /* Test the status of touch 1 throughout all frames */
  ASSERT_EQ(UFTouchStateBegin, frame_touch_get_state(frame1_touch));
  ASSERT_EQ(UFTouchStateUpdate, frame_touch_get_state(frame2_touch));
  ASSERT_EQ(UFTouchStateUpdate, frame_touch_get_state(frame3_touch));
  ASSERT_EQ(frame2_touch, frame3_touch);
  ASSERT_EQ(UFTouchStateEnd, frame_touch_get_state(frame4_touch));
  ASSERT_EQ(0, frame_frame_get_num_touches(frame5));

  /* clean up */
  frame_backend_frame_delete(frame1_backend);
  frame_backend_frame_delete(frame2_backend);
  frame_backend_frame_delete(frame3_backend);
  frame_backend_frame_delete(frame4_backend);
  frame_backend_frame_delete(frame5_backend);
}

TEST(Backend, Event)
{
  UFStatus status;

  UFEvent event = frame_event_new();
  ASSERT_NE(nullptr, event);

  frame_event_set_type(event, UFEventTypeDeviceRemoved);
  ASSERT_EQ(UFEventTypeDeviceRemoved, frame_event_get_type(event));

  UFBackendDevice device_backend = frame_backend_device_new();
  UFDevice device = frame_backend_device_get_device(device_backend);
  UFDevice device_retrieved = nullptr;
  frame_event_set_device(event, device_backend);
  status = frame_event_get_property(event, UFEventPropertyDevice, &device_retrieved);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(device, device_retrieved);

  UFBackendFrame frame_backend = frame_backend_frame_new();
  UFFrame frame = frame_backend_frame_get_frame(frame_backend);
  UFFrame frame_retrieved = nullptr;
  frame_event_set_frame(event, frame_backend);
  status = frame_event_get_property(event, UFEventPropertyFrame, &frame_retrieved);
  ASSERT_EQ(UFStatusSuccess, status);
  ASSERT_EQ(frame, frame_retrieved);

  frame_event_set_time(event, 1234);
  ASSERT_EQ(1234, frame_event_get_time(event));

  frame_event_unref(event);
  frame_backend_device_delete(device_backend);
  frame_backend_frame_delete(frame_backend);
}
