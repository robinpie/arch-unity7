/**
 * @file geis_dbus_locator.c
 * @brief Implementation of the GEIS DBus locator.
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
#include "geis_dbus_locator.h"

#include <dbus/dbus.h>
#include "geis_dbus.h"
#include "geis_logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum GeisDBusLocatorState
{
  GEIS_DBUS_LOCATOR_STATE_INITIALIZING,
  GEIS_DBUS_LOCATOR_STATE_LOCATING,
  GEIS_DBUS_LOCATOR_STATE_WAITING,
  GEIS_DBUS_LOCATOR_STATE_FINALIZING
} GeisDBusLocatorState;


struct GeisDBusLocator 
{
  GeisDBusClient        client;
  GeisDBusLocatorState  state;
  DBusConnection       *session_bus;
  char                 *server_address;
  dbus_uint32_t         serial;
};


/*
 * Performs the act of actually locating the server.
 */
static void
_locator_find_server(GeisDBusLocator locator)
{
  locator->state = GEIS_DBUS_LOCATOR_STATE_LOCATING;
  DBusMessage *msg = dbus_message_new_method_call(GEIS_DBUS_SERVICE_INTERFACE,
                                                  GEIS_DBUS_SERVICE_PATH,
                                                  GEIS_DBUS_SERVICE_INTERFACE,
                                                  GEIS_DBUS_GET_SERVER_ADDRESS);
  dbus_connection_send(locator->session_bus, msg, &locator->serial);
  dbus_message_unref(msg);
}


/*
 * A generic message handler function.
 */
static DBusHandlerResult
_locator_message_handler(DBusConnection *connection GEIS_UNUSED,
                         DBusMessage    *message,
                         void           *user_data)
{
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  GeisDBusLocator locator = (GeisDBusLocator)user_data;
  int type = dbus_message_get_type(message);

  if (dbus_message_is_signal(message, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
  {
    char *name;
    char *old_owner;
    char *new_owner;
    dbus_message_get_args(message, NULL,
                          DBUS_TYPE_STRING, &name,
                          DBUS_TYPE_STRING, &old_owner,
                          DBUS_TYPE_STRING, &new_owner,
                          DBUS_TYPE_INVALID);
    if (strlen(old_owner))
    {
      geis_debug("%s has gone away", name);
      geis_dbus_client_server_dislocated(locator->client);
      result = DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (strlen(new_owner))
    {
      geis_debug("%s has appeared", name);
      _locator_find_server(locator);
      result = DBUS_HANDLER_RESULT_HANDLED;
    }
  }
  else if (type == DBUS_MESSAGE_TYPE_METHOD_RETURN)
  {
    if (locator->serial == dbus_message_get_reply_serial(message))
    {
      const char *s = NULL;
      dbus_message_get_args(message, NULL,
                            DBUS_TYPE_STRING, &s,
                            DBUS_TYPE_INVALID);
      locator->server_address = strdup(s);
      geis_dbus_client_server_located(locator->client);
      result = DBUS_HANDLER_RESULT_HANDLED;
    }
  }
  else if (type == DBUS_MESSAGE_TYPE_ERROR)
  {
    if (dbus_message_is_error(message, DBUS_ERROR_SERVICE_UNKNOWN))
    {
      geis_warning("server not found!");
      geis_dbus_client_server_dislocated(locator->client);
      result = DBUS_HANDLER_RESULT_HANDLED;
    }
    else
    {
      const char *str = NULL;
      dbus_message_get_args(message, NULL,
                            DBUS_TYPE_STRING, &str,
                            DBUS_TYPE_INVALID);
      geis_warning("error %s: %s", dbus_message_get_error_name(message), str);
    }
  }

  return result;
}


/*
 * Adds the locator watches to the dispatcher watch list.
 */
static dbus_bool_t
_locator_add_watch(DBusWatch *watch, void *data)
{
  dbus_bool_t status = TRUE;
  GeisDBusLocator locator = (GeisDBusLocator)data;
  GeisDBusDispatcher dispatcher = geis_dbus_client_dispatcher(locator->client);

  geis_dbus_dispatcher_register(dispatcher, locator->session_bus, watch);
  return status;
}


/*
 * Toggles the enabled/disabled status of the locator watches.
 */
static void
_locator_toggle_watch(DBusWatch *watch, void *data)
{
  GeisDBusLocator locator = (GeisDBusLocator)data;
  GeisDBusDispatcher dispatcher = geis_dbus_client_dispatcher(locator->client);

  geis_dbus_dispatcher_toggle_watch(dispatcher, watch);
}


/*
 * Removes the locator watches from the dispatcher watch list.
 */
static void
_locator_remove_watch(DBusWatch *watch, void *data)
{
  GeisDBusLocator locator = (GeisDBusLocator)data;
  GeisDBusDispatcher dispatcher = geis_dbus_client_dispatcher(locator->client);

  geis_dbus_dispatcher_unregister(dispatcher, watch);
}


/*
 * Creates a new GeisDBusLocator object.
 */
GeisDBusLocator
geis_dbus_locator_new(GeisDBusClient client)
{
  GeisDBusLocator locator = NULL;

  /* Fail to locate Geis on the DBus if there is no DBus session. */
  if (NULL == getenv("DBUS_SESSION_BUS_ADDRESS"))
  {
    goto final_exit;
  }

  locator = calloc(1, sizeof(struct GeisDBusLocator));
  if (!locator)
  {
    goto final_exit;
  }

  locator->client = client;
  locator->state = GEIS_DBUS_LOCATOR_STATE_INITIALIZING;

  /* Connect to the DBus session bus. */
  DBusError error = DBUS_ERROR_INIT;
  locator->session_bus = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!locator->session_bus || dbus_error_is_set(&error))
  {
    char msg[512];
    snprintf(msg, sizeof(msg), "error %s connecting to session bus: %s",
            error.name, error.message);
    geis_error("%s", msg);
    goto unwind_error;
  }

  /* Integrate with the app event loop via the GEIS multiplexor. */
  dbus_connection_set_watch_functions(locator->session_bus,
                                      _locator_add_watch,
                                      _locator_remove_watch,
                                      _locator_toggle_watch,
                                      locator, 0);

  /* Look for server-connect and server-disconnect messages. */
  dbus_bus_add_match(locator->session_bus,
                     "type='signal',sender='" DBUS_SERVICE_DBUS "'," \
                     "interface='" DBUS_INTERFACE_DBUS "'," \
                     "member='NameOwnerChanged'," \
                     "arg0='" GEIS_DBUS_SERVICE_INTERFACE "'",
                     &error);
  if (dbus_error_is_set(&error))
  {
    char msg[512];
    snprintf(msg, sizeof(msg), "error %s adding match to session bus: %s",
            error.name, error.message);
    geis_error("%s", msg);
    goto unwind_error;
  }

  /* Install a handler for any and all messages. */
  dbus_connection_add_filter(locator->session_bus,
                             _locator_message_handler,
                             locator, 0);

  /* OK, go eh? */
  _locator_find_server(locator);

unwind_error:
  dbus_error_free(&error);
final_exit:
  return locator;
}


/*
 * Destroys a %GeisDBusLocator object.
 */
void
geis_dbus_locator_delete(GeisDBusLocator locator)
{
  if (locator)
  {
    if (locator->server_address)
    {
      free(locator->server_address);
    }
    if (locator->session_bus)
    {
      dbus_connection_set_watch_functions(locator->session_bus,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL, 0);
      dbus_connection_remove_filter(locator->session_bus,
                                    _locator_message_handler,
                                    locator);
      dbus_connection_unref(locator->session_bus);
    }
    free(locator);
  }
}


char *
geis_dbus_locator_server_address(GeisDBusLocator locator)
{
  return locator->server_address;
}

