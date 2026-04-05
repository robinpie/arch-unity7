/**
 * @file check_instance.c
 * @brief Unit testing driver for GEIS v1 API geisInstance functions.
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

#include <geis/geis.h>

#define GEIS_TEST_WINDOW geis_win_type_str(Test)


/* compile-time test to ensure required functions are defined */
START_TEST(geis_init_and_finish)
{
  GeisStatus   status;
  GeisInstance instance;
  GeisWinInfo  win_info = {
    GEIS_TEST_WINDOW,
    NULL
  };

  status = geis_init(&win_info, &instance);
  fail_unless(status == GEIS_STATUS_SUCCESS, "geis_init failed");

  status = geis_finish(instance);
  fail_unless(status == GEIS_STATUS_SUCCESS, "geis_finish failed");
}
END_TEST


Suite *
geis1_instance_suite_new()
{
  TCase *create;
  Suite *s = suite_create("geis1_instance_suite");

  create = tcase_create("geis_init");
  tcase_add_test(create, geis_init_and_finish);
  suite_add_tcase(s, create);

  return s;
}

