/**
 * unit tests for the geis_attr_bag module
 */
#include <check.h>

#include "libgeis/geis_attr.h"
#include <stdio.h>


/* fixtures */
static GeisAttrBag g_attr_bag;
static const GeisString test_attr_string = "zot!";

/* fixture setup */
static void
construct_bag()
{
  g_attr_bag = geis_attr_bag_new(1);
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_attr_bag_delete(g_attr_bag);
}


/* verify bag construction/destruction */
START_TEST(construction)
{
  GeisAttrBag bag = geis_attr_bag_new(1);
  fail_unless(bag != NULL, "failed to create attr bag");
  geis_attr_bag_delete(bag);
}
END_TEST


/* verify bag insertion */
START_TEST(insertion)
{
  GeisAttr attr = geis_attr_new("test-attr",
                                GEIS_ATTR_TYPE_STRING,
                                (void*)test_attr_string);
  geis_attr_bag_insert(g_attr_bag, attr);
  fail_unless(geis_attr_bag_count(g_attr_bag) == 1,
              "unexpected bag size after insertion");
}
END_TEST


/* verify bag get operation (positive results) */
START_TEST(get_success)
{
  GeisAttr dst_attr;
  GeisAttr src_attr = geis_attr_new("test-attr",
                                    GEIS_ATTR_TYPE_STRING,
                                    (void*)test_attr_string);
  geis_attr_bag_insert(g_attr_bag, src_attr);
  dst_attr = geis_attr_bag_attr(g_attr_bag, 0);
  fail_if(dst_attr == NULL, "expected instance not found");
  fail_unless(0 == strcmp(geis_attr_value_to_string(dst_attr), test_attr_string),
              "unexpected attribute value returned");
}
END_TEST


/* verify bag get operation (negative results) */
START_TEST(get_fail)
{
  GeisAttr attr = geis_attr_bag_attr(g_attr_bag, 5);
  fail_unless(attr == NULL, "unexpected instance indexed");
}
END_TEST


/* verify bag find operation (positive results) */
START_TEST(find_success)
{
  GeisAttr dst_attr;
  GeisAttr src_attr = geis_attr_new("test-attr",
                                    GEIS_ATTR_TYPE_STRING,
                                    (void*)test_attr_string);
  geis_attr_bag_insert(g_attr_bag, src_attr);
  dst_attr = geis_attr_bag_find(g_attr_bag, "test-attr");
  fail_if(dst_attr == NULL, "expected instance not found");
  fail_unless(0 == strcmp(geis_attr_value_to_string(dst_attr), test_attr_string),
  	      "unexpected attribute value returned");
}
END_TEST


/* verify bag find operation (negative results) */
START_TEST(find_fail)
{
  GeisAttr attr = geis_attr_bag_find(g_attr_bag, "bogus");
  fail_unless(attr == NULL, "unexpected instance found");
}
END_TEST

START_TEST(expansion)
{
  GeisSize i;
  for (i = 0; i < 24; ++i)
  {
    GeisSize count;
    char name[32];
    sprintf(name, "%04zu", i);
    GeisAttr attr = geis_attr_new(name, GEIS_ATTR_TYPE_INTEGER, &i);
    geis_attr_bag_insert(g_attr_bag, attr);
    count = geis_attr_bag_count(g_attr_bag);
    fail_unless(count == (i+1),
                "unexpected bag size %ld after insertion, expected %d",
                count, i+1);
  }
}
END_TEST


/* boilerplate */
Suite *
make_attr_suite()
{
  Suite *s = suite_create("geis2-attrs");

  TCase *create = tcase_create("attr-bag-creation");
  tcase_add_test(create, construction);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("attr-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, insertion);
  tcase_add_test(usage, get_success);
  tcase_add_test(usage, get_fail);
  tcase_add_test(usage, find_success);
  tcase_add_test(usage, find_fail);
  tcase_add_test(usage, expansion);
  suite_add_tcase(s, usage);

  return s;
}

