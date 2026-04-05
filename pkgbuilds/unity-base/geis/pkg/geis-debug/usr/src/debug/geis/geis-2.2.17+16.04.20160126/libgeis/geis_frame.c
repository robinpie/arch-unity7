/**
 * @file geis_frame.c
 * @brief uFrame Geis gesture frame module implementation
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
#include "geis_frame.h"

#include "geis_attr.h"
#include "geis_logging.h"
#include <stdlib.h>
#include <string.h>


/* Use a hard-coded value for this for simplicity. */
#define MAX_FRAME_CLASSES 4

/* Use a hard-coded value for this for simplicity. */
#define MAX_FRAME_TOUCHES 16

struct _GeisFrame
{
  GeisFrame         next;
  GeisInteger       id;
  GeisAttrBag       attr_bag;
  GeisFloat         matrix[16];
  GeisSize          class_count;
  GeisSize          class_size;
  GeisGestureClass  classes[MAX_FRAME_CLASSES];
  GeisSize          touch_count;
  GeisSize          touch_size;
  GeisTouchId      *touches;
};

struct _GeisFrameSet
{
  GeisSize  count;
  GeisFrame first;
};


/*
 * Creates a new, empty frame set.
 */
GeisFrameSet
geis_frameset_new()
{
  GeisFrameSet frameset = calloc(1, sizeof(struct _GeisFrameSet));
  if (!frameset)
  {
    geis_error("error allocating frame set");
    goto final_exit;
  }

final_exit:
  return frameset;
}


/*
 * Destroys a frame set and all framees contained in it.
 */
void
geis_frameset_delete(GeisFrameSet frameset)
{
  GeisFrame p = frameset->first;
  while (p)
  {
    GeisFrame tmp = p->next;
    geis_frame_delete(p);
    p = tmp;
  }
  free(frameset);
}


/*
 * Inserts a frame into a frame set.
 */
GeisStatus
geis_frameset_insert(GeisFrameSet frameset, GeisFrame frame)
{
  if (frameset->count == 0)
  {
    frameset->first = frame;
  }
  else
  {
    GeisFrame p = frameset->first;
    while (p->next)
      p = p->next;
    p->next = frame;
  }
  ++frameset->count;
  return GEIS_STATUS_SUCCESS;
}


GeisSize
geis_frameset_frame_count(GeisFrameSet frameset)
{
  return frameset->count;
}


GeisFrame
geis_frameset_frame(GeisFrameSet frameset, GeisSize index)
{
  GeisFrame frame = NULL;
  if (index >= frameset->count)
  {
    geis_warning("frame set index out of range");
  }
  else
  {
    GeisSize i;
    frame = frameset->first;
    for (i = 0; i < index; ++i)
    {
      frame = frame->next;
    }
  }
  return frame;
}


/*
 * Creates a new gesture frame.
 */
GeisFrame
geis_frame_new(GeisInteger id)
{
  GeisFrame frame = calloc(1, sizeof(struct _GeisFrame));
  if (!frame)
  {
    geis_error("error allocating frame");
    goto final_exit;
  }

  frame->attr_bag = geis_attr_bag_new(5);
  if (!frame->attr_bag)
  {
    geis_error("error allocating frame attr bag");
    goto unwind_frame;
  }
  
  frame->class_size = MAX_FRAME_CLASSES;

  frame->touch_size = MAX_FRAME_TOUCHES; /** @todo make this configurable */
  frame->touches = calloc(frame->touch_size, sizeof(GeisTouchId));
  if (!frame->touches)
  {
    geis_error("error allocating frame touoches");
    goto unwind_attr_bag;
  }

  frame->id = id;
  goto final_exit;

unwind_attr_bag:
  geis_attr_bag_delete(frame->attr_bag);
unwind_frame:
  free(frame);
  frame = NULL;
final_exit:
  return frame;
}


/*
 * Destroys a gesture frame and all gesture frames contained in it.
 */
void
geis_frame_delete(GeisFrame frame)
{
  free(frame->touches);
  geis_attr_bag_delete(frame->attr_bag);
  free(frame);
}


/*
 * Gets the identifier of a gesture frame.
 */
GeisGestureId
geis_frame_id(GeisFrame frame)
{
  return frame->id;
}


/*
 * Marks the frame as belonging to a given class of gestures.
 */
GeisStatus
geis_frame_set_is_class(GeisFrame frame, GeisGestureClass gesture_class)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisSize i;

  if (!gesture_class)
  {
    geis_error("NULL gesture class received.");
    return status;
  }

  for (i = 0; i < frame->class_count; ++i)
  {
    if (0 == strcmp(geis_gesture_class_name(gesture_class),
                    geis_gesture_class_name(frame->classes[i])))
    {
      return GEIS_TRUE;
    }
  }

  if (frame->class_count < frame->class_size)
  {
    frame->classes[frame->class_count++] = gesture_class;
    status = GEIS_TRUE;
  }
  return status;
}


/*
 * Indicates if a gesture frame belongs to a gesture class.
 */
GeisBoolean
geis_frame_is_class(GeisFrame frame, GeisGestureClass gesture_class)
{
  return geis_frame_is_class_by_name(frame,
                                     geis_gesture_class_name(gesture_class));
}


/*
 * Indicates if a gesture frame belongs to a gesture class.
 */
GeisBoolean
geis_frame_is_class_by_name(GeisFrame frame, GeisString class_name)
{
  GeisBoolean is_class = GEIS_FALSE;
  GeisSize i;

  for (i = 0; i < frame->class_count; ++i)
  {
    if (0 == strcmp(class_name, geis_gesture_class_name(frame->classes[i])))
    {
      is_class = GEIS_TRUE;
      break;
    }
  }
  return is_class;
}


GeisSize
geis_frame_class_count(GeisFrame frame)
{
  return frame->class_count;
}


GeisGestureClass
geis_frame_class(GeisFrame frame, GeisSize index)
{
  return frame->classes[index];
}


/*
 * Inserts an attr.
 */
GeisStatus
geis_frame_add_attr(GeisFrame frame, GeisAttr attr)
{
  return geis_attr_bag_insert(frame->attr_bag, attr);
}


/*
 * Gets the number of attrs associated with a frame.
 */
GeisSize
geis_frame_attr_count(GeisFrame frame)
{
  return geis_attr_bag_count(frame->attr_bag);
}


/*
 * Gets an indicated attr from a frame.
 */
GeisAttr
geis_frame_attr(GeisFrame frame, GeisSize index)
{
  return geis_attr_bag_attr(frame->attr_bag, index);
}


/*
 * Gets a named attr from a frame.
 */
GeisAttr
geis_frame_attr_by_name(GeisFrame frame, GeisString name)
{
  return geis_attr_bag_find(frame->attr_bag, name);
}


/**
 * Sets the frame transform matrix.
 */
void
geis_frame_set_matrix(GeisFrame frame, const GeisFloat *matrix)
{
  memcpy(frame->matrix, matrix, sizeof(frame->matrix));
}


/*
 * Gets the current transform matrix of a gesture.
 */
GeisFloat *
geis_frame_matrix(GeisFrame frame)
{
  return frame->matrix;
}


/*
 * Inserts a touch index into a gesture frame.
 */
GeisStatus
geis_frame_add_touchid(GeisFrame frame, GeisTouchId touchid)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (frame->touch_count < frame->touch_size)
  {
    frame->touches[frame->touch_count++] = touchid;
  }
  return status;
}


/*
 * Gets the number of touches making up a gesture for the frame.
 */
GeisSize
geis_frame_touchid_count(GeisFrame frame)
{
  return frame->touch_count;
}

/*
 * Gets the index of the indicated touch within the gesture frame.
 */
GeisTouchId
geis_frame_touchid(GeisFrame frame, GeisSize index)
{
  if (index < frame->touch_count)
  {
    return frame->touches[index];
  }
  return 0;
}


