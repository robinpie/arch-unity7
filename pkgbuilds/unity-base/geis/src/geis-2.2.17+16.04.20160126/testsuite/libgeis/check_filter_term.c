/**
 * @file check_filter_term.c
 * @brief unit tests for GEIS filter terms
 *
 * Copyright 2012 Canonical Ltd.
 */
#include <check.h>
#include "libgeis/geis_attr.h"
#include "libgeis/geis_device.h"
#include "libgeis/geis_filter_term.h"

/* verify filter term construction/destruction */
START_TEST(construct)
{
  GeisInteger ival = 0;
  GeisAttr attr = geis_attr_new("dummy", GEIS_ATTR_TYPE_INTEGER, &ival);
  fail_unless(attr != NULL, "failed to create filter term attr");

  GeisFilterTerm term = geis_filter_term_new(GEIS_FILTER_SPECIAL,
                                             GEIS_FILTER_OP_NE,
                                             attr);
  fail_unless(term != NULL, "failed to create filter term");

  geis_filter_term_unref(term);
}
END_TEST


/* verify device filtering capability */
START_TEST(device_filter)
{
  GeisString device_name = "direct";
  GeisAttr name_attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_NAME,
                                     GEIS_ATTR_TYPE_STRING,
                                     (void *)device_name);
  fail_unless(name_attr != NULL, "failed to create name filter term attr");
  GeisFilterTerm name_term = geis_filter_term_new(GEIS_FILTER_DEVICE,
                                                  GEIS_FILTER_OP_NE,
                                                  name_attr);
  fail_unless(name_term != NULL, "failed to create id filter term");

  GeisBoolean is_direct = GEIS_TRUE;
  GeisAttr direct_attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,
                                       GEIS_ATTR_TYPE_BOOLEAN,
                                       &is_direct);
  fail_unless(direct_attr != NULL, "failed to create direct filter term attr");
  GeisFilterTerm direct_term = geis_filter_term_new(GEIS_FILTER_DEVICE,
                                                    GEIS_FILTER_OP_EQ,
                                                    direct_attr);
  fail_unless(direct_term != NULL, "failed to create direct filter term");

  GeisInteger id = 1;
  GeisAttr id_attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_ID,
                                   GEIS_ATTR_TYPE_INTEGER,
                                   &id);
  fail_unless(id_attr != NULL, "failed to create id filter term attr");
  GeisFilterTerm id_term = geis_filter_term_new(GEIS_FILTER_DEVICE,
                                                GEIS_FILTER_OP_EQ,
                                                id_attr);
  fail_unless(id_term != NULL, "failed to create id filter term");

  GeisDevice direct_device = geis_device_new(device_name, id);
  GeisAttr device_attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,
                                       GEIS_ATTR_TYPE_BOOLEAN,
                                       &is_direct);
  fail_unless(device_attr != NULL, "failed to create direct filter term attr");
  geis_device_add_attr(direct_device, device_attr);
  GeisDevice indirect_device = geis_device_new("indirect", id + 1);

  GeisBoolean passed = GEIS_FALSE;
  passed = geis_filter_term_match_device(name_term, direct_device);
  fail_unless(passed != GEIS_TRUE, "direct device was passed by name term");
  passed = geis_filter_term_match_device(name_term, indirect_device);
  fail_unless(passed == GEIS_TRUE, "indirect device was not passed by name term");
  passed = geis_filter_term_match_device(direct_term, direct_device);
  fail_unless(passed == GEIS_TRUE, "direct device was not passed by direct term");
  passed = geis_filter_term_match_device(direct_term, indirect_device);
  fail_unless(passed != GEIS_TRUE, "indirect device was passed by direct term");
  passed = geis_filter_term_match_device(id_term, direct_device);
  fail_unless(passed == GEIS_TRUE, "direct device was not passed by id term");
  passed = geis_filter_term_match_device(id_term, indirect_device);
  fail_unless(passed != GEIS_TRUE, "indirect device was passed by id term");

  geis_device_unref(direct_device);
  geis_device_unref(indirect_device);
  geis_filter_term_unref(name_term);
  geis_filter_term_unref(direct_term);
  geis_filter_term_unref(id_term);
}
END_TEST

/* boilerplate */
Suite *
make_filter_term_suite()
{
  Suite *s = suite_create("geis2-filter-term");

  TCase *create = tcase_create("filter-term-creation");
  tcase_add_test(create, construct);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("filter-term-usage");
  tcase_add_test(usage, device_filter);
  suite_add_tcase(s, usage);

  return s;
}
