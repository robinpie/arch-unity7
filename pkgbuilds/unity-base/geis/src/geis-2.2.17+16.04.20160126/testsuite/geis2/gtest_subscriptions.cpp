/**
 * GTest-based test suite for GEIS subscription actions.
 *
 * Copyright 2012 Canonical Ltd.
 */

#include "gtest_evemu_device.h"
#include "gtest_geis_fixture.h"
#include <X11/extensions/XInput2.h>


static const std::string TEST_DEVICE_PROP_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/device.prop");
static const std::string TEST_DEVICE_EVENTS_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/rotate90.record");

/**
 * Tests need to make sure at least one multi-touch device is available.
 */
class GeisSubscriptionTests
: public GTestGeisFixture
{
public:
  GeisSubscriptionTests()
  : evemu_device_(TEST_DEVICE_PROP_FILE),
    device_is_ready_(false),
    recording_was_sent_(false),
    rotate_class_(nullptr)
  { }

  void 
  pre_event_handler(GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
      EXPECT_TRUE(attr != NULL);
      GeisDevice device = (GeisDevice)geis_attr_value_to_pointer(attr);
      if (evemu_device_.name() == geis_device_name(device))
      {
        device_is_ready_ = true;
      }
    }
    else if (geis_event_type(event) == GEIS_EVENT_CLASS_AVAILABLE)
    {
      GeisGestureClass gesture_class = nullptr;
      GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
      EXPECT_TRUE(attr != NULL) << "event is missing class attr";
      gesture_class = (GeisGestureClass)geis_attr_value_to_pointer(attr);
      for (GeisSize i = 0; i < geis_gesture_class_attr_count(gesture_class); ++i)
      {
        GeisAttr attr2 = geis_gesture_class_attr(gesture_class, i);
        if (0 == strcmp(geis_attr_name(attr2), GEIS_CLASS_ATTRIBUTE_NAME))
        {
          if (0 == strcmp(geis_attr_value_to_string(attr2), GEIS_GESTURE_ROTATE))
          {
            rotate_class_ = gesture_class;
          }
        }
      }
    }
  }

  void 
  post_event_handler(GeisEvent event)
  {
    GTestGeisFixture::post_event_handler(event);
    if (geis_is_initialized() && device_is_ready_ && !recording_was_sent_)
    {
      evemu_device_.play(TEST_DEVICE_EVENTS_FILE);
      recording_was_sent_ = true;
    }
  }

protected:
  Testsuite::EvemuDevice evemu_device_;
  bool                   device_is_ready_;
  bool                   recording_was_sent_;
  GeisGestureClass       rotate_class_;
};


/**
 * Regression test for lp:937021: Geis subscription touch grabs need to check
 * for failure.
 *
 * This test creates two subscriptions, both attached to the root window but
 * each on a different X client.  Since multiple subscriptions for the same
 * window but a different client are expected to fail using the XI2.2-based
 * grail back end, activating the second subscription is extected to resut in
 * a failure.
 *
 * This behaviour is very dependent on the particular back end used and internal
 * implementattion details of that back end.  It is not a unit test or a
 * reliable group or system test.
 */
TEST_F(GeisSubscriptionTests, duplicate_window_subscription)
{
  Geis geis2 = geis_new(GEIS_INIT_SYNCHRONOUS_START,
                        GEIS_INIT_NO_ATOMIC_GESTURES,
                        NULL);
  EXPECT_TRUE(geis2 != NULL) << "can not create second geis instance";
  GeisInteger fd;
  geis_get_configuration(geis2, GEIS_CONFIGURATION_FD, &fd);

  GeisSubscription sub1 = geis_subscription_new(geis_,
                                                "subscription 1",
                                                GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub1 != NULL) << "can not create first subscription";

  GeisSubscription sub2 = geis_subscription_new(geis2,
                                                "subscription 2",
                                                GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub1 != NULL) << "can not create second subscription";

  GeisFilter filter1 = geis_filter_new(geis_, "root window 1");
  EXPECT_TRUE(filter1 != NULL) << "can not create filter 1";

  GeisStatus fs = geis_filter_add_term(filter1,
               GEIS_FILTER_CLASS,
               GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_ROTATE,
               GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
               NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter 1";
  fs = geis_subscription_add_filter(sub1, filter1);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter 1";

  GeisFilter filter2 = geis_filter_new(geis2, "root window 2");
  EXPECT_TRUE(filter2 != NULL) << "can not create filter 2";

  fs = geis_filter_add_term(filter2,
               GEIS_FILTER_CLASS,
               GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_ROTATE,
               GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
               NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter 2";
  fs = geis_subscription_add_filter(sub2, filter2);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter 2";

  bool geis1_device_is_available = false;
  bool geis2_device_is_available = false;

  /*
   * Special handling for the second geis instance to make sure the device has
   * appeared before the subscription is activated, otherwise we might get a
   * false positive waiting for it to get asynchronously activated.
   */
  add_event_callback(fd, [&]()
  {
    geis_dispatch_events(geis2);
    GeisEvent  event;
    for (GeisStatus estatus = geis_next_event(geis2, &event);
         estatus == GEIS_STATUS_CONTINUE || estatus == GEIS_STATUS_SUCCESS;
         estatus = geis_next_event(geis2, &event))
    {
      if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
      {
        geis2_device_is_available = true;
      }
    }
  });

  /*
   * The actual test can sometimes fail if the captive device has not yet been
   * recognized by the GEIS instances because activating subscriptions require
   * grabbing the device on all matching (window, device) tuples.
   */
  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      geis1_device_is_available = true;
    }

    if (geis1_device_is_available && geis2_device_is_available)
    {
      EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub1))
                  << "can not activate subscription 1";
      EXPECT_NE(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub2))
                  << "mistakenly activated subscription 2";
      geis_dispatch_stop(true);
    }
  });

  geis_dispatch_loop();
  geis_subscription_delete(sub2);
  geis_subscription_delete(sub1);
  geis_delete(geis2);
}

/**
 * Regression test 1 for lp:934207: gesture rejection
 */
TEST_F(GeisSubscriptionTests, reject_gesture)
{
  GeisGestureId    rejected_gesture_id = 0;
  bool             gesture_rejected = false;
  GeisSubscription sub = geis_subscription_new(geis_,
                                               "subscription",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create subscription";
  GeisFilter filter = geis_filter_new(geis_, "root window");
  EXPECT_TRUE(filter != NULL) << "can not create filter";
  GeisStatus fs = geis_filter_add_term(filter,
               GEIS_FILTER_CLASS,
               GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_ROTATE,
               GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
               NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter";
  fs = geis_subscription_add_filter(sub, filter);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter";
  EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub))
              << "can not activate subscription";

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
      case GEIS_EVENT_GESTURE_UPDATE:
      {
        GeisAttr attr = geis_event_attr_by_name(event,
                                                GEIS_EVENT_ATTRIBUTE_GROUPSET);
        EXPECT_TRUE(attr != NULL) << "event is missing groupset attr";
        GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
        EXPECT_TRUE(groupset != NULL) << "event is missing groupset";
        for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
        {
          GeisGroup group = geis_groupset_group(groupset, i);
          EXPECT_TRUE(group != NULL) << "group " << i
                                     << " not found in groupset";

          for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
          {
            GeisFrame frame = geis_group_frame(group, j);
            EXPECT_TRUE(frame != NULL) << "frame " << j
                                       << " not found in group";

            if (geis_frame_is_class(frame, rotate_class_))
            {
              if (!gesture_rejected)
              {
                rejected_gesture_id = geis_frame_id(frame);
                GeisStatus gs = geis_gesture_reject(geis_,
                                                    group,
                                                    rejected_gesture_id);
                EXPECT_EQ(gs, GEIS_STATUS_SUCCESS) << "rejection failed";
                gesture_rejected = true;
              }
              else
              {
                EXPECT_NE(geis_frame_id(frame), rejected_gesture_id)
                          << "gesture events after gesture rejected";
                geis_dispatch_stop(true);
              }
            }
          }
        }
        break;
      }

      case GEIS_EVENT_GESTURE_END:
        geis_dispatch_stop(true);
        break;

      default:
        break;
    }
  });

  geis_dispatch_loop();
  EXPECT_EQ(gesture_rejected, true) << "gesture was never rejected";
  geis_subscription_delete(sub);
}


/**
 * Regression test 2 for lp:934207: gesture acceptance
 */
TEST_F(GeisSubscriptionTests, accept_gesture)
{
  bool gesture_accepted = false;
  bool gesture_ended = false;
  GeisSubscription sub = geis_subscription_new(geis_,
                                               "subscription",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create subscription";
  GeisFilter filter = geis_filter_new(geis_, "root window");
  EXPECT_TRUE(filter != NULL) << "can not create filter";
  GeisStatus fs = geis_filter_add_term(filter,
               GEIS_FILTER_CLASS,
               GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_ROTATE,
               GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
               NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter";
  fs = geis_subscription_add_filter(sub, filter);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter";
  EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub))
              << "can not activate subscription";

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
      case GEIS_EVENT_GESTURE_UPDATE:
      {
        GeisAttr attr = geis_event_attr_by_name(event,
                                                GEIS_EVENT_ATTRIBUTE_GROUPSET);
        EXPECT_TRUE(attr != NULL) << "event is missing groupset attr";
        GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
        EXPECT_TRUE(groupset != NULL) << "event is missing groupset";
        for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
        {
          GeisGroup group = geis_groupset_group(groupset, i);
          EXPECT_TRUE(group != NULL) << "group " << i
                                     << " not found in groupset";

          for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
          {
            GeisFrame frame = geis_group_frame(group, j);
            EXPECT_TRUE(frame != NULL) << "frame " << j
                                       << " not found in group";

            if (!gesture_accepted && geis_frame_is_class(frame, rotate_class_))
            {
              GeisStatus gs = geis_gesture_accept(geis_,
                                                  group,
                                                  geis_frame_id(frame));
              EXPECT_EQ(gs, GEIS_STATUS_SUCCESS) << "aceptance failed";
              gesture_accepted = true;
            }
          }
        }
        break;
      }

      case GEIS_EVENT_GESTURE_END:
        gesture_ended = true;
        break;

      default:
        break;
    }
  });

  geis_dispatch_loop();
  EXPECT_EQ(gesture_accepted, true) << "gesture was never accepted";
  EXPECT_EQ(gesture_ended, true) << "gesture did not end";
  geis_subscription_delete(sub);
}

/**
 * Regression test for lp:968736: Windows are sometimes not ungrabbed on
 * subscription deactivation.
 *
 * When a subscription is deactivated, the touch grab on the window must be
 * ungrabbed. This test deactivates a subscription and then tries to grab the
 * window through a different X connection.
 */
TEST_F(GeisSubscriptionTests, WindowUngrab)
{
  GeisSubscription sub = geis_subscription_new(geis_,
                                               "subscription",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create first subscription";

  GeisFilter filter = geis_filter_new(geis_, "root window");
  EXPECT_TRUE(filter != NULL) << "can not create filter";

  GeisStatus fs = geis_filter_add_term(filter, GEIS_FILTER_CLASS,
                                       GEIS_CLASS_ATTRIBUTE_NAME,
                                       GEIS_FILTER_OP_EQ, GEIS_GESTURE_ROTATE,
                                       GEIS_GESTURE_ATTRIBUTE_TOUCHES,
                                       GEIS_FILTER_OP_GT, 1, NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter";
  fs = geis_subscription_add_filter(sub, filter);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter";

  EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub))
              << "can not activate subscription";

  geis_subscription_deactivate(sub);

  ::Display* display2 = XOpenDisplay(NULL);
  
  XIEventMask mask;
  mask.deviceid = XIAllMasterDevices;
  mask.mask_len = XIMaskLen(XI_LASTEVENT);
  mask.mask = reinterpret_cast<unsigned char*>(calloc(mask.mask_len,
                                                      sizeof(char)));

  XISetMask(mask.mask, XI_TouchBegin);
  XISetMask(mask.mask, XI_TouchUpdate);
  XISetMask(mask.mask, XI_TouchEnd);
  XISetMask(mask.mask, XI_TouchOwnership);
  XISetMask(mask.mask, XI_HierarchyChanged);

  XIGrabModifiers mods = { XIAnyModifier, 0 };

  Window win = DefaultRootWindow(display2);

  XIGrabTouchBegin(display2, XIAllMasterDevices, win, 0, &mask, 1, &mods);

  free(mask.mask);

  EXPECT_EQ(XIGrabSuccess, mods.status);

  geis_subscription_delete(sub);
}
