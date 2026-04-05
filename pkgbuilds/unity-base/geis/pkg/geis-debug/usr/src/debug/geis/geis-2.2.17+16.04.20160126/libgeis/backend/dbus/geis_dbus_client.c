/**
 * @file geis_dbus_client.c
 * @brief Implementations of the GEIS DBus client.
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
#include "geis_dbus_client.h"

#include "geis_dbus.h"
#include "geis_dbus_class.h"
#include "geis_dbus_device.h"
#include "geis_dbus_gesture_event.h"
#include "geis_dbus_locator.h"
#include "geis_dbus_region.h"
#include "geis_dbus_subscription.h"
#include "geis_event.h"
#include "geis_logging.h"
#include "geis_private.h"
#include <stdio.h>
#include <stdlib.h>


typedef enum GeisDBusClientState
{
  GEIS_DBUS_CLIENT_DISCONNECTED,   /* no server available */
  GEIS_DBUS_CLIENT_INITIALIZING,   /* server connected, client initializing */
  GEIS_DBUS_CLIENT_CONNECTING,     /* server connected, not initialized */
  GEIS_DBUS_CLIENT_CONNECTED       /* server connected, all systems go */
} GeisDBusClientState;


struct GeisDBusClient
{
  Geis                geis;
  GeisDBusDispatcher  dispatcher;
  GeisDBusLocator     locator;
  GeisDBusClientState state;
  DBusConnection     *connection;
  GeisSubBag          subscription_bag;
};


/**
 * Handles a device-available message from the server.
 *
 * @param[in] client  A %GeisDBusClient.
 * @param[in] message The %DBusMessage.
 */
static void
_client_device_available(GeisDBusClient client, DBusMessage *message)
{
  GeisDevice device = geis_dbus_device_device_from_available_message(message);
  if (device)
  {
    geis_register_device(client->geis, device, 0, NULL);
  }
  else
  {
    geis_error("no device received from remote back end");
  }
}


/**
 * Handles a device-unavailable message from the server.
 *
 * @param[in] client  A %GeisDBusClient.
 * @param[in] message The %DBusMessage.
 */
static void
_client_device_unavailable(GeisDBusClient client, DBusMessage *message)
{
  GeisDevice device = geis_dbus_device_device_from_unavailable_message(message);
  if (device)
  {
    geis_unregister_device(client->geis, device);
  }
  else
  {
    geis_error("no device received from remote back end");
  }
}


/**
 * Handles a class-available message from the server.
 *
 * @param[in] client  A %GeisDBusClient.
 * @param[in] message The %DBusMessage.
 */
static void
_client_class_available(GeisDBusClient client, DBusMessage *message)
{
  GeisGestureClass gesture_class;

  gesture_class = geis_dbus_class_class_from_available_message(message);
  if (gesture_class)
  {
    geis_register_gesture_class(client->geis, gesture_class, 0, NULL);
  }
  else
  {
    geis_error("no gesture class received from remote back end");
  }
}


/**
 * Handles a region-available message from the server.
 *
 * @param[in] client  A %GeisDBusClient.
 * @param[in] message The %DBusMessage.
 */
static void
_client_region_available(GeisDBusClient client, DBusMessage *message)
{
  GeisFilterableAttribute attr;

  attr = geis_dbus_region_from_region_available_message(message);
  if (attr)
  {
    attr->add_term_callback = 0;
    attr->add_term_context = 0;
    geis_register_region(client->geis, NULL, 1, attr);
  }
  else
  {
    geis_error("no region attr received from remote back end");
  }
}


/**
 * Handles a class-unavailable message from the server.
 *
 * @param[in] client  A %GeisDBusClient.
 * @param[in] message The %DBusMessage.
 */
static void
_client_gesture_event(GeisDBusClient client, DBusMessage *message)
{
  GeisEvent event = geis_dbus_gesture_event_from_message(client->geis, message);
  if (!event)
  {
    geis_error("no gesture event received from remote back end");
  }
  else
  {
    geis_post_event(client->geis, event);
  }
}


/**
 * Processes an subscription-activate reply from the server.
 *
 * @param[in] pending    A DBusPendingCall object.
 * @param[in] user_data  The %GeisDBusClient object.
 */
static void
_geis_dbus_client_activate_reply(DBusPendingCall *pending, void *user_data)
{
  GeisDBusClient client GEIS_UNUSED = (GeisDBusClient)user_data;
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);

  if (DBUS_MESSAGE_TYPE_ERROR == dbus_message_get_type(reply))
  {
    const char *s = NULL;
    dbus_message_get_args(reply, NULL, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
    geis_error("error %s: %s", dbus_message_get_error_name(reply), s);
  }
}


/**
 * Processes a subscription-create reply from the server.
 *
 * @param[in] pending    A DBusPendingCall object.
 * @param[in] user_data  The %GeisDBusClient object.
 */
static void
_geis_dbus_client_subscribe_reply(DBusPendingCall *pending, void *user_data)
{
  GeisDBusClient client = (GeisDBusClient)user_data;
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);

  if (DBUS_MESSAGE_TYPE_ERROR == dbus_message_get_type(reply))
  {
    const char *s = NULL;
    dbus_message_get_args(reply, NULL, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
    geis_error("error %s: %s", dbus_message_get_error_name(reply), s);
  }
  else
  {
    DBusMessage *msg;
    DBusPendingCall *pending;
    DBusError error = DBUS_ERROR_INIT;
    dbus_int32_t client_sub_id;
    dbus_int32_t server_sub_id;
    GeisSubscription subscription;

    dbus_message_get_args(reply,
                          &error,
                          DBUS_TYPE_INT32, &client_sub_id,
                          DBUS_TYPE_INT32, &server_sub_id,
                          DBUS_TYPE_INVALID);
    if (dbus_error_is_set(&error))
    {
      geis_error("error %s: %s", error.name, error.message);
      dbus_error_free(&error);
    }

    subscription = geis_subscription_bag_find(client->subscription_bag,
                                              client_sub_id);
    if (!subscription)
    {
      geis_error("invalid client subcription id %d returned from server",
                 client_sub_id);
    }
    else
    {
      geis_subscription_set_pdata(subscription, (GeisPointer)(intptr_t)server_sub_id);

      msg = geis_dbus_subscription_activate_call_message(subscription);
      dbus_connection_send_with_reply(client->connection, msg, &pending, -1);
      dbus_message_unref(msg);
      if (!pending)
      {
        geis_error("error sending DBus CreateSubscription method call");
      }
      else
      {
        dbus_pending_call_set_notify(pending,
                                     _geis_dbus_client_activate_reply,
                                     client, 0);
      }
    }
  }

  dbus_message_unref(reply);
  dbus_pending_call_unref(pending);
}


/**
 * Processes a deactivate-subscription reply from the server.
 *
 * @param[in] pending    A DBusPendingCall object.
 * @param[in] user_data  The %GeisDBusClient object.
 */
static void
_geis_dbus_client_unsubscribe_reply(DBusPendingCall *pending, void *user_data)
{
  GeisDBusClient client GEIS_UNUSED = (GeisDBusClient)user_data;
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);

  if (DBUS_MESSAGE_TYPE_ERROR == dbus_message_get_type(reply))
  {
    const char *s = NULL;
    dbus_message_get_args(reply, NULL, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
    geis_error("error %s: %s", dbus_message_get_error_name(reply), s);
  }
  else
  {
    geis_warning("signature=\"%s\"", dbus_message_get_signature(reply));
    geis_warning("path=\"%s\"", dbus_message_get_path(reply));
    geis_warning("interface=\"%s\"", dbus_message_get_interface(reply));
    geis_warning("member=\"%s\"", dbus_message_get_member(reply));
  }

  dbus_message_unref(reply);
  dbus_pending_call_unref(pending);
}


/**
 * Creates a remote subscription.
 */
void
_dbus_client_subscribe(GeisDBusClient   client,
                       GeisSubscription subscription)
{
  DBusPendingCall *pending_return;

  GeisSubscription sub = geis_subscription_bag_find(client->subscription_bag,
                                 geis_subscription_id(subscription));
  if (sub && geis_subscription_pdata(sub))
  {
    geis_warning("subscription already activated!");
  }
  else
  {
    DBusMessage *msg = geis_dbus_subscription_create_call_message(subscription);
    dbus_connection_send_with_reply(client->connection, msg, &pending_return, -1);
    dbus_message_unref(msg);
    if (!pending_return)
    {
      geis_error("error sending DBus CreateSubscription method call");
    }
    else
    {
      dbus_pending_call_set_notify(pending_return,
                                   _geis_dbus_client_subscribe_reply,
                                   client, 0);
    }
  }
}


/**
 * Re-subscribes all existing sibscriptions when the server appears or
 * reappears.
 */
void
_dbus_client_resubscribe_all(GeisDBusClient client)
{
  GeisSubBagIterator it;
  for (it = geis_subscription_bag_begin(client->subscription_bag);
       it != geis_subscription_bag_end(client->subscription_bag);
       it = geis_subscription_bag_iterator_next(client->subscription_bag, it))
  {
    geis_subscription_set_pdata(*it, 0);
    _dbus_client_subscribe(client, *it);
  }
}


/**
 * The DBus message dispatch function for the GEIS DBus client.
 *
 * @param[in] connection  The %GeisDBusClient DBus connection.
 * @param[in] message     The DBus message received.
 * @param[in] user_data   The %GeisDBusClient.
 */
static DBusHandlerResult
_geis_dbus_client_message_handler(DBusConnection *connection GEIS_UNUSED,
                                  DBusMessage    *message,
                                  void           *user_data)
{
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  GeisDBusClient client = (GeisDBusClient)user_data;
  int type = dbus_message_get_type(message);

  if (dbus_message_is_signal(message,
                             DBUS_INTERFACE_LOCAL,
                             "Disconnected"))
  {
    geis_warning("server disconnected?");
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (dbus_message_is_signal(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_DEVICE_AVAILABLE))
  {
    _client_device_available(client, message);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (dbus_message_is_signal(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_DEVICE_UNAVAILABLE))

  {
    _client_device_unavailable(client, message);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (dbus_message_is_signal(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_CLASS_AVAILABLE))
  {
    _client_class_available(client, message);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (dbus_message_is_signal(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_REGION_AVAILABLE))
  {
    _client_region_available(client, message);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (dbus_message_is_signal(message,
                                  GEIS_DBUS_SERVICE_INTERFACE,
                                  GEIS_DBUS_INIT_COMPLETE))
  {
    if (client->state == GEIS_DBUS_CLIENT_INITIALIZING)
    {
      geis_post_event(client->geis, geis_event_new(GEIS_EVENT_INIT_COMPLETE));
    }
    client->state = GEIS_DBUS_CLIENT_CONNECTED;
    _dbus_client_resubscribe_all(client);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (geis_dbus_message_is_gesture_event(message))
  {
    _client_gesture_event(client, message);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (type == DBUS_MESSAGE_TYPE_ERROR)
  {
    const char *str = NULL;
    dbus_message_get_args(message, NULL,
                          DBUS_TYPE_STRING, &str,
                          DBUS_TYPE_INVALID);
    geis_warning("error %s: %s", dbus_message_get_error_name(message), str);
  }
  else 
  {
    geis_warning("unhandled DBus %s received:",
                 dbus_message_type_to_string(dbus_message_get_type(message)));
    geis_warning("  signature=\"%s\"", dbus_message_get_signature(message));
    geis_warning("  sender=\"%s\"", dbus_message_get_sender(message));
    geis_warning("  path=\"%s\"",
                 dbus_message_get_path(message) ?
                 dbus_message_get_path(message) :
                 "(no path)");
    geis_warning("  interface=\"%s\"",
                 dbus_message_get_interface(message) ?
                 dbus_message_get_interface(message) :
                 "(no interface)");
    geis_warning("  member=\"%s\"",
                 dbus_message_get_member(message) ?
                 dbus_message_get_member(message) :
                 "(no member)");
  }
  return result;
}


/**
 * Adds the client watches to the dispatcher watch list.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static dbus_bool_t
_client_add_watch(DBusWatch *watch, void *data)
{
  dbus_bool_t status = TRUE;
  GeisDBusClient client = (GeisDBusClient)data;

  geis_dbus_dispatcher_register(client->dispatcher, client->connection, watch);
  return status;
}


/**
 * Toggles the enabled/disabled status of the client watches.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static void
_client_toggle_watch(DBusWatch *watch, void *data)
{
  GeisDBusClient client = (GeisDBusClient)data;

  geis_dbus_dispatcher_toggle_watch(client->dispatcher, watch);
}


/**
 * Removes the client watches from the dispatcher watch list.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static void
_client_remove_watch(DBusWatch *watch, void *data)
{
  GeisDBusClient client = (GeisDBusClient)data;

  geis_dbus_dispatcher_unregister(client->dispatcher, watch);
}


/**
 * Connects to the GEIS server once an address is located.
 *
 * @param[in] client  A %GeisDBusClient object.
 * @param[in] address The address of the server.
 */
static void
_client_connect(GeisDBusClient client, const char *address)
{
  geis_debug("server address=\"%s\"", address);
  DBusError error = DBUS_ERROR_INIT;
  client->connection = dbus_connection_open(address, &error);
  if (!client->connection || dbus_error_is_set(&error))
  {
    char msg[512];
    snprintf(msg, sizeof(msg), "error %s connecting to server at address %s: %s",
            error.name, address, error.message);
    geis_error("%s", msg);
    dbus_error_free(&error);
    goto final_exit;
  }

  /* Integrate with the app event loop via the GEIS multiplexor. */
  dbus_connection_set_watch_functions(client->connection,
                                      _client_add_watch,
                                      _client_remove_watch,
                                      _client_toggle_watch,
                                      client, 0);

  /* Install a handler for any and all messages. */
  dbus_connection_add_filter(client->connection,
                             _geis_dbus_client_message_handler,
                             client, 0);
  if (client->state != GEIS_DBUS_CLIENT_INITIALIZING)
  {
    client->state = GEIS_DBUS_CLIENT_CONNECTING;
  }

final_exit:
  return;
}


/*
 * Creates a new GeisDBusClient.
 */
GeisDBusClient
geis_dbus_client_new(Geis geis)
{
  GeisDBusClient client = calloc(1, sizeof(struct GeisDBusClient));
  if (!client)
  {
    goto final_exit;
  }

  client->geis = geis;
  client->state = GEIS_DBUS_CLIENT_INITIALIZING;

  client->dispatcher = geis_dbus_dispatcher_new(geis);
  if (!client->dispatcher)
  {
    goto unwind_client;
  }

  client->locator = geis_dbus_locator_new(client);
  if (!client->locator)
  {
    goto unwind_dispatcher;
  }

  client->subscription_bag = geis_subscription_bag_new(1);
  if (!client->subscription_bag)
  {
    goto unwind_locator;
  }

  goto final_exit;

unwind_locator:
  geis_dbus_locator_delete(client->locator);
unwind_dispatcher:
  geis_dbus_dispatcher_delete(client->dispatcher);
unwind_client:
  free(client);
  client = NULL;
final_exit:
  return client;
}


/*
 * Destroys a GeisDBusClient.
 */
void
geis_dbus_client_delete(GeisDBusClient client)
{
  geis_subscription_bag_delete(client->subscription_bag);
  geis_dbus_locator_delete(client->locator);
  if (client->connection)
  {
    dbus_connection_unref(client->connection);
  }
  geis_dbus_dispatcher_delete(client->dispatcher);
  free(client);
}


/*
 * Gets the client dispatcher.
 */
GeisDBusDispatcher
geis_dbus_client_dispatcher(GeisDBusClient client)
{
  return client->dispatcher;
}


/*
 * Signals the client the server has been located.
 */
void
geis_dbus_client_server_located(GeisDBusClient client)
{
  _client_connect(client, geis_dbus_locator_server_address(client->locator));
}


/*
 * Signals the client the server has been dislocated.
 */
void
geis_dbus_client_server_dislocated(GeisDBusClient client)
{
  GeisEvent event =  geis_event_new(GEIS_EVENT_ERROR);
  client->state = GEIS_DBUS_CLIENT_DISCONNECTED;
  geis_post_event(client->geis, event);
}


/*
 * Requests a subscription on the remote end.
 */
GeisStatus
geis_dbus_client_subscribe(GeisDBusClient   client,
                           GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;

  if (client->state == GEIS_DBUS_CLIENT_CONNECTED)
  {
    _dbus_client_subscribe(client, subscription);
  }
  geis_subscription_bag_insert(client->subscription_bag, subscription);

  return status;
}


/*
 * Destroys a subscription on the remote end.
 */
GeisStatus
geis_dbus_client_unsubscribe(GeisDBusClient   client,
                             GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (geis_subscription_bag_find(client->subscription_bag,
                                 geis_subscription_id(subscription)))
  {
    DBusMessage *msg;
    DBusPendingCall *pending_return;

    msg = geis_dbus_subscription_destroy_call_message(subscription);
    dbus_connection_send_with_reply(client->connection, msg, &pending_return, -1);
    dbus_message_unref(msg);
    if (!pending_return)
    {
      geis_error("error sending DBus CreateSubscription method call");
      goto final_exit;
    }

    dbus_pending_call_set_notify(pending_return,
                                 _geis_dbus_client_unsubscribe_reply,
                                 client, 0);
    geis_subscription_bag_remove(client->subscription_bag, subscription);
    status = GEIS_STATUS_SUCCESS;
  }

final_exit:
  return status;
}


/*
 * Asks the remote server to accept a gesture.
 */
GeisStatus
geis_dbus_client_accept_gesture(GeisDBusClient client GEIS_UNUSED,
                                GeisGroup      group GEIS_UNUSED,
                                GeisGestureId  gesture_id GEIS_UNUSED)
{
  return GEIS_STATUS_SUCCESS;
}


/*
 * Asks the remote server to reject a gesture.
 */
GeisStatus
geis_dbus_client_reject_gesture(GeisDBusClient client GEIS_UNUSED,
                                GeisGroup      group GEIS_UNUSED,
                                GeisGestureId  gesture_id GEIS_UNUSED)
{
  return GEIS_STATUS_SUCCESS;
}




