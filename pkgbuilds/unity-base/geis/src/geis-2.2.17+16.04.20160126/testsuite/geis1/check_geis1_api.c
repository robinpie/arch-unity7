/**
 * @file check_geis1_api.c
 * @brief Unit testing driver for GEIS v1 API.
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <check.h>

extern Suite *geis1_instance_suite_new();
extern Suite *geis1_gesture_types_new();
extern Suite *geis1_gesture_attrs_new();
extern Suite *geis1_subscription_new();

int
main(int argc CK_ATTRIBUTE_UNUSED, char* argv[] CK_ATTRIBUTE_UNUSED)
{
  int num_failed = 0;

  Suite *s = suite_create("GEIS v1.0 API");
  SRunner *sr = srunner_create(s);
  srunner_add_suite(sr, geis1_instance_suite_new());
  srunner_add_suite(sr, geis1_gesture_types_new());
  srunner_add_suite(sr, geis1_gesture_attrs_new());
  srunner_add_suite(sr, geis1_subscription_new());

  srunner_set_log(sr, "geis1_api.log");
  srunner_set_xml(sr, "geis1_api.xml");
  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);

  srunner_free(sr);
  return !(num_failed == 0);
}
