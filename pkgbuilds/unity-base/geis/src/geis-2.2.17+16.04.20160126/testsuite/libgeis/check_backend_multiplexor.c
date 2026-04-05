/**
 * internal unit tests for the GEIS v2.0 backend_multiplexor module
 */
#include <check.h>

#include <errno.h>
#include "geis/geis.h"
#include "libgeis/geis_backend_multiplexor.h"
#include <string.h>
#include <sys/select.h>
#include <unistd.h>


/* fixtures */
static GeisBackendMultiplexor g_mx;

/* fixture setup */
static void
construct_mx()
{
  g_mx = geis_backend_multiplexor_new();
}

/* fixture teardown */
static void
destroy_mx()
{
  geis_backend_multiplexor_delete(g_mx);
}

static void
testcase_event_callback(int fd,
                        GeisBackendMultiplexorActivity event CK_ATTRIBUTE_UNUSED,
                        void *context)
{
  char buf[2];
  ssize_t ssize CK_ATTRIBUTE_UNUSED;

  ssize = read(fd, buf, 1);
  *(int *)context += 1; 
}


/* verify bag construction/destruction */
START_TEST(construction)
{
  construct_mx();
  fail_unless(g_mx != NULL, "failed to create backend_multiplexor");
  fail_unless(geis_backend_multiplexor_fd(g_mx) >= 0, "invalid MX fd");
  fail_unless(geis_backend_multiplexor_max_events_per_pump(g_mx) == GEIS_BE_MX_DEFAULT_EVENTS_PER_PUMP, "unexpected max fd per pump value");
  destroy_mx();
}
END_TEST


/* verify multiplexor wait */
START_TEST(mx_wait)
{
  int pfd[2];
  int mx_fd = geis_backend_multiplexor_fd(g_mx);
  int called = 0;
  int status = 0;
  int first_time = 1;

  fail_unless(pipe(pfd) == 0, "error %d creating self-pipe: %d",
              errno, strerror(errno));
  geis_backend_multiplexor_add_fd(g_mx,
                                  pfd[0],
                                  GEIS_BE_MX_READ_AVAILABLE,
                                  testcase_event_callback,
                                  &called);

  while (1)
  {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mx_fd, &fds);

    struct timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 5000;

    status = select(mx_fd+1, &fds, NULL, NULL, &tm);
    fail_if(status < 0, "error in select");
    if (status < 0)
    {
      break;
    }
    else if (0 == status)
    {
      fail_unless(first_time, "select timed out before read");
      if (!first_time)
      {
	break;
      }
      first_time = 0;
      ssize_t ssize = write(pfd[1], "1", 1);
      fail_unless(ssize == 1, "error writing to self-pipe");
    }
    else
    {
      geis_backend_multiplexor_pump(g_mx);
      break;
    }
  }

  fail_if(called == 0, "MX event callback not called");
  fail_if(called >  1, "MX event callback called too many times");
}
END_TEST

START_TEST(mx_config)
{
  geis_backend_multiplexor_set_max_events_per_pump(g_mx, 24);
  fail_unless(geis_backend_multiplexor_max_events_per_pump(g_mx) == 24,
              "unexpected max fd per pump value");
}
END_TEST

/* boilerplate */
Suite *
make_backend_multiplexor_suite()
{
  Suite *s = suite_create("geis2-backend-multiplexor");

  TCase *create = tcase_create("backend-multiplexor-creation");
  tcase_add_test(create, construction);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("backend-multiplexor-usage");
  tcase_add_checked_fixture(usage, construct_mx, destroy_mx);
  tcase_add_test(usage, mx_wait);
  tcase_add_test(usage, mx_config);
  suite_add_tcase(s, usage);

  return s;
}

