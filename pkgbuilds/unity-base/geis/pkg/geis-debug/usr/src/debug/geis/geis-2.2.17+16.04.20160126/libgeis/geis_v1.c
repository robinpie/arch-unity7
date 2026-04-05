/**
 * @file libgeis/geis_v1.c
 * @brief implementation of the GEIS v1.0 API instance
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <errno.h>
#include "geis/geis.h"
#include "geis_class.h"
#include "geis_config.h"
#include "geis_filter.h"
#include "geis_logging.h"
#include "geis_private.h"
#include "geis_subscription.h"
#include "geis_test_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define WINDOW_FILTER "geis v1"

struct _GeisInstance
{
  Geis              geis;
  GeisSubscription  subscription;
  GeisFilter        window_filter;
  GeisInputFuncs    input_funcs;
  GeisPointer       input_context;
  GeisGestureFuncs  gesture_funcs;
  GeisPointer       gesture_cookie;
  GeisBoolean       init_failed;
};

/* maps GEIS v1 gesture names into GEIS v2 gesture attrs */
typedef struct _GeisV1AttrMap *GeisV1AttrMap;
static struct _GeisV1AttrMap
{
  const char* v1_type;
  const char* v2_name;
  GeisInteger v2_id;
  GeisInteger touches;
} s_v1_attr_map[] =
{
  { GEIS_GESTURE_TYPE_DRAG1,   GEIS_GESTURE_DRAG,   GEIS_GESTURE_PRIMITIVE_DRAG,   1 },
  { GEIS_GESTURE_TYPE_DRAG2,   GEIS_GESTURE_DRAG,   GEIS_GESTURE_PRIMITIVE_DRAG,   2 },
  { GEIS_GESTURE_TYPE_DRAG3,   GEIS_GESTURE_DRAG,   GEIS_GESTURE_PRIMITIVE_DRAG,   3 },
  { GEIS_GESTURE_TYPE_DRAG4,   GEIS_GESTURE_DRAG,   GEIS_GESTURE_PRIMITIVE_DRAG,   4 },
  { GEIS_GESTURE_TYPE_DRAG5,   GEIS_GESTURE_DRAG,   GEIS_GESTURE_PRIMITIVE_DRAG,   5 },
  { GEIS_GESTURE_TYPE_PINCH1,  GEIS_GESTURE_PINCH,  GEIS_GESTURE_PRIMITIVE_PINCH,  1 },
  { GEIS_GESTURE_TYPE_PINCH2,  GEIS_GESTURE_PINCH,  GEIS_GESTURE_PRIMITIVE_PINCH,  2 },
  { GEIS_GESTURE_TYPE_PINCH3,  GEIS_GESTURE_PINCH,  GEIS_GESTURE_PRIMITIVE_PINCH,  3 },
  { GEIS_GESTURE_TYPE_PINCH4,  GEIS_GESTURE_PINCH,  GEIS_GESTURE_PRIMITIVE_PINCH,  4 },
  { GEIS_GESTURE_TYPE_PINCH5,  GEIS_GESTURE_PINCH,  GEIS_GESTURE_PRIMITIVE_PINCH,  5 },
  { GEIS_GESTURE_TYPE_ROTATE1, GEIS_GESTURE_ROTATE, GEIS_GESTURE_PRIMITIVE_ROTATE, 1 },
  { GEIS_GESTURE_TYPE_ROTATE2, GEIS_GESTURE_ROTATE, GEIS_GESTURE_PRIMITIVE_ROTATE, 2 },
  { GEIS_GESTURE_TYPE_ROTATE3, GEIS_GESTURE_ROTATE, GEIS_GESTURE_PRIMITIVE_ROTATE, 3 },
  { GEIS_GESTURE_TYPE_ROTATE4, GEIS_GESTURE_ROTATE, GEIS_GESTURE_PRIMITIVE_ROTATE, 4 },
  { GEIS_GESTURE_TYPE_ROTATE5, GEIS_GESTURE_ROTATE, GEIS_GESTURE_PRIMITIVE_ROTATE, 5 },
  { GEIS_GESTURE_TYPE_TAP1,    GEIS_GESTURE_TAP,    GEIS_GESTURE_PRIMITIVE_TAP,    1 },
  { GEIS_GESTURE_TYPE_TAP2,    GEIS_GESTURE_TAP,    GEIS_GESTURE_PRIMITIVE_TAP,    2 },
  { GEIS_GESTURE_TYPE_TAP3,    GEIS_GESTURE_TAP,    GEIS_GESTURE_PRIMITIVE_TAP,    3 },
  { GEIS_GESTURE_TYPE_TAP4,    GEIS_GESTURE_TAP,    GEIS_GESTURE_PRIMITIVE_TAP,    4 },
  { GEIS_GESTURE_TYPE_TAP5,    GEIS_GESTURE_TAP,    GEIS_GESTURE_PRIMITIVE_TAP,    5 },
  { GEIS_GESTURE_TYPE_TOUCH1,  GEIS_GESTURE_TOUCH,  GEIS_GESTURE_PRIMITIVE_TOUCH,  1 },
  { GEIS_GESTURE_TYPE_TOUCH2,  GEIS_GESTURE_TOUCH,  GEIS_GESTURE_PRIMITIVE_TOUCH,  2 },
  { GEIS_GESTURE_TYPE_TOUCH3,  GEIS_GESTURE_TOUCH,  GEIS_GESTURE_PRIMITIVE_TOUCH,  3 },
  { GEIS_GESTURE_TYPE_TOUCH4,  GEIS_GESTURE_TOUCH,  GEIS_GESTURE_PRIMITIVE_TOUCH,  4 },
  { GEIS_GESTURE_TYPE_TOUCH5,  GEIS_GESTURE_TOUCH,  GEIS_GESTURE_PRIMITIVE_TOUCH,  5 },
  { GEIS_GESTURE_TYPE_FLICK1,  GEIS_GESTURE_FLICK,  100,                           1 },
  { GEIS_GESTURE_TYPE_FLICK2,  GEIS_GESTURE_FLICK,  100,                           2 },
  { GEIS_GESTURE_TYPE_FLICK3,  GEIS_GESTURE_FLICK,  100,                           3 },
  { GEIS_GESTURE_TYPE_FLICK4,  GEIS_GESTURE_FLICK,  100,                           4 },
  { GEIS_GESTURE_TYPE_FLICK5,  GEIS_GESTURE_FLICK,  100,                           5 },
  { NULL,                      NULL,                0,                             0 }
};

static const GeisString UNKNOWN_GESTURE_TYPE = "(unknown)";

static GeisString
_v1_map_v2_name_to_v1_name(GeisString v2_name, GeisInteger touches)
{
  for (int i = 0; s_v1_attr_map[i].v1_type; ++i)
  {
    if (0 == strcmp(s_v1_attr_map[i].v2_name, v2_name)
     && s_v1_attr_map[i].touches == touches)
    {
      return s_v1_attr_map[i].v1_type;
    }
  }
  return UNKNOWN_GESTURE_TYPE;
}


static struct { const char *id; const char* x; const char* y; } s_touch_attr_names[] =
{
  {
    GEIS_GESTURE_ATTRIBUTE_TOUCH_0_ID,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_0_X,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_0_Y
  },
  {
    GEIS_GESTURE_ATTRIBUTE_TOUCH_1_ID,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_1_X,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_1_Y
  },
  {
    GEIS_GESTURE_ATTRIBUTE_TOUCH_2_ID,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_2_X,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_2_Y
  },
  {
    GEIS_GESTURE_ATTRIBUTE_TOUCH_3_ID,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_3_X,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_3_Y
  },
  {
    GEIS_GESTURE_ATTRIBUTE_TOUCH_4_ID,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_4_X,
    GEIS_GESTURE_ATTRIBUTE_TOUCH_4_Y
  }
};


static GeisString
_generate_subscription_name(GeisXcbWinInfo* win_info)
{
  static char buffer[32];
  if (win_info)
    sprintf(buffer, "0x%08x", win_info->window_id);
  else
    sprintf(buffer, "mock window");
  return buffer;
}


/*
 * Maps the geis v2 attrs in a frame to geis v1 attrs.
 */
static GeisGestureAttr *
_map_frame_to_attrs(Geis          geis,
                    GeisFrame     frame, 
                    GeisTouchSet  touchset,
                    GeisInteger  *class_id,
                    GeisSize     *attr_count)
{
  GeisSize num_attrs = geis_frame_attr_count(frame);
  GeisSize i;
  GeisSize free_slot = 9; /* yes, it's magic! */
  GeisSize num_touches = geis_frame_touchid_count(frame) * 3; /* more magic! */
  GeisSize gga_size = free_slot + num_touches + num_attrs; 
  GeisString gesture_name = NULL;

  *attr_count = 0;

  geis_debug("num_attrs = %zu", num_attrs);
  GeisGestureAttr *attrs = calloc(gga_size, sizeof(GeisGestureAttr));
  if (!attrs)
  {
    geis_error("can not allocate gesture attrs");
    goto final_exit;
  }

  /* Determine the dominant class. */
  {
    GeisGestureClassBag classes = geis_gesture_classes(geis);
    GeisSize classes_count = geis_gesture_class_bag_count(classes);
    GeisSize i;
    for (i = 0; i < classes_count; ++i)
    {
      GeisGestureClass gesture_class = NULL;
      GeisInteger id = 0;

      gesture_class = geis_gesture_class_bag_gesture_class(classes, i);
      id = geis_gesture_class_id(gesture_class);
      if (geis_frame_is_class(frame, gesture_class))
      {
	*class_id = id;
	gesture_name = geis_gesture_class_name(gesture_class);
	if (id > 100)
	  break;
      }
    }
  }

  /* Map V2 attrs into V1 attrs. */
  for (i = 0; i < num_attrs; ++i)
  {
    GeisAttr attr = geis_frame_attr(frame, i);
    GeisString attr_name = geis_attr_name(attr);

    if (0 == strcmp(attr_name, GEIS_CLASS_ATTRIBUTE_ID))
    {
      /* skip this, do not pass it on */
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_DEVICE_ID))
    {
      attrs[0].name        = attr_name;
      attrs[0].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[0].integer_val = geis_attr_value_to_integer(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP))
    {
      attrs[1].name        = attr_name;
      attrs[1].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[1].integer_val = geis_attr_value_to_integer(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_ROOT_WINDOW_ID))
    {
      attrs[2].name        = attr_name;
      attrs[2].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[2].integer_val = geis_attr_value_to_integer(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID))
    {
      attrs[3].name        = attr_name;
      attrs[3].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[3].integer_val = geis_attr_value_to_integer(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_CHILD_WINDOW_ID))
    {
      attrs[4].name        = attr_name;
      attrs[4].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[4].integer_val = geis_attr_value_to_integer(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_FOCUS_X))
    {
      attrs[5].name        = attr_name;
      attrs[5].type        = GEIS_ATTR_TYPE_FLOAT;
      attrs[5].float_val = geis_attr_value_to_float(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_FOCUS_Y))
    {
      attrs[6].name        = attr_name;
      attrs[6].type        = GEIS_ATTR_TYPE_FLOAT;
      attrs[6].float_val = geis_attr_value_to_float(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME))
    {
      attrs[7].name       = attr_name;
      attrs[7].type       = GEIS_ATTR_TYPE_STRING;
      if (!gesture_name) gesture_name = geis_attr_value_to_string(attr);
      ++*attr_count;
    }
    else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_TOUCHES))
    {
      num_touches = geis_attr_value_to_integer(attr);
      attrs[8].name        = attr_name;
      attrs[8].type        = GEIS_ATTR_TYPE_INTEGER;
      attrs[8].integer_val = num_touches;
      ++*attr_count;
    }
    else
    {
      attrs[free_slot].name        = attr_name;
      attrs[free_slot].type        = GEIS_ATTR_TYPE_FLOAT;
      attrs[free_slot].float_val = geis_attr_value_to_float(attr);
      ++free_slot;
      ++*attr_count;
    }
  }

  /* fix up the gesture name for backwards compatibility */
  attrs[7].string_val = _v1_map_v2_name_to_v1_name(gesture_name, num_touches);

  for (i = 0; i < geis_frame_touchid_count(frame); ++i)
  {
    GeisTouch touch = geis_touchset_touch_by_id(touchset,
                                                geis_frame_touchid(frame, i));
    GeisSize touch_attr_count = geis_touch_attr_count(touch);
    GeisSize j;
    for (j = 0; j < touch_attr_count; ++j)
    {
      GeisAttr attr = geis_touch_attr(touch, j);
      GeisString attr_name = geis_attr_name(attr);

      if (0 == strcmp(attr_name, GEIS_TOUCH_ATTRIBUTE_ID))
      {
	attrs[free_slot].name      = s_touch_attr_names[i].id;
	attrs[free_slot].type      = GEIS_ATTR_TYPE_FLOAT;
	attrs[free_slot].float_val = geis_attr_value_to_float(attr);
	++free_slot;
	++*attr_count;
      }
      else if (0 == strcmp(attr_name, GEIS_TOUCH_ATTRIBUTE_X))
      {
	attrs[free_slot].name      = s_touch_attr_names[i].x;
	attrs[free_slot].type      = GEIS_ATTR_TYPE_FLOAT;
	attrs[free_slot].float_val = geis_attr_value_to_float(attr);
	++free_slot;
	++*attr_count;
      }
      else if (0 == strcmp(attr_name, GEIS_TOUCH_ATTRIBUTE_Y))
      {
	attrs[free_slot].name      = s_touch_attr_names[i].y;
	attrs[free_slot].type      = GEIS_ATTR_TYPE_FLOAT;
	attrs[free_slot].float_val = geis_attr_value_to_float(attr);
	++free_slot;
	++*attr_count;
      }
    }
  }

final_exit:
  return attrs;
}


static void
_v1_event_callback(Geis        geis,
                   GeisEvent   event,
                   GeisPointer context)
{
  GeisInstance v1_instance = (GeisInstance)context;
  switch (geis_event_type(event))
  {
    case GEIS_EVENT_INIT_COMPLETE:
      geis_debug("received INIT event");
      goto final_exit;

    case GEIS_EVENT_ERROR:
      geis_debug("received ERROR event");
      v1_instance->init_failed = GEIS_TRUE;
      goto final_exit;

    case GEIS_EVENT_DEVICE_AVAILABLE:
    case GEIS_EVENT_DEVICE_UNAVAILABLE:
    case GEIS_EVENT_CLASS_AVAILABLE:
    case GEIS_EVENT_CLASS_CHANGED:
    case GEIS_EVENT_CLASS_UNAVAILABLE:
      geis_debug("received DEVICE/CLASS event");
      goto final_exit;

    default:
      geis_debug("received GESTURE event");
      break;
  }

  GeisInteger class_id = 0;
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  if (!attr)
  {
    geis_error("no touchset for gesture event");
    goto final_exit;
  }

  GeisTouchSet touchset = geis_attr_value_to_pointer(attr);
  if (!touchset)
  {
    geis_warning("can not convert attr to touchset");
    goto final_exit;
  }

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  if (!attr)
  {
    geis_error("no groupset for gesture event");
    goto final_exit;
  }

  GeisGroupSet groupset = geis_attr_value_to_pointer(attr);
  if (!groupset)
  {
    geis_warning("can not convert attr to groupset");
    goto final_exit;
  }

  GeisSize i;
  for (i = 0; i < geis_groupset_group_count(groupset); ++i)
  {
    GeisGroup group = geis_groupset_group(groupset, i);
    if (!group)
    {
      geis_warning("can not extract group %zu from groupset", i);
      goto final_exit;
    }

    GeisSize j;
    for (j = 0; j < geis_group_frame_count(group); ++j)
    {
      GeisSize attr_count = 0;
      GeisFrame frame = geis_group_frame(group, j);
      if (!frame)
      {
	geis_warning("can not extract frame %zu from group", j);
	goto final_exit;
      }

      GeisGestureAttr *attrs = _map_frame_to_attrs(geis,
                                                   frame,
                                                   touchset,
                                                   &class_id,
                                                   &attr_count);
      if (!attrs)
      {
	geis_error("can not allocate gesture attrs");
	goto final_exit;
      }

      switch (geis_event_type(event))
      {
	case GEIS_EVENT_GESTURE_BEGIN:
	  v1_instance->gesture_funcs.start(v1_instance->gesture_cookie,
	                                    class_id,
	                                    geis_frame_id(frame),
	                                    attr_count, attrs);
	  break;
	case GEIS_EVENT_GESTURE_UPDATE:
	  v1_instance->gesture_funcs.update(v1_instance->gesture_cookie,
	                                     class_id,
	                                     geis_frame_id(frame),
	                                     attr_count, attrs);
	  break;
	case GEIS_EVENT_GESTURE_END:
	  v1_instance->gesture_funcs.finish(v1_instance->gesture_cookie,
	                                     class_id,
	                                     geis_frame_id(frame),
	                                     attr_count, attrs);
	  break;
	default:
	  geis_debug("-- event ignored --");
	  break;
      }

      free(attrs);

      /* in geis v1 only the first frame counts. */
      break;
    }

    /* in geis v1 only the first group counts. */
    break;
  }

final_exit:
  geis_event_delete(event);
}


GeisStatus
geis_init(GeisWinInfo* win_info, GeisInstance *geis_instance)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisXcbWinInfo *xcb_win_info = (GeisXcbWinInfo*)win_info->win_info;
  GeisInteger window = 0;
  GeisInstance instance;

  instance = calloc(1, sizeof(struct _GeisInstance));
  if (!instance)
  {
    geis_error("error allocating GEIS API instance.");
    goto final_exit;
  }

  if (xcb_win_info)
  {
    window = (GeisInteger)(xcb_win_info->window_id);
  }

  if (win_info->win_type == geis_win_type_str(Test))
  {
    if (xcb_win_info)
      instance->geis = geis_new(GEIS_INIT_MOCK_BACKEND,
                                GEIS_INIT_TRACK_GESTURE_CLASSES,
                                NULL);
    else
      instance->geis = geis_new(GEIS_INIT_MOCK_BACKEND, NULL);
  }
  else
  {
    instance->geis = geis_new(GEIS_INIT_TRACK_DEVICES,
                              GEIS_INIT_TRACK_GESTURE_CLASSES,
                              GEIS_INIT_SYNCHRONOUS_START,
                              NULL);
  }
  if (!instance->geis)
  {
    goto unwind_instance;
  }

  geis_register_event_callback(instance->geis, _v1_event_callback, instance);

  instance->subscription = geis_subscription_new(instance->geis,
                          _generate_subscription_name(xcb_win_info),
                          GEIS_SUBSCRIPTION_CONT);
  instance->window_filter = geis_filter_new(instance->geis, WINDOW_FILTER);
  geis_filter_add_term(instance->window_filter,
                       GEIS_FILTER_REGION,
                       GEIS_REGION_ATTRIBUTE_WINDOWID, GEIS_FILTER_OP_EQ, window,
                       NULL);
  status = geis_subscription_add_filter(instance->subscription,
                                        instance->window_filter);

  instance->init_failed = GEIS_FALSE;

  *geis_instance = instance;
  goto final_exit;

unwind_instance:
  free(instance);
final_exit:
  return status;
}


GeisStatus
geis_finish(GeisInstance instance)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  geis_subscription_delete(instance->subscription);
  geis_delete(instance->geis);
  free(instance);
  return status;
}


GeisStatus
geis_configuration_supported(GeisInstance geis_instance GEIS_UNUSED,
                             int          configuration_item)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;

  switch (configuration_item)
  {
    case GEIS_CONFIG_UNIX_FD:
      status = GEIS_STATUS_SUCCESS;
      break;
  }
  return status;
}


GeisStatus
geis_configuration_get_value(GeisInstance  instance,
                             int           configuration_item, 
                             void         *value)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;

  if (!value)
  {
    return GEIS_BAD_ARGUMENT;
  }

  switch (configuration_item)
  {
    case GEIS_CONFIG_UNIX_FD:
      status = geis_get_configuration(instance->geis,
                                      GEIS_CONFIGURATION_FD, value);
      break;
  }
  return status;
}


/*
 * Sets a feature configuration value.
 */
GeisStatus
geis_configuration_set_value(GeisInstance geis_instance GEIS_UNUSED,
                             int          configuration_item, 
                             void         *value)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;
  if (!value)
  {
    return GEIS_BAD_ARGUMENT;
  }

  switch (configuration_item)
  {
    default:
      break;
  }
  return status;
}


GeisStatus
geis_event_dispatch(GeisInstance instance)
{
  geis_dispatch_events(instance->geis);
  return GEIS_STATUS_SUCCESS;
}


static GeisStatus
_v1_subscribe_device(GeisInstance instance,
                     GeisInteger device_id,
                     const char **gesture_list)
{
  GeisStatus result = GEIS_UNKNOWN_ERROR;
  if (gesture_list == GEIS_ALL_GESTURES)
  {
    geis_debug("subscribing device %d for all gestures", device_id);
  }
  else
  {
    const char **g;

    geis_debug("subscribing device %d for the following gestures:", device_id);
    for (g = gesture_list; *g; ++g)
    {
      GeisV1AttrMap v1attr;
      geis_debug("\t\"%s\"", *g);

      if (0 == strcmp(GEIS_GESTURE_TYPE_SYSTEM, *g))
      {
	geis_subscription_set_flags(instance->subscription,
                          GEIS_SUBSCRIPTION_CONT | GEIS_SUBSCRIPTION_GRAB);
        continue;
      }

      for (v1attr = s_v1_attr_map; v1attr->v1_type; ++v1attr)
      {
	if (0 == strcmp(*g, v1attr->v1_type))
	{
	  GeisFilter filter = geis_filter_clone(instance->window_filter, *g);
	  if (!filter)
	  {
	    geis_error("error creating new filter");
	  }
	  else
	  {
	    result = geis_filter_add_term(filter, GEIS_FILTER_CLASS,
	      GEIS_CLASS_ATTRIBUTE_ID, GEIS_FILTER_OP_EQ, v1attr->v2_id,
	      GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, v1attr->touches,
	      NULL);
	    if (result != GEIS_STATUS_SUCCESS)
	    {
	      geis_error("error adding gesture class filter term");
	    }

            if (device_id != 0)
            {
              result = geis_filter_add_term(filter,
                                  GEIS_FILTER_DEVICE, GEIS_DEVICE_ATTRIBUTE_ID, GEIS_FILTER_OP_EQ,
                                  device_id, NULL);
              if (result != GEIS_STATUS_SUCCESS)
              {
                geis_error("error adding device filter term");
              }
            }

	    result = geis_subscription_add_filter(instance->subscription, filter); 
	    if (result != GEIS_STATUS_SUCCESS)
	    {
	      geis_error("error adding gesture class filter");
	    }
	  }
	  break;
	}
      }
    }
  }
  return result;
}


GeisStatus
geis_subscribe(GeisInstance         instance,
               GeisInputDeviceId   *input_list,
               const char*         *gesture_list,
               GeisGestureFuncs    *funcs,
               void                *cookie)
{
  GeisStatus result = GEIS_UNKNOWN_ERROR;

  /**
   * If there is no window filter, the instance has already been subscribed to
   * devices and gestures so skip.
   */
  if (NULL == instance->window_filter)
  {
    geis_warning("instance has been subscribed twice");
    return GEIS_STATUS_SUCCESS;
  }

  memcpy(&instance->gesture_funcs, funcs, sizeof(GeisGestureFuncs));
  instance->gesture_cookie = cookie;

  /* pump the geis event queue to force device and class events to be picked up */
  while (GEIS_STATUS_CONTINUE == geis_dispatch_events(instance->geis))
    ;

  if (input_list == GEIS_ALL_INPUT_DEVICES)
  {
    result = _v1_subscribe_device(instance, 0, gesture_list);
  }
  else
  {
    GeisInputDeviceId *device_id;
    for (device_id = input_list; *device_id; ++device_id)
    {
      GeisStatus a_result = _v1_subscribe_device(instance, *device_id, gesture_list);
      if (a_result == GEIS_STATUS_SUCCESS)
      {
        result = a_result;
      }
    }
  }

  /*
   * If there are specific gesture classes, remove the window-id-only filter since it allows
   * ALL gesture classes.  It will have been replaced by device-specific or
   * class-specific filters on the same window.
   */
  if (gesture_list && *gesture_list)
  {
    result = geis_subscription_remove_filter(instance->subscription, 
                                             instance->window_filter);
    if (result != GEIS_STATUS_SUCCESS)
    {
      geis_warning("error removing V1 window filter");
    }
    instance->window_filter = NULL;
  }

  result = geis_subscription_activate(instance->subscription);
  return result;
}


/*
 * Unsubscribes from one or more gestures.
 *
 * Note the evil wicked dirty case from GeisGestureType* to const char**:
 * it turns out the API should have used const char** and apps using the API
 * seem to cast GeisGestureType* to const char**, but they would fail to build
 * from source if the API changed now and would crash and burn if the
 * functionality was changed, hence the brutish method here.
 *
 * My advice:  move to GEIS v2.0 and avoid the whole problem.
 */
GeisStatus
geis_unsubscribe(GeisInstance     instance,
                 GeisGestureType *gesture_list)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;
  if (gesture_list == GEIS_ALL_GESTURES)
  {
    status = geis_subscription_deactivate(instance->subscription);
  }
  else
  {
    const char **gesture_name_list = (const char **)gesture_list;
    const char **g;

    status = geis_subscription_deactivate(instance->subscription);
    for (g = gesture_name_list; *g; ++g)
    {
      GeisFilter filter;

      filter = geis_subscription_filter_by_name(instance->subscription, *g);
      if (filter)
      {
	geis_subscription_remove_filter(instance->subscription, filter);
      }
    }

    if (geis_subscription_filter_count(instance->subscription))
    {
      status = geis_subscription_activate(instance->subscription);
    }
  }

  return status;
}


/*
 * Translates a GEISv2 device into a GEISv1 device and invokes the appropriate
 * callback.
 */
static void
_v1_report_device(GeisInstance instance,
                  GeisDevice   device,
                  GeisBoolean  is_added)
{
  GeisInputDeviceId device_id = 0;
  GeisGestureAttr *attrs = calloc(geis_device_attr_count(device) + 1,
                                  sizeof(GeisGestureAttr));
  if (!attrs)
  {
    geis_error("can not allocate device attrs");
    return;
  }

  for (GeisSize i = 0; i < geis_device_attr_count(device); ++i)
  {
    GeisAttr attr = geis_device_attr(device, i);
    if (0 == strcmp(geis_attr_name(attr), GEIS_DEVICE_ATTRIBUTE_ID))
    {
      device_id = geis_attr_value_to_integer(attr);
    }
    attrs[i].name        = geis_attr_name(attr);
    attrs[i].type        = geis_attr_type(attr);
    switch (attrs[i].type)
    {
      case GEIS_ATTR_TYPE_BOOLEAN:
	attrs[i].boolean_val = geis_attr_value_to_boolean(attr);
	break;
      case GEIS_ATTR_TYPE_FLOAT:
	attrs[i].float_val = geis_attr_value_to_float(attr);
	break;
      case GEIS_ATTR_TYPE_INTEGER:
	attrs[i].integer_val = geis_attr_value_to_integer(attr);
	break;
      case GEIS_ATTR_TYPE_STRING:
	attrs[i].string_val = geis_attr_value_to_string(attr);
	break;
      default:
	break;
    }
  }

  if (is_added)
  {
    instance->input_funcs.added(instance->input_context, device_id, attrs);
  }
  else
  {
    instance->input_funcs.removed(instance->input_context, device_id, attrs);
  }

  free(attrs);
}


/*
 * Handler for device events.
 *
 * Converts GEISv2 device-add and device-removed events into similar GEISv1
 * callbacks.
 */
static void
_v1_input_callback(Geis       geis GEIS_UNUSED,
                   GeisEvent  event,
                   void      *context)
{
  GeisInstance instance = (GeisInstance)context;

  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  if (!attr)
  {
    geis_error("no touchset for gesture event");
    goto final_exit;
  }

  GeisDevice device = geis_attr_value_to_pointer(attr);
  if (!device)
  {
    geis_warning("can not convert attr to device");
    goto final_exit;
  }

  switch (geis_event_type(event))
  {
    case GEIS_EVENT_DEVICE_AVAILABLE:
      _v1_report_device(instance, device, GEIS_TRUE);
      break;
    case GEIS_EVENT_DEVICE_UNAVAILABLE:
      _v1_report_device(instance, device, GEIS_FALSE);
      break;
    default:
      geis_debug("-- event ignored --");
      break;
  }

final_exit:
  geis_event_delete(event);
}


GeisStatus
geis_input_devices(GeisInstance    instance,
                   GeisInputFuncs *funcs,
                   void           *context)
{
  GeisStatus result = GEIS_STATUS_SUCCESS;

  memcpy(&instance->input_funcs, funcs, sizeof(GeisInputFuncs));
  instance->input_context = context;

  geis_register_device_callback(instance->geis, _v1_input_callback, instance);

  /* report pre-existing devices */
  GeisDeviceBag device_bag = geis_devices(instance->geis);
  for (GeisSize d = 0; d != geis_device_bag_count(device_bag); ++d)
  {
    GeisDevice device = geis_device_bag_device(device_bag, d);
    _v1_report_device(instance, device, GEIS_TRUE);
  }

  return result;
}

