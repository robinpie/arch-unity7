/**
 * unit tests for the geis device module
 */
#include <check.h>

#include "libgeis/geis_device.h"
#include "libgeis/geis_attr.h"
#include <stdio.h>


/* fixtures */
static GeisDeviceBag g_device_bag;
static GeisString    g_device_name = "device";
static GeisInteger   g_device_id   = 12;

/* fixture setup */
static void
construct_bag()
{
  g_device_bag = geis_device_bag_new();
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_device_bag_delete(g_device_bag);
}

/* verify bag construction/destruction */
START_TEST(construct)
{
  GeisDeviceBag bag = geis_device_bag_new();
  fail_unless(bag != NULL, "failed to create device bag");
  geis_device_bag_delete(bag);
}
END_TEST

/* verify bag insertion */
START_TEST(insert_device)
{
  GeisDevice device = geis_device_new(g_device_name, g_device_id);
  geis_device_bag_insert(g_device_bag, device);
  fail_unless(geis_device_bag_count(g_device_bag) == 1,
              "unexpected bag size after insertion");
}
END_TEST

/* verify bag removal */
START_TEST(remove_device)
{
  GeisDevice device = geis_device_new(g_device_name, g_device_id);
  geis_device_bag_insert(g_device_bag, device);
  fail_unless(geis_device_bag_count(g_device_bag) == 1,
              "unexpected bag size after insertion");
  geis_device_bag_remove(g_device_bag, device);
  fail_unless(geis_device_bag_count(g_device_bag) == 0,
              "unexpected bag size after removal");
}
END_TEST


/*
 * Verify retrieval by name works.
 */
START_TEST(attribute_name)
{
  GeisDevice device = geis_device_new(g_device_name, g_device_id);
  GeisAttr in_attr, out_attr;
  const char *attribute_name = "TEST_ATTRIBUTE";
  int attr_value = 60;
  fail_if(geis_device_attr_by_name(device, attribute_name) != NULL);
  in_attr = geis_attr_new(attribute_name, GEIS_ATTR_TYPE_INTEGER, (void*)&attr_value);
  geis_device_add_attr(device, in_attr);
  out_attr = geis_device_attr_by_name(device, attribute_name);
  fail_if(out_attr == NULL);
  fail_if(geis_attr_value_to_integer(out_attr) != attr_value);
}
END_TEST

START_TEST(expand)
{
  GeisSize i;
  for (i = 0; i < 24; ++i)
  {
    GeisSize count;
    char name[32];
    sprintf(name, "%04zu", i);
    GeisDevice device = geis_device_new(name, i);
    geis_device_bag_insert(g_device_bag, device);
    count = geis_device_bag_count(g_device_bag);
    fail_unless(count == (i+1),
                "unexpected bag size %ld after insertion, expected %d",
                count, i+1);
  }
}
END_TEST



/* boilerplate */
Suite *
make_device_suite()
{
  Suite *s = suite_create("geis2-device");

  TCase *create = tcase_create("device-bag-creation");
  tcase_add_test(create, construct);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("device-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, insert_device);
  tcase_add_test(usage, remove_device);
  tcase_add_test(usage, expand);
  suite_add_tcase(s, usage);

  TCase *dev_attr = tcase_create("device-attributes");
  tcase_add_test(usage, attribute_name);
  suite_add_tcase(s, dev_attr);
  return s;
}

