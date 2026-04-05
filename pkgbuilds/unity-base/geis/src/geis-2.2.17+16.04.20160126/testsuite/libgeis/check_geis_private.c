/**
 * internal unit test for the back end event posting interface
 */
#include <check.h>

#include <errno.h>
#include "geis/geis.h"
#include "geis_event.h"
#include "geis_private.h"
#include "geis_test_api.h"
#include <string.h>
#include <sys/select.h>
#include <unistd.h>


/* fixtures */
Geis g_geis;

static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
}

static void
destroy_geis()
{
  geis_delete(g_geis);
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

START_TEST(backend_post)
{
  GeisStatus status;
  GeisEvent event_in = geis_event_new(GEIS_EVENT_GESTURE_END);
  GeisEvent event_out;

  /* empty the event queue */
  while (GEIS_STATUS_CONTINUE == geis_dispatch_events(g_geis))
    ;
  while (GEIS_STATUS_CONTINUE == geis_next_event(g_geis, &event_out))
    geis_event_delete(event_out);

  /* verify a single event */
  geis_post_event(g_geis, event_in);
  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  status = geis_next_event(g_geis, &event_out);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_next_event");
  fail_unless(geis_event_type(event_in) == geis_event_type(event_out),
              "event in and event out types do not match");

  /* verify a multiple events */
  geis_post_event(g_geis, event_in);
  geis_post_event(g_geis, event_in);
  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  status = geis_next_event(g_geis, &event_out);
  fail_unless(status == GEIS_STATUS_CONTINUE,
              "expected CONTINUE status from geis_next_event");
  status = geis_next_event(g_geis, &event_out);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "expected SUCCESS status from geis_next_event");

  geis_event_delete(event_in);
}
END_TEST

/* Check selective queue removal. */
static GeisBoolean
_is_event_type(GeisEvent event, void* context)
{
  GeisEventType event_type = *(GeisEventType*)context;
  return geis_event_type(event) == event_type;
}

START_TEST(remove_matching)
{
  GeisStatus status;

  /* empty the event queue */
  while (GEIS_STATUS_CONTINUE == geis_dispatch_events(g_geis))
    ;
  GeisEvent event_out;
  while (GEIS_STATUS_CONTINUE == geis_next_event(g_geis, &event_out))
    geis_event_delete(event_out);

  /* push 3 controlled events into the queue */
  GeisEvent event_in_1 = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
  geis_post_event(g_geis, event_in_1);
  GeisEvent event_in_2 = geis_event_new(GEIS_EVENT_USER_DEFINED);
  geis_post_event(g_geis, event_in_2);
  GeisEvent event_in_3 = geis_event_new(GEIS_EVENT_GESTURE_END);
  geis_post_event(g_geis, event_in_3);

  /* remove the middle event */
  GeisEventType event_type = GEIS_EVENT_USER_DEFINED;
  geis_remove_matching_events(g_geis, _is_event_type, &event_type);

  /* verify it's no longer in the queue */
  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  for (status = geis_next_event(g_geis, &event_out);
       status != GEIS_STATUS_EMPTY;
       status = geis_next_event(g_geis, &event_out))
  {
    fail_if(GEIS_EVENT_USER_DEFINED == geis_event_type(event_out),
                "event was not successfully removed");
    geis_event_delete(event_out);
  }

}
END_TEST

START_TEST(multiplex_fd)
{
  int pfd[2];
  int geis_fd;
  int called = 0;
  int status = 0;
  int first_time = 1;
  GeisEvent event_out;
  GeisStatus gstat = geis_get_configuration(g_geis,
                                            GEIS_CONFIGURATION_FD,
                                            &geis_fd);
  fail_unless(gstat == GEIS_STATUS_SUCCESS, "unable to get GEIS fd");

  fail_unless(pipe(pfd) == 0, "error %d creating self-pipe: %d",
              errno, strerror(errno));
  geis_multiplex_fd(g_geis, pfd[0], GEIS_BE_MX_READ_AVAILABLE,
                    testcase_event_callback, &called);

  /* empty the event queue */
  while (GEIS_STATUS_CONTINUE == geis_dispatch_events(g_geis))
    ;
  while (GEIS_STATUS_CONTINUE == geis_next_event(g_geis, &event_out))
    geis_event_delete(event_out);

  while (1)
  {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(geis_fd, &fds);

    struct timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 5000;

    status = select(geis_fd+1, &fds, NULL, NULL, &tm);
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
      geis_dispatch_events(g_geis);
      break;
    }
  }

  fail_if(called == 0, "event callback not called");
  fail_if(called >  1, "event callback called too many times");
}
END_TEST

/* boilerplate */
Suite *
make_backend_event_posting_suite()
{
  Suite *s = suite_create("geis2-geis-private");

  TCase *usage = tcase_create("backend-event-posting");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, backend_post);
  tcase_add_test(usage, remove_matching);
  tcase_add_test(usage, multiplex_fd);
  suite_add_tcase(s, usage);

  return s;
}

