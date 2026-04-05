/**
 * @file geis_dbus_region.c
 * @brief Implementations of the GEIS DBus region transport.
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
#include "geis_dbus_region.h"

#include "geis_dbus.h"
#include "geis_logging.h"


/*
 * Creates a Dbus "region available" message from a GEIS region.
 */
DBusMessage *
geis_dbus_region_available_message_from_region(GeisFilterableAttribute fa)
{
  DBusMessage *message = dbus_message_new_signal(GEIS_DBUS_SERVICE_PATH,
                                                 GEIS_DBUS_SERVICE_INTERFACE,
                                                 GEIS_DBUS_REGION_AVAILABLE);
  DBusMessageIter iter;
  dbus_message_iter_init_append(message, &iter);

  const char *attr_name = fa->name;
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &attr_name);

  dbus_int32_t attr_type = fa->type;
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &attr_type);


  return message;
}


/*
 * Creates GEIS region filterable attribute from a DBus "region available"
 * message.
 */
GeisFilterableAttribute
geis_dbus_region_from_region_available_message(DBusMessage *message)
{
  geis_debug("begins");
  static struct GeisFilterableAttribute attr;
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);

  int type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_STRING)
  {
    geis_error("error getting attr name name from DBus message.");
    goto final_exit;
  }
  char *attr_name;
  dbus_message_iter_get_basic(&iter, &attr_name);
  dbus_message_iter_next(&iter);

  type = dbus_message_iter_get_arg_type(&iter);
  if (type != DBUS_TYPE_INT32)
  {
    geis_error("error getting attr type from DBus message.");
    goto final_exit;
  }
  dbus_int32_t attr_type;
  dbus_message_iter_get_basic(&iter, &attr_type);

  attr.name = attr_name;
  attr.type = attr_type;

final_exit:
  geis_debug("ends");
  return &attr;
}


