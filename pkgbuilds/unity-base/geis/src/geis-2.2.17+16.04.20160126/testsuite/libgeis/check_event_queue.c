/**
 * unit tests for the geis_event_queue module
 */
#include <check.h>

#include "geis/geis.h"
#include "libgeis/geis_event.h"
#include "libgeis/geis_event_queue.h"


/* fixtures */
static GeisEventQueue g_queue;

/* fixture setup */
static void
construct_event_queue()
{
  g_queue = geis_event_queue_new();
}

/* fixture teardown */
static void
destroy_event_queue()
{
  geis_event_queue_delete(g_queue);
}


/* verify event queue construction/destruction */
START_TEST(construction)
{
  construct_event_queue();
  fail_unless(g_queue != NULL, "failed to create the event queue");
  fail_unless(geis_event_queue_is_empty(g_queue), "queue is not empty");
  destroy_event_queue();
}
END_TEST


/* verify event_queue insertion */
START_TEST(enqueue_dequeue)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisEvent event1 = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
  GeisEvent event2 = geis_event_new(GEIS_EVENT_GESTURE_END);
  GeisEvent ev;

  ev = geis_event_queue_dequeue(g_queue);
  fail_unless(ev == NULL, "unexpected failure at pop(0)");

  status = geis_event_queue_enqueue(g_queue, event1);
  fail_unless(status == GEIS_STATUS_SUCCESS, "failure at enqueue(event1)");
  fail_unless(!geis_event_queue_is_empty(g_queue), "queue is unexpectedly empty");
  status = geis_event_queue_enqueue(g_queue, event2);
  fail_unless(status == GEIS_STATUS_SUCCESS, "failure at enqueue(event2)");

  ev = geis_event_queue_dequeue(g_queue);
  fail_unless(ev != NULL, "failure at dequeue(1)");
  fail_unless(ev == event1, "unexpected value returned from front(1)");

  ev = geis_event_queue_dequeue(g_queue);
  fail_unless(ev != NULL, "failure at dequeue(2)");
  fail_unless(ev == event2, "unexpected value returned from front(2)");

  ev = geis_event_queue_dequeue(g_queue);
  fail_unless(ev == NULL, "failure at dequeue(3)");
  fail_unless(geis_event_queue_is_empty(g_queue), "queue is not empty");
}
END_TEST

/* verify event_queue remove_if */
static GeisBoolean
_is_event_type(GeisEvent event, void* context)
{
  GeisEventType event_type = *(GeisEventType*)context;
  return geis_event_type(event) == event_type;
}

START_TEST(remove_if)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisEvent event1 = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
  status = geis_event_queue_enqueue(g_queue, event1);
  fail_unless(status == GEIS_STATUS_SUCCESS, "failure at enqueue(event1)");

  GeisEvent event2 = geis_event_new(GEIS_EVENT_USER_DEFINED);
  status = geis_event_queue_enqueue(g_queue, event2);
  fail_unless(status == GEIS_STATUS_SUCCESS, "failure at enqueue(event2)");

  GeisEvent event3 = geis_event_new(GEIS_EVENT_GESTURE_END);
  status = geis_event_queue_enqueue(g_queue, event3);
  fail_unless(status == GEIS_STATUS_SUCCESS, "failure at enqueue(event3)");

  GeisEventType event_type = GEIS_EVENT_USER_DEFINED;
  geis_event_queue_remove_if(g_queue, _is_event_type, &event_type);

  event_type = GEIS_EVENT_GESTURE_BEGIN;
  geis_event_queue_remove_if(g_queue, _is_event_type, &event_type);

  event_type = GEIS_EVENT_GESTURE_END;
  geis_event_queue_remove_if(g_queue, _is_event_type, &event_type);

  fail_unless(geis_event_queue_is_empty(g_queue), "queue is not empty");
}
END_TEST

/* boilerplate */
Suite *
make_event_queue_suite()
{
  Suite *s = suite_create("geis2-event-queue");

  TCase *create = tcase_create("event-queue-creation");
  tcase_add_test(create, construction);
  suite_add_tcase(s, create);

  TCase *usage = tcase_create("event-queue-operation");
  tcase_add_checked_fixture(usage, construct_event_queue, destroy_event_queue);
  tcase_add_test(usage, enqueue_dequeue);
  tcase_add_test(usage, remove_if);
  suite_add_tcase(s, usage);

  return s;
}

