/**
 * @file geis_dbus_server.h
 * @brief Interface for the GEIS DBus server.
 *
 * The GEIS DBus server offers remote GEIS functionality over a set of managed
 * DBus connections.
 *
 * This header is for internal GEIS use only and contains no client
 * (externally-visible) symbols.
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
#include "geis_dbus_server.h"

#include "geis_dbus_announcer.h"
#include "geis_dbus_client_proxy.h"
#include "geis_dbus_dispatcher.h"
#include "geis_dbus_proxy_box.h"
#include "geis_logging.h"
#include <stdio.h>


struct GeisDBusServer
{
  Geis                geis;
  DBusServer         *dbus_server;
  GeisDBusDispatcher  dispatcher;
  GeisDBusAnnouncer   announcer;
  GeisDBusProxyBox    proxy_box;
};


static void
_new_geis_dbus_client_connection(DBusServer     *server GEIS_UNUSED,
                                 DBusConnection *new_connection,
                                 void           *data)
{
  GeisDBusServer geis_server = (GeisDBusServer)data;
  GeisDBusClientProxy proxy = geis_dbus_client_proxy_new(geis_server,
                                                         new_connection);
  if (proxy)
  {
    geis_dbus_proxy_box_insert(geis_server->proxy_box, proxy);
  }
}

  
/*
 * Shuts down and disconnects a client.
 */
void
geis_dbus_server_client_disconnect(GeisDBusServer      geis_server,
                                   GeisDBusClientProxy proxy)
{
  geis_dbus_proxy_box_remove(geis_server->proxy_box, proxy);
  geis_dbus_client_proxy_delete(proxy);
}


/*
 * Processes GEIS events.
 */
static void
_geis_event_callback(Geis        geis GEIS_UNUSED,
                     GeisEvent   event,
                     GeisPointer context)
{
  GeisDBusServer server = (GeisDBusServer)context;
  for (GeisDBusProxyBoxIterator it = geis_dbus_proxy_box_begin(server->proxy_box);
       it != geis_dbus_proxy_box_end(server->proxy_box);
       it = geis_dbus_proxy_box_iter_next(server->proxy_box, it))
  {
    GeisDBusClientProxy proxy = geis_dbus_proxy_box_iter_value(it);
    geis_dbus_client_proxy_handle_geis_event(proxy, event);
  }
  geis_event_delete(event);
}


/*
 * Adds the announcer watches to the dispatcher watch list.
 */
static dbus_bool_t
_server_add_watch(DBusWatch *watch, void *data)
{
  dbus_bool_t status = TRUE;
  dbus_bool_t is_enabled = dbus_watch_get_enabled(watch);
  GeisDBusServer server = (GeisDBusServer)data;

  if (is_enabled) /* fixme: ?? */
  {
    geis_dbus_dispatcher_register(server->dispatcher, NULL, watch);
  }
  return status;
}


/*
 * Toggles the enabled/disabled status of the server watches.
 */
static void
_server_toggle_watch(DBusWatch *watch, void *data)
{
  GeisDBusServer server = (GeisDBusServer)data;

  geis_dbus_dispatcher_toggle_watch(server->dispatcher, watch);
}


/*
 * Removes the server watches from the dispatcher watch list.
 */
static void
_server_remove_watch(DBusWatch *watch, void *data)
{
  GeisDBusServer server = (GeisDBusServer)data;

  geis_dbus_dispatcher_unregister(server->dispatcher, watch);
}


/*
 * Adds a server timeout.
 */
static dbus_bool_t
_server_add_timeout(DBusTimeout *timeout GEIS_UNUSED, void *data GEIS_UNUSED)
{
  dbus_bool_t status = TRUE;
  return status;
}


/*
 * Removes a server timeout.
 */
static void
_server_remove_timeout(DBusTimeout *timeout GEIS_UNUSED, void *data GEIS_UNUSED)
{
}


/*
 * Adds a server timeout.
 */
static void
_server_toggle_timeout(DBusTimeout *timeout GEIS_UNUSED, void *data GEIS_UNUSED)
{
}


/*
 * Creates a new GeisDBusServer.
 */
GeisDBusServer
geis_dbus_server_new(Geis geis)
{
  GeisDBusServer server = NULL;
  DBusError      error;

  server = calloc(1, sizeof(struct GeisDBusServer));
  if (!server)
  {
    goto final_exit;
  }

  server->geis = geis;

  dbus_error_init(&error);
  server->dbus_server = dbus_server_listen("unix:abstract=geis", &error);
  if (!server->dbus_server || dbus_error_is_set(&error))
  {
    char msg[128];
    snprintf(msg, sizeof(msg),
             "error %s creating DBus server: %s",
             error.name, error.message);
    geis_error("%s", msg);
    dbus_error_free(&error);
    goto unwind_alloc;
  }

  server->dispatcher = geis_dbus_dispatcher_new(server->geis);
  if (!server->dispatcher)
  {
    geis_error("error creating server dispatcher.");
    goto unwind_server;
  }

  server->announcer = geis_dbus_announcer_new(server);
  if (!server->announcer)
  {
    geis_error("error creating server announcer.");
    goto unwind_dispatcher;
  }

  server->proxy_box = geis_dbus_proxy_box_new();
  if (!server->proxy_box)
  {
    geis_error("error creating server proxy box.");
    goto unwind_announcer;
  }

  dbus_server_set_watch_functions(server->dbus_server,
                                  _server_add_watch,
                                  _server_remove_watch,
                                  _server_toggle_watch,
                                  server,
                                  NULL);

  dbus_server_set_timeout_functions(server->dbus_server,
                                    _server_add_timeout,
                                    _server_remove_timeout,
                                    _server_toggle_timeout,
                                    server,
                                    NULL);

  dbus_server_set_new_connection_function(server->dbus_server,
                                          _new_geis_dbus_client_connection,
                                          server,
                                          NULL);

  geis_register_event_callback(server->geis, _geis_event_callback, server);
  goto final_exit;

unwind_announcer:
  geis_dbus_announcer_delete(server->announcer);
unwind_dispatcher:
  geis_dbus_dispatcher_delete(server->dispatcher);
unwind_server:
  dbus_server_disconnect(server->dbus_server);
  dbus_server_unref(server->dbus_server);
unwind_alloc:
  free(server);
  server= NULL;
final_exit:
  return server;
}


/*
 * Destroys a GeisDBusServer.
 */
void
geis_dbus_server_delete(GeisDBusServer server)
{
  if (server)
  {
    geis_dbus_proxy_box_delete(server->proxy_box);
    geis_dbus_announcer_delete(server->announcer);
    dbus_server_disconnect(server->dbus_server);
    dbus_server_unref(server->dbus_server);
    geis_dbus_dispatcher_delete(server->dispatcher);
    free(server);
  }
}


/*
 * Gets the address of a GeisDBusServer.
 */
char *
geis_dbus_server_address(GeisDBusServer server)
{
  return dbus_server_get_address(server->dbus_server);
}


/*
 * Gets the server dispatcher.
 */
GeisDBusDispatcher
geis_dbus_server_dispatcher(GeisDBusServer server)
{
  return server->dispatcher;
}


/*
 * Gets the server geis instance.
 */
Geis
geis_dbus_server_geis(GeisDBusServer server)
{
  return server->geis;
}



