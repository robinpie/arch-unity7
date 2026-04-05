/**
 * Gtest test suite for GEIS v1 subscriptions.
 *
 * Copyright 2012 Canonical Ltd.
 */

#include "geis/geis.h"
#include "gtest_geis1_fixture.h"
#include <gtest/gtest.h>


namespace
{

/**
 * Fixture for testing expected attributes.  This has to be a separate class
 * because of the way Java reflection is used in jUnit.
 */
class Geis1SubscriptionTests
: public GTestGeis1Fixture
{
};

void gesture_null_func(void* cookie, GeisGestureType type, GeisGestureId id,
                       GeisSize count, GeisGestureAttr* attrs) {
}


/*
 * Regression test for lp:936815.
 *
 * Ths test would segfault in geis_filter_delete() before the fix for lp:936815
 * was applied.  The problem only obtained when more than one gesture type was
 * subscribed.
 *
 */
TEST_F(Geis1SubscriptionTests, basic)
{
  static const char* gestures[] =
  {
    GEIS_GESTURE_TYPE_DRAG3,
    GEIS_GESTURE_TYPE_TAP3,
    GEIS_GESTURE_TYPE_ROTATE3,
    GEIS_GESTURE_TYPE_PINCH3,
    GEIS_GESTURE_TYPE_TOUCH3,
    GEIS_GESTURE_TYPE_DRAG4,
    GEIS_GESTURE_TYPE_TAP4,
    GEIS_GESTURE_TYPE_ROTATE4,
    GEIS_GESTURE_TYPE_PINCH4,
    GEIS_GESTURE_TYPE_TOUCH4,
    GEIS_GESTURE_TYPE_SYSTEM,
    NULL
  };

  static GeisGestureFuncs callbacks = {
    &gesture_null_func,
    &gesture_null_func,
    &gesture_null_func,
    &gesture_null_func,
    &gesture_null_func,
  };

  ASSERT_EQ(GEIS_STATUS_SUCCESS,
            geis_subscribe(geis(), GEIS_ALL_INPUT_DEVICES, gestures, &callbacks,
                           this));
}

} // anonymous namespace
