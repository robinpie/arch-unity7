/**
 * @file geis_dbus_gesture_event.c
 * @brief Implementation of the GEIS DBus gesture event transport.
 */

/*
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
#include "geis_config.h"
#include "geis_dbus_gesture_event.h"

#include "geis_attr.h"
#include "geis_dbus.h"
#include "geis_dbus_attr.h"
#include "geis_event.h"
#include "geis_group.h"
#include "geis_logging.h"
#include "geis_private.h"
#include "geis_touch.h"

/**
 * A frame is marshalled as a dict entry of
 * {id: [array of attrs, array of classes, array of touch ids]}, which is
 * {i(a(sv)aiai))} in DBus terminaology.
 */
#define GEIS_DBUS_TYPE_SIGNATURE_FRAME \
                       DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING \
                       DBUS_TYPE_INT32_AS_STRING \
                       DBUS_STRUCT_BEGIN_CHAR_AS_STRING \
                       DBUS_TYPE_ARRAY_AS_STRING \
                       GEIS_DBUS_TYPE_SIGNATURE_ATTR \
                       DBUS_TYPE_ARRAY_AS_STRING \
                       DBUS_TYPE_INT32_AS_STRING \
                       DBUS_TYPE_ARRAY_AS_STRING \
                       DBUS_TYPE_INT32_AS_STRING \
                       DBUS_STRUCT_END_CHAR_AS_STRING \
                       DBUS_DICT_ENTRY_END_CHAR_AS_STRING \

#define GEIS_DBUS_TYPE_SIGNATURE_FRAMESET \
                       DBUS_TYPE_ARRAY_AS_STRING \
                       GEIS_DBUS_TYPE_SIGNATURE_FRAME 


static void
_marshall_touchset(GeisTouchSet touchset, DBusMessageIter *iter)
{
  DBusMessageIter touchset_iter;
  dbus_message_iter_open_container(iter,
                                   DBUS_TYPE_ARRAY,
                                   DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                   DBUS_TYPE_INT32_AS_STRING
                                   DBUS_TYPE_ARRAY_AS_STRING
                                   GEIS_DBUS_TYPE_SIGNATURE_ATTR
                                   DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                   &touchset_iter);
  for (GeisSize t = 0; t < geis_touchset_touch_count(touchset); ++t)
  {
    DBusMessageIter touch_iter;
    dbus_message_iter_open_container(&touchset_iter,
                                     DBUS_TYPE_DICT_ENTRY,
                                     NULL,
                                     &touch_iter);
    GeisTouch touch = geis_touchset_touch(touchset, t);
    dbus_int32_t touch_id = geis_touch_id(touch);

    dbus_message_iter_append_basic(&touch_iter, DBUS_TYPE_INT32, &touch_id);
    DBusMessageIter attr_iter;
    dbus_message_iter_open_container(&touch_iter,
                                     DBUS_TYPE_ARRAY,
                                     GEIS_DBUS_TYPE_SIGNATURE_ATTR,
                                     &attr_iter);
    for (GeisSize a = 0; a < geis_touch_attr_count(touch); ++a)
    {
      geis_dbus_attr_marshall(geis_touch_attr(touch, a), &attr_iter);
    }
    dbus_message_iter_close_container(&touch_iter, &attr_iter);
    dbus_message_iter_close_container(&touchset_iter, &touch_iter);
  }
  dbus_message_iter_close_container(iter, &touchset_iter);
}


static void
_unmarshall_touchset(DBusMessageIter *iter, GeisTouchSet touchset)
{
  int dtype = dbus_message_iter_get_arg_type(iter);
  if (dtype != DBUS_TYPE_ARRAY)
  {
    geis_error("malformed GeisEvent touchset");
  }

  DBusMessageIter touch_iter;
  dbus_message_iter_recurse(iter, &touch_iter);
  for (dtype = dbus_message_iter_get_arg_type(&touch_iter);
       dtype != DBUS_TYPE_INVALID;
       dbus_message_iter_next(&touch_iter),
       dtype = dbus_message_iter_get_arg_type(&touch_iter))
  {
    DBusMessageIter dict_iter;
    dbus_message_iter_recurse(&touch_iter, &dict_iter);
    int type = dbus_message_iter_get_arg_type(&dict_iter);
    if (type !=  DBUS_TYPE_INT32)
    {
      geis_error("malformed GeisEvent touchset");
      continue;
    }
    dbus_int32_t touch_id;
    dbus_message_iter_get_basic(&dict_iter, &touch_id);
    dbus_message_iter_next(&dict_iter);
    GeisTouch touch = geis_touch_new(touch_id);

    type = dbus_message_iter_get_arg_type(&dict_iter);
    if (type != DBUS_TYPE_ARRAY)
    {
      geis_error("malformed GeisEvent touchset");
      continue;
    }

    DBusMessageIter attr_iter;
    dbus_message_iter_recurse(&dict_iter, &attr_iter);
    for (int type = dbus_message_iter_get_arg_type(&attr_iter);
         type != DBUS_TYPE_INVALID;
         dbus_message_iter_next(&attr_iter),
         type = dbus_message_iter_get_arg_type(&attr_iter))
    {
      GeisAttr attr = geis_dbus_attr_unmarshall(&attr_iter);
      geis_touch_add_attr(touch, attr);
    }
    geis_touchset_insert(touchset, touch);
  }
}


/**
 * Marshalls a GEIS frame to a DBus message via a message iterator.
 * @param[in]  frame  The GEIS frame to marshall.
 * @param[in]  iter   The DBus message iterator.
 *
 * @todo The class set and matrix need to be added.
 */
static void
_marshall_frame(GeisFrame frame,  DBusMessageIter *frame_iter)
{
  DBusMessageIter dict_iter;
  dbus_message_iter_open_container(frame_iter,
                                   DBUS_TYPE_DICT_ENTRY,
                                   NULL,
                                   &dict_iter);

  dbus_int32_t frame_id = geis_frame_id(frame);
  dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_INT32, &frame_id);

  {
    DBusMessageIter struct_iter;
    dbus_message_iter_open_container(&dict_iter,
                                     DBUS_TYPE_STRUCT,
                                     NULL,
                                     &struct_iter);
    {
      DBusMessageIter attr_iter;
      dbus_message_iter_open_container(&struct_iter,
                                       DBUS_TYPE_ARRAY,
                                       GEIS_DBUS_TYPE_SIGNATURE_ATTR,
                                       &attr_iter);
      for (GeisSize a = 0; a < geis_frame_attr_count(frame); ++a)
      {
        geis_dbus_attr_marshall(geis_frame_attr(frame, a), &attr_iter);
      }
      dbus_message_iter_close_container(&struct_iter, &attr_iter);
    }
    {
      DBusMessageIter class_iter;
      dbus_message_iter_open_container(&struct_iter,
                                       DBUS_TYPE_ARRAY,
                                       DBUS_TYPE_INT32_AS_STRING,
                                       &class_iter);
      for (GeisSize t = 0; t < geis_frame_class_count(frame); ++t)
      {
        GeisGestureClass frame_class = geis_frame_class(frame, t);
        dbus_int32_t class_id = geis_gesture_class_id(frame_class);
        dbus_message_iter_append_basic(&class_iter, DBUS_TYPE_INT32, &class_id);
      }
      dbus_message_iter_close_container(&struct_iter, &class_iter);
    }
    {
      DBusMessageIter touch_iter;
      dbus_message_iter_open_container(&struct_iter,
                                       DBUS_TYPE_ARRAY,
                                       DBUS_TYPE_INT32_AS_STRING,
                                       &touch_iter);
      for (GeisSize t = 0; t < geis_frame_touchid_count(frame); ++t)
      {
        dbus_int32_t touch_id = geis_frame_touchid(frame, t);
        dbus_message_iter_append_basic(&touch_iter, DBUS_TYPE_INT32, &touch_id);
      }
      dbus_message_iter_close_container(&struct_iter, &touch_iter);
    }
    dbus_message_iter_close_container(&dict_iter, &struct_iter);
  }
  dbus_message_iter_close_container(frame_iter, &dict_iter);
}


/**
 * Unmarshalls a GEIS frame from a DBus message via a message iterator.
 * @param[in]  frame_iter The DBus message iterator.
 * @param[in]  group      The group the unmarshalled frame will belong to.
 */
static void
_unmarshall_frame(Geis geis, DBusMessageIter *frame_iter, GeisGroup group)
{
  int type = dbus_message_iter_get_arg_type(frame_iter);
  if (type != DBUS_TYPE_DICT_ENTRY)
  {
    geis_error("malformed GeisEvent frame: expected %c, received %c",
               DBUS_TYPE_DICT_ENTRY, type);
    goto final_exit;
  }
  DBusMessageIter dict_iter;
  dbus_message_iter_recurse(frame_iter, &dict_iter);

  type = dbus_message_iter_get_arg_type(&dict_iter);
  if (type != DBUS_TYPE_INT32)
  {
    geis_error("malformed GeisEvent frame: expected %c, received %c",
               DBUS_TYPE_INT32, type);
    goto final_exit;
  }
  dbus_int32_t frame_id;
  dbus_message_iter_get_basic(&dict_iter, &frame_id);
  GeisFrame frame = geis_frame_new(frame_id);
  geis_group_insert_frame(group, frame);

  dbus_message_iter_next(&dict_iter);
  type = dbus_message_iter_get_arg_type(&dict_iter);
  if (type != DBUS_TYPE_STRUCT)
  {
    geis_error("malformed GeisEvent frame: expected %c, received %c",
               DBUS_TYPE_STRUCT, type);
  }
  else
  {
    DBusMessageIter struct_iter;
    dbus_message_iter_recurse(&dict_iter, &struct_iter);

    type = dbus_message_iter_get_arg_type(&struct_iter);
    if (type != DBUS_TYPE_ARRAY)
    {
      geis_error("malformed GeisEvent frame: expected %c, received %c",
                 DBUS_TYPE_ARRAY, type);
    }
    else
    {
      DBusMessageIter attr_iter;
      dbus_message_iter_recurse(&struct_iter, &attr_iter);
      for (int type = dbus_message_iter_get_arg_type(&attr_iter);
           type != DBUS_TYPE_INVALID;
           dbus_message_iter_next(&attr_iter),
           type = dbus_message_iter_get_arg_type(&attr_iter))
      {
        GeisAttr attr = geis_dbus_attr_unmarshall(&attr_iter);
        geis_frame_add_attr(frame, attr);
      }
    }

    dbus_message_iter_next(&struct_iter),
    type = dbus_message_iter_get_arg_type(&struct_iter);
    if (type != DBUS_TYPE_ARRAY)
    {
      geis_error("malformed GeisEvent frame: expected %c, received %c",
                 DBUS_TYPE_ARRAY, type);
    }
    else
    {
      DBusMessageIter class_iter;
      dbus_message_iter_recurse(&struct_iter, &class_iter);
      for (int type = dbus_message_iter_get_arg_type(&class_iter);
           type != DBUS_TYPE_INVALID;
           dbus_message_iter_next(&class_iter),
           type = dbus_message_iter_get_arg_type(&class_iter))
      {
        type = dbus_message_iter_get_arg_type(&class_iter);
        if (type != DBUS_TYPE_INT32)
        {
          geis_error("malformed GeisEvent frame: expected %c, received %c",
                     DBUS_TYPE_INT32, type);
          break;
        }

        dbus_int32_t class_id;
        dbus_message_iter_get_basic(&class_iter, &class_id);
        GeisGestureClassBag bag = geis_gesture_classes(geis);
        for (GeisSize i = 0; i < geis_gesture_class_bag_count(bag); ++i)
        {
          GeisGestureClass gesture_class;
          gesture_class = geis_gesture_class_bag_gesture_class(bag, i);
          if (geis_gesture_class_id(gesture_class) == class_id)
          {
            geis_frame_set_is_class(frame, gesture_class);
            break;
          }
        }
      }
    }

    dbus_message_iter_next(&struct_iter),
    type = dbus_message_iter_get_arg_type(&struct_iter);
    if (type != DBUS_TYPE_ARRAY)
    {
      geis_error("malformed GeisEvent frame: expected %c, received %c",
                 DBUS_TYPE_ARRAY, type);
    }
    else
    {
      DBusMessageIter touch_iter;
      dbus_message_iter_recurse(&struct_iter, &touch_iter);
      for (int type = dbus_message_iter_get_arg_type(&touch_iter);
           type != DBUS_TYPE_INVALID;
           dbus_message_iter_next(&touch_iter),
           type = dbus_message_iter_get_arg_type(&touch_iter))
      {
        type = dbus_message_iter_get_arg_type(&touch_iter);
        if (type != DBUS_TYPE_INT32)
        {
          geis_error("malformed GeisEvent frame: expected %c, received %c",
                     DBUS_TYPE_INT32, type);
          break;
        }

        dbus_int32_t touch_id;
        dbus_message_iter_get_basic(&touch_iter, &touch_id);
        geis_frame_add_touchid(frame, touch_id);
      }
    }
  }

final_exit:
  return;
}


/**
 * Marshalls a GEIS groupset to a DBus message via a message iterator.
 * @param[in]  groupset  The GEIS groupset.
 * @param[in]  iter      A DBus message iterator.
 */
static void
_marshall_groupset(GeisGroupSet groupset, DBusMessageIter *iter)
{
  DBusMessageIter groupset_iter;
  dbus_message_iter_open_container(iter,
                                   DBUS_TYPE_ARRAY,
                                   DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                   DBUS_TYPE_INT32_AS_STRING
                                   GEIS_DBUS_TYPE_SIGNATURE_FRAMESET 
                                   DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                   &groupset_iter);
  for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
  {
    GeisGroup group = geis_groupset_group(groupset, i);
    if (!group)
    {
      geis_warning("can not extract group %zu from groupset", i);
      goto final_exit;
    }
    DBusMessageIter group_iter;
    dbus_message_iter_open_container(&groupset_iter,
                                     DBUS_TYPE_DICT_ENTRY,
                                     NULL,
                                     &group_iter);
    dbus_int32_t group_id = geis_group_id(group);
    dbus_message_iter_append_basic(&group_iter, DBUS_TYPE_INT32, &group_id);
    DBusMessageIter frameset_iter;
    dbus_message_iter_open_container(&group_iter,
                                     DBUS_TYPE_ARRAY,
                                     GEIS_DBUS_TYPE_SIGNATURE_FRAME,
                                     &frameset_iter);
    for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
    {
      GeisFrame frame = geis_group_frame(group, j);
      if (!frame)
      {
        geis_warning("can not extract frame %zu from group", j);
        goto final_exit;
      }
      _marshall_frame(frame, &frameset_iter);
    }
    dbus_message_iter_close_container(&group_iter, &frameset_iter);
    dbus_message_iter_close_container(&groupset_iter, &group_iter);
  }
  dbus_message_iter_close_container(iter, &groupset_iter);

final_exit:
  return;
}


static void
_unmarshall_groupset(Geis geis, DBusMessageIter *iter, GeisGroupSet groupset)
{
  int dtype = dbus_message_iter_get_arg_type(iter);
  if (dtype != DBUS_TYPE_ARRAY)
  {
    geis_error("malformed GeisEvent groupset");
  }

  DBusMessageIter groupset_iter;
  dbus_message_iter_recurse(iter, &groupset_iter);
  for (dtype = dbus_message_iter_get_arg_type(&groupset_iter);
       dtype != DBUS_TYPE_INVALID;
       dbus_message_iter_next(&groupset_iter),
       dtype = dbus_message_iter_get_arg_type(&groupset_iter))
  {
    DBusMessageIter group_iter;
    dbus_message_iter_recurse(&groupset_iter, &group_iter);

    int type = dbus_message_iter_get_arg_type(&group_iter);
    if (type !=  DBUS_TYPE_INT32)
    {
      geis_error("malformed GeisEvent group");
      continue;
    }
    dbus_int32_t group_id;
    dbus_message_iter_get_basic(&group_iter, &group_id);
    GeisGroup group = geis_group_new(group_id);
    geis_groupset_insert(groupset, group);
    dbus_message_iter_next(&group_iter);

    DBusMessageIter frameset_iter;
    dbus_message_iter_recurse(&group_iter, &frameset_iter);
    for (int ftype = dbus_message_iter_get_arg_type(&frameset_iter);
         ftype != DBUS_TYPE_INVALID;
         dbus_message_iter_next(&frameset_iter),
         ftype = dbus_message_iter_get_arg_type(&frameset_iter))
    {
      _unmarshall_frame(geis, &frameset_iter, group);
    }
  }
}


/*
 * Creates a Dbus "gesture event" message from a GEIS gesture event.
 *
 * A gesture event has the following structure.
 *   - a numeric event type (begin/update/end)
 *   - a set of one or more touches, where each touch has
 *     - a touch ID
 *     - a set of one or more attrs
 *   - a set of one or more gesture groups, where is group has
 *     - a group ID
 *     - a set of one or more gesture frames, where each frame has
 *       - a set of one or more gesture classes
 *       - a set of one or more gesture attrs
 *       - a zet of one or more touch indexes
 *
 * @todo add the gesture classes
 */
DBusMessage *
geis_dbus_gesture_event_message_from_geis_event(GeisEvent event)
{
  DBusMessage *message = dbus_message_new_signal(GEIS_DBUS_SERVICE_PATH,
                                                 GEIS_DBUS_SERVICE_INTERFACE,
                                                 GEIS_DBUS_GESTURE_EVENT);
  DBusMessageIter iter;
  dbus_message_iter_init_append(message, &iter);

  dbus_uint32_t event_type = geis_event_type(event);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &event_type);

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

  _marshall_touchset(touchset, &iter);

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

  _marshall_groupset(groupset, &iter);

final_exit:
  return message;
}


/*
 * Indicates if a DBus message is a "gesture event" message.
 */
GeisBoolean
geis_dbus_message_is_gesture_event(DBusMessage *message)
{
  GeisBoolean is_gesture_event_message = GEIS_FALSE;
  if (dbus_message_is_signal(message,
                             GEIS_DBUS_SERVICE_INTERFACE,
                             GEIS_DBUS_GESTURE_EVENT))
  {
    is_gesture_event_message = GEIS_TRUE;
  }

  return is_gesture_event_message;
}


/*
 * Creates GEIS event from a DBus "gesture_event" message.
 */
GeisEvent
geis_dbus_gesture_event_from_message(Geis geis, DBusMessage *message)
{
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);

  dbus_uint32_t event_type;
  dbus_message_iter_get_basic(&iter, &event_type);
  GeisEvent event = geis_event_new(event_type);

  dbus_message_iter_next(&iter);
  GeisTouchSet  touchset = geis_touchset_new();
  _unmarshall_touchset(&iter, touchset);
  GeisAttr touch_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_TOUCHSET,
                                      GEIS_ATTR_TYPE_POINTER,
                                      touchset);
  geis_attr_set_destructor(touch_attr, (GeisAttrDestructor)geis_touchset_delete);
  geis_event_add_attr(event, touch_attr);

  dbus_message_iter_next(&iter);
  GeisGroupSet  groupset = geis_groupset_new();
  _unmarshall_groupset(geis, &iter, groupset);
  GeisAttr      group_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_GROUPSET,
                                           GEIS_ATTR_TYPE_POINTER,
                                           groupset);
  geis_attr_set_destructor(group_attr, (GeisAttrDestructor)geis_groupset_delete);
  geis_event_add_attr(event, group_attr);

  return event;
}


