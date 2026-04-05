/**
 * GTEst test suite for GEIS v2 device handling.
 *
 * Copyright 2012-2013 Canonical Ltd.
 */
#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include "gtest_geis_fixture.h"
#include "libgeis/geis_test_api.h"
#include <memory>


namespace
{

static const std::string TEST_DEVICE_PROP_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/device.prop");
static const std::string TEST_DEVICE_EVENTS_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/rotate90.record");

/**
 * Fixture for testing device handling.
 * This is a separate class because gtest uses Java reflection.
 */
class GeisBasicDeviceTests
: public GTestGeisFixture
{
  void
  setup_geis()
  {
    geis_ = geis_new(GEIS_INIT_SYNCHRONOUS_START,
                     GEIS_INIT_NO_ATOMIC_GESTURES,
                     GEIS_CONFIG_DISCARD_DEVICE_MESSAGES,
                     NULL);
  }
};


/**
 * Fixture for testing device handling.
 * This is a separate class because gtest uses Java reflection.
 */
class GeisAdvancedDeviceTests
: public GTestGeisFixture
{

  void
  setup_geis()
  {
    new_device_.reset(new Testsuite::EvemuDevice(TEST_DEVICE_PROP_FILE));
    geis_ = geis_new(GEIS_INIT_TRACK_DEVICES,
                     NULL);
    geis_initialized_in_callback_ = false;
    device_count_ = 0;
  }

public:
  std::unique_ptr<Testsuite::EvemuDevice> new_device_;
  bool                                    geis_initialized_in_callback_;
  int                                     device_count_;
  GeisSubscription                        subscription_;
};


TEST_F(GeisBasicDeviceTests, filterWithNoDevices)
{
  GeisSubscription sub = geis_subscription_new(geis_,
                                               "no devices",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create subscription";
  GeisFilter filter = geis_filter_new(geis_, "rotate");
  EXPECT_TRUE(filter != NULL) << "can not create filter";

  GeisStatus fs = geis_filter_add_term(filter,
            GEIS_FILTER_DEVICE,
            GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, GEIS_FILTER_OP_EQ, GEIS_TRUE,
            NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add device filter";

  geis_filter_delete(filter);
  geis_subscription_delete(sub);
}


/*
 * Test case 1 for lp:944822 -- device added after subscription activated.
 *
 * Creates a subscriptions with a device filter (there are no devices present in
 * the system).  When initializatiom signalled as complete, device events are
 * reenabled in the geis instance and a new device is added.  When the
 * device-added event is received, a recording is run through the device.
 * Gesture events should be detected.
 */
TEST_F(GeisBasicDeviceTests, addDeviceSubscription)
{
  std::unique_ptr<Testsuite::EvemuDevice> new_device;
  GeisBoolean device_has_been_created = GEIS_FALSE;
  GeisBoolean gesture_events_received = GEIS_FALSE;

  GeisSubscription sub = geis_subscription_new(geis_,
                                               "no devices",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create subscription";
  GeisFilter filter = geis_filter_new(geis_, "rotate");
  EXPECT_TRUE(filter != NULL) << "can not create filter";
  GeisStatus fs = geis_filter_add_term(filter,
            GEIS_FILTER_DEVICE,
            GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, GEIS_FILTER_OP_EQ, GEIS_TRUE,
            NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add device term";
  fs = geis_subscription_add_filter(sub, filter);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add device term";

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_INIT_COMPLETE:
      {
        EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub))
                    << "can not activate subscription";

        GeisBoolean off = GEIS_FALSE;
        geis_set_configuration(geis_,
                               GEIS_CONFIG_DISCARD_DEVICE_MESSAGES,
                               &off);
        new_device.reset(new Testsuite::EvemuDevice(TEST_DEVICE_PROP_FILE));
        device_has_been_created = GEIS_TRUE;
        break;
      }

     case GEIS_EVENT_DEVICE_AVAILABLE:
       new_device->play(TEST_DEVICE_EVENTS_FILE);
       break;

     case GEIS_EVENT_GESTURE_BEGIN:
     case GEIS_EVENT_GESTURE_UPDATE:
     {
       EXPECT_EQ(device_has_been_created, GEIS_TRUE)
         << "gesture events without device";
       gesture_events_received = device_has_been_created;
     }

     default:
        break;
    }
  });

  geis_dispatch_loop();
  EXPECT_TRUE(gesture_events_received) << "no gesture events received";
  EXPECT_TRUE(device_has_been_created) << "no device created";
  geis_subscription_delete(sub);
}


/*
 * Test case 2 for lp:944822 -- device removed after subscription activated.
 *
 * This test really just makes sure nothing segfaults on device removal.
 */
TEST_F(GeisBasicDeviceTests, removeDeviceSubscription)
{
  std::unique_ptr<Testsuite::EvemuDevice> new_device;

  GeisSubscription sub = geis_subscription_new(geis_,
                                               "remove devices",
                                               GEIS_SUBSCRIPTION_NONE);
  EXPECT_TRUE(sub != NULL) << "can not create subscription";
  GeisFilter filter = geis_filter_new(geis_, "rotate");
  EXPECT_TRUE(filter != NULL) << "can not create filter";
  GeisStatus fs = geis_filter_add_term(filter,
            GEIS_FILTER_DEVICE,
            GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, GEIS_FILTER_OP_EQ, GEIS_TRUE,
            NULL);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add device term";
  fs = geis_subscription_add_filter(sub, filter);
  EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add device term";

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_INIT_COMPLETE)
    {
      GeisBoolean off = GEIS_FALSE;
      geis_set_configuration(geis_,
                             GEIS_CONFIG_DISCARD_DEVICE_MESSAGES,
                             &off);
      new_device.reset(new Testsuite::EvemuDevice(TEST_DEVICE_PROP_FILE));
    }
    else if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(sub))
                    << "can not activate subscription";
      new_device.reset();
    }
    else if (geis_event_type(event) == GEIS_EVENT_DEVICE_UNAVAILABLE)
    {
      geis_dispatch_stop(true);
    }
  });

  geis_dispatch_loop();
  geis_subscription_delete(sub);
}


/*
 * Test case for lp:987539: report X and Y axis extents.
 *
 * This test creates a devicve with known X and Y extents and verifies the
 * extent attributes are reported with the expected values.
 */
TEST_F(GeisBasicDeviceTests, deviceAttrs)
{
  GeisBoolean off = GEIS_FALSE;
  geis_set_configuration(geis_, GEIS_CONFIG_DISCARD_DEVICE_MESSAGES, &off);

  Testsuite::EvemuDevice test_device(TEST_DEVICE_PROP_FILE);
  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
      EXPECT_TRUE(attr != NULL);
      GeisDevice device = (GeisDevice)geis_attr_value_to_pointer(attr);
      ASSERT_TRUE(test_device.name() == geis_device_name(device));

      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_MIN_X);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(0.0f, geis_attr_value_to_float(attr));
      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_MAX_X);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(9600.0f, geis_attr_value_to_float(attr));
      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_RES_X);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(0.0f, geis_attr_value_to_float(attr));
      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_MIN_Y);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(0.0f, geis_attr_value_to_float(attr));
      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_MAX_Y);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(7200.0f, geis_attr_value_to_float(attr));
      attr = geis_device_attr_by_name(device, GEIS_DEVICE_ATTRIBUTE_RES_Y);
      ASSERT_TRUE(attr != NULL);
      EXPECT_FLOAT_EQ(0.0f, geis_attr_value_to_float(attr));
    }
  });
  geis_dispatch_loop();
}


/**
 * GEIS event callback to verify lp:1252447.
 *
 * This bug reveals that subscriptions created in an event callback when the
 * INIT_COMPLETE event is received never get activated for devices that are
 * present at GEIS start time.
 */
static void
lp_1252447_event_callback(::Geis geis, GeisEvent event, void* context)
{
  GeisAdvancedDeviceTests* fixture = (GeisAdvancedDeviceTests*)context;
  switch (geis_event_type(event))
  {
    case GEIS_EVENT_INIT_COMPLETE:
    {
      fixture->geis_initialized_in_callback_ = true;
      fixture->subscription_ = geis_subscription_new(geis,
                                                     "any device",
                                                     GEIS_SUBSCRIPTION_NONE);
      GeisFilter filter = geis_filter_new(geis, "filter");
      geis_filter_add_term(filter,
                           GEIS_FILTER_CLASS,
                           GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, 2,
                           NULL);
      EXPECT_EQ(geis_subscription_add_filter(fixture->subscription_, filter),
                GEIS_STATUS_SUCCESS);
      EXPECT_EQ(geis_subscription_activate(fixture->subscription_),
                GEIS_STATUS_SUCCESS);
      break;
    }

    case GEIS_EVENT_DEVICE_AVAILABLE:
    {
      ++fixture->device_count_;
      GeisSize active_sub_count = 0;
      geis_subscription_get_configuration(fixture->subscription_,
                                          GEIS_CONFIG_NUM_ACTIVE_SUBSCRIPTIONS,
                                          &active_sub_count);
      EXPECT_GT(active_sub_count, 0);
      break;
    }

    default:
      break;
  }
}

TEST_F(GeisAdvancedDeviceTests, initialDeviceRegistrationWithCallbacks)
{
  geis_register_event_callback(geis_, lp_1252447_event_callback, this);
  geis_register_device_callback(geis_, lp_1252447_event_callback, this);

  set_geis_event_handler([&](Geis, GeisEvent event) { });
  geis_dispatch_loop();
  ASSERT_TRUE(geis_initialized_in_callback_);
  ASSERT_GT(device_count_, 0);
}


} // anonymous namespace
