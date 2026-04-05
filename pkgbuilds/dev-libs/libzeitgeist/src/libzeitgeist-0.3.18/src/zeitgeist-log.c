/*
 * Copyright (C) 2010-2012 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by
 *             Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 *             Michal Hruby <michal.hruby@canonical.com>
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-log.h"
#include "zeitgeist-simple-result-set.h"
#include "org.gnome.zeitgeist.Monitor-xml.h"

/**
 * SECTION:zeitgeist-log
 * @short_description: Primary access point for talking to the Zeitgeist daemon
 * @include: zeitgeist.h
 *
 * #ZeitgeistLog encapsulates the low level access to the Zeitgeist daemon.
 * You can use it to manage the log by inserting and deleting entries as well
 * as do queries on the logged data.
 *
 * It's important to realize that the #ZeitgeistLog class does not expose
 * any API that does synchronous communications with the message bus - 
 * everything is asynchronous. To ease development some of the methods have
 * variants that are "fire and forget" ignoring the normal return value, so
 * that callbacks does not have to be set up.
 */

G_DEFINE_TYPE (ZeitgeistLog, zeitgeist_log, G_TYPE_OBJECT);
#define ZEITGEIST_LOG_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_LOG, ZeitgeistLogPrivate))

typedef struct
{
  /* Our connection to the bus */
  GDBusConnection *connection;

  /* The connection to the ZG daemon.
   * If log != NULL it means we have a connection */
  GDBusProxy *log;

  /* In case auto-launching the ZG daemon failed, this
   * variable will hold the error. */
  GError *log_error;

  /* Hash set of ZeitgeistMonitors we've installed.
   * We store a map of (monitor, registration_id)  */
  GHashTable *monitors;

  /* Method calls queued up while waiting for a proxy  */
  GSList *method_dispatch_queue;

  /* Are we connected to Zeitgeist? */
  gboolean is_connected;

  /* Cached variant with daemon version info */
  GVariant *engine_version;
} ZeitgeistLogPrivate;

/* Property ids */
enum
{
	PROP_0,

	PROP_CONNECTED,

	LAST_PROPERTY
};

typedef struct
{
  ZeitgeistLog        *self;
  const gchar         *method_name;
  GVariant            *params;
  GCancellable        *cancellable;
  GAsyncReadyCallback  cb;
  gpointer             user_data;
} MethodDispatchContext;

static void    _zeitgeist_log_on_name_owner_changed (GObject *proxy,
                                                     GParamSpec *pspec,
                                                     gpointer user_data);

static void    _zeitgeist_log_on_zg_proxy_acquired (GObject *source_object,
                                                    GAsyncResult *res,
                                                    gpointer user_data);

static void    _zeitgeist_log_install_monitor          (ZeitgeistLog        *self,
                                                        ZeitgeistMonitor    *monitor);

static void    _zeitgeist_log_on_monitor_destroyed     (ZeitgeistLog     *self,
                                                        ZeitgeistMonitor *monitor);

static void    _zeitgeist_monitor_weak_unref           (ZeitgeistMonitor *monitor,
                                                        gpointer          _ignored,
                                                        ZeitgeistLog     *log);

static void    dispatch_method         (MethodDispatchContext *ctx);

static void    dispatch_async_callback (GObject               *source_object,
                                        GAsyncResult          *res,
                                        gpointer               user_data);

static ZeitgeistLog *zeitgeist_log_singleton = NULL;

static void
zeitgeist_log_init (ZeitgeistLog *self)
{
  ZeitgeistLogPrivate *priv;

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);
  priv->log = NULL;
  priv->log_error = NULL;

  /* Reset hash set of monitors */
  priv->monitors = g_hash_table_new (g_direct_hash, g_direct_equal);

  /* Set up the connection to the ZG daemon */
  g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION,
                            G_DBUS_PROXY_FLAGS_NONE,
                            NULL,
                            "org.gnome.zeitgeist.Engine",
                            "/org/gnome/zeitgeist/log/activity",
                            "org.gnome.zeitgeist.Log",
                            NULL,
                            _zeitgeist_log_on_zg_proxy_acquired,
                            g_object_ref (self));
}

static void
zeitgeist_log_finalize (GObject *object)
{
  ZeitgeistLog *log = ZEITGEIST_LOG (object);
  ZeitgeistLogPrivate *priv;
  
  priv = ZEITGEIST_LOG_GET_PRIVATE (log);

  if (priv->connection != NULL)
    {
      g_object_unref (priv->connection);
      priv->connection = NULL;
    }

  if (priv->log)
    {
      g_object_unref (priv->log);
      priv->log = NULL;
    }
  
  /* Out list of monitors only holds weak refs to the monitors */
  if (priv->monitors)
    {
      g_hash_table_foreach (priv->monitors,
                            (GHFunc) _zeitgeist_monitor_weak_unref,
                            log);
      g_hash_table_unref (priv->monitors);
    }

  if (priv->engine_version)
    {
      g_variant_unref (priv->engine_version);
    }

  if (priv->log_error)
    {
      g_error_free (priv->log_error);
    }
  
  G_OBJECT_CLASS (zeitgeist_log_parent_class)->finalize (object); 
}

static void
zeitgeist_log_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ZeitgeistLogPrivate *priv = ZEITGEIST_LOG_GET_PRIVATE (object);
  gchar *name_owner;

  switch (prop_id)
    {
      case PROP_CONNECTED:
        g_value_set_boolean (value, priv->is_connected);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}

static void
zeitgeist_log_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  ZeitgeistLogPrivate *priv = ZEITGEIST_LOG_GET_PRIVATE (object);

  switch (prop_id)
    {
      case PROP_CONNECTED:
        g_warning ("Can not set read-only property ZeitgeistLog:connected");
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}


static void
zeitgeist_log_class_init (ZeitgeistLogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec   *pspec;
  
  object_class->finalize     = zeitgeist_log_finalize;
  object_class->get_property = zeitgeist_log_get_property;
  object_class->set_property = zeitgeist_log_set_property;

  /**
   * ZeitgeistLog:connected:
   *
   * Determines if this Log instance is currently connected to Zeitgeist
   * daemon.
   */
  pspec = g_param_spec_boolean ("connected",
                                "Connected",
                                "Whether this instance is connected to Zeitgeist",
                                FALSE,
                                G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_CONNECTED, pspec);

  g_type_class_add_private (object_class, sizeof (ZeitgeistLogPrivate));
}

/* Send off the DBus method call, or queue it if we don't
 * have a proxy at this point */
static void
dispatch_method (MethodDispatchContext *ctx)
{
  ZeitgeistLogPrivate *priv;
  GSimpleAsyncResult *async_result;

  priv = ZEITGEIST_LOG_GET_PRIVATE (ctx->self);

  if (priv->log)
    {
      g_dbus_proxy_call (priv->log,
                         ctx->method_name,
                         ctx->params,
                         G_DBUS_CALL_FLAGS_NONE,
                         -1,
                         ctx->cancellable,
                         dispatch_async_callback,
                         ctx);
    }
  else if (priv->log_error)
    {
      // Zeitgeist couldn't be auto-started. We'll run the callback
      // anyway and give it an error.
      if (ctx->cb != NULL)
        {
          async_result = g_simple_async_result_new (G_OBJECT (ctx->self),
                                                    ctx->cb,
                                                    ctx->user_data,
                                                    NULL);
          g_simple_async_result_set_check_cancellable (async_result,
                                                       ctx->cancellable);
          g_simple_async_result_set_from_error (async_result, priv->log_error);
          g_simple_async_result_complete_in_idle (async_result);
          g_object_unref (async_result);
        }

      g_object_unref (ctx->self);
      g_free (ctx);
    }
  else
    {
      // Queue the request while we wait for Zeitgeist to show up
      priv->method_dispatch_queue = g_slist_prepend (priv->method_dispatch_queue,
                                                     ctx);
    }
}

/* Used to marshal the async callbacks from GDBus into ones
 * coming from this ZeitgeistLog instance */
static void
dispatch_async_callback (GObject      *source_object,
                         GAsyncResult *res,
                         gpointer      user_data)
{
  GVariant              *var;
  MethodDispatchContext *ctx = (MethodDispatchContext*) user_data;

  if (ctx->cb != NULL)
      ctx->cb (G_OBJECT (ctx->self), res, ctx->user_data);
  else
    {
      /* Caller ignores response - finish the call our selves */
      var = g_dbus_proxy_call_finish (G_DBUS_PROXY (source_object), res, NULL);
      if (var != NULL)
        g_variant_unref (var);
    }
  
  g_object_unref (ctx->self);
  g_free (ctx);
}

/*
 * API BELOW HERE
 */

/**
 * zeitgeist_log_new:
 * Create a new log that interfaces with the default event log of the Zeitgeist
 * daemon.
 *
 * The #ZeitgeistLog object will asynchronously start to connect to the
 * Zeitgeist daemon. Any request you send to via this log object will be queued
 * until the connection is ready - at which point you will get a notify signal
 * on the #ZeitgeistLog:connected property.
 *
 * It isn't recommended to call this function more than once. If you need to
 * access #ZeitgeistLog from different parts of your codebase, consider using
 * zeitgeist_log_get_default() instead.
 *
 * Returns: A reference to a newly allocated log.
 */
ZeitgeistLog*
zeitgeist_log_new (void)
{
  ZeitgeistLog        *log;

  log = (ZeitgeistLog*) g_object_new (ZEITGEIST_TYPE_LOG,
                                      NULL);

  return log;
}

/**
 * zeitgeist_log_get_default:
 * Get a unique instance of #ZeitgeistLog, that you can share in your
 * application without caring about memory management.
 *
 * See zeitgeist_log_new() for more information.
 *
 * Returns: (transfer none): A unique #ZeitgeistLog. Do not ref or unref it.
 *
 * Since: 0.3.14
 */
ZeitgeistLog*
zeitgeist_log_get_default (void)
{
  if (G_UNLIKELY (!zeitgeist_log_singleton))
    zeitgeist_log_singleton = zeitgeist_log_new ();

  return zeitgeist_log_singleton;
}

/**
 * zeitgeist_log_insert_events:
 * @self: The log logging the events
 * @cancellable: To cancel the operation or %NULL
 * @callback: #GAsyncReadyCallback to invoke once the logging operation has
 *            completed. Set to %NULL to ignore the result. In this callback
 *            you can invoke zeitgeist_log_insert_events_finish() to collect
 *            the event ids of the inserted events
 * @user_data: Any user data to pass back to @callback
 * @VarArgs: A list of #ZeitgeistEvent<!-- -->s terminated by a %NULL
 *
 * Asynchronously send a set of events to the Zeitgeist daemon, requesting they
 * be inserted into the log.
 */
void
zeitgeist_log_insert_events (ZeitgeistLog        *self,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data,
                             ...)
{
  va_list                events;

  va_start (events, user_data);
  zeitgeist_log_insert_events_valist (self, cancellable,
                                      callback, user_data, events);
  va_end (events);
}

/**
 * zeitgeist_log_insert_events_no_reply:
 * @self: The log logging the events
 * @VarArgs: A list of #ZeitgeistEvent<!-- -->s terminated by a %NULL
 *
 * Asynchronously send a set of events to the Zeitgeist daemon, requesting they
 * be inserted into the log. This method is &quot;fire and forget&quot; and the
 * caller will never know whether the events was successfully inserted or not.
 *
 * This method is exactly equivalent to calling zeitgeist_log_insert_events()
 * with %NULL set as @cancellable, @callback, and @user_data.
 */
void
zeitgeist_log_insert_events_no_reply (ZeitgeistLog *self,
                                      ...)
{
  va_list                events;

  va_start (events, self);
  zeitgeist_log_insert_events_valist (self, NULL, NULL, NULL, events);
  va_end (events);
}

/**
 * zeitgeist_log_insert_events_valist:
 * @self: The log logging the events
 * @cancellable: To cancel the operation or %NULL
 * @callback: #GAsyncReadyCallback to invoke once the logging operation has
 *            completed. Set to %NULL to ignore the result. In this callback
 *            you can invoke zeitgeist_log_insert_events_finish() to collect
 *            the event ids of the inserted events
 * @user_data: Any user data to pass back to @callback
 * @events: A #GPtrArray of #ZeitgeistEvent<!-- -->s to insert. This method
 *          steals the reference to @events and consumes all floating refs
 *          on the event members.
 *
 * This method is intended for language bindings. If calling this function
 * from C code it's generally more handy to use zeitgeist_log_insert_events()
 * or zeitgeist_log_insert_events_from_ptrarray().
 * 
 * Asynchronously send a set of events to the Zeitgeist daemon, requesting they
 * be inserted into the log.
 */
void
zeitgeist_log_insert_events_valist (ZeitgeistLog        *self,
                                    GCancellable        *cancellable,
                                    GAsyncReadyCallback  callback,
                                    gpointer             user_data,
                                    va_list              events)
{
  GPtrArray *_events;

  _events = zeitgeist_events_from_valist (events);
  zeitgeist_log_insert_events_from_ptrarray (self, _events, cancellable,
                                             callback, user_data);
}

/**
 * zeitgeist_log_insert_events_from_ptrarray:
 * @self: The log logging the events
 * @events: A #GPtrArray of #ZeitgeistEvent<!-- -->s to insert. This method
 *          steals the reference to @events and consumes all floating refs
 *          on the event members. It is assumed that the free_func on @events
 *          is set to g_object_unref().
 * @cancellable: To cancel the operation or %NULL
 * @callback: #GAsyncReadyCallback to invoke once the logging operation has
 *            completed. Set to %NULL to ignore the result. In this callback
 *            you can invoke zeitgeist_log_insert_events_finish() to collect
 *            the event ids of the inserted events
 * @user_data: Any user data to pass back to @callback
 *
 * Asynchronously send a set of events to the Zeitgeist daemon, requesting they
 * be inserted into the log.
 */
void
zeitgeist_log_insert_events_from_ptrarray (ZeitgeistLog        *self,
                                           GPtrArray           *events,
                                           GCancellable        *cancellable,
                                           GAsyncReadyCallback  callback,
                                           gpointer             user_data)
{
  ZeitgeistLogPrivate   *priv;
  GVariant              *vevents;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (events != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevents = zeitgeist_events_to_variant (events); // own ref
  vevents = g_variant_new_tuple (&vevents, 1); // own ref

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "InsertEvents";
  ctx->params = vevents;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

GArray*
zeitgeist_log_insert_events_finish (ZeitgeistLog        *self,
                                    GAsyncResult        *res,
                                    GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GArray              *event_ids;
  GVariant            *val, *_val;
  gsize                n_ids;
  const guint32       *raw_ids;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return NULL;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  _val = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  raw_ids = g_variant_get_fixed_array (_val, &n_ids, sizeof (guint32));
  event_ids = g_array_sized_new (FALSE, FALSE, sizeof (guint32), n_ids);
  g_array_append_vals (event_ids, raw_ids, n_ids);

  g_variant_unref (_val);

  return event_ids;
}

void
zeitgeist_log_find_events (ZeitgeistLog        *self,
                           ZeitgeistTimeRange  *time_range,
                           GPtrArray           *event_templates,
                           ZeitgeistStorageState storage_state,
                           guint32              num_events,
                           ZeitgeistResultType  result_type,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data)
{
  ZeitgeistLogPrivate   *priv;
  GVariant              *vevents, *vtime_range, *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range));
  g_return_if_fail (event_templates != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevents = zeitgeist_events_to_variant (event_templates);

  /* Owns floating ref on time_range */
  vtime_range = zeitgeist_time_range_to_variant (time_range);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("((xx)a(asaasay)uuu)"));
  g_variant_builder_add_value (&b, vtime_range); // owns ref
  g_variant_builder_add_value (&b, vevents); // owns ref
  g_variant_builder_add (&b, "u", storage_state);
  g_variant_builder_add (&b, "u", num_events);
  g_variant_builder_add (&b, "u", result_type);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "FindEvents";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

/**
 * zeitgeist_log_find_events_finish:
 * @self:
 * @res:
 * @error:
 *
 * Retrieve the result from an asynchronous query started with
 * zeitgeist_log_find_events().
 *
 * Returns: (transfer-full): A newly allocated #ZeitgeistResultSet containing
 *          the #ZeitgeistEvent<!-- -->s
 *          matching the query. You must free the result set with
 *          g_object_unref(). The events held in the result set will
 *          automatically be unreffed when the result set is finalized.
 */
ZeitgeistResultSet*
zeitgeist_log_find_events_finish (ZeitgeistLog        *self,
                                  GAsyncResult        *res,
                                  GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GPtrArray           *events;
  GVariant            *val, *_val;
  const guint32       *raw_ids;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return NULL;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  _val = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  events = zeitgeist_events_from_variant (_val);
  g_variant_unref (_val);

  return _zeitgeist_simple_result_set_new (events, events->len);
}

void
zeitgeist_log_find_event_ids (ZeitgeistLog        *self,
                              ZeitgeistTimeRange  *time_range,
                              GPtrArray           *event_templates,
                              ZeitgeistStorageState storage_state,
                              guint32              num_events,
                              ZeitgeistResultType  result_type,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  ZeitgeistLogPrivate   *priv;
  GVariant              *vevents, *vtime_range, *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range));
  g_return_if_fail (event_templates != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevents = zeitgeist_events_to_variant (event_templates);

  /* Owns floating ref on time_range */
  vtime_range = zeitgeist_time_range_to_variant (time_range);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("((xx)a(asaasay)uuu)"));
  g_variant_builder_add_value (&b, vtime_range); // owns ref
  g_variant_builder_add_value (&b, vevents); // owns ref
  g_variant_builder_add (&b, "u", storage_state);
  g_variant_builder_add (&b, "u", num_events);
  g_variant_builder_add (&b, "u", result_type);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "FindEventIds";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

GArray*
zeitgeist_log_find_event_ids_finish (ZeitgeistLog        *self,
                                     GAsyncResult        *res,
                                     GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GArray              *event_ids;
  GVariant            *val, *_val;
  gsize                n_ids;
  const guint32       *raw_ids;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return NULL;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  _val = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  raw_ids = g_variant_get_fixed_array (_val, &n_ids, sizeof (guint32));
  event_ids = g_array_sized_new (FALSE, FALSE, sizeof (guint32), n_ids);
  g_array_append_vals (event_ids, raw_ids, n_ids);

  g_variant_unref (_val);

  return event_ids;
}

void
zeitgeist_log_get_events (ZeitgeistLog        *self,
                          GArray              *event_ids,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  GVariant              *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;
  int                    i;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (event_ids != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("(au)"));
  g_variant_builder_open (&b, G_VARIANT_TYPE ("au"));
  for (i = 0; i < event_ids->len; i++)
    g_variant_builder_add (&b, "u", g_array_index (event_ids, guint32, i));
  g_variant_builder_close (&b);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "GetEvents";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

ZeitgeistResultSet*
zeitgeist_log_get_events_finish (ZeitgeistLog        *self,
                                 GAsyncResult        *res,
                                 GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GPtrArray           *events;
  GVariant            *val, *_val;
  const guint32       *raw_ids;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return NULL;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  _val = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  events = zeitgeist_events_from_variant (_val);
  g_variant_unref (_val);

  return _zeitgeist_simple_result_set_new (events, events->len);
}

/**
 * zeitgeist_log_get_version:
 * @self: A #ZeitgeistLog instance
 * @major: (out): Location for the major version
 * @minor: (out): Location for the minor version
 * @micro: (out): Location for the micro version
 * 
 * Gets version of currently running Zeitgeist daemon.
 *
 * This method will return the version of Zeitgeist daemon this instance is
 * connected to. If you call this method right after zeitgeist_log_new(),
 * only zeros will be returned, a valid version number will only be returned
 * once this instance successfully connected to the Zeitgeist daemon - ie
 * the value of the "connected" property must be TRUE (you can connect
 * to the "notify::connected" signal otherwise).
 */
void
zeitgeist_log_get_version (ZeitgeistLog *self,
                           gint *major,
                           gint *minor,
                           gint *micro)
{
  ZeitgeistLogPrivate *priv;
  gint maj, min, mic;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->engine_version && g_variant_is_of_type (priv->engine_version,
                                                    G_VARIANT_TYPE ("(iii)")))
  {
    g_variant_get (priv->engine_version, "(iii)", &maj, &min, &mic);
  }
  else
  {
    maj = 0;
    min = 0;
    mic = 0;
  }

  if (major) *major = maj;
  if (minor) *minor = min;
  if (micro) *micro = mic;
}

void
zeitgeist_log_delete_events (ZeitgeistLog        *self,
                             GArray              *event_ids,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  GVariant              *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;
  int                    i;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (event_ids != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("(au)"));
  g_variant_builder_open (&b, G_VARIANT_TYPE ("au"));
  for (i = 0; i < event_ids->len; i++)
    g_variant_builder_add (&b, "u", g_array_index (event_ids, guint32, i));
  g_variant_builder_close (&b);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "DeleteEvents";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

gboolean
zeitgeist_log_delete_events_finish (ZeitgeistLog        *self,
                                    GAsyncResult        *res,
                                    GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GPtrArray           *events;
  GVariant            *val;
  const guint32       *raw_ids;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return FALSE;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return FALSE;

  g_variant_unref (val);
  return TRUE;
}

void
zeitgeist_log_find_related_uris (ZeitgeistLog        *self,
                                 ZeitgeistTimeRange  *time_range,
                                 GPtrArray           *event_templates,
                                 GPtrArray           *result_event_templates,
                                 ZeitgeistStorageState storage_state,
                                 guint32              num_events,
                                 ZeitgeistResultType  result_type,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  ZeitgeistLogPrivate     *priv;
  GVariant              *vevent_templates, *vresult_event_templates;
  GVariant              *vtime_range, *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range));
  g_return_if_fail (event_templates != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevent_templates = zeitgeist_events_to_variant (event_templates);
  vresult_event_templates = zeitgeist_events_to_variant (result_event_templates);

  /* Owns floating ref on time_range */
  vtime_range = zeitgeist_time_range_to_variant (time_range);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("((xx)a(asaasay)a(asaasay)uuu)"));
  g_variant_builder_add_value (&b, vtime_range); // owns ref
  g_variant_builder_add_value (&b, vevent_templates); // owns ref
  g_variant_builder_add_value (&b, vresult_event_templates); // owns ref
  g_variant_builder_add (&b, "u", storage_state);
  g_variant_builder_add (&b, "u", num_events);
  g_variant_builder_add (&b, "u", result_type);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "FindRelatedUris";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

gchar**
zeitgeist_log_find_related_uris_finish (ZeitgeistLog    *self,
                                        GAsyncResult    *res,
                                        GError         **error)
{
  ZeitgeistLogPrivate *priv;
  GVariant            *val, *_val;
  const gchar        **uris;
  gchar              **result;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return NULL;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  _val = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  uris = g_variant_get_strv (_val, NULL);
  result = g_strdupv ((gchar**)uris);

  g_free (uris);
  g_variant_unref (_val);

  return result;
}

void
zeitgeist_log_delete_log (ZeitgeistLog        *self,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "DeleteLog";
  ctx->params = g_variant_new ("()");
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

gboolean
zeitgeist_log_delete_log_finish (ZeitgeistLog        *self,
                                 GAsyncResult        *res,
                                 GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GVariant            *val;
  gchar              **result;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return FALSE;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return FALSE;

  g_variant_unref (val);

  return TRUE;
}

void
zeitgeist_log_quit (ZeitgeistLog        *self,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "Quit";
  ctx->params = g_variant_new ("()");
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

gboolean
zeitgeist_log_quit_finish (ZeitgeistLog        *self,
                           GAsyncResult        *res,
                           GError             **error)
{
  ZeitgeistLogPrivate *priv;
  GVariant            *val;
  gchar              **result;

  g_return_val_if_fail (ZEITGEIST_IS_LOG (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log_error && g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
      return FALSE;

  val = g_dbus_proxy_call_finish (priv->log, res, error);

  if (val == NULL)
    return FALSE;

  g_variant_unref (val);

  return TRUE;
}

/* Async callback for when a proxy is ready
 * - triggered from zeitgeist_log_init */
static void
_zeitgeist_log_on_zg_proxy_acquired (GObject *source_object,
                                     GAsyncResult *res,
                                     gpointer user_data)
{
  ZeitgeistLog        *self;
  ZeitgeistLogPrivate *priv;
  GHashTableIter       iter;
  gpointer             monitor, dummy;
  GVariant            *version_property;
  GError              *error;

  self = ZEITGEIST_LOG (user_data);
  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  if (priv->log != NULL)
    {
      g_critical ("Internal error in libzeitgeist. Registered new connection,"
                  "but we already have one. Discarding the old and using the "
                  "new one");
      g_object_unref (priv->log);
      g_clear_error (&priv->log_error);
      priv->log = NULL;
    }

  error = NULL;
  priv->log = g_dbus_proxy_new_finish (res, &error);

  if (error != NULL)
    {
      priv->log_error = g_error_copy (error);
      g_critical ("Failed to create proxy for Zeitgeist daemon: %s",
                  error->message);
      goto process_pending_calls;
    }

  priv->connection = G_DBUS_CONNECTION (g_object_ref (
        g_dbus_proxy_get_connection (priv->log)));

  version_property = g_dbus_proxy_get_cached_property (priv->log, "version");
  if (version_property)
    {
      if (priv->engine_version) g_variant_unref (priv->engine_version);
      priv->engine_version = version_property; // steals the ref
    }

  g_signal_connect (priv->log, "notify::g-name-owner",
      G_CALLBACK (_zeitgeist_log_on_name_owner_changed), self);

  /* Reinstate all active monitors */
  g_hash_table_iter_init (&iter, priv->monitors);
  while (g_hash_table_iter_next (&iter, &monitor, &dummy))
    {
      _zeitgeist_log_install_monitor (self, ZEITGEIST_MONITOR (monitor));
    }

  priv->is_connected = TRUE;
  g_object_notify (G_OBJECT (self), "connected");

  process_pending_calls:

    /* Dispatch all queued method calls we got while we didn't have a proxy.
     * Note that dispatch_method() also frees all the queue members */
    priv->method_dispatch_queue = g_slist_reverse (priv->method_dispatch_queue);
    g_slist_foreach (priv->method_dispatch_queue, (GFunc) dispatch_method, NULL);
    g_slist_free (priv->method_dispatch_queue);
    priv->method_dispatch_queue = NULL;

  cleanup:
    g_object_unref (self);
}

/* Called when the Zeitgeist daemon leaves the bus */
static void
_zeitgeist_log_on_name_owner_changed (GObject *proxy,
                                      GParamSpec *pspec,
                                      gpointer user_data)
{
  ZeitgeistLog        *self;
  ZeitgeistLogPrivate *priv;
  gchar               *name_owner;
  gboolean             connected;
  GHashTableIter       iter;
  GVariant            *version_property;
  gpointer             monitor, dummy;

  self = ZEITGEIST_LOG (user_data);
  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  name_owner = g_dbus_proxy_get_name_owner (G_DBUS_PROXY (proxy));
  connected = name_owner != NULL;

  if (connected)
    {
      /* Reinstate all active monitors */
      g_hash_table_iter_init (&iter, priv->monitors);
      while (g_hash_table_iter_next (&iter, &monitor, &dummy))
        {
          _zeitgeist_log_install_monitor (self, ZEITGEIST_MONITOR (monitor));
        }

      /* Update our cached version property */
      version_property = g_dbus_proxy_get_cached_property (priv->log,
                                                           "version");
      if (version_property)
        {
          if (priv->engine_version) g_variant_unref (priv->engine_version);
          priv->engine_version = version_property; // steals the ref
        }
    }

  if (priv->is_connected ^ connected) /* XOR here! */
    {
      priv->is_connected = connected;
      g_object_notify (G_OBJECT (self), "connected");
    }

  if (name_owner) g_free (name_owner);
}

gboolean
zeitgeist_log_is_connected (ZeitgeistLog *self)
{
  ZeitgeistLogPrivate *priv;

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);
  return priv->is_connected;
}

/*
 * ZEITGEIST MONITOR STUFF BELOW HERE
 */

static void
monitor_handle_method_call (GDBusConnection       *connection,
                            const gchar           *sender,
                            const gchar           *object_path,
                            const gchar           *interface_name,
                            const gchar           *method_name,
                            GVariant              *parameters,
                            GDBusMethodInvocation *invocation,
                            gpointer               user_data)
{
  ZeitgeistMonitor   *mon;
  GVariant           *vtime_range, *vevents, *vids;
  GPtrArray          *aevents;
  ZeitgeistResultSet *events;
  ZeitgeistTimeRange *time_range;
  GArray             *event_ids;
  gsize               n_ids;
  gconstpointer       raw_ids;

  mon = ZEITGEIST_MONITOR (user_data);

  if (g_strcmp0 (method_name, "NotifyInsert") == 0)
    {
      /* parameters has signature '((xx)a(asaasay))' */
      vtime_range = g_variant_get_child_value (parameters, 0);
      time_range = zeitgeist_time_range_new_from_variant (vtime_range);
      vevents = g_variant_get_child_value (parameters, 1);
      aevents = zeitgeist_events_from_variant (vevents);
      events = _zeitgeist_simple_result_set_new (aevents, aevents->len);
      g_signal_emit_by_name (mon, "events-inserted", time_range, events);
      g_object_unref (time_range);
      g_object_unref (events);
      g_variant_unref (vtime_range);
      g_variant_unref (vevents);
      g_dbus_method_invocation_return_value (invocation, g_variant_new ("()"));
    }
  else if (g_strcmp0 (method_name, "NotifyDelete") == 0)
    {
      /* parameters has signature '((xx)au)' */
      vtime_range = g_variant_get_child_value (parameters, 0);
      time_range = zeitgeist_time_range_new_from_variant (vtime_range);
      vids = g_variant_get_child_value (parameters, 1);
      raw_ids = g_variant_get_fixed_array (vids, &n_ids, sizeof (guint32));
      event_ids = g_array_sized_new (FALSE, FALSE, sizeof (guint32), n_ids);
      g_array_append_vals (event_ids, raw_ids, n_ids);
      g_signal_emit_by_name (mon, "events-deleted", time_range, event_ids);
      g_object_unref (time_range);
      g_variant_unref (vtime_range);
      g_variant_unref (vids);
      g_array_unref (event_ids);
      g_dbus_method_invocation_return_value (invocation, g_variant_new ("()"));
    }
}

static const GDBusInterfaceVTable monitor_interface_vtable =
{
  monitor_handle_method_call,
  NULL,
  NULL
};

/* INVARIANT: This method is only called when priv->log != NULL */
/* INVARIANT: This method is only called when @monitor is in priv->monitors */
static void
_zeitgeist_log_install_monitor (ZeitgeistLog        *self,
                                ZeitgeistMonitor    *monitor)
{
  ZeitgeistLogPrivate   *priv;
  MethodDispatchContext *ctx;
  GDBusNodeInfo         *monitor_introspection_data;
  guint                  registration_id;
  GVariantBuilder        b;
  GVariant              *time_range, *event_templates, *params;

  /* Keep the parsed introspection data of the Monitor interface around */
  static GDBusInterfaceInfo *monitor_interface_info = NULL;

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Load Monitor introspection XML on first run */
  if (monitor_interface_info == NULL)
    {
      monitor_introspection_data = g_dbus_node_info_new_for_xml (
                                         org_gnome_zeitgeist_Monitor_xml, NULL);
      monitor_interface_info = g_dbus_node_info_lookup_interface (
                                                monitor_introspection_data,
                                                "org.gnome.zeitgeist.Monitor");
      g_assert (monitor_interface_info != NULL);

      g_dbus_interface_info_ref (monitor_interface_info);
      g_dbus_node_info_unref (monitor_introspection_data);
    }

  /* Check invariant priv->log != NULL */
  if (priv->log == NULL)
    {
      g_critical ("Internal error in libzeitgeist: Monitors should not be"
                  "published before when we don't have a connection");
      return;
    }

  /* Export the monitor, client side, on the session bus if it isn't already */
  registration_id = GPOINTER_TO_UINT (g_hash_table_lookup (priv->monitors, monitor));
  if (registration_id == 0)
    {
      registration_id = g_dbus_connection_register_object (priv->connection,
                                                           zeitgeist_monitor_get_path (monitor),
                                                           monitor_interface_info,
                                                           &monitor_interface_vtable,
                                                           monitor,  /* user_data */
                                                           NULL,  /* user_data_free_func */
                                                           NULL); /* GError** */

      g_hash_table_insert (priv->monitors, monitor,
                           GUINT_TO_POINTER (registration_id));
    }
  
  /* Build the method params */
  time_range = zeitgeist_time_range_to_variant (
                                    zeitgeist_monitor_get_time_range (monitor));
  event_templates = zeitgeist_events_to_variant (
                   g_ptr_array_ref (zeitgeist_monitor_get_templates (monitor)));

  g_variant_builder_init (&b, G_VARIANT_TYPE ("(o(xx)a(asaasay))"));
  g_variant_builder_add (&b, "o", zeitgeist_monitor_get_path (monitor));
  g_variant_builder_add_value (&b, time_range); // owns ref
  g_variant_builder_add_value (&b, event_templates); // owns ref;
  params = g_variant_builder_end (&b);

  /* Send the shebang to Zeitgeist */
  g_dbus_proxy_call (priv->log,
                     "InstallMonitor",
                     params,
                     G_DBUS_CALL_FLAGS_NONE,
                     -1, NULL, NULL, NULL);
}

void
zeitgeist_log_install_monitor (ZeitgeistLog        *self,
                               ZeitgeistMonitor    *monitor)
{
  ZeitgeistLogPrivate   *priv;
  
  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (ZEITGEIST_IS_MONITOR (monitor));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);
  
  /* Track the monitor so we can reinstate it if the Zeitgeist daemon
   * leaves and reappears on the bus */
  g_object_weak_ref (G_OBJECT (monitor),
                     (GWeakNotify) _zeitgeist_log_on_monitor_destroyed,
                     self);

  /* The guint value in the monitors table holds the gdbus registration id.
   * For non-registered monitors this is 0 */
  g_hash_table_insert (priv->monitors, monitor, GUINT_TO_POINTER (0));

  /* If we are connected send the monitor to Zeitgeist now,
   * otherwise it will be installed once the connection is up */
  if (priv->log != NULL)
    _zeitgeist_log_install_monitor (self, monitor);
}

static void
monitor_removed_cb (GObject *source_object,
                    GAsyncResult *res,
                    gpointer user_data)
{
  ZeitgeistLog        *self = ZEITGEIST_LOG (((gpointer*)user_data)[0]);
  ZeitgeistMonitor    *mon = ZEITGEIST_MONITOR (((gpointer*)user_data)[1]);
  ZeitgeistLogPrivate *priv;
  guint                registration_id;
  GVariant            *val;
  GError              *error;

  error = NULL;
  val = g_dbus_proxy_call_finish (G_DBUS_PROXY (source_object), res, &error);

  if (error != NULL)
    {
      g_critical ("Failed to remove monitor from Zeitgeist. Retracting"
                  "%s from the bus nonetheless: %s",
                  zeitgeist_monitor_get_path (mon), error->message);
      g_error_free (error);
    }

  if (val != NULL)
    g_variant_unref (val);

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);
  registration_id = GPOINTER_TO_UINT (g_hash_table_lookup (priv->monitors, mon));
  g_hash_table_remove (priv->monitors, mon);

  /* Retract the monitor from the session bus,
   * now that ZG is informed we are about to do so */
  if (registration_id != 0)
    {
      g_dbus_connection_unregister_object (priv->connection,
                                           registration_id);
    }

  /* Drop refs we held during async call */
  g_object_unref (self);
  g_object_unref (mon);

  g_free (user_data);
}

void
zeitgeist_log_remove_monitor (ZeitgeistLog        *self,
                              ZeitgeistMonitor    *monitor)
{
  ZeitgeistLogPrivate   *priv;
  GDBusConnection       *connection;
  gpointer              *dispatch_data;

  g_return_if_fail (ZEITGEIST_IS_LOG (self));
  g_return_if_fail (ZEITGEIST_IS_MONITOR (monitor));

  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* Keep refs while async call is on going */
  dispatch_data = g_new (gpointer, 2);
  dispatch_data[0] = g_object_ref (self);
  dispatch_data[1] = g_object_ref (monitor);

  /* Inform ZG that we are about to remove the monitor from the bus,
   * only after that has been acked will we retract it */
  g_dbus_proxy_call (priv->log,
                     "RemoveMonitor",
                     g_variant_new ("o", zeitgeist_monitor_get_path(monitor)),
                     G_DBUS_CALL_FLAGS_NONE,
                     -1, NULL, monitor_removed_cb, dispatch_data);
}

/* Helper for dropping a weak ref from a log on a monitor. The method
 * signature makes it suitable for casting to a GHFunc */
static void
_zeitgeist_monitor_weak_unref (ZeitgeistMonitor *monitor,
                               gpointer          _ignored,
                               ZeitgeistLog     *log)
{
  g_object_weak_unref (G_OBJECT (monitor),
                       (GWeakNotify) _zeitgeist_log_on_monitor_destroyed,
                       log);
}

/* Called when a monitor is finalized, by virtue of our weak ref */
static void
_zeitgeist_log_on_monitor_destroyed (ZeitgeistLog     *self,
                                     ZeitgeistMonitor *monitor)
{
  ZeitgeistLogPrivate *priv;
  
  priv = ZEITGEIST_LOG_GET_PRIVATE (self);

  /* If we have this monitor registered we remove it */
  if (g_hash_table_lookup (priv->monitors, monitor) != NULL)
    zeitgeist_log_remove_monitor (self, monitor);
}
