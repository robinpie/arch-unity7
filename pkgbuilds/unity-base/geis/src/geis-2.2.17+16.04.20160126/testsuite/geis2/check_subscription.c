/**
 * Unit tests for GEIS v2.0 subsciption module
 */
#include <check.h>

#include <geis/geis.h>
#include "libgeis/geis_test_api.h"
#include <string.h>


/* fixtures */
static Geis g_geis;

/* fixture setup */
static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND,
                    GEIS_INIT_TRACK_DEVICES,
                    GEIS_INIT_TRACK_GESTURE_CLASSES,
                    NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(g_geis);
}


/* compile-time test to ensure required types are defined */
START_TEST(subscription_constants)
{
  GeisSubscriptionFlags f;
  f = GEIS_SUBSCRIPTION_NONE
   | GEIS_SUBSCRIPTION_GRAB
   | GEIS_SUBSCRIPTION_CONT;
}
END_TEST

START_TEST(construction)
{
  GeisSubscription sub = geis_subscription_new(g_geis,
                                               "name",
                                               GEIS_SUBSCRIPTION_NONE);
  fail_unless(sub != NULL,
              "failed to create subscription");
  fail_unless(0 == strcmp(geis_subscription_name(sub), "name"),
              "unexpected subscription name returned");
  fail_unless(GEIS_STATUS_SUCCESS == geis_subscription_activate(sub),
              "unable to activate subscription");
  fail_unless(GEIS_STATUS_SUCCESS == geis_subscription_deactivate(sub),
              "unable to deactivate subscription");
  geis_subscription_delete(sub);
}
END_TEST


START_TEST(filter)
{
  GeisFilter filter = geis_filter_new(g_geis, "filter");
  GeisSubscription sub = geis_subscription_new(g_geis,
                                               "name",
                                               GEIS_SUBSCRIPTION_NONE);
  fail_unless(GEIS_STATUS_SUCCESS == geis_subscription_add_filter(sub, filter),
              "filter add fail");

  filter = geis_subscription_filter_by_name(sub, "bogus");
  fail_unless(filter == NULL,
              "bogus filter retrieval fail");
  filter = geis_subscription_filter_by_name(sub, "filter");
  fail_unless(filter != NULL,
              "filter retrieval fail");

  fail_unless(GEIS_STATUS_SUCCESS == geis_subscription_remove_filter(sub, filter),
              "filter remove fail");
}
END_TEST


START_TEST(device_filter)
{
  GeisStatus status;
  GeisEvent event;

  GeisSubscription sub = geis_subscription_new(g_geis,
                                               "devices",
                                               GEIS_SUBSCRIPTION_NONE);

  while (geis_dispatch_events(g_geis) == GEIS_STATUS_CONTINUE)
    ;
  status = geis_next_event(g_geis, &event);
  while (status == GEIS_STATUS_SUCCESS || status == GEIS_STATUS_CONTINUE)
  {
    if (geis_event_type(event) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      GeisAttr   attr;
      GeisDevice device;
      GeisFilter filter;
      GeisStatus fs;

      attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
      fail_if (!attr, "geis-device attr not found in device event");

      device = geis_attr_value_to_pointer(attr);
      fail_if (!device, "geis device not found in device event");

      filter = geis_filter_new(g_geis, "device filter");
      fail_if(!filter, "can not create filter");

      fs = geis_filter_add_term(filter, GEIS_FILTER_DEVICE,
          GEIS_DEVICE_ATTRIBUTE_NAME, GEIS_FILTER_OP_NE, geis_device_name(device),
          NULL);
      fail_if(fs != GEIS_STATUS_SUCCESS, "can not add device to filter");

      fs = geis_subscription_add_filter(sub, filter);
      fail_if(fs != GEIS_STATUS_SUCCESS, "can not subscribe filter");
    }
    else if (geis_event_type(event) == GEIS_EVENT_CLASS_AVAILABLE)
    {
      GeisAttr         attr;
      GeisGestureClass gesture_class;
      GeisFilter       filter;
      GeisStatus       fs;

      attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
      fail_if (!attr, "attr not found in class event");

      gesture_class = geis_attr_value_to_pointer(attr);
      fail_if (!gesture_class, "geis class not found in class event");

      filter = geis_filter_new(g_geis, "class filter");
      fail_if(!filter, "can not create filter");

      fs = geis_filter_add_term(filter, GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_NE, geis_gesture_class_name(gesture_class),
          NULL);
      fail_if(fs != GEIS_STATUS_SUCCESS, "can not add class to filter");

      fs = geis_subscription_add_filter(sub, filter);
      fail_if(fs != GEIS_STATUS_SUCCESS, "can not subscribe filter");
    }
    geis_event_delete(event);

    if (status == GEIS_STATUS_CONTINUE)
    {
      status = geis_next_event(g_geis, &event);
    }
    else
    {
      break;
    }
  }

  fail_unless(GEIS_STATUS_SUCCESS == geis_subscription_activate(sub),
              "unable to activate subscription");
}
END_TEST


/* boilerplate */
Suite *
geis2_subscription_suite_new()
{
  TCase *create;
  TCase *usage;
  Suite *s = suite_create("geis2_subscriptions");

  create = tcase_create("subscription-constants");
  tcase_add_test(create, subscription_constants);
  suite_add_tcase(s, create);

  usage = tcase_create("subscription-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, construction);
  tcase_add_test(usage, filter);
  tcase_add_test(usage, device_filter);
  suite_add_tcase(s, usage);

  return s;
}

