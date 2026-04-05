/**
 * @file libgeis/geis_filter_term.c
 * @brief implementation of the GEIS v2.0 API filter term module
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
#include "geis_config.h"
#include "geis_filter_term.h"

#include "geis_atomic.h"
#include "geis_attr.h"
#include "geis_error.h"
#include "geis_frame.h"
#include "geis_logging.h"
#include <stdlib.h>
#include <string.h>


/*
 * One of the terms of a filter. 
 */
struct _GeisFilterTerm
{
  GeisRefCount        refcount;
  GeisFilterFacility  facility;
  GeisFilterOperation op;
  GeisAttr            attr;
};


/*
 * All of the terms of a filter.
 */
struct _GeisFilterTermBag
{
  GeisFilterTerm *store;
  GeisSize        store_size;
  GeisSize        count;
};

static const GeisSize term_bag_growth_constant = 2;


GeisFilterTermBag
geis_filter_term_bag_new(GeisSize store_size)
{
  GeisFilterTermBag bag = calloc(1, sizeof(struct _GeisFilterTermBag));
  if (!bag)
  {
    geis_error("failed to allocate filter termbag");
    goto final_exit;
  }

  bag->store_size = store_size ? store_size : 3;
  bag->count = 0;
  bag->store = calloc(bag->store_size, sizeof(GeisFilterTerm));
  if (!bag->store)
  {
    geis_error("failed to allocate filter bag store");
    goto unwind_bag;
  }
  goto final_exit;

unwind_bag:
  free(bag);
  bag = NULL;
final_exit:
  return bag;
}


/*
 * Creates a new filter term bag by deep-copying an existing filter term bag.
 */
GeisFilterTermBag
geis_filter_term_bag_clone(GeisFilterTermBag original)
{
  GeisSize i;
  GeisFilterTermBag bag = geis_filter_term_bag_new(original->store_size);
  if (!bag)
  {
    goto final_exit;
  }

  bag->count = original->count;
  for (i = 0; i < bag->count; ++i)
  {
    bag->store[i] = geis_filter_term_ref(original->store[i]);
  }

final_exit:
  return bag;
}


void
geis_filter_term_bag_delete(GeisFilterTermBag bag)
{
  GeisSize i;
  for (i = 0; i < bag->count; ++i)
  {
    geis_filter_term_unref(bag->store[i]);
  }
  free(bag->store);
  free(bag);
}


GeisSize
geis_filter_term_bag_count(GeisFilterTermBag bag)
{
  return bag->count;
}


GeisFilterTerm
geis_filter_term_bag_term(GeisFilterTermBag bag, GeisSize index)
{
  GeisFilterTerm term = NULL;
  if (index < bag->count)
  {
    term = bag->store[index];
  }
  return term;
}


GeisStatus
geis_filter_term_bag_insert(GeisFilterTermBag bag,
                            GeisFilterTerm    term)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->count >= bag->store_size)
  {
    GeisSize new_store_size = bag->store_size * term_bag_growth_constant;
    GeisFilterTerm *new_store = realloc(bag->store,
             new_store_size * sizeof(struct _GeisFilterTerm));
    if (!new_store)
    {
      geis_error("failed to reallocate filter term bag");
      goto error_exit;
    }
    bag->store = new_store;
    bag->store_size = new_store_size;
  }
  bag->store[bag->count++] = term;
  status = GEIS_STATUS_SUCCESS;
  goto final_exit;

error_exit:
final_exit:
  return status;
}


GeisFilterTerm
geis_filter_term_new(GeisFilterFacility  facility,
                     GeisFilterOperation operation,
                     GeisAttr            attr)
{
  GeisFilterTerm term = calloc(1, sizeof(struct _GeisFilterTerm));
  if (!term)
  {
    geis_error("failed to allocate filter termbag");
    goto final_exit;
  }
  term->facility = facility;
  term->op       = operation;
  term->attr     = attr;
  geis_filter_term_ref(term);

final_exit:
  return term;
}


static void
_filter_term_destroy(GeisFilterTerm term)
{
  geis_attr_delete(term->attr);
  free(term);
}


/*
 * Increments the filter term's reference count.
 */
GeisFilterTerm
geis_filter_term_ref(GeisFilterTerm term)
{
  geis_atomic_ref(&term->refcount);
  return term;
}


/*
 * Decrements the filter term's reference count and maybe destroys the term.
 */
void
geis_filter_term_unref(GeisFilterTerm term)
{
  if (0 == geis_atomic_unref(&term->refcount))
  {
    _filter_term_destroy(term);
  }
}


GeisFilterFacility
geis_filter_term_facility(GeisFilterTerm term)
{
  return term->facility;
}


GeisFilterOperation
geis_filter_term_operation(GeisFilterTerm term)
{
  return term->op;
}


GeisAttr
geis_filter_term_attr(GeisFilterTerm term)
{
  return term->attr;
}


static GeisBoolean
_filter_term_device_match_event(GeisFilterTerm term, GeisFrame frame)
{
  GeisBoolean matches = GEIS_FALSE;
  GeisString attr_name = NULL;
  if (0 == strcmp(geis_attr_name(term->attr), GEIS_DEVICE_ATTRIBUTE_ID))
  {
    attr_name = GEIS_GESTURE_ATTRIBUTE_DEVICE_ID;
  }
  else if (0 == strcmp(geis_attr_name(term->attr), GEIS_DEVICE_ATTRIBUTE_TOUCHES))
  {
    attr_name = GEIS_GESTURE_ATTRIBUTE_TOUCHES;
  }

  if (attr_name)
  {
    GeisAttr attr;
    attr = geis_frame_attr_by_name(frame, attr_name);
    if (attr)
    {
      return geis_attr_compare(attr, term->attr, term->op);
    }
  }
  return matches;
}


static GeisBoolean
_filter_term_class_match_event(GeisFilterTerm term, GeisFrame frame)
{
  GeisBoolean matches = GEIS_FALSE;
  if (0 == strcmp(geis_attr_name(term->attr), GEIS_CLASS_ATTRIBUTE_NAME))
  {
    matches = geis_frame_is_class_by_name(frame,
                                          geis_attr_value_to_string(term->attr));
  }
  else
  {
    GeisAttr attr = geis_frame_attr_by_name(frame, geis_attr_name(term->attr));
    if (attr)
    {
      matches = geis_attr_compare(attr, term->attr, term->op);
    }
  }
  return matches;
}


static GeisBoolean
_filter_term_region_match_event(GeisFilterTerm term, GeisFrame frame)
{
  GeisBoolean matches = GEIS_FALSE;
  if (0 == strcmp(geis_attr_name(term->attr), GEIS_REGION_ATTRIBUTE_WINDOWID))
  {
    GeisAttr attr;
    attr = geis_frame_attr_by_name(frame, GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID);
    if (attr)
    {
      matches = geis_attr_compare(attr, term->attr, term->op);
    }
  }
  return matches;
}


GeisBoolean
geis_filter_term_match_event(GeisFilterTerm term, GeisEvent event)
{
  GeisBoolean matches = GEIS_FALSE;
  GeisEventType event_type = geis_event_type(event);
  if (event_type == GEIS_EVENT_GESTURE_BEGIN
   || event_type == GEIS_EVENT_GESTURE_UPDATE
   || event_type == GEIS_EVENT_GESTURE_END)
  {
    GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
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

    for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
    {
      GeisGroup group = geis_groupset_group(groupset, i);
      if (!group)
      {
        geis_warning("can not extract group %zu from groupset", i);
        goto final_exit;
      }

      for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
      {
        GeisFrame frame = geis_group_frame(group, j);
        if (!frame)
        {
          geis_warning("can not extract frame %zu from group", j);
          goto final_exit;
        }

        switch (term->facility)
        {
          case GEIS_FILTER_DEVICE:
            matches = _filter_term_device_match_event(term, frame);
            break;
          case GEIS_FILTER_CLASS:
            matches = _filter_term_class_match_event(term, frame);
            break;
          case GEIS_FILTER_REGION:
            matches = _filter_term_region_match_event(term, frame);
            break;
          default:
            break;
        }
      }
    }
  }

final_exit:
  return matches;
}


/**
 * Indicates if an attr string value matches a given string and condition.
 *
 * @param[in] attr   The attr with a string value.
 * @param[in] op     A match condition.
 * @param[in] svalue A target string value to match.
 *
 * @returns GEIS_TRUE if the attr string value matches the target string and
 * condition, GEIS_FALSE otherwise.
 */
static GeisBoolean
_filter_matches_attr_string(GeisAttr            attr,
                            GeisFilterOperation op,
                            GeisString          svalue)
{
  GeisBoolean strings_match = GEIS_FALSE;
  GeisString attr_value = geis_attr_value_to_string(attr);
  if (attr_value)
  {
    strings_match = (0 == strcmp(attr_value, svalue));
  }
  return (op == GEIS_FILTER_OP_EQ && strings_match)
      || (op == GEIS_FILTER_OP_NE && !strings_match);
}


/**
 * Indicates if an attr boolean value matches a given value and condition.
 *
 * @param[in] attr   The attr with a string value.
 * @param[in] op     A match condition.
 * @param[in] bvalue A target boolean value to match.
 *
 * @returns GEIS_TRUE if the attr boolean value matches the target value and
 * condition, GEIS_FALSE otherwise.
 */
static GeisBoolean
_filter_matches_attr_boolean(GeisAttr            attr,
                             GeisFilterOperation op,
                             GeisBoolean         bvalue)
{
  GeisBoolean attr_value = geis_attr_value_to_boolean(attr);
  return (op == GEIS_FILTER_OP_EQ && attr_value == bvalue)
      || (op == GEIS_FILTER_OP_NE && attr_value != bvalue);
}


/**
 * Indicates if an attr integer value matches a given value and condition.
 *
 * @param[in] attr   The attr with a string value.
 * @param[in] op     A match condition.
 * @param[in] ivalue A target integer value to match.
 *
 * @returns GEIS_TRUE if the attr integer value matches the target value and
 * condition, GEIS_FALSE otherwise.
 */
static GeisBoolean
_filter_matches_attr_integer(GeisAttr            attr,
                             GeisFilterOperation op,
                             GeisInteger         ivalue)
{
  GeisInteger attr_value = geis_attr_value_to_integer(attr);
  return (op == GEIS_FILTER_OP_EQ && attr_value == ivalue)
      || (op == GEIS_FILTER_OP_NE && attr_value != ivalue)
      || (op == GEIS_FILTER_OP_GT && attr_value > ivalue)
      || (op == GEIS_FILTER_OP_GE && attr_value >= ivalue)
      || (op == GEIS_FILTER_OP_LE && attr_value <= ivalue)
      || (op == GEIS_FILTER_OP_LT && attr_value < ivalue);
}


/*
 * Indicates a filter passes a device.
 */
GeisBoolean
geis_filter_term_match_device(GeisFilterTerm term, GeisDevice device)
{
  if (geis_filter_term_facility(term) != GEIS_FILTER_DEVICE)
  {
    return GEIS_TRUE;
  }

  GeisAttr            filter_attr = geis_filter_term_attr(term);
  GeisString          attr_name   = geis_attr_name(filter_attr);
  GeisFilterOperation op          = geis_filter_term_operation(term);

  if (0 == strcmp(attr_name, GEIS_DEVICE_ATTRIBUTE_NAME))
  {
    GeisString device_name = geis_device_name(device);
    if (device_name)
    {
      return _filter_matches_attr_string(filter_attr, op, device_name);
    }
  }
  else if (0 == strcmp(attr_name, GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH)
        || 0 == strcmp(attr_name, GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH))
  {
    GeisAttr device_attr = geis_device_attr_by_name(device, attr_name);
    if (device_attr)
    {
      GeisBoolean device_value = geis_attr_value_to_boolean(device_attr);
      return _filter_matches_attr_boolean(filter_attr, op, device_value);
    }
  }
  else if (0 == strcmp(attr_name, GEIS_DEVICE_ATTRIBUTE_ID)
        || 0 == strcmp(attr_name, GEIS_DEVICE_ATTRIBUTE_TOUCHES))
  {
    GeisAttr device_attr = geis_device_attr_by_name(device, attr_name);
    if (device_attr)
    {
      GeisInteger device_value = geis_attr_value_to_integer(device_attr);
      /* legacy special case for device ID == 0:  means ALL devices in v1 */
      if (device_value == 0)
      {
        return GEIS_TRUE;
      }
      return _filter_matches_attr_integer(filter_attr, op, device_value);
    }
  }
  return GEIS_FALSE;
}

