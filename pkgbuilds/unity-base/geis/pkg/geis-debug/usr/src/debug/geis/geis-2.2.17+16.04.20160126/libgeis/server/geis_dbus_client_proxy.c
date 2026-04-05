/**
 * @file geis_dbus_client_proxy.c
 * @brief Implementation of the GEIS DBus client proxy.
 *
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
#include "geis_dbus_client_proxy.h"

#include "geis_dbus.h"
#include "geis_dbus_class.h"
#include "geis_dbus_device.h"
#include "geis_dbus_gesture_event.h"
#include "geis_dbus_region.h"
#include "geis_dbus_subscription.h"
#include "geis_device.h"
#include "geis_filter.h"
#include "geis_logging.h"
#include "geis_private.h"
#include <stdlib.h>


struct GeisDBusClientProxy
{
  GeisDBusServer  server;
  DBusConnection *connection;
  GeisSubBag      subscriptions;
};


/**
 * Allocates a client proxy from a memory pool.
 *
 * This is a customization point for future refinement of object allocation.
 *
 * @returns an allocated GeisDBusClientProxy object or NULL on allocation
 * failure.
 */
GeisDBusClientProxy
_client_proxy_allocate()
{
  GeisDBusClientProxy proxy = calloc(1, sizeof(struct GeisDBusClientProxy));
  return proxy;
}


/**
 * Deallocates a client proxy object, returning its memory to a pool.
 *
 * @param[in] proxy The %GeisDBusClientProxy to deallocate.
 *
 * This is a customization point for future refinement of object allocation.
 */
void
_client_proxy_deallocate(GeisDBusClientProxy proxy)
{
  free(proxy);
}


/**
 * Handles subscription creation requests.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 * @param[in] message  The DBus message from the proxied client.
 */
static DBusMessage *
_client_proxy_subscribe(GeisDBusClientProxy proxy, DBusMessage *message)
{
  Geis geis = geis_dbus_server_geis(proxy->server);
  GeisSubscription sub;
  DBusMessage *reply;

  sub = geis_dbus_subscription_from_create_call_message(geis, message);
  if (!sub)
  {
    reply = dbus_message_new_error(message,
                                   GEIS_DBUS_ERROR_SUBSCRIPTION_FAIL,
                                   "error creating proxy from DBus message");
    goto final_exit;
  }

  geis_subscription_bag_insert(proxy->subscriptions, sub);

  if (GEIS_STATUS_SUCCESS != geis_subscription_activate(sub))
  {
    reply = dbus_message_new_error(message,
                                   GEIS_DBUS_ERROR_SUBSCRIPTION_FAIL,
                                   "error activating proxy subscription");
    goto final_exit;
  }

  reply = geis_dbus_subscription_create_return_message(message, sub);

final_exit:
  return reply;
}


/**
 * Handles subscription activation requests.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 * @param[in] message  The DBus message from the proxied client.
 *
 * @returns a DBus reply message to send.
 *
 * @todo Implement subscription retrieval.
 */
static DBusMessage *
_client_proxy_activate(GeisDBusClientProxy  proxy GEIS_UNUSED,
                       DBusMessage         *message)
{
  GeisSubscription sub = NULL;
  DBusMessage *reply;

  reply = geis_dbus_subscription_activate_return_message(message, sub);
  return reply;
}


/**
 * Handles subscription deactivation requests.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 * @param[in] message  The DBus message from the proxied client.
 *
 * @returns a DBus reply message to send.
 *
 * @todo Implement subscription retrieval.
 */
static DBusMessage *
_client_proxy_deactivate(GeisDBusClientProxy  proxy GEIS_UNUSED,
                         DBusMessage         *message)
{
  GeisSubscription sub = NULL;
  DBusMessage *reply;

  reply = geis_dbus_subscription_deactivate_return_message(message, sub);
  return reply;
}


/**
 * Handles subscription destroy requests.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 * @param[in] message  The DBus message from the proxied client.
 *
 * @returns a DBus reply message to send.
 *
 * @todo Implement this function.
 */
static DBusMessage *
_client_proxy_unsubscribe(GeisDBusClientProxy  proxy GEIS_UNUSED,
                          DBusMessage         *message)
{
  dbus_int32_t server_sub_id;
  dbus_message_get_args(message, NULL,
                        DBUS_TYPE_INT32 , &server_sub_id,
                        DBUS_TYPE_INVALID);
  GeisSubscription sub = geis_subscription_bag_find(proxy->subscriptions,
                                                    server_sub_id);
  geis_subscription_deactivate(sub);
  geis_subscription_bag_remove(proxy->subscriptions, sub);
  geis_subscription_delete(sub);

  DBusMessage *reply = geis_dbus_subscription_destroy_return_message(message);
  return reply;
}


/**
 * The DBus message dispatch function for the client proxy.
 *
 * @param[in] connection  The %GeisDBusClientProxy DBus connection.
 * @param[in] message     The DBus message received.
 * @param[in] user_data   The %GeisDBusClientProxy.
 */
static DBusHandlerResult
_client_proxy_message_handler(DBusConnection *connection,
                              DBusMessage    *message,
                              void           *user_data)
{
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  GeisDBusClientProxy proxy = (GeisDBusClientProxy)user_data;

  if (dbus_message_is_signal(message,
                             DBUS_INTERFACE_LOCAL,
                             "Disconnected"))
  {
    geis_dbus_server_client_disconnect(proxy->server, proxy);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (geis_dbus_message_is_subscription_create_call(message))
  {
    DBusMessage *reply = _client_proxy_subscribe(proxy, message);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (geis_dbus_message_is_subscription_activate_call(message))
  {
    DBusMessage *reply = _client_proxy_activate(proxy, message);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (geis_dbus_message_is_subscription_deactivate_call(message))
  {
    DBusMessage *reply = _client_proxy_deactivate(proxy, message);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (geis_dbus_message_is_subscription_destroy_call(message))
  {
    DBusMessage *reply = _client_proxy_unsubscribe(proxy, message);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
    result = DBUS_HANDLER_RESULT_HANDLED;
  }
  else if (DBUS_MESSAGE_TYPE_ERROR == dbus_message_get_type(message))
  {
    const char *s = NULL;
    dbus_message_get_args(message, NULL,
                          DBUS_TYPE_STRING, &s,
                          DBUS_TYPE_INVALID);
    geis_error("error %s: %s", dbus_message_get_error_name(message), s);
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
 * Reports all currently-known gesture-capable input devices to the proxied
 * client.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 */
static void
_client_proxy_report_devices(GeisDBusClientProxy proxy)
{
  GeisDeviceBag devices = geis_devices(geis_dbus_server_geis(proxy->server));
  for (GeisSize i = 0; i < geis_device_bag_count(devices); ++i)
  {
    GeisDevice device = geis_device_bag_device(devices, i);
    DBusMessage *message = geis_dbus_device_available_message_from_device(device);
    dbus_connection_send(proxy->connection, message, NULL);
    dbus_message_unref(message);
  }
}


/**
 * Reports all currently-known gesture classes to the proxied client.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 */
static void
_client_proxy_report_classes(GeisDBusClientProxy proxy)
{
  GeisGestureClassBag classes;

  classes = geis_gesture_classes(geis_dbus_server_geis(proxy->server));
  for (GeisSize i = 0; i < geis_gesture_class_bag_count(classes); ++i)
  {
    GeisGestureClass gesture_class;
    DBusMessage *message;

    gesture_class = geis_gesture_class_bag_gesture_class(classes, i);
    message = geis_dbus_class_available_message_from_class(gesture_class);
    dbus_connection_send(proxy->connection, message, NULL);
    dbus_message_unref(message);
  }
}


/**
 * Reports all currently-known regions to the proxied client.
 *
 * @param[in] proxy    A %GeisDBusClientProxy.
 *
 * OK, in reality, this just sends the valid and available filterable region
 * attributes.  Go wild.
 */
static void
_client_proxy_report_regions(GeisDBusClientProxy proxy GEIS_UNUSED)
{
  Geis geis = geis_dbus_server_geis(proxy->server);
  GeisFilterableAttributeBagIter it = geis_region_filter_attrs_begin(geis);
  while (it != geis_region_filter_attrs_end(geis))
  {
    DBusMessage *message = geis_dbus_region_available_message_from_region(&*it);
    dbus_connection_send(proxy->connection, message, NULL);
    dbus_message_unref(message);
    it = geis_region_filter_attrs_next(geis, it);
  }
}


/**
 * Adds the client proxy watches to the dispatcher watch list.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static dbus_bool_t
_client_proxy_add_watch(DBusWatch *watch, void *data)
{
  dbus_bool_t status = TRUE;
  GeisDBusClientProxy proxy = (GeisDBusClientProxy)data;

  geis_dbus_dispatcher_register(geis_dbus_server_dispatcher(proxy->server),
                                proxy->connection,
                                watch);
  return status;
}


/**
 * Toggles the enabled/disabled status of the client proxy watches.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static void
_client_proxy_toggle_watch(DBusWatch *watch, void *data)
{
  GeisDBusClientProxy proxy = (GeisDBusClientProxy)data;

  geis_dbus_dispatcher_toggle_watch(geis_dbus_server_dispatcher(proxy->server),
                                    watch);
}


/**
 * Removes the client proxy watches from the dispatcher watch list.
 *
 * @param[in] watch  A %DBusWatch.
 * @param[in] data   The %GeisDBusClientProxy.
 */
static void
_client_proxy_remove_watch(DBusWatch *watch, void *data)
{
  GeisDBusClientProxy proxy = (GeisDBusClientProxy)data;

  geis_dbus_dispatcher_unregister(geis_dbus_server_dispatcher(proxy->server),
                                  watch);
}


/*
 * Creates a new %GeisDBusClientProxy object.
 */
GeisDBusClientProxy
geis_dbus_client_proxy_new(GeisDBusServer server, DBusConnection *connection)
{
  GeisDBusClientProxy proxy = _client_proxy_allocate();
  if (!proxy)
  {
    geis_error("error allocating client proxy");
    goto final_exit;
  }

  proxy->server = server;
  proxy->connection = dbus_connection_ref(connection);
  proxy->subscriptions = geis_subscription_bag_new(2);
  if (!proxy->subscriptions)
  {
    dbus_connection_unref(proxy->connection);
    goto final_exit;
  }

  /* Don't shut down the app on disconnect, that would be unmutual. */
  dbus_connection_set_exit_on_disconnect(proxy->connection, FALSE);
  
  /* Integrate with the app event loop via the GEIS multiplexor. */
  dbus_connection_set_watch_functions(proxy->connection,
                                      _client_proxy_add_watch,
                                      _client_proxy_remove_watch,
                                      _client_proxy_toggle_watch,
                                      proxy, 0);

  /* Install a handler for any and all messages. */
  dbus_connection_add_filter(proxy->connection,
                             _client_proxy_message_handler,
                             proxy, 0);

  /* Kick off reporting devices and classes. */
  _client_proxy_report_devices(proxy);
  _client_proxy_report_classes(proxy);
  _client_proxy_report_regions(proxy);

  /* Tell the remote end it's a go. */
  DBusMessage *message = dbus_message_new_signal(GEIS_DBUS_SERVICE_PATH,
                                                 GEIS_DBUS_SERVICE_INTERFACE,
                                                 GEIS_DBUS_INIT_COMPLETE);
  dbus_connection_send(proxy->connection, message, NULL);
  dbus_message_unref(message);

final_exit:
  return proxy;
}


/*
 * Destroys a %GeisDBusClientProxy.
 */
void
geis_dbus_client_proxy_delete(GeisDBusClientProxy proxy)
{
  geis_subscription_bag_delete(proxy->subscriptions);
  dbus_connection_unref(proxy->connection);
  _client_proxy_deallocate(proxy);
}


/*
 * Handles a GEIS event.
 *
 * GEIS events are generally gesture events.  Don;t be fooled, though, there may
 * be others and this function is incomplete.
 *
 * @todo Refine this function to handle non-gesture events.
 */
void
geis_dbus_client_proxy_handle_geis_event(GeisDBusClientProxy proxy,
                                         GeisEvent           event)
{
  for (GeisSubBagIterator sit = geis_subscription_bag_begin(proxy->subscriptions);
       sit != geis_subscription_bag_end(proxy->subscriptions);
       sit = geis_subscription_bag_iterator_next(proxy->subscriptions, sit))
  {
    for (GeisFilterIterator fit = geis_subscription_filter_begin(*sit);
         fit != geis_subscription_filter_end(*sit);
         fit = geis_subscription_filter_next(*sit, fit))
    {
      if (geis_filter_pass_event(*fit, event))
      {
        DBusMessage *message;

        message = geis_dbus_gesture_event_message_from_geis_event(event);
        dbus_connection_send(proxy->connection, message, NULL);
        dbus_message_unref(message);
      }
    }
  }
}


