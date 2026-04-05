/**
 * unit tests for the geis gesture class module
 */
#include <check.h>

#include "libgeis/geis_class.h"
#include <stdio.h>


/* fixtures */
static GeisGestureClassBag g_gesture_class_bag;
static GeisString          g_gesture_class_name = "gesture_class";
static GeisInteger         g_gesture_class_id   = 12;

/* fixture setup */
static void
construct_bag()
{
  g_gesture_class_bag = geis_gesture_class_bag_new();
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_gesture_class_bag_delete(g_gesture_class_bag);
}

/* verify bag construction/destruction */
START_TEST(construct)
{
  GeisGestureClassBag bag = geis_gesture_class_bag_new();
  fail_unless(bag != NULL, "failed to create gesture class bag");
  geis_gesture_class_bag_delete(bag);
}
END_TEST

/* verify bag insertion */
START_TEST(insert_class)
{
  GeisGestureClass gesture_class = geis_gesture_class_new(g_gesture_class_name, g_gesture_class_id);
  geis_gesture_class_bag_insert(g_gesture_class_bag, gesture_class);
  fail_unless(geis_gesture_class_bag_count(g_gesture_class_bag) == 1,
              "unexpected bag size after insertion");
}
END_TEST

/* verify bag removal */
START_TEST(remove_class)
{
  GeisGestureClass gesture_class = geis_gesture_class_new(g_gesture_class_name, g_gesture_class_id);
  geis_gesture_class_bag_insert(g_gesture_class_bag, gesture_class);
  fail_unless(geis_gesture_class_bag_count(g_gesture_class_bag) == 1,
              "unexpected bag size after insertion");
  geis_gesture_class_bag_remove(g_gesture_class_bag, gesture_class);
  fail_unless(geis_gesture_class_bag_count(g_gesture_class_bag) == 0,
              "unexpected bag size after removal");
}
END_TEST

START_TEST(expand)
{
  GeisSize i;
  for (i = 0; i < 24; ++i)
  {
    GeisSize count;
    char name[32];
    sprintf(name, "%04zu", i);
    GeisGestureClass gesture_class = geis_gesture_class_new(name, i);
    geis_gesture_class_bag_insert(g_gesture_class_bag, gesture_class);
    count = geis_gesture_class_bag_count(g_gesture_class_bag);
    fail_unless(count == (i+1),
                "unexpected bag size %ld after insertion, expected %d",
                count, i+1);
  }
}
END_TEST



/* boilerplate */
Suite *
make_gesture_class_suite()
{
  Suite *s = suite_create("geis2-gesture_class");

  TCase *create = tcase_create("gesture-class-bag-creation");
  tcase_add_test(create, construct);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("gesture-class-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, insert_class);
  tcase_add_test(usage, remove_class);
  tcase_add_test(usage, expand);
  suite_add_tcase(s, usage);

  return s;
}

