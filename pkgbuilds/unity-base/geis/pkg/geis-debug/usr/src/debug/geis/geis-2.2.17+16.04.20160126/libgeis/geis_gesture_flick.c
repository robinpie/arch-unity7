/**
 * @file geis_gesture_flick.c
 * @brief higher-level "flick" gesture recognizer
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_gesture_flick.h"

#include "geis_class.h"
#include "geis_frame.h"
#include "geis_logging.h"
#include "geis_private.h"
#include <string.h>


struct GeisGestureFlick
{
  Geis             geis;
  GeisGestureClass flick_class;
  GeisBoolean      enabled;
};

static GeisFloat _flick_threshold_squared = 1.0f;


static GeisProcessingResult
_recognize_flick(GeisEvent event, void *context)
{
  GeisProcessingResult result = GEIS_PROCESSING_IGNORED;
  GeisGestureFlick flick = (GeisGestureFlick)context;

  if (!flick->enabled)
    goto final_exit;

  switch (geis_event_type(event))
  {
    case GEIS_EVENT_GESTURE_END:
      {
	GeisAttr attr;
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
	    GeisFloat   dx = 0.0f;
	    GeisFloat   dy = 0.0f;
	    GeisFloat   rv = 0.0f;
	    GeisFrame   frame = geis_group_frame(group, j);
	    if (!frame)
	    {
	      geis_warning("can not extract frame %zu from group", j);
	      goto final_exit;
	    }

	    GeisSize num_attrs = geis_frame_attr_count(frame);
	    for (i = 0; i < num_attrs; ++i)
	    {
	      GeisAttr   attr = geis_frame_attr(frame, i);
	      GeisString attr_name = geis_attr_name(attr);

	      if (0 == strcmp(attr_name, GEIS_CLASS_ATTRIBUTE_ID))
	      {
		GeisInteger class_id = geis_attr_value_to_integer(attr);
		if (class_id != GEIS_GESTURE_PRIMITIVE_DRAG)
		{
		  goto final_exit;
		}
	      }
	      else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_VELOCITY_X))
	      {
		dx = geis_attr_value_to_float(attr);
	      }
	      else if (0 == strcmp(attr_name, GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y))
	      {
		dy = geis_attr_value_to_float(attr);
	      }
	    }

	    rv = dx * dx + dy * dy;
	    if (rv > _flick_threshold_squared)
	    {
	      geis_frame_set_is_class(frame, flick->flick_class);
	    }
	  }
	}
      }
      break;

    default:
      break;
  }

final_exit:
  return result;
}


static GeisStatus
_add_class_term(GeisBackendToken     token,
                void                *context,
                GeisString           name,
                GeisFilterOperation  op,
                void                *value)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisGestureFlick flick = (GeisGestureFlick)context;

  if (0 == strcmp(name, GEIS_CLASS_ATTRIBUTE_NAME))
  {
    if (op == GEIS_FILTER_OP_EQ)
    {
      GeisString class_name = (GeisString)value;
      if (0 == strcmp(class_name, geis_gesture_class_name(flick->flick_class)))
      {
	geis_filterable_attribute_foreach(flick->geis,
                                          GEIS_FILTER_CLASS,
                                          token,
                                          GEIS_CLASS_ATTRIBUTE_NAME,
                                          GEIS_FILTER_OP_EQ,
                                          GEIS_GESTURE_DRAG);
	flick->enabled = GEIS_TRUE;
      }
      else
      {
	flick->enabled = GEIS_FALSE;
      }
    }
    status = GEIS_STATUS_SUCCESS;
  }
  else if (0 == strcmp(name, GEIS_CLASS_ATTRIBUTE_ID))
  {
    if (op == GEIS_FILTER_OP_EQ)
    {
      GeisInteger id = *(GeisInteger*)value;
      if (id == geis_gesture_class_id(flick->flick_class))
      {
	geis_filterable_attribute_foreach(flick->geis,
	                                  GEIS_FILTER_CLASS,
	                                  token,
	                                  GEIS_CLASS_ATTRIBUTE_NAME,
	                                  GEIS_FILTER_OP_EQ,
	                                  GEIS_GESTURE_DRAG);
	flick->enabled = GEIS_TRUE;
      }
      else
      {
	flick->enabled = GEIS_FALSE;
      }
    }
    status = GEIS_STATUS_SUCCESS;
  }
  else if (0 == strcmp(name, GEIS_GESTURE_ATTRIBUTE_TOUCHES))
  {
    /* GeisInteger touches = *(GeisInteger*)value; */
    /* must explicitly ask for high-level gestures */
    /* flick->enabled = GEIS_TRUE; */
    status = GEIS_STATUS_SUCCESS;
  }
  return status;
}


GeisGestureFlick
geis_gesture_flick_new(Geis geis)
{
  GeisSize attr_count = 0;
  struct GeisFilterableAttribute attrs[3];
  GeisGestureFlick flick = calloc(1, sizeof(struct GeisGestureFlick));
  if (!flick)
  {
    geis_error("can not create flick");
    goto final_exit;
  }

  flick->geis = geis;
  flick->flick_class = geis_gesture_class_new(GEIS_GESTURE_FLICK,
                                              GEIS_GESTURE_ID_FLICK);
  geis_gesture_class_ref(flick->flick_class);
  geis_filterable_attribute_init(&attrs[attr_count++],
                                 GEIS_CLASS_ATTRIBUTE_NAME,
                                 GEIS_ATTR_TYPE_STRING,
                                 _add_class_term,
                                 flick);
  geis_filterable_attribute_init(&attrs[attr_count++],
                                 GEIS_CLASS_ATTRIBUTE_ID,
                                 GEIS_ATTR_TYPE_INTEGER,
                                 _add_class_term,
                                 flick);
  geis_filterable_attribute_init(&attrs[attr_count++],
                                 GEIS_GESTURE_ATTRIBUTE_TOUCHES,
                                 GEIS_ATTR_TYPE_INTEGER,
                                 _add_class_term,
                                 flick);

  geis_register_gesture_class(geis, flick->flick_class, attr_count, attrs);
  geis_register_processing_callback(geis, 10, _recognize_flick, flick);

final_exit:
  return flick;
}


void
geis_gesture_flick_delete(GeisGestureFlick flick)
{
  if (flick)
  {
    geis_gesture_class_unref(flick->flick_class);
    free(flick);
  }
}

