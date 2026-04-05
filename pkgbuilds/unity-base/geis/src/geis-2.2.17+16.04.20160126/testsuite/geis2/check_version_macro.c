/**
 * unit tests for the geis_instance_table module
 */
#include <check.h>

#include "geis/geis.h"


START_TEST(existence)
{
#ifdef GEIS_VERSION_2_0
  fail_unless(0==0);
#else
  fail_unless(0!=0);
#endif
}
END_TEST


/* boilerplate */
Suite *
make_version_macro_suite()
{
  Suite *s = suite_create("geis_version_macro");

  TCase *create = tcase_create("existence");
  tcase_add_test(create, existence);
  suite_add_tcase(s, create);

  return s;
}

