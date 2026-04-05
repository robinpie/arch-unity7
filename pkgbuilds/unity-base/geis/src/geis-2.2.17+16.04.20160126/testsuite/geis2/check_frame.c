/**
 * Unit tests for GEIS v2.0 Gesture Frame Module.
 */
#include <check.h>

#include <geis/geis.h>
#include "libgeis/geis_test_api.h"
#include <stdio.h>

#define MAX_CLASS_COUNT 10


/* fixtures */
static Geis g_geis;
static GeisSubscription g_sub;
static GeisSize g_class_count = 0;
static GeisGestureClass g_class[MAX_CLASS_COUNT];

/* fixture setup */
static void
construct_geis()
{
  g_geis = geis_new(GEIS_INIT_MOCK_BACKEND,
                    GEIS_INIT_TRACK_GESTURE_CLASSES,
                    NULL);
  g_sub = geis_subscription_new(g_geis, "test", 0);
  geis_subscription_activate(g_sub);
}

/* fixture teardown */
static void
destroy_geis()
{
  geis_subscription_delete(g_sub);
  geis_delete(g_geis);
}


/* Compile-time test to ensure types are defined */
START_TEST(geis_gesture_frame_types)
{
  GeisGroup     group CK_ATTRIBUTE_UNUSED;
  GeisGroupSet  groupset CK_ATTRIBUTE_UNUSED;
  GeisTouch     touch CK_ATTRIBUTE_UNUSED;
  GeisTouchSet  touchset CK_ATTRIBUTE_UNUSED;
  GeisFrame     frame CK_ATTRIBUTE_UNUSED;
  GeisGestureId gesture_id CK_ATTRIBUTE_UNUSED;
}
END_TEST

/* Compile-time test to ensure constants are defined */
START_TEST(geis_gesture_frame_constants)
{
  GeisString attr_name;
  attr_name = GEIS_GESTURE_ATTRIBUTE_ANGLE;
  attr_name = GEIS_GESTURE_ATTRIBUTE_ANGLE_DELTA;
  attr_name = GEIS_GESTURE_ATTRIBUTE_ANGULAR_VELOCITY;
  attr_name = GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X1;
  attr_name = GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y1;
  attr_name = GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X2;
  attr_name = GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y2;
  attr_name = GEIS_GESTURE_ATTRIBUTE_DELTA_X;
  attr_name = GEIS_GESTURE_ATTRIBUTE_DELTA_Y;
  attr_name = GEIS_GESTURE_ATTRIBUTE_RADIUS;
  attr_name = GEIS_GESTURE_ATTRIBUTE_POSITION_X;
  attr_name = GEIS_GESTURE_ATTRIBUTE_POSITION_Y;
  attr_name = GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA;
  attr_name = GEIS_GESTURE_ATTRIBUTE_RADIAL_VELOCITY;
  attr_name = GEIS_GESTURE_ATTRIBUTE_VELOCITY_X;
  attr_name = GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y;
  attr_name = GEIS_EVENT_ATTRIBUTE_GROUPSET;
  attr_name = GEIS_EVENT_ATTRIBUTE_TOUCHSET;
}
END_TEST

/* Compile-and-link-time test to verify required functions exist */
START_TEST(geis_gesture_frame_functions)
{
  GeisGroupSet groupset = NULL;
  GeisSize size CK_ATTRIBUTE_UNUSED;

  size = geis_groupset_group_count(groupset);
}
END_TEST

START_TEST(receive_events)
{
  GeisStatus status;
  GeisEvent  event;
  int        gesture_event_count = 0;

  status = geis_dispatch_events(g_geis);
  fail_unless(status == GEIS_STATUS_SUCCESS,
              "unexpected status from geis_dispatch_events");
  status = geis_next_event(g_geis, &event);
  while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    if (geis_event_type(event) == GEIS_EVENT_GESTURE_BEGIN)
    {
      GeisSize i;
      GeisTouchSet touchset;
      GeisGroupSet groupset;
      GeisBoolean  is_poke = GEIS_FALSE;

      GeisAttr attr = geis_event_attr_by_name(event,
                                              GEIS_EVENT_ATTRIBUTE_TOUCHSET);
      fail_if(!attr, "touchset attr not found in gesture event");

      touchset = geis_attr_value_to_pointer(attr);
      fail_if(!touchset, "touchset not found in gesture event attr");

      attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
      fail_if(!attr, "groupset attr not found in gesture event");

      groupset = geis_attr_value_to_pointer(attr);
      fail_if(!groupset, "groupset not found in gesture event attr");

      for (i= 0; i < geis_groupset_group_count(groupset); ++i)
      {
	GeisSize j;
	GeisGroup group = geis_groupset_group(groupset, i);
	fail_if(!group, "group %d not found in groupset", i);

	for (j=0; j < geis_group_frame_count(group); ++j)
	{
	  GeisSize k;
	  GeisSize c;
	  GeisFrame frame = geis_group_frame(group, j);
	  fail_if(!frame, "frame %d not found in group", j);

	  for (c=0; c < g_class_count; ++c)
	  {
	    if (0 == strcmp(geis_gesture_class_name(g_class[c]), "poke"))
	    {
	      is_poke = geis_frame_is_class(frame, g_class[c]);
	      break;
	    }
	  }
	  fail_if(!is_poke, "gesture is not of expect type");

	  for (k = 0; k < geis_frame_touchid_count(frame); ++k)
	  {
	    GeisSize touchid = geis_frame_touchid(frame, k);
	    GeisTouch touch = geis_touchset_touch_by_id(touchset, touchid);
	    fail_if(!touch, "touch %d not found in touch set", touchid);
	  }
	}
      }
      ++gesture_event_count;
    }
    else if (geis_event_type(event) == GEIS_EVENT_CLASS_AVAILABLE)
    {
      GeisAttr         attr;
      GeisGestureClass gesture_class;

      attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
      fail_if (!attr, "attr not found in class event");

      gesture_class = geis_attr_value_to_pointer(attr);
      fail_if (!gesture_class, "geis class not found in class event");

      g_class[g_class_count++] = gesture_class;
    }
    geis_event_delete(event);
    status = geis_next_event(g_geis, &event);
  }
  fail_unless(gesture_event_count > 0, "no gesture events received");

}
END_TEST


/* boilerplate */
Suite *
geis2_gesture_frame_suite_new()
{
  Suite *s = suite_create("geis2_gesture_frame");
  TCase *gesture_frame;
  TCase *usage;

  gesture_frame = tcase_create("gesture-frame-api");
  tcase_add_test(gesture_frame, geis_gesture_frame_types);
  tcase_add_test(gesture_frame, geis_gesture_frame_constants);
  suite_add_tcase(s, gesture_frame);

  usage = tcase_create("gesture-frame-usage");
  tcase_add_checked_fixture(usage, construct_geis, destroy_geis);
  tcase_add_test(usage, receive_events);
  suite_add_tcase(s, usage);

  return s;
}

