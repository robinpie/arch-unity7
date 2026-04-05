/**
 * @file libgeis/geis_class.c
 * @brief implementation of the GEIS v2.0 API Gesture Class module
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
#include "geis_class.h"

#include "geis_atomic.h"
#include "geis_attr.h"
#include "geis_logging.h"
#include <stdlib.h>


struct _GeisGestureClass
{
  GeisRefCount ref_count;
  GeisAttrBag  attrs;
};

struct _GeisGestureClassBag
{
  GeisGestureClass *store;
  GeisSize          store_size;
  GeisSize          count;
};

static const int gesture_class_bag_growth_constant = 2;


/*
 * Creates a new class bag,
 */
GeisGestureClassBag
geis_gesture_class_bag_new()
{
  GeisGestureClassBag bag = calloc(1, sizeof(struct _GeisGestureClassBag));
  if (!bag)
  {
    geis_error("error allocating gesture class bag");
    goto final_exit;
  }

  bag->store = calloc(1, sizeof(struct _GeisGestureClass));
  if (!bag->store)
  {
    geis_error("error allocating gesture class bag store");
    goto unwind_bag;
  }

  bag->store_size = 1;
  bag->count = 0;
  goto final_exit;

unwind_bag:
  free(bag);
  bag = NULL;
final_exit:
  return bag;
}


/*
 * Destroys a gesture class bag.
 */
void
geis_gesture_class_bag_delete(GeisGestureClassBag bag)
{
  GeisSize i;
  for (i = bag->count; i > 0; --i)
  {
    geis_gesture_class_unref(bag->store[i-1]);
  }
  free(bag->store);
  free(bag);
}


/*
 * Gets the number of gesture classs in the bag.
 */
GeisSize
geis_gesture_class_bag_count(GeisGestureClassBag bag)
{
  return bag->count;
}


/*
 * Gets an indicated gesture class from a bag.
 */
GeisGestureClass
geis_gesture_class_bag_gesture_class(GeisGestureClassBag bag,
                                     GeisSize            index)
{
  GeisGestureClass gesture_class = NULL;
  if (index >= bag->count)
  {
    geis_warning("class bag index out of range");
  }
  else
  {
    gesture_class = bag->store[index];
  }
  return gesture_class;
}


/*
 * Inserts a gesture class in the bag.
 */
GeisStatus
geis_gesture_class_bag_insert(GeisGestureClassBag bag,
                              GeisGestureClass    gesture_class)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->count >= bag->store_size)
  {
    GeisSize new_store_size = bag->store_size * gesture_class_bag_growth_constant;
    GeisGestureClass *new_store = realloc(bag->store,
             new_store_size * sizeof(struct _GeisGestureClass));
    if (!new_store)
    {
      geis_error("failed to reallocate class bag");
      goto error_exit;
    }
    bag->store = new_store;
    bag->store_size = new_store_size;
  }
  bag->store[bag->count++] = gesture_class;
  status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}


/*
 * Remoes a gesture class from the bag.
 */
GeisStatus
geis_gesture_class_bag_remove(GeisGestureClassBag bag,
                              GeisGestureClass    gesture_class)
{
  GeisSize i;
  GeisStatus status = GEIS_STATUS_SUCCESS;
  for (i = 0; i < bag->count; ++i)
  {
    if (bag->store[i] == gesture_class)
    {
      GeisSize j;
      geis_gesture_class_unref(bag->store[i]);
      --bag->count;
      for (j = i; j < bag->count; ++j)
      {
	bag->store[j] = bag->store[j+1];
      }
      break;
    }
  }
  return status;
}


/*
 * Creates a new, empty gesture_class.
 */
GeisGestureClass
geis_gesture_class_new(GeisString name, GeisInteger id)
{
  GeisAttr attr;

  GeisGestureClass gesture_class = calloc(1, sizeof(struct _GeisGestureClass));
  if (!gesture_class)
  {
    geis_error("error allocating gesture class");
    goto final_exit;
  }

  gesture_class->attrs = geis_attr_bag_new(3);
  if (!gesture_class->attrs)
  {
    geis_debug("error allocating attr bag");
    goto unwind_gesture_class;
  }

  attr = geis_attr_new(GEIS_CLASS_ATTRIBUTE_NAME, GEIS_ATTR_TYPE_STRING, (void *)name);
  if (!attr)
  {
    geis_debug("error creating gesture class name attr");
    goto unwind_attrs;
  }
  geis_attr_bag_insert(gesture_class->attrs, attr);

  attr = geis_attr_new(GEIS_CLASS_ATTRIBUTE_ID, GEIS_ATTR_TYPE_INTEGER, &id);
  if (!attr)
  {
    geis_debug("error creating gesture class id attr");
    goto unwind_attrs;
  }
  geis_attr_bag_insert(gesture_class->attrs, attr);

  geis_gesture_class_ref(gesture_class);
  goto final_exit;

unwind_attrs:
  geis_attr_bag_delete(gesture_class->attrs);
unwind_gesture_class:
  free(gesture_class);
  gesture_class = NULL;
final_exit:
  return gesture_class;
}


/*
 * Destroys a gesture class.
 */
static void
_gesture_class_delete(GeisGestureClass gesture_class)
{
  geis_attr_bag_delete(gesture_class->attrs);
  free(gesture_class);
}


/*
 * Increments the reference count of a gesture class object.
 */
void
geis_gesture_class_ref(GeisGestureClass gesture_class)
{
  geis_atomic_ref(&gesture_class->ref_count);
}


/*
 * Decrements the reference count of a gesture class object.
 */
void
geis_gesture_class_unref(GeisGestureClass gesture_class)
{
  if (0 == geis_atomic_unref(&gesture_class->ref_count))
  {
    _gesture_class_delete(gesture_class);
  }
}


/*
 * Gets the name of the gesture class.
 */
GeisString
geis_gesture_class_name(GeisGestureClass gesture_class)
{
  GeisString name = NULL;
  GeisAttr attr = geis_attr_bag_find(gesture_class->attrs,
                                     GEIS_CLASS_ATTRIBUTE_NAME);
  if (attr)
  {
    name = geis_attr_value_to_string(attr);
  }
  return name;
}


/**
 * Gets the numeric identifier of the gesture class.
 */
GeisInteger
geis_gesture_class_id(GeisGestureClass gesture_class)
{
  GeisInteger id = -1;
  GeisAttr attr = geis_attr_bag_find(gesture_class->attrs,
                                     GEIS_CLASS_ATTRIBUTE_ID);
  if (attr)
  {
    id = geis_attr_value_to_integer(attr);
  }
  return id;
}


/*
 * Gets the number of attributes of the gesture class.
 */
GeisSize
geis_gesture_class_attr_count(GeisGestureClass gesture_class)
{
  return geis_attr_bag_count(gesture_class->attrs);
}


/*
 * Gets the indicated attribute of the gesture class.
 */
GeisAttr
geis_gesture_class_attr(GeisGestureClass gesture_class,
                        int              index)
{
  return geis_attr_bag_attr(gesture_class->attrs, index);
}


/*
 * Inserts an attr into a gesture class.
 */
GeisStatus
geis_gesture_class_add_attr(GeisGestureClass gesture_class, GeisAttr attr)
{
  return geis_attr_bag_insert(gesture_class->attrs, attr);
}

