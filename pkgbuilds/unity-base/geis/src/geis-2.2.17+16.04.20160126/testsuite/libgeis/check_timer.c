/**
 * internal unit tests for the GEIS v2.0 subscription module
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include <check.h>
#include "geis/geis.h"
#include "geis_timer.h"
#include "geis_test_api.h"
#include <stdio.h>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <unistd.h>


/* fixtures */
static Geis g_geis;
int         g_geis_fd;

static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  geis_get_configuration(g_geis, GEIS_CONFIGURATION_FD, &g_geis_fd);
}

static void
destroy_geis()
{
  geis_delete(g_geis);
}

static void
_timer_callback(GeisTimer timer CK_ATTRIBUTE_UNUSED, void *context)
{
  int efd = *(int *)context;
  uint64_t datum = 1;
  (void)write(efd, &datum, sizeof(datum));
}


START_TEST(timer_expire)
{
  int timer_fired = 0;
  int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  GeisTimer timer = geis_timer_new(g_geis, _timer_callback, &efd);
  geis_timer_start(timer, 100);

  while (1)
  {
    int status = 0;
    int max_fd = (g_geis_fd > efd ? g_geis_fd : efd) + 1;
    struct timeval tm;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(g_geis_fd, &fds);
    FD_SET(efd, &fds);
    tm.tv_sec = 1;
    tm.tv_usec = 0;

    status = select(max_fd + 1, &fds, NULL, NULL, &tm);
    if (status < 0)
    {
      fail_if(status < 0, "error in select");
      break;
    }
    if (status == 0)
    {
      fail_if(status == 0, "no events detected");
      break;
    }
    else if (FD_ISSET(g_geis_fd, &fds))
    {
      geis_dispatch_events(g_geis);
    }
    if (FD_ISSET(efd, &fds))
    {
      timer_fired = 1;
      break;
    }
  }

  geis_timer_delete(timer);
  close(efd);

  fail_unless(timer_fired, "timer did not fire");
}
END_TEST


START_TEST(timer_cancel)
{
  static const int timer_timeout = 100; /* milliseconds */
  static const int microseconds_per_millisecond = 1000;
  int timer_cancelled = 0;
  int timer_fired = 0;
  int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  GeisTimer timer = geis_timer_new(g_geis, _timer_callback, &efd);
  geis_timer_start(timer, timer_timeout);

  while (1)
  {
    int status = 0;
    int max_fd = (g_geis_fd > efd ? g_geis_fd : efd) + 1;
    struct timeval tm;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(g_geis_fd, &fds);
    FD_SET(efd, &fds);
    tm.tv_sec = 0;
    tm.tv_usec = 1 * microseconds_per_millisecond; /* 1 millisecond */

    status = select(max_fd + 1, &fds, NULL, NULL, &tm);
    if (status < 0)
    {
      fail_if(status < 0, "error in select");
      break;
    }
    if (status == 0)
    {
      if (timer_cancelled)
        break;
      geis_timer_cancel(timer);
      timer_cancelled = 1;
      tm.tv_usec = timer_timeout * microseconds_per_millisecond * 2;
    }
    else if (FD_ISSET(g_geis_fd, &fds))
    {
      geis_dispatch_events(g_geis);
    }
    if (FD_ISSET(efd, &fds))
    {
      timer_fired = 1;
      break;
    }
  }

  geis_timer_delete(timer);
  close(efd);

  fail_if(timer_fired, "timer fired after cancellation");
}
END_TEST


/* boilerplate */
Suite *
make_timer_suite()
{
  Suite *s = suite_create("geis2-geis-timer");

  TCase *usage = tcase_create("timer");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, timer_expire);
  tcase_add_test(usage, timer_cancel);
  suite_add_tcase(s, usage);

  return s;
}

