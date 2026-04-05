/**
 * internal unit tests for the GEIS v2.0 internal backend token interface
 */
#include <check.h>

#include "geis/geis.h"
#include "geis_backend_token.h"
#include "geis_private.h"
#include "geis_test_api.h"


/* fixtures */
static Geis g_geis;

/* fixture setup */
static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(g_geis);
}


START_TEST(creation)
{
  GeisBackendToken token1 = NULL;
  GeisBackendToken token2 = NULL;

  token1 = geis_backend_token_new(g_geis, GEIS_BACKEND_TOKEN_INIT_ALL);
  fail_if(token1 == NULL, "failed to create token1");
  token2 = geis_backend_token_clone(token1);
  fail_if(token2 == NULL, "failed to create token2");
  fail_if(token2 == token1, "token clone failed");

  geis_backend_token_compose(token1, token2);

  geis_backend_token_delete(token2);
  geis_backend_token_delete(token1);
}
END_TEST


/* boilerplate */
Suite *
make_backend_token_suite()
{
  Suite *s = suite_create("geis2-backend-tokens");

  TCase *usage = tcase_create("token-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, creation);
  suite_add_tcase(s, usage);

  return s;
}

