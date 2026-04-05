/**
 * @file geis_dbus_announcer.c
 * @brief Implementation of the GEIS DBus announcer.
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
#include "geis_dbus_announcer.h"

#include <dbus/dbus.h>
#include "geis_dbus.h"
#include "geis_logging.h"
#include <stdio.h>
#include <stdlib.h>


struct GeisDBusAnnouncer
{
  GeisDBusServer  server;
  DBusConnection *session_bus;
};


static void
_unregister_messages(DBusConnection *connection GEIS_UNUSED,
                     void           *user_data GEIS_UNUSED)
{
  /* dummy */
}


/*
 * Processes DBus metamessages.
 */
static DBusHandlerResult
_announcer_dbus_messages(DBusConnection *connection GEIS_UNUSED,
                         DBusMessage    *message,
                         void           *user_data GEIS_UNUSED)
{
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (dbus_message_is_signal(message,
                             DBUS_SERVICE_DBUS,
                             "NameAcquired"))
  {
    result = DBUS_HANDLER_RESULT_HANDLED;
  }

  return result;
}


/*
 * Processes GEIS messages.
 */
static DBusHandlerResult
_announcer_geis_messages(DBusConnection *connection,
               DBusMessage    *message,
               void           *user_data)
{
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  GeisDBusAnnouncer announcer = (GeisDBusAnnouncer)user_data;

  if (dbus_message_is_method_call(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_GET_SERVER_ADDRESS))
  {
    char *server_addr = geis_dbus_server_address(announcer->server);
    DBusMessage *reply = dbus_message_new_method_return(message);
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING, &server_addr,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    dbus_free(server_addr);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }

  return result;
}


/*
 * Virtual dispatch table for handling DBus messages.
 */
static struct DBusObjectPathVTable _dbus_message_vtbl = 
{
  _unregister_messages,
  _announcer_dbus_messages,
  0, 0, 0, 0
};

/*
 * Virtual dispatch table for handling Geis messages.
 */
static struct DBusObjectPathVTable _geis_message_vtbl = 
{
  _unregister_messages,
  _announcer_geis_messages,
  0, 0, 0, 0
};


/*
 * Adds the announcer watches to the dispatcher watch list.
 */
static dbus_bool_t
_announcer_add_watch(DBusWatch *watch, void *data)
{
  dbus_bool_t status = TRUE;
  GeisDBusAnnouncer announcer = (GeisDBusAnnouncer)data;
  GeisDBusDispatcher dispatcher = geis_dbus_server_dispatcher(announcer->server);

  geis_dbus_dispatcher_register(dispatcher, announcer->session_bus, watch);
  return status;
}


/*
 * Toggles the enabled/disabled status of the announcer watches.
 */
static void
_announcer_toggle_watch(DBusWatch *watch, void *data)
{
  GeisDBusAnnouncer announcer = (GeisDBusAnnouncer)data;
  GeisDBusDispatcher dispatcher = geis_dbus_server_dispatcher(announcer->server);

  geis_dbus_dispatcher_toggle_watch(dispatcher, watch);
}


/*
 * Removes the announcer watches from the dispatcher watch list.
 */
static void
_announcer_remove_watch(DBusWatch *watch, void *data)
{
  GeisDBusAnnouncer announcer = (GeisDBusAnnouncer)data;
  GeisDBusDispatcher dispatcher = geis_dbus_server_dispatcher(announcer->server);

  geis_dbus_dispatcher_unregister(dispatcher, watch);
}


/*
 * Creates a new GeisDBusAnouncer object.
 */
GeisDBusAnnouncer
geis_dbus_announcer_new(GeisDBusServer server)
{
  DBusError error;
  GeisDBusAnnouncer announcer = NULL;
  
  announcer = calloc(1, sizeof(struct GeisDBusAnnouncer));
  if (!announcer)
    goto final_exit;

  announcer->server = server;

  dbus_error_init(&error);
  announcer->session_bus = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!announcer->session_bus || dbus_error_is_set(&error))
  {
    char msg[128];
    sprintf(msg, "error %s connecting to session bus: %s",
            error.name, error.message);
    geis_error("%s", msg);
    goto unwind_error;
  }

  dbus_connection_set_watch_functions(announcer->session_bus,
                                      _announcer_add_watch,
                                      _announcer_remove_watch,
                                      _announcer_toggle_watch,
                                      announcer, 0);

  dbus_connection_register_object_path(announcer->session_bus,
                                       DBUS_PATH_DBUS,
                                       &_dbus_message_vtbl,
                                       announcer);

  dbus_connection_register_object_path(announcer->session_bus,
                                       GEIS_DBUS_SERVICE_PATH,
                                       &_geis_message_vtbl,
                                       announcer);

  dbus_bus_request_name(announcer->session_bus,
                        GEIS_DBUS_SERVICE_INTERFACE,
                        DBUS_NAME_FLAG_REPLACE_EXISTING,
                        &error);
  if (dbus_error_is_set(&error))
  {
    geis_error("error requesting server name from DBus session bus");
  }

unwind_error:
  dbus_error_free(&error);
final_exit:
  return announcer;
}


/*
 * Destroys a %GeisDBusAnnouncer object.
 */
void
geis_dbus_announcer_delete(GeisDBusAnnouncer announcer)
{
  if (announcer)
  {
    if (announcer->session_bus)
    {
      dbus_connection_unref(announcer->session_bus);
    }
    free(announcer);
  }
}

