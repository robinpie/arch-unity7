/**
 * Gtest test suite for GEIS v1 attributes.
 *
 * Copyright 2012 Canonical Ltd.
 */

#include <functional>
#include "geis/geis.h"
#include "gtest_evemu_device.h"
#include "gtest_geis1_fixture.h"
#include <gtest/gtest.h>
#include <mutex>
#include <sys/select.h>
#include <sys/time.h>


namespace
{

const std::string TEST_DEVICE_PROP_FILE(TEST_ROOT_DIR "recordings/apple_magic_trackpad/device.prop");
const std::string TEST_DEVICE_EVENTS_FILE(TEST_ROOT_DIR "recordings/apple_magic_trackpad/four_tap.record");

/**
 * Fixture for testing expected attributes.  This has to be a separate class
 * because of the way Java reflection is used in jUnit.
 */
class Geis1AttributeTests
: public GTestGeis1Fixture
{
public:
  Geis1AttributeTests()
  : evemu_device_(TEST_DEVICE_PROP_FILE),
    saw_four_tap(false)
  { }

  void GestureUpdate(GeisGestureType type, GeisGestureId id, GeisSize count,
                     GeisGestureAttr* attrs);

protected:
  Testsuite::EvemuDevice evemu_device_;
  bool saw_four_tap;
};

void gesture_begin(void* cookie, GeisGestureType type, GeisGestureId id,
                   GeisSize count, GeisGestureAttr* attrs) {
  FAIL() << "received unexpected gesture begin event";
}

void gesture_update(void* cookie, GeisGestureType type, GeisGestureId id,
                    GeisSize count, GeisGestureAttr* attrs) {
  Geis1AttributeTests* fixture = reinterpret_cast<Geis1AttributeTests*>(cookie);
  fixture->GestureUpdate(type, id, count, attrs);
}

void gesture_end(void* cookie, GeisGestureType type, GeisGestureId id,
                 GeisSize count, GeisGestureAttr* attrs) {
  FAIL() << "received unexpected gesture end event";
}

void gesture_null_func(void* cookie, GeisGestureType type, GeisGestureId id,
                       GeisSize count, GeisGestureAttr* attrs) {
}

void Geis1AttributeTests::GestureUpdate(GeisGestureType type, GeisGestureId id,
                                        GeisSize count,
                                        GeisGestureAttr* attrs) {
  EXPECT_EQ(GEIS_GESTURE_PRIMITIVE_TAP, type);

  EXPECT_FALSE(saw_four_tap);

  for (int i = 0; i < count; i++) {
    GeisGestureAttr attr = attrs[i];
    if (strcmp(attr.name, GEIS_GESTURE_ATTRIBUTE_TOUCHES) == 0) {
      ASSERT_EQ(4, attr.integer_val);
      saw_four_tap = true;
      return;
    }
  }
}

/*
 * Regression test for lp:957331, lp:957334, and lp:957344: geis v1 tap handling
 *
 * A geis v1 four tap system subscription is created, and a four tap gesture is
 * replayed on a touchpad device.
 *
 * Expected: One four touch tap gesture update event is received. No other
 * events should be seen.
 */
TEST_F(Geis1AttributeTests, tap_touch_count)
{
  static const char* gestures[] = {
    GEIS_GESTURE_TYPE_TAP4,
    GEIS_GESTURE_TYPE_SYSTEM,
  };

  static GeisGestureFuncs callbacks = {
    &gesture_null_func,
    &gesture_null_func,
    &gesture_begin,
    &gesture_update,
    &gesture_update,
  };

  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_subscribe(geis(), GEIS_ALL_INPUT_DEVICES, gestures, &callbacks,
                           this));

  evemu_device_.play(TEST_DEVICE_EVENTS_FILE);

  while (1) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(geis_fd(), &read_fds);
    timeval tmo = { 1, 0 };
    int sstat = select(geis_fd() + 1, &read_fds, NULL, NULL, &tmo);
    EXPECT_GT(sstat, -1) << "error in select";
    if (sstat == 0)
      break;
    ASSERT_EQ(GEIS_STATUS_SUCCESS, geis_event_dispatch(geis()));
  }

  EXPECT_TRUE(saw_four_tap);
}

} // anonymous namespace
