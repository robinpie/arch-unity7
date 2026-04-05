/**
 * @file check_geis_bag.c
 * @brief Unit test for geis-util/geis_bag
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

#include "geis_bag.h"

/* fixtures */
GeisBag g_bag;

struct TestStruct
{
    int  ival;
    char sval[32];
};


/* fixture setup */
static void
construct_bag()
{
  g_bag = geis_bag_new(sizeof(struct TestStruct),
                       1,
                       geis_bag_default_growth_factor);
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_bag_delete(g_bag);
}


/* verify bag construction/destruction */
START_TEST(construction)
{
  GeisBag bag = geis_bag_new(1,
                             geis_bag_default_init_alloc,
                             geis_bag_default_growth_factor);
  fail_unless(bag != NULL, "failed to create generic bag");
  geis_bag_delete(bag);
}
END_TEST


START_TEST(append)
{
  struct TestStruct datum = { 1, "borf" };
  GeisStatus status = geis_bag_append(g_bag, &datum);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");
}
END_TEST


START_TEST(count)
{
  struct TestStruct datum1 = { 1, "borf" };
  struct TestStruct datum2 = { 2, "barf" };
  GeisSize count = geis_bag_count(g_bag);
  fail_unless(0 == count,
              "unexpected count in bag, expected %zu got %zu", 0, count);
  GeisStatus status = geis_bag_append(g_bag, &datum1);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");
  count = geis_bag_count(g_bag);
  fail_unless(1 == count,
              "unexpected count in bag: expected %zu got %zu", 1, count);
  status = geis_bag_append(g_bag, &datum2);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");
  count = geis_bag_count(g_bag);
  fail_unless(2 == count,
              "unexpected count in bag: expected %zu got %zu", 2, count);
}
END_TEST


START_TEST(at)
{
  struct TestStruct datum1 = { 1, "borf" };
  struct TestStruct datum2 = { 2, "barf" };
  GeisStatus status = geis_bag_append(g_bag, &datum1);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");
  status = geis_bag_append(g_bag, &datum2);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");

  struct TestStruct *odatum = geis_bag_at(g_bag, 0);
  fail_unless(datum1.ival == odatum->ival,
              "inserted and retrived data do not match: expected %d got %d",
              datum1.ival, odatum->ival);
}
END_TEST


START_TEST(remove)
{
  struct TestStruct datum1 = { 1, "borf" };
  struct TestStruct datum2 = { 2, "barf" };
  GeisStatus status = geis_bag_append(g_bag, &datum1);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");
  status = geis_bag_append(g_bag, &datum2);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_append()");

  status = geis_bag_remove(g_bag, 0);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected result from geis_bag_remove()");
  GeisSize count = geis_bag_count(g_bag);
  fail_unless(1 == count,
              "unexpected count in bag, expected %zu got %zu", 1, count);
  struct TestStruct *odatum = geis_bag_at(g_bag, 0);
  fail_unless(datum2.ival == odatum->ival,
              "bad retrived data: expected %d got %d",
              datum2.ival, odatum->ival);
}
END_TEST


/* boilerplate */
Suite *
make_geis_bag_suite()
{
  Suite *s = suite_create("geis_bag");

  TCase *create = tcase_create("geis-bag-creation");
  tcase_add_test(create, construction);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("geis-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, append);
  tcase_add_test(usage, count);
  tcase_add_test(usage, at);
  tcase_add_test(usage, remove);
  suite_add_tcase(s, usage);

  return s;
}

