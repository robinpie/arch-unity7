/*
 * Copyright (C) 2010 Canonical, Ltd.
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
 * Authored by: Michal Hruby <michal.mhr@gmail.com>
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-data-source-registry.h"
#include "zeitgeist-marshal.h"

/**
 * SECTION:zeitgeist-data-source-registry
 * @short_description: Query the Zeitgeist Data Source Registry extension
 * @include: zeitgeist.h
 *
 * The Zeitgeist engine maintains a publicly available list of recognized
 * data-sources (components inserting information into Zeitgeist).
 * #ZeitgeistDataSourceRegistry is used to register new data sources,
 * get information about them and gives the ability to enable or disable
 * the data sources.
 */

G_DEFINE_TYPE (ZeitgeistDataSourceRegistry,
               zeitgeist_data_source_registry,
               G_TYPE_OBJECT);
#define ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_DATA_SOURCE_REGISTRY, \
                               ZeitgeistDataSourceRegistryPrivate))

typedef struct
{
  /* Our connection to the bus */
  GDBusConnection *connection;

  /* The connection to the ZG daemon.
   * If index != NULL it means we have a connection */
  GDBusProxy *registry;

  /* Method calls queued up while waiting for a proxy  */
  GSList *method_dispatch_queue;

  /* DBus signal handlers */
  guint dbus_signals_id;

} ZeitgeistDataSourceRegistryPrivate;

/* Property ids */
enum
{
	PROP_0,

	LAST_PROPERTY
};

enum
{
  SOURCE_REGISTERED,
  SOURCE_ENABLED,
  SOURCE_DISCONNECTED,

  LAST_SIGNAL
};

static guint _registry_signals[LAST_SIGNAL] = { 0 };

typedef struct
{
  ZeitgeistDataSourceRegistry *self;
  const gchar                 *method_name;
  GVariant                    *params;
  GCancellable                *cancellable;
  GAsyncReadyCallback          cb;
  gpointer                     user_data;
} MethodDispatchContext;

static void    on_proxy_acquired (GObject *source_object,
                                  GAsyncResult *res,
                                  gpointer user_data);

static void    dispatch_method         (MethodDispatchContext *ctx);

static void    dispatch_async_callback (GObject               *source_object,
                                        GAsyncResult          *res,
                                        gpointer               user_data);

static void
zeitgeist_data_source_registry_init (ZeitgeistDataSourceRegistry *self)
{
  ZeitgeistDataSourceRegistryPrivate *priv;

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  /* Set up the connection to the ZG daemon */
  g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION,
                            G_DBUS_PROXY_FLAGS_NONE,
                            NULL,
                            "org.gnome.zeitgeist.Engine",
                            "/org/gnome/zeitgeist/data_source_registry",
                            "org.gnome.zeitgeist.DataSourceRegistry",
                            NULL,
                            on_proxy_acquired,
                            g_object_ref (self));
}

static void
zeitgeist_data_source_registry_finalize (GObject *object)
{
  ZeitgeistDataSourceRegistry *registry;
  ZeitgeistDataSourceRegistryPrivate *priv;
  
  registry = ZEITGEIST_DATA_SOURCE_REGISTRY (object);
  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (registry);

  if (priv->dbus_signals_id)
    {
      g_dbus_connection_signal_unsubscribe (priv->connection,
                                            priv->dbus_signals_id);
      priv->dbus_signals_id = 0;
    }

  if (priv->registry)
    {
      g_object_unref (priv->registry);
      priv->registry = NULL;
    }
  
  if (priv->connection)
    {
      g_object_unref (priv->connection);
      priv->connection = NULL;
    }



  G_OBJECT_CLASS (zeitgeist_data_source_registry_parent_class)->finalize (object);
}

static void
zeitgeist_data_source_registry_get_property (GObject    *object,
                                             guint       prop_id,
                                             GValue     *value,
                                             GParamSpec *pspec)
{
  ZeitgeistDataSourceRegistryPrivate *priv;

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (object);

  switch (prop_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}

static void
zeitgeist_data_source_registry_set_property (GObject      *object,
                                             guint         prop_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
  ZeitgeistDataSourceRegistryPrivate *priv;

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (object);

  switch (prop_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}

static void
on_dbus_signal_received (GDBusConnection *connection,
                         const gchar *sender_name,
                         const gchar *object_path,
                         const gchar *interface_name,
                         const gchar *signal_name,
                         GVariant *parameters,
                         gpointer user_data)
{
  ZeitgeistDataSourceRegistry        *self;
  ZeitgeistDataSourceRegistryPrivate *priv;
  ZeitgeistDataSource                *src;
  gchar                              *unique_id;
  gboolean                            enabled;
  GVariant                           *vsrc;

  self = ZEITGEIST_DATA_SOURCE_REGISTRY (user_data);
  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  if (g_strcmp0 (signal_name, "DataSourceRegistered") == 0)
    {
      vsrc = g_variant_get_child_value (parameters, 0);
      src = zeitgeist_data_source_new_from_variant (vsrc);
      g_signal_emit (self, _registry_signals[SOURCE_REGISTERED], 0, src);
      g_variant_unref (vsrc);
      g_object_unref (src);
    }
  else if (g_strcmp0 (signal_name, "DataSourceDisconnected") == 0)
    {
      vsrc = g_variant_get_child_value (parameters, 0);
      src = zeitgeist_data_source_new_from_variant (vsrc);
      g_signal_emit (self, _registry_signals[SOURCE_DISCONNECTED], 0, src);
      g_variant_unref (vsrc);
      g_object_unref (src);
    }
  else if (g_strcmp0 (signal_name, "DataSourceEnabled") == 0)
    {
      g_variant_get (parameters, "(sb)", &unique_id, &enabled); // FIXME: unref?
      g_signal_emit (self, _registry_signals[SOURCE_ENABLED],
          0, unique_id, enabled);
      g_free (unique_id);
    }
  else
    g_warning ("Unknown signal from Zeitgeist Data Source Registry: %s",
               signal_name);
}

static void
zeitgeist_data_source_registry_class_init (ZeitgeistDataSourceRegistryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec   *pspec;
  
  object_class->finalize     = zeitgeist_data_source_registry_finalize;
  object_class->get_property = zeitgeist_data_source_registry_get_property;
  object_class->set_property = zeitgeist_data_source_registry_set_property;

  _registry_signals[SOURCE_REGISTERED] =
    g_signal_new ("source-registered",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ZeitgeistDataSourceRegistryClass, source_registered),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, ZEITGEIST_TYPE_DATA_SOURCE);

  _registry_signals[SOURCE_DISCONNECTED] =
    g_signal_new ("source-disconnected",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ZeitgeistDataSourceRegistryClass, source_disconnected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1, ZEITGEIST_TYPE_DATA_SOURCE);

  // FIXME: This signal should really be "source-toggled" and give ZgDataSrc as only arg
  _registry_signals[SOURCE_ENABLED] =
    g_signal_new ("source-enabled",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (ZeitgeistDataSourceRegistryClass, source_enabled),
                  NULL, NULL,
                  _zeitgeist_cclosure_marshal_VOID__STRING_BOOLEAN,
                  G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_BOOLEAN);

  g_type_class_add_private (object_class,
                            sizeof (ZeitgeistDataSourceRegistryPrivate));
}

/*
 * API BELOW HERE
 */

/**
 * zeitgeist_data_source_registry_new:
 *
 * Create a new data source registry instance.
 *
 * DataSourceRegistry instances are not overly expensive for neither 
 * client or the Zeitgeist daemon so there's no need to go to lengths
 * to keep singleton instances around.
 *
 * Returns: A reference to a newly allocated registry.
 */
ZeitgeistDataSourceRegistry*
zeitgeist_data_source_registry_new (void)
{
  ZeitgeistDataSourceRegistry *registry;

  registry = (ZeitgeistDataSourceRegistry*)
    g_object_new (ZEITGEIST_TYPE_DATA_SOURCE_REGISTRY, NULL);

  return registry;
}

void
zeitgeist_data_source_registry_get_data_sources (
                                     ZeitgeistDataSourceRegistry *self,
                                     GCancellable                *cancellable,
                                     GAsyncReadyCallback          callback,
                                     gpointer                     user_data)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  MethodDispatchContext              *ctx;

  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "GetDataSources";
  ctx->params = g_variant_new ("()");
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

/**
 * zeitgeist_data_source_registry_get_data_sources_finish:
 * @self: Instance of #ZeitgeistDataSourceRegistry.
 * @res: a #GAsyncResult.
 * @error: a #GError or #NULL.
 *
 * Returns: Newly created #GPtrArray containing #ZeitgeistDataSource<!-- -->(s)
 *          registered in Zeitgeist. Free using g_ptr_array_unref() once
 *          you're done using it.
 */
GPtrArray*
zeitgeist_data_source_registry_get_data_sources_finish (
                                     ZeitgeistDataSourceRegistry *self,
                                     GAsyncResult                *res,
                                     GError                      **error)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  GVariant                           *val, *vsources;
  GPtrArray                          *sources;

  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  val = g_dbus_proxy_call_finish (priv->registry, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  vsources = g_variant_get_child_value (val, 0);
  g_variant_unref (val);

  sources = zeitgeist_data_sources_from_variant (vsources);
  g_variant_unref (vsources);

  return sources;
}

/**
 * zeitgeist_data_source_registry_register_data_source:
 * @self: Instance of #ZeitgeistDataSourceRegistry.
 * @source: Data source to register. If this is a
 *          floating reference it will be consumed
 * @cancellable: a #GCancellable or #NULL.
 * @callback: a GAsyncReadyCallback to call when the request is finished.
 * @user_data: the data to pass to callback function.
 *
 * Registers new data source in the registry, the @source parameter needs to
 * have unique-id, name, description and optionally event_templates set,
 * therefore it is useful to pass #ZeitgeistDataSource instance created using
 * zeitgeist_data_source_new_full(). The registry will assume its ownership.
 */
void
zeitgeist_data_source_registry_register_data_source (
    ZeitgeistDataSourceRegistry *self,
    ZeitgeistDataSource         *source,
    GCancellable                *cancellable,
    GAsyncReadyCallback          callback,
    gpointer                     user_data)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  MethodDispatchContext              *ctx;
  GVariant                           *vsource;

  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self));
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (source));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  vsource = zeitgeist_data_source_to_variant (source); // sinks ref
  
  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "RegisterDataSource";
  ctx->params = vsource;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

/**
 * zeitgeist_data_source_registry_register_data_source_finish:
 * @self: Instance of #ZeitgeistDataSourceRegistry.
 * @res: Result of the asynchronous operation.
 * @error: a #GError or NULL.
 *
 * Returns: If error is unset, returns whether this data source is enabled.
 */
gboolean
zeitgeist_data_source_registry_register_data_source_finish (
    ZeitgeistDataSourceRegistry *self,
    GAsyncResult                *res,
    GError                     **error)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  GVariant                           *val;

  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);
  val = g_dbus_proxy_call_finish (priv->registry, res, error);

  if (val == NULL)
    return FALSE;

  g_variant_unref (val);
  return TRUE;
}

void
zeitgeist_data_source_registry_set_data_source_enabled (
    ZeitgeistDataSourceRegistry *self,
    const gchar                 *unique_id,
    gboolean                     enabled,
    GCancellable                *cancellable,
    GAsyncReadyCallback          callback,
    gpointer                     user_data)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  MethodDispatchContext              *ctx;
  GVariant                           *params;
  GVariantBuilder                     b;

  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self));
  g_return_if_fail (unique_id != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("(sb)"));
  g_variant_builder_add (&b, "s", unique_id);
  g_variant_builder_add (&b, "b", enabled);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "SetDataSourceEnabled";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

gboolean
zeitgeist_data_source_registry_set_data_source_enabled_finish (
    ZeitgeistDataSourceRegistry *self,
    GAsyncResult                *res,
    GError                     **error)
{
  ZeitgeistDataSourceRegistryPrivate *priv;
  GVariant                           *val;

  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE_REGISTRY (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);
  val = g_dbus_proxy_call_finish (priv->registry, res, error);

  if (val == NULL)
    return FALSE;

  g_variant_unref (val);
  return TRUE;
}

static void
on_proxy_acquired (GObject *source_object,
                   GAsyncResult *res,
                   gpointer user_data)
{
  ZeitgeistDataSourceRegistry        *self;
  ZeitgeistDataSourceRegistryPrivate *priv;
  GError                             *error;

  self = ZEITGEIST_DATA_SOURCE_REGISTRY (user_data);
  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (self);

  error = NULL;
  priv->registry = g_dbus_proxy_new_finish (res, &error);

  if (error != NULL)
    {
      g_critical ("Unable to connect to Zeitgeist daemon: %s",
                  error->message);
      g_error_free (error);
    }
  else
    {
      /* Dispatch all method calls queued while waiting for the proxy */
      priv->method_dispatch_queue = g_slist_reverse (priv->method_dispatch_queue);
      g_slist_foreach (priv->method_dispatch_queue, (GFunc) dispatch_method, NULL);
      g_slist_free (priv->method_dispatch_queue);
      priv->method_dispatch_queue = NULL;
    }

  /* Grab a ref on the GDBusConnection as well */
  priv->connection = G_DBUS_CONNECTION (g_object_ref (g_dbus_proxy_get_connection (priv->registry)));

  /* Connect to all DBus signals from the data source registry */
  priv->dbus_signals_id = g_dbus_connection_signal_subscribe (
      priv->connection,
      "org.gnome.zeitgeist.Engine",
      "org.gnome.zeitgeist.DataSourceRegistry",
      NULL, /* Listen for any signal on this interface */
      "/org/gnome/zeitgeist/data_source_registry",
      NULL,
      G_DBUS_SIGNAL_FLAGS_NONE,
      on_dbus_signal_received,
      self, NULL);

  /* Release ref we held during async op */
  g_object_unref (self);
}

/* Send off the DBus method call, or queue it if we don't
 * have a proxy at this point */
static void
dispatch_method (MethodDispatchContext *ctx)
{
  ZeitgeistDataSourceRegistryPrivate *priv;

  priv = ZEITGEIST_DATA_SOURCE_REGISTRY_GET_PRIVATE (ctx->self);

  if (priv->registry)
    {
      g_dbus_proxy_call (priv->registry,
                         ctx->method_name,
                         ctx->params,
                         G_DBUS_CALL_FLAGS_NONE,
                         -1,
                         ctx->cancellable,
                         dispatch_async_callback,
                         ctx);
    }
  else
    priv->method_dispatch_queue = g_slist_prepend (priv->method_dispatch_queue,
                                                   ctx);
}

/* Used to marshal the async callbacks from GDBus into ones
 * coming from this ZeitgeistIndex instance */
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
