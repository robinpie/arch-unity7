/**
 * Unit tests for GEIS v2.0 Attr Module.
 */
#include <check.h>

#include <geis/geis.h>

/* Compile-time test to ensure types are defined*/
START_TEST(geis_attr_types)
{
  GeisAttrType type;
  type = GEIS_ATTR_TYPE_BOOLEAN;
  type = GEIS_ATTR_TYPE_INTEGER;
  type = GEIS_ATTR_TYPE_FLOAT;
  type = GEIS_ATTR_TYPE_POINTER;
  type = GEIS_ATTR_TYPE_STRING;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_attr_functions)
{
  GeisAttr attr = 0;
  GeisBoolean n CK_ATTRIBUTE_UNUSED;
  GeisFloat f CK_ATTRIBUTE_UNUSED;
  GeisInteger i CK_ATTRIBUTE_UNUSED;
  GeisPointer p CK_ATTRIBUTE_UNUSED;
  GeisString s CK_ATTRIBUTE_UNUSED;
  GeisString name CK_ATTRIBUTE_UNUSED;
  GeisAttrType type CK_ATTRIBUTE_UNUSED;

  name = geis_attr_name(attr);
  type = geis_attr_type(attr);
  n = geis_attr_value_to_boolean(attr);
  f = geis_attr_value_to_float(attr);
  i = geis_attr_value_to_integer(attr);
  p = geis_attr_value_to_pointer(attr);
  s = geis_attr_value_to_string(attr);
}
END_TEST


/* boilerplate */
Suite *
geis2_attr_suite_new()
{
  Suite *s = suite_create("geis2_attr");

  TCase *init_args = tcase_create("geis2_attr");
  tcase_add_test(init_args, geis_attr_types);
  suite_add_tcase(s, init_args);

  return s;
}

