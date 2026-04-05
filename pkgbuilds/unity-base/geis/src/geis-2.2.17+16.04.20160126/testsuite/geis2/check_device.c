/**
 * Unit tests for GEIS v2.0 Input Device Module.
 */
#include <check.h>

#include <geis/geis.h>
#include "libgeis/geis_test_api.h"


/* fixtures */
static Geis g_geis;

/* fixture setup */
static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, GEIS_INIT_TRACK_DEVICES, NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(g_geis);
}


/* Compile-time test to ensure types and constants are defined */
START_TEST(geis_device_types)
{
  GeisString attr_name CK_ATTRIBUTE_UNUSED;

  /* Types */
  GeisEventType type CK_ATTRIBUTE_UNUSED;

  /* 5.3 Events */
  attr_name = GEIS_EVENT_ATTRIBUTE_DEVICE;

  /* 5.1.2 Device Attributes */
  attr_name = GEIS_DEVICE_ATTRIBUTE_NAME;
  attr_name = GEIS_DEVICE_ATTRIBUTE_ID;
  attr_name = GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH;
  attr_name = GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH;
  attr_name = GEIS_DEVICE_ATTRIBUTE_TOUCHES;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_device_functions)
{
  Geis geis = NULL;
  GeisEventCallback callback = 0;
  GeisDevice device = NULL;
  GeisString n CK_ATTRIBUTE_UNUSED;
  GeisInteger i CK_ATTRIBUTE_UNUSED;
  GeisSize s CK_ATTRIBUTE_UNUSED;
  GeisAttr a CK_ATTRIBUTE_UNUSED;

  geis_register_device_callback(geis, callback, NULL);

  geis_device_ref(device);
  geis_device_unref(device);
  n = geis_device_name(device);
  i = geis_device_id(device);
  s = geis_device_attr_count(device);
  a = geis_device_attr(device, 0);
}
END_TEST

START_TEST(receive_events)
{
  GeisStatus status;
  GeisEvent  event_out;
  int        device_event_count = 0;

  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  status = geis_next_event(g_geis, &event_out);
  while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    if (geis_event_type(event_out) == GEIS_EVENT_DEVICE_AVAILABLE)
    {
      ++device_event_count;
    }
    geis_event_delete(event_out);
    status = geis_next_event(g_geis, &event_out);
  }
  fail_unless(device_event_count > 0, "no device events received");

  GeisDevice failer = geis_get_device(g_geis, 123);
  fail_unless(failer == NULL, "unexpected device retrieved");

  GeisDevice gooder = geis_get_device(g_geis, 0);
  fail_unless(gooder != NULL, "failed to retrive cached device");
}
END_TEST


/* boilerplate */
Suite *
geis2_device_suite_new()
{
  TCase *device;
  TCase *usage;
  Suite *s = suite_create("geis2_device");

  device = tcase_create("device-api");
  tcase_add_test(device, geis_device_types);
  suite_add_tcase(s, device);

  usage = tcase_create("device-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, receive_events);
  suite_add_tcase(s, usage);

  return s;
}

