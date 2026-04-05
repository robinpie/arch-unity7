/**
 * unit tests for the geis filter module
 */
#include <check.h>

#include "geis_attr.h"
#include "geis_private.h"
#include "geis_test_api.h"


static const char* device_1_name = "device 1";

/* fixtures */
Geis g_geis;

static void
construct_geis()
{
  static struct GeisFilterableAttribute attrs[] = {
    { GEIS_DEVICE_ATTRIBUTE_NAME,    GEIS_ATTR_TYPE_STRING,  0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_ID,      GEIS_ATTR_TYPE_INTEGER, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_TOUCHES, GEIS_ATTR_TYPE_INTEGER, 0, NULL },
  };
  static GeisSize attr_count = sizeof(attrs)
                             / sizeof(struct GeisFilterableAttribute);
  int ival = 2;
  int bval = GEIS_TRUE;

  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);

  GeisDevice device = geis_device_new(device_1_name, 1);
  geis_device_add_attr(device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_TOUCHES,
                                             GEIS_ATTR_TYPE_INTEGER,
                                             &ival));
  geis_register_device(g_geis, device, attr_count, attrs);

  device = geis_device_new("device 2", 2);
  geis_device_add_attr(device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_TOUCHES,
                                             GEIS_ATTR_TYPE_INTEGER,
                                             &ival));
  geis_device_add_attr(device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,
                                             GEIS_ATTR_TYPE_BOOLEAN,
                                             &bval));
  geis_register_device(g_geis, device, attr_count, attrs);

  ++ival;
  device = geis_device_new("device 3", 3);
  geis_device_add_attr(device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_TOUCHES,
                                             GEIS_ATTR_TYPE_INTEGER,
                                             &ival));
  geis_register_device(g_geis, device, attr_count, attrs);

  while (geis_dispatch_events(g_geis) == GEIS_STATUS_CONTINUE)
    ;
}

static void
destroy_geis()
{
  geis_delete(g_geis);
}

/* should select ALL devices */
START_TEST(select_all)
{
  GeisDeviceBag device_bag = geis_device_bag_new();
  GeisFilter filter = geis_filter_new(g_geis, "no device terms");
  GeisSelectResult matches = geis_select_devices(g_geis, filter, device_bag);

  fail_unless(matches == GEIS_SELECT_RESULT_ALL, "unexpected match result");

  geis_device_bag_delete(device_bag);
}
END_TEST

/* should select SOME devices */
START_TEST(select_some_name)
{
  GeisDeviceBag device_bag = geis_device_bag_new();
  GeisFilter filter = geis_filter_new(g_geis, "just device 1");
  geis_filter_add_term(filter, GEIS_FILTER_DEVICE,
             GEIS_DEVICE_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, device_1_name,
             NULL);
  GeisSelectResult matches = geis_select_devices(g_geis, filter, device_bag);

  fail_unless(matches == GEIS_SELECT_RESULT_SOME, "unexpected match result");

  geis_device_bag_delete(device_bag);
}
END_TEST

/* should select SOME devices */
START_TEST(select_some_indirect)
{
  GeisDeviceBag device_bag = geis_device_bag_new();
  GeisFilter filter = geis_filter_new(g_geis, "just ndirect devices");
  geis_filter_add_term(filter, GEIS_FILTER_DEVICE,
             GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, GEIS_FILTER_OP_NE, GEIS_FALSE,
             NULL);
  GeisSelectResult matches = geis_select_devices(g_geis, filter, device_bag);

  fail_unless(matches == GEIS_SELECT_RESULT_SOME, "unexpected match result");

  geis_device_bag_delete(device_bag);
}
END_TEST

/* should select SOME devices */
START_TEST(select_some_touches)
{
  GeisDeviceBag device_bag = geis_device_bag_new();
  GeisFilter filter = geis_filter_new(g_geis, "just 3-touch devices");
  geis_filter_add_term(filter, GEIS_FILTER_DEVICE,
                       GEIS_DEVICE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 2,
                       NULL);
  GeisSelectResult matches = geis_select_devices(g_geis, filter, device_bag);

  fail_unless(matches == GEIS_SELECT_RESULT_SOME, "unexpected match result");

  geis_device_bag_delete(device_bag);
}
END_TEST

/* should select NONE device */
START_TEST(select_none)
{
  GeisDeviceBag device_bag = geis_device_bag_new();
  GeisFilter filter = geis_filter_new(g_geis, "just 4-touch devices");
  geis_filter_add_term(filter, GEIS_FILTER_DEVICE,
                       GEIS_DEVICE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 3,
                       NULL);
  GeisSelectResult matches = geis_select_devices(g_geis, filter, device_bag);

  fail_unless(matches == GEIS_SELECT_RESULT_NONE, "unexpected match result");

  geis_device_bag_delete(device_bag);
}
END_TEST

/* boilerplate */
Suite *
make_select_devices_suite()
{
  Suite *s = suite_create("geis2-select_devices");

  TCase *usage = tcase_create("device-selection");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, select_all);
  tcase_add_test(usage, select_some_name);
  tcase_add_test(usage, select_some_indirect);
  tcase_add_test(usage, select_some_touches);
  tcase_add_test(usage, select_none);
  suite_add_tcase(s, usage);

  return s;
}

