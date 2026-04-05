/**
 * @file geis_dbus_gesture_class.c
 * @brief Implementations of the GEIS DBus gesture_class transport.
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_config.h"
#include "geis_dbus_class.h"

#include "geis_dbus.h"
#include "geis_dbus_attr.h"
#include "geis_class.h"
#include "geis_logging.h"


/*
 * Creates a Dbus "gesture_class available" message from a GEIS gesture_class.
 *
 * The Wire protocol for this message is class_id and class_name followed by
 * the list of class attributes.
 */
DBusMessage *
geis_dbus_class_available_message_from_class(GeisGestureClass gesture_class)
{
  DBusMessage *message = dbus_message_new_signal(GEIS_DBUS_SERVICE_PATH,
                                                 GEIS_DBUS_SERVICE_INTERFACE,
                                                 GEIS_DBUS_CLASS_AVAILABLE);
  DBusMessageIter iter;
  dbus_message_iter_init_append(message, &iter);

  dbus_int32_t gesture_class_id = geis_gesture_class_id(gesture_class);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &gesture_class_id);

  const char *gesture_class_name = geis_gesture_class_name(gesture_class);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &gesture_class_name);

  DBusMessageIter array_iter;
  dbus_message_iter_open_container(&iter,
                                   DBUS_TYPE_ARRAY,
                                   GEIS_DBUS_TYPE_SIGNATURE_ATTR,
                                   &array_iter);
  GeisSize attr_count = geis_gesture_class_attr_count(gesture_class);
  for (GeisSize i = 0; i < attr_count; ++i)
  {
    geis_dbus_attr_marshall(geis_gesture_class_attr(gesture_class, i),
                            &array_iter);
  }
  dbus_message_iter_close_container(&iter, &array_iter);
  return message;
}


/*
 * Creates GEIS gesture_class from a DBus "gesture_class available" message.
 */
GeisGestureClass
geis_dbus_class_class_from_available_message(DBusMessage *message)
{
  GeisGestureClass gesture_class = NULL;
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);

  int type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_INT32)
  {
    geis_error("error getting gesture_class ID from DBus message.");
    goto final_exit;
  }
  dbus_int32_t gesture_class_id;
  dbus_message_iter_get_basic(&iter, &gesture_class_id);

  dbus_message_iter_next(&iter);
  type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_STRING)
  {
    geis_error("error getting gesture_class name from DBus message.");
    goto final_exit;
  }

  char *gesture_class_name;
  dbus_message_iter_get_basic(&iter, &gesture_class_name);
  gesture_class = geis_gesture_class_new(gesture_class_name, gesture_class_id);

  dbus_message_iter_next(&iter);
  type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_ARRAY)
  {
    geis_error("error getting gesture_class attr list from DBus message.");
    goto final_exit;
  }

  DBusMessageIter array_iter;
  dbus_message_iter_recurse(&iter, &array_iter);
  int atype = dbus_message_iter_get_arg_type(&array_iter);
  while (atype == DBUS_TYPE_DICT_ENTRY)
  {
    GeisAttr attr = geis_dbus_attr_unmarshall(&array_iter);
    if (attr)
    {
      geis_gesture_class_add_attr(gesture_class, attr);
    }

    dbus_message_iter_next(&array_iter);
    atype = dbus_message_iter_get_arg_type(&array_iter);
  }

final_exit:
  return gesture_class;
}


