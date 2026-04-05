/**
 * unit tests for the geis_instance_table module
 */
#include <check.h>

#include <geis/geis.h>

/* compile-time test to ensure required types are defined */
START_TEST(general_types)
{
  GeisBoolean  geis_boolean CK_ATTRIBUTE_UNUSED;
  GeisInteger  geis_integer CK_ATTRIBUTE_UNUSED;
  GeisFloat    geis_float CK_ATTRIBUTE_UNUSED;
  GeisPointer  geis_pointer CK_ATTRIBUTE_UNUSED;
  GeisSize     geis_size CK_ATTRIBUTE_UNUSED;
  GeisString   geis_string CK_ATTRIBUTE_UNUSED;
}
END_TEST


/* boilerplate */
Suite *
geis2_general_types_suite_new()
{
  Suite *s = suite_create("geis2_general_types");
  TCase *create;

  create = tcase_create("general_types");
  tcase_add_test(create, general_types);
  suite_add_tcase(s, create);

  return s;
}

