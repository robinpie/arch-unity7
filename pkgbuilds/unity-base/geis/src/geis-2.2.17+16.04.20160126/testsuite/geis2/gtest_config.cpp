/**
 * Gtest test suite for GEIS v2 configuration items.
 *
 * Copyright 2012 Canonical Ltd.
 */

#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include "gtest_geis_fixture.h"
#include <gtest/gtest.h>


namespace
{

static const std::string TEST_DEVICE_PROP_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/device.prop");
static const std::string TEST_DEVICE_EVENTS_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/drag_with_3s_pause.record");

/**
 * Fixture for testing expected attributes.
 */
class Geis2ConfigTests
: public GTestGeisFixture
{
public:
  Geis2ConfigTests();
  void SetUp();
  void TearDown();
  void pre_event_handler(GeisEvent event);
  void post_event_handler(GeisEvent event);
  void memoizeDragClass(GeisEvent event);
  void recognizeDevice(GeisEvent event);

protected:
  Testsuite::EvemuDevice evemu_device_;
  bool                   device_is_ready_;
  bool                   recording_was_sent_;
  GeisSubscription       subscription_;
  GeisFilter             filter_;
  GeisGestureClass       drag_class_;
};


Geis2ConfigTests::
Geis2ConfigTests()
: evemu_device_(TEST_DEVICE_PROP_FILE),
  device_is_ready_(false),
  recording_was_sent_(false),
  subscription_(nullptr),
  filter_(nullptr),
  drag_class_(nullptr)
{ }

void Geis2ConfigTests::
SetUp()
{
  GTestGeisFixture::SetUp();
  subscription_ = geis_subscription_new(geis_, "config", GEIS_SUBSCRIPTION_NONE);
  ASSERT_TRUE(subscription_ != NULL) << "can not create subscription";
  filter_ = geis_filter_new(geis_, "config");
  ASSERT_TRUE(filter_ != NULL) << "can not create filter";
  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_filter_add_term(filter_,
               GEIS_FILTER_CLASS,
               GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, GEIS_GESTURE_DRAG,
               GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
               NULL))
            << "can not add class to filter";
  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_subscription_add_filter(subscription_, filter_))
            << "can not subscribe filter";
  ASSERT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(subscription_))
            <<"can not activate subscription";
}

void Geis2ConfigTests::
TearDown()
{
  geis_subscription_delete(subscription_);
  GTestGeisFixture::TearDown();
}

void Geis2ConfigTests::
pre_event_handler(GeisEvent event)
{
  if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
  {
    recognizeDevice(event);
  }
  else if (geis_event_type(event) == GEIS_EVENT_CLASS_AVAILABLE)
  {
    memoizeDragClass(event);
  }
}

void Geis2ConfigTests::
post_event_handler(GeisEvent event)
{
  GTestGeisFixture::post_event_handler(event);
  if (geis_is_initialized() && device_is_ready_ && !recording_was_sent_)
  {
    evemu_device_.play(TEST_DEVICE_EVENTS_FILE);
    recording_was_sent_ = true;
  }
}

void Geis2ConfigTests::
memoizeDragClass(GeisEvent event)
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
      if (0 == strcmp(geis_attr_value_to_string(attr2), GEIS_GESTURE_DRAG))
      {
        drag_class_ = gesture_class;
      }
    }
  }
}

void Geis2ConfigTests::
recognizeDevice(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  EXPECT_TRUE(attr != NULL);
  GeisDevice device = (GeisDevice)geis_attr_value_to_pointer(attr);
  if (evemu_device_.name() == geis_device_name(device))
  {
    device_is_ready_ = true;
  }
}


/*
 * Tries to recognize a drag that takes too long and fails.  This uses the
 * default drag timeout property and proves the recorded gesture is not
 * recognized as a drag.
 */
TEST_F(Geis2ConfigTests, dragWithLongTimeoutXFail)
{
  bool recognized_gesture = false;

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_GESTURE_BEGIN)
    {
      GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
      EXPECT_TRUE(attr != NULL) << "event is missing groupset attr";

      GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
      EXPECT_TRUE(groupset != NULL) << "event is missing groupset";

      for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
      {
        GeisGroup group = geis_groupset_group(groupset, i);
        EXPECT_TRUE(group != NULL) << "group " << i << " not found in groupset";

        for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
        {
          GeisFrame frame = geis_group_frame(group, j);
          EXPECT_TRUE(frame != NULL) << "frame " << j << " not found in group";

          if (geis_frame_is_class(frame, drag_class_))
          {
            recognized_gesture = GEIS_TRUE;
            geis_dispatch_stop(true);
          }
        }
      }
    }
  });

  geis_dispatch_loop();
  EXPECT_FALSE(recognized_gesture);
}

/*
 * Tries to recognize the same drag that takes too long and succeeds because the
 * timeout has been adjusted.
 */
TEST_F(Geis2ConfigTests, dragWithLongTimeout)
{
  bool recognized_gesture = false;

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_GESTURE_BEGIN
     || geis_event_type(event) == GEIS_EVENT_GESTURE_UPDATE)
    {
      GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
      EXPECT_TRUE(attr != NULL) << "event is missing groupset attr";

      GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
      EXPECT_TRUE(groupset != NULL) << "event is missing groupset";

      for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
      {
        GeisGroup group = geis_groupset_group(groupset, i);
        EXPECT_TRUE(group != NULL) << "group " << i << " not found in groupset";

        for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
        {
          GeisFrame frame = geis_group_frame(group, j);
          EXPECT_TRUE(frame != NULL) << "frame " << j << " not found in group";

          if (geis_frame_is_class(frame, drag_class_))
          {
            recognized_gesture = GEIS_TRUE;
            geis_dispatch_stop(true);
          }
        }
      }
    }
    if (geis_is_initialized() && device_is_ready_)
    {
      GeisInteger timeout = 4000; // 4 s should be good enough
      EXPECT_EQ(GEIS_STATUS_SUCCESS,
                geis_subscription_set_configuration(subscription_,
                                                    GEIS_CONFIG_DRAG_TIMEOUT,
                                                    &timeout))
                << "can not set drag timeout";
    }
  });

  geis_dispatch_loop();
  EXPECT_TRUE(recognized_gesture);
}

} // anonymous namespace

