/**
 * @file check_geis_util.c
 * @brief Test driver for unit testing of GEIS internal utility library.
 */
/*
 * Copyright 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License version 3, as published 
 * by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <check.h>

#define LOGFILE_PREFIX "geis_util"

extern Suite *make_geis_bag_suite();


int
main(int argc CK_ATTRIBUTE_UNUSED, char* argv[] CK_ATTRIBUTE_UNUSED)
{
  int num_failed = 0;

  Suite *s = suite_create("\"GEIS internal utilities\"");

  SRunner *sr = srunner_create(s);
  srunner_add_suite(sr, make_geis_bag_suite());

  srunner_set_log(sr, LOGFILE_PREFIX".log");
  srunner_set_xml(sr, LOGFILE_PREFIX".xml");
  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return !(num_failed == 0);
}
