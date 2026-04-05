/**
 * internal unit tests for the GEIS v2.0 region module
 *
 * Note the public API is checked through the geis2 testsuite.
 */
#include <check.h>

#include "geis/geis.h"
#include "geis_test_api.h"
#include "geis_region.h"
#include <stdio.h>


/* fixtures */
static Geis g_geis;
static GeisRegionBag g_region_bag;
static uint16_t g_sample_windowid = 0x11;

/* fixture setup */
static void
construct_bag()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  g_region_bag = geis_region_bag_new(1);
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_region_bag_delete(g_region_bag);
  geis_delete(g_geis);
}


START_TEST(region_access)
{
  GeisRegion region;
  region = geis_region_new(g_geis, "test01",
                           GEIS_REGION_X11_WINDOWID, g_sample_windowid,
                           NULL);
  fail_unless(0 == strcmp(geis_region_type(region), GEIS_REGION_X11_WINDOWID),
              "bad region type");
  fail_unless(g_sample_windowid == *(int *)geis_region_data(region),
              "bad region data");
}
END_TEST

/* verify bag construction/destruction */
START_TEST(bag_construction)
{
  GeisRegionBag bag = geis_region_bag_new();
  fail_unless(bag != NULL,
              "failed to create region bag");
  fail_unless(geis_region_bag_count(bag) == 0,
              "unexpected size");
  geis_region_bag_delete(bag);
}
END_TEST


/* verify bag insertion */
START_TEST(bag_insertion)
{
  GeisRegion region;
  region = geis_region_new(g_geis, "test02", GEIS_REGION_X11_ROOT, NULL);
  geis_region_bag_insert(g_region_bag, region);
  fail_unless(geis_region_bag_count(g_region_bag) == 1,
           "unexpected bag size after insertion");
}
END_TEST


START_TEST(bag_expansion)
{
  int i;
  GeisRegion region;
  for (i = 0; i < 24; ++i)
  {
    char name[32];
    sprintf(name, "%04d", i);
    region = geis_region_new(g_geis, name, GEIS_REGION_X11_ROOT, NULL);
    geis_region_bag_insert(g_region_bag, region);
    int size = geis_region_bag_count(g_region_bag);
    fail_if((size - 1) != i, "expected region bag size %d, got %d", i, size);
  }
}
END_TEST


/* boilerplate */
Suite *
make_region_suite()
{
  Suite *s = suite_create("geis2-region");

  TCase *create = tcase_create("region-access");
  tcase_add_test(create, region_access);
  suite_add_tcase(s, create);

  TCase *bag_create = tcase_create("region-bag-creation");
  tcase_add_test(bag_create, bag_construction);
  suite_add_tcase(s, bag_create);

  TCase *bag_usage = tcase_create("region-bag-usage");
  tcase_add_checked_fixture(bag_usage, construct_bag, destroy_bag);
  tcase_add_test(bag_usage, bag_insertion);
  tcase_add_test(bag_usage, bag_expansion);
  suite_add_tcase(s, bag_usage);

  return s;
}

