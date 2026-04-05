/**
 * @file geis_dbus_device.c
 * @brief Implementations of the GEIS DBus device transport.
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
#include "geis_dbus_device.h"

#include "geis_dbus.h"
#include "geis_dbus_attr.h"
#include "geis_device.h"
#include "geis_logging.h"


/*
 * Creates a Dbus "device available" message from a GEIS device.
 *
 * The Wire protocol for this message is device_id and device_name followed by
 * the list of device attributes.
 */
DBusMessage *
geis_dbus_device_available_message_from_device(GeisDevice device)
{
  DBusMessage *message = dbus_message_new_signal(GEIS_DBUS_SERVICE_PATH,
                                                 GEIS_DBUS_SERVICE_INTERFACE,
                                                 GEIS_DBUS_DEVICE_AVAILABLE);
  DBusMessageIter iter;
  dbus_message_iter_init_append(message, &iter);

  dbus_int32_t device_id = geis_device_id(device);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &device_id);

  const char *device_name = geis_device_name(device);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &device_name);

  DBusMessageIter array_iter;
  dbus_message_iter_open_container(&iter,
                                   DBUS_TYPE_ARRAY,
                                   GEIS_DBUS_TYPE_SIGNATURE_ATTR,
                                   &array_iter);
  GeisSize attr_count = geis_device_attr_count(device);
  for (GeisSize i = 0; i < attr_count; ++i)
  {
    geis_dbus_attr_marshall(geis_device_attr(device, i), &array_iter);
  }
  dbus_message_iter_close_container(&iter, &array_iter);
  return message;
}


/*
 * Creates GEIS device from a DBus "device available" message.
 */
GeisDevice
geis_dbus_device_device_from_available_message(DBusMessage *message)
{
  geis_debug("begins");
  GeisDevice device = NULL;
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);

  int type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_INT32)
  {
    geis_error("error getting device ID from DBus message.");
    goto final_exit;
  }
  dbus_int32_t device_id;
  dbus_message_iter_get_basic(&iter, &device_id);

  dbus_message_iter_next(&iter);
  type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_STRING)
  {
    geis_error("error getting device name from DBus message.");
    goto final_exit;
  }

  char *device_name;
  dbus_message_iter_get_basic(&iter, &device_name);
  device = geis_device_new(device_name, device_id);

  dbus_message_iter_next(&iter);
  type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_ARRAY)
  {
    geis_error("error getting device attr list from DBus message.");
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
      geis_device_add_attr(device, attr);
    }

    dbus_message_iter_next(&array_iter);
    atype = dbus_message_iter_get_arg_type(&array_iter);
  }

final_exit:
  geis_debug("ends");
  return device;
}


/*
 * Creates a Dbus "device unavailable" message from a GEIS device.
 */
DBusMessage *
geis_dbus_device_unavailable_message_from_device(GeisDevice device GEIS_UNUSED)
{
  geis_debug("begins");
  DBusMessage *message = NULL;
  geis_debug("ends");
  return message;
}


/*
 * Creates GEIS device from a DBus "device unavailable" message.
 */
GeisDevice
geis_dbus_device_device_from_unavailable_message(DBusMessage *message GEIS_UNUSED)
{
  geis_debug("begins");
  GeisDevice device = NULL;
  geis_debug("ends");
  return device;
}

