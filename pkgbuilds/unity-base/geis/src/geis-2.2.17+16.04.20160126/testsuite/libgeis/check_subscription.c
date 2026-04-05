/**
 * internal unit tests for the GEIS v2.0 subscription module
 */
#include <check.h>

#include "geis/geis.h"
#include "geis_subscription.h"
#include "geis_test_api.h"
#include <stdio.h>


/* fixtures */
static Geis g_geis;
static GeisSubBag g_sub_bag;
static const GeisString test_sub_string = "zot!";

/* fixture setup */
static void
construct_bag()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  g_sub_bag = geis_subscription_bag_new(1);
}

/* fixture teardown */
static void
destroy_bag()
{
  geis_subscription_bag_delete(g_sub_bag);
  geis_delete(g_geis);
}


/* verify bag construction/destruction */
START_TEST(construction)
{
  GeisSubBag bag = geis_subscription_bag_new(1);
  fail_unless(bag != NULL,
              "failed to create subscription bag");
  fail_unless(geis_subscription_bag_count(bag) == 0,
              "unexpected size");
  geis_subscription_bag_delete(bag);
}
END_TEST


/* verify bag insertion */
START_TEST(insertion)
{
  GeisSubscription sub = geis_subscription_new(g_geis,
                                               "test-sub",
                                               GEIS_SUBSCRIPTION_NONE);
  geis_subscription_bag_insert(g_sub_bag, sub);
  fail_unless(geis_subscription_bag_count(g_sub_bag) == 1,
  	      "unexpected bag size after insertion");
}
END_TEST


/* verify bag find operation (positive results) */
START_TEST(find_success)
{
  GeisSubscription sub2;
  GeisSubscription sub1 = geis_subscription_new(g_geis,
                                                "test-sub",
                                                GEIS_SUBSCRIPTION_NONE);
  GeisSize id = geis_subscription_bag_insert(g_sub_bag, sub1);
  sub2 = geis_subscription_bag_find(g_sub_bag, id);
  fail_if(sub2 == NULL, "expected instance not found");
  fail_unless(0 == strcmp(geis_subscription_name(sub1),
                          geis_subscription_name(sub2)),
              "unexpected subscription name returned");
}
END_TEST


/* verify bag find operation (negative results) */
START_TEST(find_fail)
{
  GeisSubscription sub = geis_subscription_bag_find(g_sub_bag, 999);
  fail_unless(sub == NULL, "unexpected instance found");
}
END_TEST

START_TEST(expansion)
{
  int i;
  for (i = 0; i < 24; ++i)
  {
    char name[32];
    sprintf(name, "%04d", i);
    GeisSubscription sub = geis_subscription_new(g_geis,
                                                 name,
                                                 GEIS_SUBSCRIPTION_NONE);
    fail_if(geis_subscription_id(sub) != i,
            "unexpected subscription ID returned");
  }
}
END_TEST


/* boilerplate */
Suite *
make_subscription_suite()
{
  Suite *s = suite_create("geis2-subscriptions");

  TCase *create = tcase_create("sub-bag-creation");
  tcase_add_test(create, construction);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("sub-bag-usage");
  tcase_add_checked_fixture(usage, construct_bag, destroy_bag);
  tcase_add_test(usage, insertion);
  tcase_add_test(usage, find_success);
  tcase_add_test(usage, find_fail);
  tcase_add_test(usage, expansion);
  suite_add_tcase(s, usage);

  return s;
}

