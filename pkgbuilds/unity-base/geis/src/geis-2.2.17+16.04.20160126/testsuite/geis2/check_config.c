/**
 * unit tests for the geis_geis_config module
 */
#include <check.h>

#include "geis/geis.h"
#include "libgeis/geis_test_api.h"


/* fixtures */
static Geis geis;

/* fixture setup */
static void
construct_geis()
{
  geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(geis);
}

START_TEST(get_success)
{
  int fd = -1;
  GeisStatus status = geis_get_configuration(geis, GEIS_CONFIGURATION_FD, &fd);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected return status from geis_get_configuration()");
  fail_unless(fd >= 0, "invalid Unix FD returned");
}
END_TEST


/* verify table find operation (negative results) */
START_TEST(get_fail)
{
  int fd = -1;
  GeisStatus status = geis_get_configuration(geis, "no such configuration", &fd);
  fail_unless(status == GEIS_STATUS_NOT_SUPPORTED,
              "unexpected return status from geis_get_configuration()");
}
END_TEST


/* boilerplate */
Suite *
geis2_config_suite_new()
{
  Suite *s = suite_create("geis2_configuration");

  TCase *usage = tcase_create("table-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, get_success);
  tcase_add_test(usage, get_fail);
  suite_add_tcase(s, usage);

  return s;
}

