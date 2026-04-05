/**
 * Unit tests for GEIS v2.0 API instance creation.
 */
#include <check.h>

#include <geis/geis.h>
#include "libgeis/geis_test_api.h"


/* compile-time test to ensure required init args are defined*/
START_TEST(geis_init_args)
{
  GeisString init_arg;
  init_arg = GEIS_INIT_SERVICE_PROVIDER;
  init_arg = GEIS_INIT_TRACK_DEVICES;
  init_arg = GEIS_INIT_TRACK_GESTURE_CLASSES;
}
END_TEST

/* compile-time test to ensure required functions are defined */
START_TEST(geis_new_and_delete)
{
  GeisStatus status;
  Geis geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  fail_unless(geis != NULL, "failed to create GEIS v2.0 API instance");

  status = geis_delete(geis);
  fail_unless(status == GEIS_STATUS_SUCCESS, "geis_delete failed");
}
END_TEST


/* boilerplate */
Suite *
geis2_geis_new_suite_new()
{
  Suite *s = suite_create("geis2_geis_init");

  TCase *create = tcase_create("geis_init");
  tcase_add_test(create, geis_init_args);
  tcase_add_test(create, geis_new_and_delete);
  suite_add_tcase(s, create);

  return s;
}

