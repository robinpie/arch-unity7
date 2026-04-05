/**
 * Unit tests for GEIS v2.0 Event Module.
 */
#include <check.h>

#include <geis/geis.h>

/* Compile-time test to ensure types are defined*/
START_TEST(geis_event_types)
{
  GeisEventType type;
  type = GEIS_EVENT_DEVICE_AVAILABLE;
  type = GEIS_EVENT_DEVICE_UNAVAILABLE;
  type = GEIS_EVENT_CLASS_AVAILABLE;
  type = GEIS_EVENT_CLASS_CHANGED;
  type = GEIS_EVENT_CLASS_UNAVAILABLE;
  type = GEIS_EVENT_GESTURE_BEGIN;
  type = GEIS_EVENT_GESTURE_UPDATE;
  type = GEIS_EVENT_GESTURE_END;
  type = GEIS_EVENT_INIT_COMPLETE;
  type = GEIS_EVENT_USER_DEFINED;
  type = GEIS_EVENT_ERROR;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_event_functions)
{
  GeisEvent event = NULL;
  GeisEventType t CK_ATTRIBUTE_UNUSED;
  GeisSize s CK_ATTRIBUTE_UNUSED;
  GeisAttr a CK_ATTRIBUTE_UNUSED;
  GeisAttr n CK_ATTRIBUTE_UNUSED;

  geis_event_delete(event);
  t = geis_event_type(event);
  s = geis_event_attr_count(event);
  a = geis_event_attr(event, 0);
  n = geis_event_attr_by_name(event, "none");
}
END_TEST


/* boilerplate */
Suite *
geis2_event_suite_new()
{
  Suite *s = suite_create("geis2_event");

  TCase *event = tcase_create("geis2_event");
  tcase_add_test(event, geis_event_types);
  suite_add_tcase(s, event);

  return s;
}

