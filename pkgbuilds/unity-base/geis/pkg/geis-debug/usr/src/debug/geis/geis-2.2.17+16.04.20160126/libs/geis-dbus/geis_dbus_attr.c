/**
 * @file geis_dbus_attr.c
 * @brief Implementation of the GEIS DBus attr transport.
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
#include "geis_dbus_attr.h"

#include "geis_attr.h"
#include "geis_logging.h"


/*
 * Marshalls a single GEIS attr to an open DBus message container iterator.
 */
void 
geis_dbus_attr_marshall(GeisAttr attr, DBusMessageIter *iter)
{
  DBusMessageIter dict_iter;
  GeisString attr_name = geis_attr_name(attr);

  dbus_message_iter_open_container(iter,
                                   DBUS_TYPE_STRUCT,
                                   NULL,
                                   &dict_iter);
  dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &attr_name);
  switch (geis_attr_type(attr))
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
    {
      DBusMessageIter variant_iter;
      dbus_bool_t val = geis_attr_value_to_boolean(attr);
      dbus_message_iter_open_container(&dict_iter,
                                       DBUS_TYPE_VARIANT,
                                       DBUS_TYPE_BOOLEAN_AS_STRING,
                                       &variant_iter);
      dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_BOOLEAN, &val);
      dbus_message_iter_close_container(&dict_iter, &variant_iter);
      break;
    }

    case GEIS_ATTR_TYPE_FLOAT:
    {
      DBusMessageIter variant_iter;
      double val = geis_attr_value_to_float(attr);
      dbus_message_iter_open_container(&dict_iter,
                                       DBUS_TYPE_VARIANT,
                                       DBUS_TYPE_DOUBLE_AS_STRING,
                                       &variant_iter);
      dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_DOUBLE, &val);
      dbus_message_iter_close_container(&dict_iter, &variant_iter);
      break;
    }

    case GEIS_ATTR_TYPE_INTEGER:
    {
      DBusMessageIter variant_iter;
      dbus_int32_t val = geis_attr_value_to_integer(attr);
      dbus_message_iter_open_container(&dict_iter,
                                       DBUS_TYPE_VARIANT,
                                       DBUS_TYPE_INT32_AS_STRING,
                                       &variant_iter);
      dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_INT32, &val);
      dbus_message_iter_close_container(&dict_iter, &variant_iter);
      break;
    }

    case GEIS_ATTR_TYPE_STRING:
    {
      DBusMessageIter variant_iter;
      GeisString val = geis_attr_value_to_string(attr);
      dbus_message_iter_open_container(&dict_iter,
                                       DBUS_TYPE_VARIANT,
                                       DBUS_TYPE_STRING_AS_STRING,
                                       &variant_iter);
      dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &val);
      dbus_message_iter_close_container(&dict_iter, &variant_iter);
      break;
    }

    default:
      geis_error("invalid attribute type for DBus");
  }
  dbus_message_iter_close_container(iter, &dict_iter);
}


/*
 * Unmarshalls a single GEIS attr from a DBus message iterator.
 */
GeisAttr
geis_dbus_attr_unmarshall(DBusMessageIter *iter)
{
  GeisAttr attr = NULL;
  DBusMessageIter dict_iter;

  dbus_message_iter_recurse(iter, &dict_iter);
  int dtype = dbus_message_iter_get_arg_type(&dict_iter);
  if (dtype != DBUS_TYPE_STRING)
  {
    geis_error("error getting attr name from DBus message");
    goto final_exit;
  }

  char *attr_name;
  dbus_message_iter_get_basic(&dict_iter, &attr_name);

  dbus_message_iter_next(&dict_iter);
  dtype = dbus_message_iter_get_arg_type(&dict_iter);
  if (dtype != DBUS_TYPE_VARIANT)
  {
    geis_error("error getting attr variant from DBus message");
    goto final_exit;
  }

  DBusMessageIter variant_iter;
  dbus_message_iter_recurse(&dict_iter, &variant_iter);
  int vtype = dbus_message_iter_get_arg_type(&variant_iter);
  switch (vtype)
  {
    case DBUS_TYPE_BOOLEAN:
    {
      dbus_bool_t val;
      dbus_message_iter_get_basic(&variant_iter, &val);
      attr = geis_attr_new(attr_name, GEIS_ATTR_TYPE_BOOLEAN, &val);
      break;
    }

    case DBUS_TYPE_DOUBLE:
    {
      double dval;
      dbus_message_iter_get_basic(&variant_iter, &dval);
      float fval = dval;
      attr = geis_attr_new(attr_name, GEIS_ATTR_TYPE_FLOAT, &fval);
      break;
    }

    case DBUS_TYPE_INT32:
    {
      dbus_int32_t val;
      dbus_message_iter_get_basic(&variant_iter, &val);
      attr = geis_attr_new(attr_name, GEIS_ATTR_TYPE_INTEGER, &val);
      break;
    }

    case DBUS_TYPE_STRING:
    {
      GeisString val;
      dbus_message_iter_get_basic(&variant_iter, &val);
      attr = geis_attr_new(attr_name, GEIS_ATTR_TYPE_STRING, (void *)val);
      break;
    }

    default:
      geis_error("unexpected attr data type from DBus");
      break;
  }

final_exit:
  return attr;
}

