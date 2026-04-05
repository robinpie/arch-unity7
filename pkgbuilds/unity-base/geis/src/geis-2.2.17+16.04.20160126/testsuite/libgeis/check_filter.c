/**
 * unit tests for the geis filter module
 */
#include <check.h>

#include "geis_filter.h"
#include "geis_test_api.h"
#include <stdio.h>


/* fixtures */
static Geis          g_geis;
static GeisFilterBag g_filter_bag;
static GeisString    g_filter_name = "filter";

/* fixture setup */
static void
construct_bag()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  g_filter_bag = geis_filter_bag_new();
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_filter_bag_delete(g_filter_bag);
  geis_delete(g_geis);
}

/* verify bag construction/destruction */
START_TEST(construct)
{
  GeisFilterBag bag = geis_filter_bag_new();
  fail_unless(bag != NULL, "failed to create filter bag");
  geis_filter_bag_delete(bag);
}
END_TEST

/* verify bag insertion */
START_TEST(insert_filter)
{
  GeisFilter filter = geis_filter_new(g_geis, g_filter_name);
  geis_filter_bag_insert(g_filter_bag, filter);
  fail_unless(geis_filter_bag_count(g_filter_bag) == 1,
              "unexpected bag size after insertion");
}
END_TEST

/* verify bag removal */
START_TEST(remove_filter)
{
  GeisFilter filter = geis_filter_new(g_geis, g_filter_name);
  geis_filter_bag_insert(g_filter_bag, filter);
  fail_unless(geis_filter_bag_count(g_filter_bag) == 1,
              "unexpected bag size after insertion");
  geis_filter_bag_remove(g_filter_bag, filter);
  fail_unless(geis_filter_bag_count(g_filter_bag) == 0,
              "unexpected bag size after removal");
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
    GeisFilter filter = geis_filter_new(g_geis, name);
    geis_filter_bag_insert(g_filter_bag, filter);
    count = geis_filter_bag_count(g_filter_bag);
    fail_unless(count == (i+1),
                "unexpected bag size %ld after insertion, expected %d",
                count, i+1);
  }
}
END_TEST



/* boilerplate */
Suite *
make_filter_suite()
{
  Suite *s = suite_create("geis2-filter");

  TCase *create = tcase_create("filter-bag-creation");
  tcase_add_test(create, construct);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("filter-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, insert_filter);
  tcase_add_test(usage, remove_filter);
  tcase_add_test(usage, expand);
  suite_add_tcase(s, usage);

  return s;
}

