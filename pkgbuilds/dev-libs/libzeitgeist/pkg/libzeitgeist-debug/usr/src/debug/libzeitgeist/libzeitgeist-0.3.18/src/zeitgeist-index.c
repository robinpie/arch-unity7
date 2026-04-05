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

#include "zeitgeist-index.h"
#include "zeitgeist-simple-result-set.h"

/**
 * SECTION:zeitgeist-index
 * @short_description: Query the Zeitgeist Full Text Search Extension
 * @include: zeitgeist.h
 *
 * 
 */

G_DEFINE_TYPE (ZeitgeistIndex, zeitgeist_index, G_TYPE_OBJECT);
#define ZEITGEIST_INDEX_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_INDEX, ZeitgeistIndexPrivate))

typedef struct
{
  /* The connection to the ZG daemon.
   * If index != NULL it means we have a connection */
  GDBusProxy *index;

  /* Method calls queued up while waiting for a proxy  */
  GSList *method_dispatch_queue;

} ZeitgeistIndexPrivate;

typedef struct
{
  ZeitgeistIndex      *self;
  const gchar         *method_name;
  GVariant            *params;
  GCancellable        *cancellable;
  GAsyncReadyCallback  cb;
  gpointer             user_data;
} MethodDispatchContext;

/* Property ids */
enum
{
	PROP_0,

	LAST_PROPERTY
};

static void    on_proxy_acquired (GObject *source_object,
                                  GAsyncResult *res,
                                  gpointer user_data);

static void    dispatch_method         (MethodDispatchContext *ctx);

static void    dispatch_async_callback (GObject               *source_object,
                                        GAsyncResult          *res,
                                        gpointer               user_data);

static void
zeitgeist_index_init (ZeitgeistIndex *self)
{
  ZeitgeistIndexPrivate *priv;

  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);

  /* Set up the connection to the ZG daemon */
  g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION,
                            G_DBUS_PROXY_FLAGS_NONE,
                            NULL,
                            "org.gnome.zeitgeist.Engine",
                            "/org/gnome/zeitgeist/index/activity",
                            "org.gnome.zeitgeist.Index",
                            NULL,
                            on_proxy_acquired,
                            g_object_ref (self));
}

static void
zeitgeist_index_finalize (GObject *object)
{
  ZeitgeistIndex *index = ZEITGEIST_INDEX (object);
  ZeitgeistIndexPrivate *priv;
  
  priv = ZEITGEIST_INDEX_GET_PRIVATE (index);

  if (priv->index)
    {
      g_object_unref (priv->index);
    }
  
  if (priv->method_dispatch_queue != NULL)
    {
      g_critical ("Internal error in libzeitgeist: The method dispatch queue"
                  "should be empty on finalization of ZeitgeistIndex");
    }

  G_OBJECT_CLASS (zeitgeist_index_parent_class)->finalize (object); 
}

static void
zeitgeist_index_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  ZeitgeistIndexPrivate *priv = ZEITGEIST_INDEX_GET_PRIVATE (object);

  switch (prop_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}

static void
zeitgeist_index_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  ZeitgeistIndexPrivate *priv = ZEITGEIST_INDEX_GET_PRIVATE (object);

  switch (prop_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        return;
        break;
    }
}


static void
zeitgeist_index_class_init (ZeitgeistIndexClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec   *pspec;
  
  object_class->finalize     = zeitgeist_index_finalize;
  object_class->get_property = zeitgeist_index_get_property;
  object_class->set_property = zeitgeist_index_set_property;
  
  g_type_class_add_private (object_class, sizeof (ZeitgeistIndexPrivate));
}

/* Send off the DBus method call, or queue it if we don't
 * have a proxy at this point */
static void
dispatch_method (MethodDispatchContext *ctx)
{
  ZeitgeistIndexPrivate *priv;

  priv = ZEITGEIST_INDEX_GET_PRIVATE (ctx->self);

  if (priv->index)
    {
      g_dbus_proxy_call (priv->index,
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

/*
 * API BELOW HERE
 */

/**
 * zeitgeist_index_new:
 * Create a new index that interfaces with the default event index of the Zeitgeist
 * daemon.
 *
 * Create a new #ZeitgeistIndex instance. The index will start to connect to
 * Zeitgeist asynchronously. You can however start calling methods on the
 * returned instance immediately, any method calls issued before the connection
 * has been established will simply be queued and executed once the connection
 * is up.
 *
 * Returns: A reference to a newly allocated index. Free with g_object_unref().
 */
ZeitgeistIndex*
zeitgeist_index_new (void)
{
  ZeitgeistIndex        *index;

  index = (ZeitgeistIndex*) g_object_new (ZEITGEIST_TYPE_INDEX, NULL);

  return index;
}

/**
 * zeitgeist_index_search:
 * @self: The #ZeitgeistIndex you want to query
 * @query: The search string to send to Zeitgeist
 * @time_range: Restrict matched events to ones within this time range. If you
 *              are not interested in restricting the timerange pass
 *              zeitgeist_time_range_new_anytime() as Zeitgeist will detect this
 *              and optimize the query accordingly
 * @event_templates: Restrict matches events to ones matching these
 *                   templates
 * @offset: Offset into the result set to read events from
 * @num_events: Maximal number of events to retrieve
 * @result_type: The #ZeitgeistResultType determining the sort order.
 *               You may pass #ZEITGEIST_RESULT_TYPE_RELEVANCY to this method
 *               to have the results ordered by relevancy calculated in relation
 *               to @query
 * @cancellable: A #GCancellable used to cancel the call or %NULL
 * @callback: A #GAsyncReadyCallback to invoke when the search results are ready
 * @user_data: User data to pass back with @callback
 *
 * Perform a full text search possibly restricted to a #ZeitgeistTimeRange
 * and/or set of event templates.
 *
 * The default boolean operator is %AND. Thus the query
 * <emphasis>foo bar</emphasis> will be interpreted as
 * <emphasis>foo AND bar</emphasis>. To exclude a term from the result
 * set prepend it with a minus sign - eg <emphasis>foo -bar</emphasis>.
 * Phrase queries can be done by double quoting the string 
 * <emphasis>"foo is a bar"</emphasis>. You can truncate terms by appending
 * a *.
 *
 * There are a few keys you can prefix to a term or phrase to search within
 * a specific set of metadata. They are used like
 * <emphasis>key:value</emphasis>. The keys <emphasis>name</emphasis> and
 * <emphasis>title</emphasis> search strictly within the text field of the
 * event subjects. The key <emphasis>app</emphasis> searches within the
 * application name or description that is found in the actor attribute of
 * the events. Lastly you can use the <emphasis>site</emphasis> key to search
 * within the domain name of the subject URIs.
 *
 * You can also control the results with the boolean operators
 * <emphasis>AND</emphasis> and <emphasis>OR</emphasis> and you may
 * use brackets, ( and ), to control the operator precedence.
 */
void
zeitgeist_index_search (ZeitgeistIndex      *self,
                        const gchar         *query,
                        ZeitgeistTimeRange  *time_range,
                        GPtrArray           *event_templates,
                        guint32              offset,
                        guint32              num_events,
                        ZeitgeistResultType  result_type,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  ZeitgeistIndexPrivate *priv;
  GVariant              *vevents, *vtime_range, *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_INDEX (self));
  g_return_if_fail (query != NULL);
  g_return_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range));
  g_return_if_fail (event_templates != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevents = zeitgeist_events_to_variant (event_templates);

  /* Owns floating ref on time_range */
  vtime_range = zeitgeist_time_range_to_variant (time_range);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("(s(xx)a(asaasay)uuu)"));
  g_variant_builder_add (&b, "s", query);
  g_variant_builder_add_value (&b, vtime_range); // owns ref
  g_variant_builder_add_value (&b, vevents); // owns ref
  g_variant_builder_add (&b, "u", offset);
  g_variant_builder_add (&b, "u", num_events);
  g_variant_builder_add (&b, "u", result_type);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "Search";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

/**
 * zeitgeist_index_search_finish:
 * @self: The #ZeitgeistIndex to retrieve results from
 * @res: The #GAsyncResult you received in the #GAsyncReadyCallback you passed
 *       to zeitgeist_index_search()
 * @error: A place to store a #GError or %NULL in case you want to ignore errors
 *
 * Retrieve the result from an asynchronous query started with
 * zeitgeist_index_search().
 *
 * The total hit count of the query will be available via the returned
 * result set by calling zeitgeist_result_set_estimated_matches(). This will
 * often be bigger than the actual number of events held in the result set,
 * which is limited by the @num_events parameter passed to
 * zeitgeist_index_search().
 *
 * Returns: A newly allocated #ZeitgeistResultSet containing the
 *          #ZeitgeistEvent<!-- -->s matching the query. You must free the
 *          result set with g_object_unref(). The events held in the result set
 *          will automatically be unreffed when it is finalized.
 */
ZeitgeistResultSet*
zeitgeist_index_search_finish (ZeitgeistIndex      *self,
                               GAsyncResult        *res,
                               GError             **error)
{
  ZeitgeistIndexPrivate *priv;
  GPtrArray             *events;
  guint32                num_hits;
  GVariant              *val, *vevents, *vnum_hits;

  g_return_val_if_fail (ZEITGEIST_IS_INDEX (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);
  val = g_dbus_proxy_call_finish (priv->index, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  vevents = g_variant_get_child_value (val, 0);
  vnum_hits = g_variant_get_child_value (val, 1);
  events = zeitgeist_events_from_variant (vevents);
  num_hits = g_variant_get_uint32 (vnum_hits);
  g_variant_unref (val);
  g_variant_unref (vevents);
  g_variant_unref (vnum_hits);
  return _zeitgeist_simple_result_set_new (events, num_hits);
}

/**
 * zeitgeist_index_search_with_relevancies:
 * @self: The #ZeitgeistIndex you want to query
 * @query: The search string to send to Zeitgeist
 * @time_range: Restrict matched events to ones within this time range. If you
 *              are not interested in restricting the timerange pass
 *              zeitgeist_time_range_new_anytime() as Zeitgeist will detect this
 *              and optimize the query accordingly
 * @event_templates: Restrict matched events to ones matching these
 *                   templates
 * @storage_state: Filter the events by availability of the storage medium.
 * @offset: Offset into the result set to read events from
 * @num_events: Maximal number of events to retrieve
 * @result_type: The #ZeitgeistResultType determining the sort order.
 *               You may pass #ZEITGEIST_RESULT_TYPE_RELEVANCY to this method
 *               to have the results ordered by relevancy calculated in relation
 *               to @query
 * @cancellable: A #GCancellable used to cancel the call or %NULL
 * @callback: A #GAsyncReadyCallback to invoke when the search results are ready
 * @user_data: User data to pass back with @callback
 *
 * Perform a full text search possibly restricted to a #ZeitgeistTimeRange
 * and/or set of event templates. As opposed to zeitgeist_index_search(), this
 * call will also return numeric relevancies of the events
 * in the #ZeitgeistResultSet.
 *
 * See zeitgeist_index_search() for more details on how to create the query.
 */
void
zeitgeist_index_search_with_relevancies (ZeitgeistIndex       *self,
                                         const gchar          *query,
                                         ZeitgeistTimeRange   *time_range,
                                         GPtrArray            *event_templates,
                                         ZeitgeistStorageState storage_state,
                                         guint32               offset,
                                         guint32               num_events,
                                         ZeitgeistResultType   result_type,
                                         GCancellable         *cancellable,
                                         GAsyncReadyCallback   callback,
                                         gpointer              user_data)
{
  ZeitgeistIndexPrivate *priv;
  GVariant              *vevents, *vtime_range, *params;
  GVariantBuilder        b;
  MethodDispatchContext *ctx;

  g_return_if_fail (ZEITGEIST_IS_INDEX (self));
  g_return_if_fail (query != NULL);
  g_return_if_fail (ZEITGEIST_IS_TIME_RANGE (time_range));
  g_return_if_fail (event_templates != NULL);
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE(cancellable));

  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);

  /* Own all floating refs on the events, and frees the events array */
  vevents = zeitgeist_events_to_variant (event_templates);

  /* Owns floating ref on time_range */
  vtime_range = zeitgeist_time_range_to_variant (time_range);

  /* Build the method params */
  g_variant_builder_init (&b, G_VARIANT_TYPE ("(s(xx)a(asaasay)uuuu)"));
  g_variant_builder_add (&b, "s", query);
  g_variant_builder_add_value (&b, vtime_range); // owns ref
  g_variant_builder_add_value (&b, vevents); // owns ref
  g_variant_builder_add (&b, "u", storage_state);
  g_variant_builder_add (&b, "u", offset);
  g_variant_builder_add (&b, "u", num_events);
  g_variant_builder_add (&b, "u", result_type);
  params = g_variant_builder_end (&b);

  ctx = g_new0 (MethodDispatchContext, 1);
  ctx->self = g_object_ref (self);
  ctx->method_name = "SearchWithRelevancies";
  ctx->params = params;
  ctx->cancellable = cancellable;
  ctx->cb = callback;
  ctx->user_data = user_data;

  dispatch_method (ctx);
}

/**
 * zeitgeist_index_search_with_relevancies_finish:
 * @self: The #ZeitgeistIndex to retrieve results from
 * @res: The #GAsyncResult you received in the #GAsyncReadyCallback you passed
 *       to zeitgeist_index_search_with_relevancies()
 * @relevancies: (out) (array): Location for the newly allocated array
 *                              with relevancies.
 * @relevancies_size: (out): Location for the size of the newly allocated array.
 * @error: A place to store a #GError or %NULL in case you want to ignore errors
 *
 * Retrieve the result from an asynchronous query started with
 * zeitgeist_index_search_with_relevancies().
 *
 * The total hit count of the query will be available via the returned
 * result set by calling zeitgeist_result_set_estimated_matches(). This will
 * often be bigger than the actual number of events held in the result set,
 * which is limited by the @num_events parameter passed to
 * zeitgeist_index_search_with_relevancies().
 *
 * Returns: A newly allocated #ZeitgeistResultSet containing the
 *          #ZeitgeistEvent<!-- -->s matching the query. You must free the
 *          result set with g_object_unref(). The events held in the result set
 *          will automatically be unreffed when it is finalized.
 */
ZeitgeistResultSet*
zeitgeist_index_search_with_relevancies_finish (ZeitgeistIndex *self,
                                                GAsyncResult   *res,
                                                gdouble       **relevancies,
                                                gint           *relevancies_size,
                                                GError        **error)
{
  ZeitgeistIndexPrivate *priv;
  GPtrArray             *events;
  guint32                num_hits;
  GVariant              *val, *vevents, *vrelevancies, *vnum_hits;

  g_return_val_if_fail (ZEITGEIST_IS_INDEX (self), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);
  val = g_dbus_proxy_call_finish (priv->index, res, error);

  if (val == NULL)
    return NULL;

  /* Unpack return value from wrapper struct */
  vevents = g_variant_get_child_value (val, 0);
  vrelevancies = g_variant_get_child_value (val, 1);
  vnum_hits = g_variant_get_child_value (val, 2);

  events = zeitgeist_events_from_variant (vevents);
  num_hits = g_variant_get_uint32 (vnum_hits);
  if (relevancies_size)
  {
    *relevancies_size = g_variant_n_children (vrelevancies);
  }
  if (relevancies)
  {
    GVariantIter iter;
    gdouble relevancy;
    int i;

    *relevancies = g_new (gdouble, g_variant_n_children (vrelevancies));

    i = 0;
    g_variant_iter_init (&iter, vrelevancies);
    while (g_variant_iter_loop (&iter, "d", &relevancy))
    {
      (*relevancies)[i++] = relevancy;
    }
  }

  g_variant_unref (val);
  g_variant_unref (vevents);
  g_variant_unref (vrelevancies);
  g_variant_unref (vnum_hits);

  return _zeitgeist_simple_result_set_new (events, num_hits);
}

static void
on_proxy_acquired (GObject *source_object,
                   GAsyncResult *res,
                   gpointer user_data)
{
  ZeitgeistIndex        *self;
  ZeitgeistIndexPrivate *priv;
  GError                *error;

  self = ZEITGEIST_INDEX (user_data);
  priv = ZEITGEIST_INDEX_GET_PRIVATE (self);

  error = NULL;
  priv->index = g_dbus_proxy_new_finish (res, &error);

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

  /* Release ref we held during async op */
  g_object_unref (self);
}
