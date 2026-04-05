/**
 * unit tests for the geis_instance_table module
 */
#include <check.h>

#include "geis/geis.h"
#include "libgeis/geis_error.h"


START_TEST(global_error_stack)
{
  GeisSize i = 0;

  geis_error_clear(NULL);
  geis_error_push(NULL, GEIS_STATUS_SUCCESS);

  fail_unless(geis_error_count(NULL) == 1, "unexpected error stack size");
  for (i=0; i < geis_error_count(NULL); ++i)
  {
    fail_unless(geis_error_code(NULL, i) == GEIS_STATUS_SUCCESS,
                "unexpected status code retrieved");
  }
}
END_TEST


/* boilerplate */
Suite *
make_error_reporting_suite()
{
  Suite *s = suite_create("geis2-error-reporting");

  TCase *create = tcase_create("error-reporting");
  tcase_add_test(create, global_error_stack);
  suite_add_tcase(s, create);

  return s;
}

