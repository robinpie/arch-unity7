/**
 * @file geis_touch.c
 * @brief Geis touch module implementation
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"
#include "geis_touch.h"

#include "geis_attr.h"
#include "geis_logging.h"
#include <stdlib.h>


struct _GeisTouch
{
  GeisTouch   next;
  GeisTouchId id;
  GeisAttrBag attr_bag;
};

struct _GeisTouchSet
{
  GeisSize  count;
  GeisTouch first;
};


/*
 * Creates a new, empty touch set.
 */
GeisTouchSet
geis_touchset_new()
{
  GeisTouchSet touchset = calloc(1, sizeof(struct _GeisTouchSet));
  if (!touchset)
  {
    geis_error("error allocating touch set");
    goto final_exit;
  }

final_exit:
  return touchset;
}


/*
 * Destroys a touch set and all touches contained in it.
 */
void
geis_touchset_delete(GeisTouchSet touchset)
{
  GeisTouch p = touchset->first;
  while (p)
  {
    GeisTouch tmp = p->next;
    geis_touch_delete(p);
    p = tmp;
  }
  free(touchset);
}


/*
 * Inserts a touch into a touch set.
 */
GeisStatus
geis_touchset_insert(GeisTouchSet touchset, GeisTouch touch)
{
  if (touchset->count == 0)
  {
    touchset->first = touch;
  }
  else
  {
    GeisTouch p = touchset->first;
    while (p->next)
      p = p->next;
    p->next = touch;
  }
  ++touchset->count;
  return GEIS_STATUS_SUCCESS;
}


GeisSize
geis_touchset_touch_count(GeisTouchSet touchset)
{
  return touchset->count;
}


GeisTouch
geis_touchset_touch(GeisTouchSet touchset, GeisSize index)
{
  GeisTouch touch = NULL;
  if (index >= touchset->count)
  {
    geis_warning("touch set index out of range");
  }
  else
  {
    GeisSize i;
    touch = touchset->first;
    for (i = 0; i < index; ++i)
    {
      touch = touch->next;
    }
  }
  return touch;
}


GeisTouch
geis_touchset_touch_by_id(GeisTouchSet touchset, GeisTouchId touchid)
{
  GeisTouch result = NULL;
  GeisTouch touch = touchset->first;
  while (touch)
  {
    if (touch->id == touchid)
    {
      result = touch;
      break;
    }
    touch = touch->next;
  }
  return result;
}


/*
 * Creates a new gesture touch.
 */
GeisTouch
geis_touch_new(GeisTouchId id)
{
  GeisTouch touch = calloc(1, sizeof(struct _GeisTouch));
  if (!touch)
  {
    geis_error("error allocating touch");
    goto final_exit;
  }

  touch->attr_bag = geis_attr_bag_new(2);
  if (!touch->attr_bag)
  {
    geis_error("error allocating touch attr bag");
    goto unwind_touch;
  }
  
  touch->id = id;
  goto final_exit;

unwind_touch:
  free(touch);
  touch = NULL;
final_exit:
  return touch;
}


/*
 * Destroys a gesture touch and all gesture frames contained in it.
 */
void
geis_touch_delete(GeisTouch touch)
{
  geis_attr_bag_delete(touch->attr_bag);
  free(touch);
}


/*
 * Gets the identifier of a gesture touch.
 */
GeisTouchId
geis_touch_id(GeisTouch touch)
{
  return touch->id;
}


/*
 * Inserts an attr.
 */
GeisStatus
geis_touch_add_attr(GeisTouch touch, GeisAttr attr)
{
  return geis_attr_bag_insert(touch->attr_bag, attr);
}


/*
 * Gets the number of attrs associated with a touch.
 */
GeisSize
geis_touch_attr_count(GeisTouch touch)
{
  return geis_attr_bag_count(touch->attr_bag);
}


/*
 * Gets an indicated attr from a touch.
 */
GeisAttr
geis_touch_attr(GeisTouch touch, GeisSize index)
{
  return geis_attr_bag_attr(touch->attr_bag, index);
}


/*
 * Gets a named attr from a touch.
 */
GeisAttr
geis_touch_attr_by_name(GeisTouch touch, GeisString name)
{
  return geis_attr_bag_find(touch->attr_bag, name);
}

