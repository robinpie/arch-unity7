/**
 * Unit tests for GEIS v2.0 Filter Module.
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


/* Compile-time test to ensure types are defined */
START_TEST(geis_filter_types)
{
  GeisFilter type CK_ATTRIBUTE_UNUSED;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_filter_functions)
{
  GeisFilter filter1;
  GeisFilter filter2;
  GeisStatus status;

  filter1 = geis_filter_new(g_geis, "filter1");
  fail_unless(filter1 != NULL,
              "filter construct fail");
  fail_unless(0 == strcmp(geis_filter_name(filter1), "filter1"),
              "filter 1 name fail");

  status  = geis_filter_add_term(filter1, GEIS_FILTER_CLASS,
                          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_GT, 1,
                          NULL);

  filter2 = geis_filter_clone(filter1,"filter2");
  fail_unless(filter2 != NULL,
              "filter clone fail");
  fail_unless(0 == strcmp(geis_filter_name(filter2), "filter2"),
              "filter 2 name fail");

  geis_filter_delete(filter1);
  geis_filter_delete(filter2);
}
END_TEST


/* boilerplate */
Suite *
geis2_filter_suite_new()
{
  TCase *filter;
  TCase *usage;
  Suite *s = suite_create("geis2_filter");

  filter = tcase_create("geis2_filter");
  tcase_add_test(filter, geis_filter_types);
  suite_add_tcase(s, filter);

  usage = tcase_create("subscription-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, geis_filter_functions);
  suite_add_tcase(s, usage);

  return s;
}

