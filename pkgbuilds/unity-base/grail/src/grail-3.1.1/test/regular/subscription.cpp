#include <cmath>

#include <gtest/gtest.h>

#include <oif/grail.h>

TEST(grail, default_subscription) {
  UGSubscription subscription;

  ASSERT_EQ(UGStatusSuccess, grail_subscription_new(&subscription));

  UGGestureTypeMask gesture_type;
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyMask,
                                            &gesture_type));

  EXPECT_EQ(gesture_type, 0);

  unsigned int count = 0;
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyTouchesStart,
                                            &count));
  EXPECT_EQ(count, 2);

  /* 0 means take the value from the touches start property */
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyTouchesMinimum,
                &count));
  EXPECT_EQ(count, 0);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyTouchesMaximum,
                &count));
  EXPECT_EQ(count, 0);

  grail_subscription_delete(subscription);
}

TEST(grail, get_set_subscription) {
  UGSubscription subscription;

  ASSERT_EQ(UGStatusSuccess, grail_subscription_new(&subscription));

  const UGGestureTypeMask new_gesture_type = UGGestureTypeDrag |
                                             UGGestureTypeTap;
  const unsigned int new_count = 3;
  UGGestureType gesture_type;
  unsigned int count;

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyMask,
                                            &new_gesture_type));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyMask,
                                            &gesture_type));
  EXPECT_EQ(new_gesture_type, gesture_type);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyTouchesStart,
                                            &new_count));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyTouchesStart,
                                            &count));
  EXPECT_EQ(new_count, count);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(
                subscription,
                UGSubscriptionPropertyTouchesMinimum,
                &new_count));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyTouchesMinimum,
                &count));
  EXPECT_EQ(new_count, count);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(
                subscription,
                UGSubscriptionPropertyTouchesMaximum,
                &new_count));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyTouchesMaximum,
                &count));
  EXPECT_EQ(new_count, count);

  grail_subscription_delete(subscription);
}

TEST(grail, default_subscription_limits) {
  UGSubscription subscription;

  ASSERT_EQ(UGStatusSuccess, grail_subscription_new(&subscription));

  uint64_t timeout;
  float threshold;

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyDragTimeout,
                                            &timeout));
  EXPECT_EQ(300, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyDragThreshold,
                                            &threshold));
  EXPECT_FLOAT_EQ(0.0026, threshold);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyPinchTimeout,
                                            &timeout));
  EXPECT_EQ(300, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyPinchThreshold,
                &threshold));
  EXPECT_FLOAT_EQ(1.1, threshold);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyRotateTimeout,
                                            &timeout));
  EXPECT_EQ(500, timeout);
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyRotateThreshold,
                &threshold));
  EXPECT_FLOAT_EQ(2 * M_PI / 50, threshold);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyTapTimeout,
                                            &timeout));
  EXPECT_EQ(300, timeout);

  grail_subscription_delete(subscription);
}

TEST(grail, set_get_subscription_limits) {
  UGSubscription subscription;

  ASSERT_EQ(UGStatusSuccess, grail_subscription_new(&subscription));

  /* Adjust values and check if adjusted values are reported back correctly. */
  const uint64_t new_timeout = 1000;
  const float new_threshold = 7.3;
  uint64_t timeout;
  float threshold;

  /* Drag */
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyDragTimeout,
                                            &new_timeout));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyDragTimeout,
                                            &timeout));
  EXPECT_EQ(new_timeout, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyDragThreshold,
                                            &new_threshold));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyDragThreshold,
                                            &threshold));
  EXPECT_FLOAT_EQ(new_threshold, threshold);

  /* Pinch */
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyPinchTimeout,
                                            &new_timeout));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyPinchTimeout,
                                            &timeout));
  EXPECT_EQ(new_timeout, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(
                subscription,
                UGSubscriptionPropertyPinchThreshold,
                &new_threshold));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyPinchThreshold,
                &threshold));
  EXPECT_FLOAT_EQ(new_threshold, threshold);

  /* Rotate */
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyRotateTimeout,
                                            &new_timeout));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyRotateTimeout,
                                            &timeout));
  EXPECT_EQ(new_timeout, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(
                subscription,
                UGSubscriptionPropertyRotateThreshold,
                &new_threshold));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(
                subscription,
                UGSubscriptionPropertyRotateThreshold,
                &threshold));
  EXPECT_FLOAT_EQ(new_threshold, threshold);

  /* Tap */
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyTapTimeout,
                                            &new_timeout));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyTapTimeout,
                                            &timeout));
  EXPECT_EQ(new_timeout, timeout);

  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_set_property(subscription,
                                            UGSubscriptionPropertyTapThreshold,
                                            &new_threshold));
  EXPECT_EQ(UGStatusSuccess,
            grail_subscription_get_property(subscription,
                                            UGSubscriptionPropertyTapThreshold,
                                            &threshold));
  EXPECT_FLOAT_EQ(new_threshold, threshold);

  grail_subscription_delete(subscription);
}
