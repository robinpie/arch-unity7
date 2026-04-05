/**
 * unit tests for GEIS v2.0 error codes
 */
#include <check.h>

#include <geis/geis.h>

/* compile-time test to ensure required types are defined */
START_TEST(error_codes)
{
  GeisStatus error = GEIS_STATUS_SUCCESS ;
  error = GEIS_STATUS_CONTINUE;
  error = GEIS_STATUS_EMPTY;
  error = GEIS_STATUS_NOT_SUPPORTED;
  error = GEIS_STATUS_BAD_ARGUMENT;
  error = GEIS_STATUS_UNKNOWN_ERROR;
}
END_TEST


/* boilerplate */
Suite *
geis2_error_codes_suite_new()
{
  Suite *s = suite_create("geis2_error_codes");

  TCase *create = tcase_create("error_codes");
  tcase_add_test(create, error_codes);
  suite_add_tcase(s, create);

  return s;
}

