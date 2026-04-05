/**
 * Unit tests for GEIS v2.0 Input Gesture Class Module.
 */
#include <check.h>

#include <geis/geis.h>
#include "libgeis/geis_test_api.h"


/* fixtures */
static Geis g_geis;

/* fixture setup */
static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND,
                    GEIS_INIT_TRACK_GESTURE_CLASSES,
                    NULL);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_delete(g_geis);
}


/* Compile-time test to ensure types are defined */
START_TEST(geis_gesture_class_types)
{
  GeisGestureClass gesture_class CK_ATTRIBUTE_UNUSED;
}
END_TEST

/* Compile-time test to ensure constants are defined */
START_TEST(geis_gesture_class_constants)
{
  GeisString attr_name;
  attr_name = GEIS_CLASS_ATTRIBUTE_NAME;
  attr_name = GEIS_CLASS_ATTRIBUTE_ID;
  attr_name = GEIS_EVENT_ATTRIBUTE_CLASS;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_gesture_class_functions)
{
  Geis geis = NULL;
  GeisEventCallback callback = 0;
  GeisGestureClass gesture_class = NULL;
  GeisString n CK_ATTRIBUTE_UNUSED;
  GeisInteger i CK_ATTRIBUTE_UNUSED;
  GeisSize s CK_ATTRIBUTE_UNUSED;
  GeisAttr a CK_ATTRIBUTE_UNUSED;

  geis_register_class_callback(geis, callback, NULL);

  geis_gesture_class_ref(gesture_class);
  geis_gesture_class_unref(gesture_class);
  n = geis_gesture_class_name(gesture_class);
  i = geis_gesture_class_id(gesture_class);
  s = geis_gesture_class_attr_count(gesture_class);
  a = geis_gesture_class_attr(gesture_class, 0);
}
END_TEST

START_TEST(receive_events)
{
  GeisStatus status;
  GeisEvent  event_out;
  int        class_event_count = 0;

  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  status = geis_next_event(g_geis, &event_out);
  while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    if (geis_event_type(event_out) == GEIS_EVENT_CLASS_AVAILABLE)
    {
      ++class_event_count;
    }
    geis_event_delete(event_out);
    status = geis_next_event(g_geis, &event_out);
  }
  fail_unless(class_event_count > 0, "no class events received");
}
END_TEST


/* boilerplate */
Suite *
geis2_gesture_class_suite_new()
{
  TCase *gesture_class;
  TCase *usage;
  Suite *s = suite_create("geis2_gesture_class");

  gesture_class = tcase_create("gesture-class-api");
  tcase_add_test(gesture_class, geis_gesture_class_types);
  tcase_add_test(gesture_class, geis_gesture_class_constants);
  suite_add_tcase(s, gesture_class);

  usage = tcase_create("gesture-class-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, receive_events);
  suite_add_tcase(s, usage);

  return s;
}

