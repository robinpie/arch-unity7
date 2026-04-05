/**
 * Unit tests for GEIS v2.0 region module
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
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(g_geis);
}


/* compile-time test to ensure required types are defined */
START_TEST(region_constants)
{
  GeisString ini;
  ini = GEIS_REGION_X11_ROOT;
  ini = GEIS_REGION_X11_WINDOWID;
}
END_TEST

START_TEST(construction)
{
  GeisRegion sub = geis_region_new(g_geis, "name", GEIS_REGION_X11_ROOT, NULL);
  fail_unless(sub != NULL,
              "failed to create region");
  fail_unless(0 == strcmp(geis_region_name(sub), "name"),
              "unexpected region name returned");
  geis_region_delete(sub);
}
END_TEST

/* boilerplate */
Suite *
geis2_region_suite_new()
{
  Suite *s = suite_create("geis2_region");
  TCase *creation;
  TCase *usage;

  creation = tcase_create("region-constants");
  tcase_add_test(creation, region_constants);
  suite_add_tcase(s, creation);

  usage = tcase_create("region-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, construction);
  suite_add_tcase(s, usage);

  return s;
}

