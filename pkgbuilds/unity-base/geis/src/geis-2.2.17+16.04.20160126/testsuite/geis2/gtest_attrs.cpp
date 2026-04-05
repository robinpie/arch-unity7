/**
 * Gtest test suite for GEIS v2 attributes.
 *
 * Copyright 2012 Canonical Ltd.
 */

#include <functional>
#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include "gtest_geis_fixture.h"
#include <gtest/gtest.h>


namespace
{

static const std::string TEST_DEVICE_PROP_FILE(
    TEST_ROOT_DIR "recordings/touchscreen_a/device.prop");

/**
 * A helper class to use RAII with geis objects.
 *
 * This is a separate fixture from the gtest fixture because it is parameterized
 * for each test.
 */
class TestSubscription
{
public:
  TestSubscription(Geis geis, GeisString gesture_class)
  {
    subscription_ = geis_subscription_new(geis,
                                          "attr tests",
                                           GEIS_SUBSCRIPTION_NONE);
    EXPECT_TRUE(subscription_ != NULL) << "can not create subscription";

    filter_ = geis_filter_new(geis, "attr tests");
    EXPECT_TRUE(filter_ != NULL) << "can not create filter";

    GeisStatus fs = geis_filter_add_term(filter_,
                 GEIS_FILTER_CLASS,
                 GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, gesture_class,
                 GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
                 NULL);
    EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not add class to filter";

    fs = geis_subscription_add_filter(subscription_, filter_);
    EXPECT_EQ(fs, GEIS_STATUS_SUCCESS) << "can not subscribe filter";

    EXPECT_EQ(GEIS_STATUS_SUCCESS, geis_subscription_activate(subscription_))
                << "can not activate subscription";
  }

  ~TestSubscription()
  {
  }

private:
  GeisSubscription       subscription_;
  GeisFilter             filter_;
};


/**
 * Fixture for testing expected attributes.  This has to be a separate class
 * because of the way Java reflection is used in jUnit.
 */
class GeisAttributeTests
: public GTestGeisFixture
{
public:
  GeisAttributeTests()
  : evemu_device_(TEST_DEVICE_PROP_FILE),
    device_is_ready_(false),
    recording_was_sent_(false)
  { }

  void
  set_recording_file_name(const std::string& file_name)
  {
    recording_file_name_ = file_name;
  }

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
  }

  void 
  post_event_handler(GeisEvent event)
  {
    GTestGeisFixture::post_event_handler(event);
    if (geis_is_initialized() && device_is_ready_ && !recording_was_sent_)
    {
      evemu_device_.play(recording_file_name_);
      recording_was_sent_ = true;
    }
  }

protected:
  Testsuite::EvemuDevice evemu_device_;
  bool                   device_is_ready_;
  std::string            recording_file_name_;
  bool                   recording_was_sent_;
};


/*
 * Regression test for lp:394398: rotation angle is swapped with rotation angle
 * delta.
 *
 * The captive "rotate90" events whould produce a rotation gesture with a final
 * rotation angle of somewhere around -pi/2 radians.
 */
TEST_F(GeisAttributeTests, rotate90)
{
  bool found_angle = false;
  TestSubscription dragSubscription(geis_, GEIS_GESTURE_ROTATE);
  set_recording_file_name(TEST_ROOT_DIR "recordings/touchscreen_a/rotate90.record");

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    if (geis_event_type(event) == GEIS_EVENT_GESTURE_END)
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

          GeisAttr attr = geis_frame_attr_by_name(frame,
                                                  GEIS_GESTURE_ATTRIBUTE_ANGLE);
          EXPECT_TRUE(attr != NULL) << "angle attribute not found in frame";

          GeisFloat angle = geis_attr_value_to_float(attr);
          EXPECT_NEAR(angle, -1.6f, 0.4f);
          found_angle = GEIS_TRUE;
          return;
        }
      }
    }
  });

  geis_dispatch_loop();
  EXPECT_TRUE(found_angle);
}

/*
 * Regression test for lp:967267: Position delta attributes incorrect when touch
 * count changes.
 *
 * Some gestures continue when the number of touches changes. When this happens,
 * much of the gesture state is reset. This includes the original center and
 * cumulative transformation matrix.
 *
 * This test ensures the position delta attributes stay in a reasonable range
 * after a two to one touch drag transition.
 */
TEST_F(GeisAttributeTests, TouchChangePositionDelta)
{
  bool found_delta = false;
  TestSubscription dragSubscription(geis_, GEIS_GESTURE_DRAG);
  set_recording_file_name(TEST_ROOT_DIR "recordings/touchscreen_a/drag_2_to_1.record");

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
      case GEIS_EVENT_GESTURE_UPDATE:
      case GEIS_EVENT_GESTURE_END:
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

            GeisAttr attr = geis_frame_attr_by_name(frame,
                                                    GEIS_GESTURE_ATTRIBUTE_DELTA_X);
            EXPECT_TRUE(attr != NULL) << "angle attribute not found in frame";

            GeisFloat delta = geis_attr_value_to_float(attr);

            /* The drag is nearly vertical. All X deltas should be within 0.2%
             * of the screen width. On failure, the delta can vary up to 20%
             * of the screen width. */
            int width = WidthOfScreen(DefaultScreenOfDisplay(Display()));
            EXPECT_NEAR(delta, 0, 0.01 * width);
            found_delta = GEIS_TRUE;
            geis_dispatch_stop(true);
          }
        }
        break;
      }
    }
  });

  geis_dispatch_loop();
  EXPECT_TRUE(found_delta);
}

/*
 * Regression test for lp:985916: Position deltas are incorrect when synchronous
 * events are not enabled.
 *
 * The position deltas should add up to the entire position change from gesture
 * begin to end.
 *
 * This test ensures the X position delta attributes add up to the difference
 * between the inital X position and the final X position.
 */
TEST_F(GeisAttributeTests, PositionDelta)
{
  set_recording_file_name(TEST_ROOT_DIR "recordings/touchscreen_a/drag_2.record");
  TestSubscription dragSubscription(geis_, GEIS_GESTURE_DRAG);
  GeisFloat initial_x;
  GeisFloat cumulative_delta_x = 0;

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
      case GEIS_EVENT_GESTURE_UPDATE:
      case GEIS_EVENT_GESTURE_END:
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

            GeisAttr attr = geis_frame_attr_by_name(frame,
                                                    GEIS_GESTURE_ATTRIBUTE_DELTA_X);
            EXPECT_TRUE(attr != NULL) << "delta X attribute not found in frame";

            cumulative_delta_x += geis_attr_value_to_float(attr);

            if (geis_event_type(event) == GEIS_EVENT_GESTURE_BEGIN)
            {
              attr = geis_frame_attr_by_name(
                  frame,
                  GEIS_GESTURE_ATTRIBUTE_POSITION_X);
              initial_x = geis_attr_value_to_float(attr);
            }
            if (geis_event_type(event) == GEIS_EVENT_GESTURE_END)
            {
              attr = geis_frame_attr_by_name(
                  frame,
                  GEIS_GESTURE_ATTRIBUTE_POSITION_X);
              GeisFloat x = geis_attr_value_to_float(attr);

              EXPECT_FLOAT_EQ(x - initial_x, cumulative_delta_x);
            }
          }
        }
        break;
      }
    }
  });

  geis_dispatch_loop();
}

/*
 * Regression test for lp:986215: Radius delta values should be a ratio instead
 * of a difference.
 *
 * This test ensures the radius delta attributes multiplied together equal the
 * final radius value.
 */
TEST_F(GeisAttributeTests, RadiusDelta)
{
  TestSubscription dragSubscription(geis_, GEIS_GESTURE_PINCH);
  set_recording_file_name(TEST_ROOT_DIR "recordings/touchscreen_a/pinch_2.record");
  GeisFloat cumulative_delta_radius = 1;

  set_geis_event_handler([&](Geis, GeisEvent event)
  {
      switch (geis_event_type(event))
      {
        case GEIS_EVENT_GESTURE_BEGIN:
        case GEIS_EVENT_GESTURE_UPDATE:
        case GEIS_EVENT_GESTURE_END:
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

              GeisAttr attr = geis_frame_attr_by_name(frame,
                                                      GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA);
              EXPECT_TRUE(attr != NULL) << "radius attribute not found in frame";

              cumulative_delta_radius *= geis_attr_value_to_float(attr);

              if (geis_event_type(event) == GEIS_EVENT_GESTURE_END)
              {
                attr = geis_frame_attr_by_name(
                    frame,
                    GEIS_GESTURE_ATTRIBUTE_RADIUS);
                GeisFloat radius = geis_attr_value_to_float(attr);

                EXPECT_FLOAT_EQ(radius, cumulative_delta_radius);
              }
            }
          }
          break;
      }
    });

  geis_dispatch_loop();
}

} // anonymous namespace
