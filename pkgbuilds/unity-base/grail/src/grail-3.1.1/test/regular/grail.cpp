#include <gtest/gtest.h>

#include "oif/grail.h"

TEST(grail, grail) {
  UGHandle grail_handle = NULL;

  ASSERT_EQ(UGStatusSuccess, grail_new(&grail_handle));

  int fd = grail_get_fd(grail_handle);

  RecordProperty("fd", fd);

  UGEvent event = NULL;
  EXPECT_EQ(UGStatusErrorNoEvent, grail_get_event(grail_handle, &event));

  EXPECT_EQ(
      UGStatusErrorInvalidGesture,
      grail_accept_gesture(grail_handle,
                           UGGestureTypeDrag | UGGestureTypePinch |
                           UGGestureTypeRotate | UGGestureTypeTap));
  EXPECT_EQ(UGStatusErrorInvalidGesture,
            grail_accept_gesture(grail_handle, 0));

  UGSubscription subscription;

  ASSERT_EQ(UGStatusSuccess, grail_subscription_new(&subscription));
  unsigned int gesture_mask = UGGestureTypeDrag | UGGestureTypePinch
      | UGGestureTypeRotate | UGGestureTypeTap;
  EXPECT_EQ(
      UGStatusSuccess,
      grail_subscription_set_property(subscription, UGSubscriptionPropertyMask,
                                      &gesture_mask));

  unsigned int min_touches = 2;
  EXPECT_EQ(
      UGStatusSuccess,
      grail_subscription_set_property(subscription,
                                      UGSubscriptionPropertyTouchesMinimum,
                                      &min_touches));

  unsigned int max_touches = 2;
  EXPECT_EQ(
      UGStatusSuccess,
      grail_subscription_set_property(subscription,
                                      UGSubscriptionPropertyTouchesMaximum,
                                      &max_touches));

  unsigned int touches_start = 2;
  EXPECT_EQ(
      UGStatusSuccess,
      grail_subscription_set_property(subscription,
                                      UGSubscriptionPropertyTouchesStart,
                                      &touches_start));

  /* FIXME: Need to create a device and set it in the subscription */
  /*ASSERT_EQ(UGStatusSuccess,
            grail_subscription_activate(grail_handle, subscription)); */

  /* FIXME: Need to generate frame events and send them through */
  /*EXPECT_EQ(UGStatusSuccess,
            grail_accept_gesture(grail_handle, 0));*/

  grail_subscription_delete(subscription);
  grail_delete(grail_handle);
}
