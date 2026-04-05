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
 *              Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

/**
 * SECTION:zeitgeist-data-source
 * @short_description: Abstracts data sources used by
 *                     the #ZeitgeistDataSourceRegistry extension
 * @include: zeitgeist.h
 *
 * #ZeitgeistDataSource represents a data source used to insert events into
 * Zeitgeist. The data sources are identified using the unique_id property,
 * and when querying the data source registry you get other interesting
 * information like timestamp of the last action of the data source, flag
 * whether it is currently running etc.
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-data-source.h"
#include "zeitgeist-event.h"

G_DEFINE_TYPE (ZeitgeistDataSource, zeitgeist_data_source, G_TYPE_INITIALLY_UNOWNED);
#define ZEITGEIST_DATA_SOURCE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_DATA_SOURCE, ZeitgeistDataSourcePrivate))

typedef struct
{
  gchar *unique_id;
  gchar *name;
  gchar *description;
  GPtrArray *templates;
  gboolean running;
  gint64 timestamp;
  gboolean enabled;
} ZeitgeistDataSourcePrivate;

/**
 * zeitgeist_data_source_new:
 * 
 * Create a new empty data source structure.
 *
 * Returns: A new instance of #ZeitgeistDataSource. The returned source will
 *          have a floating reference which will be consumed if you pass the
 *          data source to any of the methods provided by this library (like
 *          registering the data source). If you do not do that then you
 *          must free the data source yourself with g_object_unref().
 */
ZeitgeistDataSource*
zeitgeist_data_source_new (void)
{
  return g_object_new (ZEITGEIST_TYPE_DATA_SOURCE, NULL);
}

/**
 * zeitgeist_data_source_new_full:
 * @id: Unique ID for the data source.
 * @name: Name of the data source (may be translated).
 * @desc: Data source description.
 * @event_templates: A #GPtrArray of #ZeitgeistEvent<!-- -->s. This parameter
 *                   is optional and purely informational to let data-source
 *                   management applications and other data-sources know what
 *                   sort of information the data source logs.
 *                   The data source will assume ownership of the events and
 *                   the pointer array.
 *                   If you want to keep a reference for yourself you must do a
 *                   g_ptr_array_ref() on @event_templates as well as reffing
 *                   the events held by it before calling this method.
 *
 * Creates a new instance of DataSource with the given ID, name and
 * description.
 *
 * Returns: New instance of #ZeitgeistDataSource with floating reference,
 *          if you do not pass the instance to method like 
 *          zeitgeist_data_source_registry_register_data_source(), then you 
 *          have to free the data source yourself using g_object_unref().
 */
ZeitgeistDataSource*
zeitgeist_data_source_new_full (const gchar *id,
                                const gchar *name,
                                const gchar *desc,
                                GPtrArray   *templates)
{
  ZeitgeistDataSource* src = zeitgeist_data_source_new ();

  zeitgeist_data_source_set_unique_id (src, id);
  zeitgeist_data_source_set_name (src, name);
  zeitgeist_data_source_set_description (src, desc);
  zeitgeist_data_source_set_event_templates (src, templates);

  return src;
}

/**
 * zeitgeist_data_source_new_from_variant:
 * @event: A #GVariant with signature defined in
 *         #ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_SIGNATURE.
 *         If @src is a floating reference the floating reference
 *         will be consumed.
 *
 * Parse the data in a #GVariant and build a #ZeitgeistDataSource from it.
 * The reverse operation of this is zeitgeist_data_source_to_variant_full().
 *
 * Returns: A newly allocated #ZeitgeistDataSource filled with the metadata and
 *          event templates described by @src. The returned data source will
 *          have a floating reference which will be consumed if you pass the
 *          data source to any of the methods provided by this library. If you
 *          do not do that then you must free the data source yourself with
 *          g_object_unref()
 */
ZeitgeistDataSource*
zeitgeist_data_source_new_from_variant (GVariant *src)
{
  ZeitgeistDataSource *result;
  gchar               *unique_id, *name, *description;
  GVariant            *event_templates_data;
  GPtrArray           *event_templates;
  gboolean             running, enabled;
  gint64               timestamp;

  g_return_val_if_fail (src != NULL, NULL);

  /* Parse the variant */
  g_variant_ref_sink (src);
  g_variant_get (src, ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_SIGNATURE,
                 &unique_id, &name, &description,
                 NULL, &running, &timestamp, &enabled);

  event_templates_data = g_variant_get_child_value (src, 3);
  event_templates = zeitgeist_events_from_variant (event_templates_data);
  g_variant_unref (event_templates_data);

  /* Build the ZeitgeistDataSource */
  result = zeitgeist_data_source_new_full (unique_id, name, description,
                                           event_templates); // own ref

  zeitgeist_data_source_set_running (result, running);
  zeitgeist_data_source_set_timestamp (result, timestamp);
  zeitgeist_data_source_set_enabled (result, enabled);

  /* Clean up */
  g_free (unique_id);
  g_free (name);
  g_free (description);
  g_variant_unref (src);

  return result;
}

const gchar*
zeitgeist_data_source_get_unique_id (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->unique_id;
}

void
zeitgeist_data_source_set_unique_id (ZeitgeistDataSource *src,
                                     const gchar* unique_id)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  if (priv->unique_id)
    {
      g_free (priv->unique_id);
    }
  
  priv->unique_id = g_strdup (unique_id);
}

const gchar*
zeitgeist_data_source_get_name (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->name;
}

void
zeitgeist_data_source_set_name (ZeitgeistDataSource *src,
                                const gchar         *name)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  if (priv->name)
    {
      g_free (priv->name);
    }
  
  priv->name = g_strdup (name);
}

const gchar*
zeitgeist_data_source_get_description (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->description;
}

void
zeitgeist_data_source_set_description (ZeitgeistDataSource *src,
                                       const gchar *description)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  if (priv->description)
    {
      g_free (priv->description);
    }

  priv->description = g_strdup (description);
}

GPtrArray*
zeitgeist_data_source_get_event_templates (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->templates;
}

/**
 * zeitgeist_data_source_set_event_templates:
 * @src: Instance of #ZeitgeistDataSource.
 * @event_templates: A #GPtrArray which contains elements of type
 *             #ZeitgeistEvent.
 *             The data source will assume ownership of the events and
 *             the pointer array.
 *             If you want to keep a reference for yourself you must do a
 *             g_ptr_array_ref() on @templates as well as reffing
 *             the events held by it before calling this method.
 *
 * Sets event templates which are logged by this #ZeitgeistDataSource
 * instance.
 */
void
zeitgeist_data_source_set_event_templates (ZeitgeistDataSource *src,
                                           GPtrArray *event_templates)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  if (priv->templates)
    {
      g_ptr_array_unref (priv->templates);
    }

  if (event_templates)
    {
      g_ptr_array_foreach (event_templates, (GFunc) g_object_ref_sink, NULL);
      g_ptr_array_set_free_func (event_templates, g_object_unref);
    }

  priv->templates = event_templates;
}

gboolean
zeitgeist_data_source_is_running (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), FALSE);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->running;
}

void
zeitgeist_data_source_set_running (ZeitgeistDataSource *src,
                                   gboolean running)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  priv->running = running;
}

gint64
zeitgeist_data_source_get_timestamp (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), 0);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->timestamp;
}

void
zeitgeist_data_source_set_timestamp (ZeitgeistDataSource *src,
                                     gint64 timestamp)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  priv->timestamp = timestamp;
}

gboolean
zeitgeist_data_source_is_enabled (ZeitgeistDataSource *src)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), FALSE);

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  return priv->enabled;
}

void
zeitgeist_data_source_set_enabled (ZeitgeistDataSource *src,
                                   gboolean enabled)
{
  ZeitgeistDataSourcePrivate *priv;
  g_return_if_fail (ZEITGEIST_IS_DATA_SOURCE (src));

  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  priv->enabled = enabled;
}

static void
zeitgeist_data_source_init (ZeitgeistDataSource *object)
{
  ZeitgeistDataSourcePrivate *priv;

  priv =  ZEITGEIST_DATA_SOURCE_GET_PRIVATE (object);
  priv->unique_id = NULL;
  priv->name = NULL;
  priv->description = NULL;
  priv->templates = NULL;
  priv->running = FALSE;
  priv->timestamp = 0;
  priv->enabled = TRUE;
}

static void
zeitgeist_data_source_finalize (GObject *object)
{
  ZeitgeistDataSource *src = ZEITGEIST_DATA_SOURCE (object);
  ZeitgeistDataSourcePrivate *priv;
  
  priv = ZEITGEIST_DATA_SOURCE_GET_PRIVATE (src);

  zeitgeist_data_source_set_unique_id (src, NULL);
  zeitgeist_data_source_set_name (src, NULL);
  zeitgeist_data_source_set_description (src, NULL);
  zeitgeist_data_source_set_event_templates (src, NULL);
  
  G_OBJECT_CLASS (zeitgeist_data_source_parent_class)->finalize (object);
}

static void
zeitgeist_data_source_class_init (ZeitgeistDataSourceClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize = zeitgeist_data_source_finalize;

  g_type_class_add_private (object_class, sizeof (ZeitgeistDataSourcePrivate));
}

/**
 * zeitgeist_data_source_to_variant_full:
 * @events: A #ZeitgeistDataSource. If this is a
 *          floating reference it will be consumed
 *
 * Convert a #ZeitgeistDataSource to a #GVariant with signature
 * #ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_SIGNATURE.
 *
 * Returns: A floating reference to a #GVariant as described above. Unless the
 *          floating reference is consumed somewhere you must free it with
 *          g_variant_unref().
 */
GVariant*
zeitgeist_data_source_to_variant_full (ZeitgeistDataSource *src)
{
  GVariantBuilder      b;
  GPtrArray           *event_templates;
  GVariant            *vevent_templates;
  const gchar         *str;

  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  g_object_ref_sink (src);
  g_variant_builder_init (&b, ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_TYPE);

  /* Add static metadata */
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_unique_id(src), str  ? str : ""));
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_name(src), str  ? str : ""));
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_description(src), str  ? str : ""));

  /* Add event templates */
  event_templates = g_ptr_array_ref (
                               zeitgeist_data_source_get_event_templates (src));
  vevent_templates = zeitgeist_events_to_variant (event_templates);
  g_variant_builder_add_value (&b, vevent_templates /* own ref */);

  /* Add volatile metadata */
  g_variant_builder_add (&b, "b", zeitgeist_data_source_is_running(src));
  g_variant_builder_add (&b, "x", zeitgeist_data_source_get_timestamp(src));
  g_variant_builder_add (&b, "b", zeitgeist_data_source_is_enabled(src));

  /* Clean up */
  g_object_unref (src);

  return g_variant_builder_end (&b);
}

/**
 * zeitgeist_data_source_to_variant:
 * @events: A #ZeitgeistDataSource. If this is a
 *          floating reference it will be consumed
 *
 * Convert a #ZeitgeistDataSource to a #GVariant with signature
 * #ZEITGEIST_DATA_SOURCE_VARIANT_SIGNATURE.
 *
 * Returns: A floating reference to a #GVariant as described above. Unless the
 *          floating reference is consumed somewhere you must free it with
 *          g_variant_unref().
 */
GVariant*
zeitgeist_data_source_to_variant  (ZeitgeistDataSource *src)
{
  GVariantBuilder      b;
  GPtrArray           *event_templates;
  GVariant            *vevent_templates;
  const gchar         *str;

  g_return_val_if_fail (ZEITGEIST_IS_DATA_SOURCE (src), NULL);

  g_object_ref_sink (src);
  g_variant_builder_init (&b, ZEITGEIST_DATA_SOURCE_VARIANT_TYPE);

  /* Add static metadata */
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_unique_id(src), str  ? str : ""));
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_name(src), str  ? str : ""));
  g_variant_builder_add (&b, "s", (str = zeitgeist_data_source_get_description(src), str  ? str : ""));

  /* Add event templates */
  event_templates = g_ptr_array_ref (
                               zeitgeist_data_source_get_event_templates (src));
  vevent_templates = zeitgeist_events_to_variant (event_templates);
  g_variant_builder_add_value (&b, vevent_templates /* own ref */);

  /* Clean up */
  g_object_unref (src);

  return g_variant_builder_end (&b);
}

/**
 * zeitgeist_data_sources_to_variant:
 * @events: A #GPtrArray of #ZeitgeistDataSource<!-- -->s. If the sources has
 *          floating references they will be consumed. Furthermore the
 *          reference on the #GPtrArray itself will also be stolen and its
 *          @free_func set to %NULL
 *
 * Convert a set of #ZeitgeistDataSource<-- -->s to a #GVariant with signature
 * as an array of #ZEITGEIST_DATA_SOURCE_VARIANT_SIGNATURE.
 *
 * Returns: A floating reference to a #GVariant as described above. Unless the
 *          floating reference is consumed somewhere you must free it with
 *          g_variant_unref().
 */
GVariant*
zeitgeist_data_sources_to_variant  (GPtrArray *sources)
{
  GVariantBuilder      b;
  ZeitgeistDataSource *src;
  GVariant            *vsrc;
  int                  i;

  g_return_val_if_fail (sources != NULL, NULL);

  g_variant_builder_init (&b,
                   G_VARIANT_TYPE ("a"ZEITGEIST_DATA_SOURCE_VARIANT_SIGNATURE));

  for (i = 0; i < sources->len; i++)
    {
      src = ZEITGEIST_DATA_SOURCE (g_ptr_array_index (sources, i));
      g_object_ref_sink (src);
      vsrc = zeitgeist_data_source_to_variant_full (src);
      g_variant_builder_add_value (&b, vsrc);
      g_object_unref (src);
    }

  /* We need to unset the free func because we can't risk double unreffing
   * the data sources held in it, quite ugly but there is no other way */
  g_ptr_array_set_free_func (sources, NULL);
  g_ptr_array_unref (sources);

  return g_variant_builder_end (&b);
}

/**
 * zeitgeist_data_sources_from_variant:
 * @sources: A #GVariant  with signature as an array of
 *          #ZEITGEIST_DATA_SOURCE_WITH_INFO_VARIANT_SIGNATURE.
 *          If @sources is floating this method will consume
 *          the floating reference.
 *
 * Returns: A reference to a #GPtrArray of #ZeitgeistDataSource<!-- -->s.
 *          All the events will be floating references, and the
 *          #GPtrArray<!-- -->'s @free_func will be set to g_object_unref().
 */
GPtrArray*
zeitgeist_data_sources_from_variant  (GVariant *sources)
{
  GPtrArray           *result;
  GVariant            *vsrc;
  ZeitgeistDataSource *src;
  int                  i, n_sources;

  g_return_val_if_fail (sources != NULL, NULL);

  g_variant_ref_sink (sources);

  n_sources = g_variant_n_children (sources);
  result = g_ptr_array_sized_new (n_sources);
  g_ptr_array_set_free_func (result, (GDestroyNotify) g_object_unref);

  for (i = 0; i < n_sources; i++)
    {
      vsrc = g_variant_get_child_value (sources, i);
      src = zeitgeist_data_source_new_from_variant (vsrc);
      g_variant_unref (vsrc);
      g_ptr_array_add (result, src);
    }

  g_variant_unref (sources);

  return result;
}

