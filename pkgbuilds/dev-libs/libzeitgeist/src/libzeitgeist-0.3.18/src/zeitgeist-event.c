/*
 * Copyright (C) 2009 Canonical, Ltd.
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
 *             Jason Smith <jason.smith@canonical.com>
 */

/**
 * SECTION:zeitgeist-event 
 * @short_description: #ZeitgeistEvent objects abstract events returned from Zeitgeist queries
 * @include: zeitgeist.h
 *
 * The #ZeitgeistEvent class is one of the primary elements for communicating
 * with the Zeitgeist daemon. #ZeitgeistEvent<!-- -->s serve two purposes.
 * Unsurprisingly they represent events that have happened, but they also
 * can act as <emphasis>templates</emphasis>. See also #ZeitgeistSubject.
 * 
 * An event in the Zeitgeist world is characterized by two main properties.
 * &quot;What happened&quot; also called the <emphasis>interpretation</emphasis>,
 * and &quot;How did it happen&quot; also called the
 * <emphasis>manifestation</emphasis>. Besides these properties and event
 * also has an <emphasis>actor</emphasis> which identifies the party responsible
 * for triggering the event which in most cases will be an application.
 * Lastly there is an event <emphasis>timestamp</emphasis> and 
 * <emphasis>event id</emphasis>. The timestamp is calculated as the number
 * of milliseconds since the Unix epoch and the event id is a serial number
 * assigned to the event by the Zeitgeist engine when it's logged. These
 * five properties are collectively known as the
 * <emphasis>event metadata</emphasis>.
 *
 * An event must also describe what it happened to. This is called the event
 * <emphasis>subjects</emphasis>. Most events have one subject, but they are
 * allowed to have zero or many too. The metadata of the subjects are 
 * recorded at the time of logging, and are encapsulated by the
 * #ZeitgeistSubject class. It's important to understand that it's just the
 * subject metadata at the time of logging, not necessarily the subject metadata
 * as it exists right now.
 *
 * In addition to the listed properties events may also carry a free form binary
 * <emphasis>payload</emphasis>. The usage of this is is application specific
 * and is generally useless unless you have some contextual information to
 * figure out what's in it.
 *
 * A large part of the Zeitgeist query and monitoring API revolves around a
 * concept of template matching. A query is simply a list of event templates
 * that you want to look for in the log.
 * An unset property on an event template indicates that anything is allowed
 * in that field. If the property is set it indicates that the property
 * must be an exact match.
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gdesktopappinfo.h>

#include "zeitgeist-event.h"

G_DEFINE_TYPE (ZeitgeistEvent, zeitgeist_event, G_TYPE_INITIALLY_UNOWNED);
#define ZEITGEIST_EVENT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_EVENT, ZeitgeistEventPrivate))

typedef enum
{
  ZEITGEIST_EVENT_ID,
  ZEITGEIST_EVENT_TIMESTAMP,
  ZEITGEIST_EVENT_INTERPRETATION,
  ZEITGEIST_EVENT_MANIFESTATION,
  ZEITGEIST_EVENT_ACTOR,
  ZEITGEIST_EVENT_ORIGIN,
} ZeitgeistEventDataOffset;

typedef struct
{
  guint32     id;
  gint64      timestamp;
  gchar      *interpretation;
  gchar      *manifestation;
  gchar      *actor;
  gchar      *origin;
  GPtrArray  *subjects;
  GByteArray *payload;
} ZeitgeistEventPrivate;

/**
 * zeitgeist_event_get_id:
 * @event: The event to get the event id for
 *
 * Get the event id as assigned by the Zeitgeist engine.
 *
 * Returns: The event id or 0 if it's unset. An event retrieved from the
 *          Zeitgeist engine will always have an event id.
 */
guint32
zeitgeist_event_get_id (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), 0);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->id;
}

/**
 * zeitgeist_event_set_id:
 * @event: The event to get the event id for
 * @event_id: The event id to assign to @event
 *
 * Set the event id of an event. Note that it is an error to send an event
 * with a pre set event id to zeitgeist_log_insert_events().
 */
void
zeitgeist_event_set_id (ZeitgeistEvent *event, guint32 id)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  priv->id = id;
}

/**
 * zeitgeist_event_get_timestamp:
 * @event: The event to get the timestamp for
 *
 * Get the event timestamp. The timestamp is in milliseconds since the
 * Unix epoch. There are a few helpers available for converting to and
 * from other time representations such a #GTimeVal. See for example
 * zeitgeist_timestamp_to_timeval() and zeitgeist_timestamp_from_timeval().
 *
 * Returns: The event timestamp. Note that 0 is ambiguous as it denotes both
 *          an unset timestamp and the time of the Unix Epoch.
 */
gint64 
zeitgeist_event_get_timestamp (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), 0);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->timestamp;
}

/**
 * zeitgeist_event_set_timestamp:
 * @event: The event to set the timestamp for
 *
 * Set the event timestamp. The timestamp is in milliseconds since the
 * Unix epoch. There are a few helpers available for converting to and
 * from other time representations such a #GTimeVal. See for example
 * zeitgeist_timestamp_to_timeval() and zeitgeist_timestamp_from_timeval().
 *
 * Note that the if you insert events into the Zeitgeist log without a
 * timestamp set the Zeiteist daemon will automatically assign the timestamp
 * of the logging time to the event.
 */
void
zeitgeist_event_set_timestamp (ZeitgeistEvent *event, gint64 timestamp)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  priv->timestamp = timestamp;
}

/**
 * zeitgeist_event_get_interpretation:
 * @event: The event to get the interpretation of
 *
 * The event interpretation represents &quot;what happened&quot;. It is encoded
 * as URI defined by the Zeitgeist Event Ontology.
 * Examples could be &quot;something was opened&quot; or
 * &quot;something was modified&quot;.
 *
 * FIXME: Needs link to ontology and some defines to help coding
 *
 * Returns: The event interpretation as a URI or %NULL if unset
 */
const gchar*
zeitgeist_event_get_interpretation (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->interpretation;
}

/**
 * zeitgeist_event_set_interpretation:
 * @event: The event to set the interpretation of
 * @interpretation: URI designating the interpretation type of the event
 *
 * The event interpretation represents &quot;what happened&quot;. It is encoded
 * as URI defined by the Zeitgeist Event Ontology.
 *
 * FIXME: Needs link to ontology and some defines to help coding
 */
void
zeitgeist_event_set_interpretation (ZeitgeistEvent *event,
                                    const gchar    *interpretation)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  gchar* copy = g_strdup (interpretation);

  if (priv->interpretation)
    {
      g_free (priv->interpretation);
    }
  
  priv->interpretation = copy;
}

/**
 * zeitgeist_event_get_manifestation:
 * @event: The event to get the manifestation of
 *
 * The event manifestation represents &quot;how did it happen&quot;.
 * It is encoded as URI defined by the Zeitgeist Event Ontology. Examples
 * could be &quot;the user did it&quot; or
 * &quot;the system send a notification&quot;.
 *
 * FIXME: Needs link to ontology and some defines to help coding
 *
 * Returns: The event interpretation as a URI or %NULL if unset
 */
const gchar*
zeitgeist_event_get_manifestation (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->manifestation;
}

/**
 * zeitgeist_event_set_manifestation:
 * @event: The event to set the manifestation of
 * @interpretation: URI designating the manifestation type of the event
 *
 * The event manifestation represents &quot;how did it happen&quot;.
 * It is encoded as URI defined by the Zeitgeist Event Ontology.
 *
 * FIXME: Needs link to ontology and some defines to help coding
 */
void
zeitgeist_event_set_manifestation (ZeitgeistEvent *event,
                                   const gchar    *manifestation)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  gchar* copy = g_strdup (manifestation);

  if (priv->manifestation)
    {
      g_free (priv->manifestation);
    }
  
  priv->manifestation = copy;
}

/**
 * zeitgeist_event_get_actor:
 * @event: The event to set the actor for
 *
 * Get the event actor. The actor represents the party responsible for
 * triggering the event. When the actor is an application
 * (which it almost always is) the actor is encoded in the
 * <emphasis>application://</emphasis> URI scheme with the base name of the .desktop
 * file for the application appended. Eg. <emphasis>application://firefox.desktop</emphasis>
 *
 * Returns: A URI designating the actor of the event
 */
const gchar* 
zeitgeist_event_get_actor (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->actor;
}

/**
 * zeitgeist_event_set_actor:
 * @event: The event to set the actor for
 * @actor: URI designating the actor triggering the event.
 *         Fx. <emphasis>application://firefox.desktop</emphasis>
 *
 * Get the event actor. The actor represents the party responsible for
 * triggering the event. When the actor is an application
 * (which it almost always is) the actor is encoded in the
 * <emphasis>application://</emphasis> URI scheme with the base name of the .desktop
 * file for the application appended. Eg. <emphasis>application://firefox.desktop</emphasis>
 */
void
zeitgeist_event_set_actor (ZeitgeistEvent *event,
                           const gchar    *actor)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  gchar* copy = g_strdup (actor);

  if (priv->actor)
    {
      g_free (priv->actor);
    }
  
  priv->actor = copy;
}

void
zeitgeist_event_set_actor_from_app_info (ZeitgeistEvent *event,
                                         GAppInfo       *appinfo)
{
  const gchar *app_id;
  gchar       *copy;
  
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));
  g_return_if_fail (G_IS_APP_INFO (appinfo));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  copy = NULL;
  app_id = g_app_info_get_id (appinfo);
  
  if (app_id != NULL)
    {      
      copy = g_strconcat ("application://", app_id, NULL);
    }
  else if (G_IS_DESKTOP_APP_INFO (appinfo) &&
           g_desktop_app_info_get_filename (G_DESKTOP_APP_INFO (appinfo)))
    {
      const gchar *path = g_desktop_app_info_get_filename (
                                                  G_DESKTOP_APP_INFO (appinfo));
      gchar *_app_id = g_path_get_basename (path);
      copy = g_strconcat ("application://", _app_id, NULL);
      g_free (_app_id);
    }
  else
    {
      /* Sometimes the name is set, but not the id... So try that */
      app_id = g_app_info_get_name (appinfo);
      if (app_id != NULL)
        {
          copy = g_strconcat ("application://", app_id, ".desktop", NULL);
        }
    }

  if (priv->actor)
    {
      g_free (priv->actor);
    }
  priv->actor = copy;
}

/**
 * zeitgeist_event_get_subject:
 * @event: The event to get a subject for
 * @index: The 0-based offset of the subject
 *
 * Get the n'th subject of this event. You can find the number of subjects
 * by calling zeitgeist_event_num_subjects().
 *
 * Returns: The subject at position @index. Do not free. If you want to
 *          keep the subject around you need to g_object_ref() it.
 */
ZeitgeistSubject*
zeitgeist_event_get_subject (ZeitgeistEvent *event,
                             gint            index)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate *priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  g_return_val_if_fail (index < priv->subjects->len, NULL);
  return ZEITGEIST_SUBJECT (g_ptr_array_index (priv->subjects, index));
}

/**
 * zeitgeist_event_num_subjects:
 * @event: The event to get the number of subjects for
 *
 * Get the number of subjects for an event. This is a constant time operation.
 *
 * Returns: The number of subjects for this event.
 */
gint
zeitgeist_event_num_subjects (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), 0);

  ZeitgeistEventPrivate *priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  return priv->subjects->len;
}

/**
 * zeitgeist_event_add_subject:
 * @event: The event to add a subject to
 * @subject: The subject to add
 *
 * Append a #ZeitgeistSubject to the list of subjects for @event. The
 * event will consume the floating reference on @subject when you call this
 * method.
 */
void
zeitgeist_event_add_subject (ZeitgeistEvent   *event,
                             ZeitgeistSubject *subject)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  g_ptr_array_add (priv->subjects, subject);
  g_object_ref_sink (subject);
}

/**
 * zeitgeist_event_get_origin:
 * @event: The event to get the origin from
 *
 * Get the origin of an event.
 * This differs from a subject's origin, as it describes where the event comes
 * from, not where it resides.
 *
 * Returns: The origin of @event.
 *
 * Since: 0.3.14
 */
const gchar*
zeitgeist_event_get_origin (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);
  
  return priv->origin;
}

/**
 * zeitgeist_event_set_origin:
 * @event: The event to set the origin of
 * @origin: The origin to set
 *
 * Set the origin of an event.
 * This differs from a subject's origin, as it describes where the event comes
 * from, not where it resides.
 *
 * Since: 0.3.14
 */
void
zeitgeist_event_set_origin (ZeitgeistEvent *event,
                            const gchar    *origin)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  gchar* copy = g_strdup (origin);

  if (priv->origin)
    {
      g_free (priv->origin);
    }
  
  priv->origin = copy;
}

/**
 * zeitgeist_event_get_payload:
 * @event: The event to get the payload for
 *
 * Look up the free form binary payload of @event.
 *
 * Returns: The event payload or %NULL if unset. Do not free. If you want to
 *          keep the subject around you need to g_byte_array_ref() it.
 */
GByteArray*
zeitgeist_event_get_payload (ZeitgeistEvent *event)
{
  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  return priv->payload;
}

/**
 * zeitgeist_event_set_payload:
 * @event: Event to add the payload to
 * @payload: (transfer-full): The payload to add to @event
 *
 * Attach a a free form binary payload to @event. Payloads are application
 * specific and can not be assumed to have any particular format unless
 * you have other contextual information about the event.
 *
 * The event will assume ownership of @payload. You should never call
 * g_byte_array_free() on @payload and only call g_byte_array_unref() on it if
 * you have added an extra reference to it.
 */
void
zeitgeist_event_set_payload (ZeitgeistEvent *event,
                             GByteArray     *payload)
{
  g_return_if_fail (ZEITGEIST_IS_EVENT (event));

  ZeitgeistEventPrivate* priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  if (priv->payload)
    {
      g_byte_array_unref (priv->payload);
    }

  priv->payload = payload;
}

static void
zeitgeist_event_init (ZeitgeistEvent *object)
{
  ZeitgeistEventPrivate *priv;
    
  priv = ZEITGEIST_EVENT_GET_PRIVATE (object);

  priv->id = 0;
  priv->timestamp = 0;
  priv->interpretation = NULL;
  priv->manifestation = NULL;
  priv->actor = NULL;
  priv->origin = NULL;
  priv->subjects = g_ptr_array_new_with_free_func (
                                                (GDestroyNotify)g_object_unref);
  priv->payload = NULL;
}

static void
zeitgeist_event_finalize (GObject *object)
{
  ZeitgeistEvent *event = ZEITGEIST_EVENT (object);
  ZeitgeistEventPrivate *priv;
  
  priv = ZEITGEIST_EVENT_GET_PRIVATE (event);

  if (priv->subjects)
    {
      /* Subjects are unreffed by the free-func of the GPtrArray */
      g_ptr_array_unref (priv->subjects);
      priv->subjects = NULL;
    }
  
  zeitgeist_event_set_interpretation (event, NULL);
  zeitgeist_event_set_manifestation (event, NULL);
  zeitgeist_event_set_actor (event, NULL);
  zeitgeist_event_set_origin (event, NULL);
  zeitgeist_event_set_payload (event, NULL);

  G_OBJECT_CLASS (zeitgeist_event_parent_class)->finalize (object);
  
}

static void
zeitgeist_event_class_init (ZeitgeistEventClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize = zeitgeist_event_finalize;

  g_type_class_add_private (object_class, sizeof (ZeitgeistEventPrivate));
}

/** 
 * zeitgeist_event_new:
 *
 * Create a new empty event structure
 *
 * Returns: A newly create #ZeitgeistEvent instance. The returned event will
 *          have a floating reference which will be consumed if you pass the
 *          event to any of the methods provided by this library. If you
 *          do not do that then you must free the event youself with
 *          g_object_unref()
 */
ZeitgeistEvent* 
zeitgeist_event_new (void)
{
  ZeitgeistEvent *self;

  self = g_object_new (ZEITGEIST_TYPE_EVENT, NULL);
  return self;
}

// FIXME: add `origin' parameter
/** 
 * zeitgeist_event_new_full:
 * @interpretation: The interpretation type of the event.
 *                  See #ZEITGEIST_ZG_EVENT_INTERPRETATION for a list of
 *                  event interpretation types
 * @manifestation: The manifestation type of the event.
 *                 See #ZEITGEIST_ZG_EVENT_MANIFESTATION for a list of
 *                  event manifestation types
 * @actor: The actor triggering the event. See zeitgeist_event_set_actor()
 *         for details on how to encode this.
 * @VarArgs: A list of #ZeitgeistSubject instances terminated by a %NULL
 * 
 * Create a new event structure with predefined data
 *
 * Returns: A newly create #ZeitgeistEvent instance. The returned event will
 *          have a floating reference which will be consumed if you pass the
 *          event to any of the methods provided by this library. If you
 *          do not do that then you must free the event yourself with
 *          g_object_unref()
 */
ZeitgeistEvent* 
zeitgeist_event_new_full (const gchar *interpretation,
                          const gchar *manifestation,
                          const gchar *actor,
                          ...)
{
  va_list         args;
  ZeitgeistEvent *self;

  va_start (args, actor);
  self = zeitgeist_event_new_full_valist (interpretation, manifestation,
                                          actor, args);
  va_end (args);
    
  return self;
}
  
/** 
 * zeitgeist_event_new_full_valist:
 * interpretation: The interpretation type of the event.
 *                  See #ZEITGEIST_ZG_EVENT_INTERPRETATION for a list of
 *                  event interpretation types
 * @manifestation: The manifestation type of the event.
 *                 See #ZEITGEIST_ZG_EVENT_MANIFESTATION for a list of
 *                  event manifestation types
 * @actor: The actor triggering the event. See zeitgeist_event_set_actor()
 *         for details on how to encode this.
 * @args: A %va_list of #ZeitgeistSubject<!-- -->s terminated by %NULL
 * 
 * As zeitgeist_event_new_full() but intended for language bindings
 */
ZeitgeistEvent* 
zeitgeist_event_new_full_valist (const gchar *interpretation,
                                 const gchar *manifestation,
                                 const gchar *actor,
                                 va_list      args)
{
  ZeitgeistEvent *self;
  ZeitgeistSubject *subject = NULL;
  
  self = g_object_new (ZEITGEIST_TYPE_EVENT, NULL);
  zeitgeist_event_set_interpretation (self, interpretation);
  zeitgeist_event_set_manifestation (self, manifestation);
  zeitgeist_event_set_actor (self, actor);

  subject = va_arg (args, ZeitgeistSubject*);
  while (subject != NULL)
    {
      g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);
      zeitgeist_event_add_subject (self, subject);
      subject = va_arg (args, ZeitgeistSubject*);
    } 
  
    
  return self;
}

/**
 * zeitgeist_event_new_from_variant:
 * @event: A #GVariant with signature defined in
 *         #ZEITGEIST_EVENT_VARIANT_SIGNATURE. If @event is a floating
 *         reference the floating reference will be consumed.
 *
 * Parse the data in a #GVariant and build a #ZeitgeistEvent from it.
 * The reverse operation of this is zeitgeist_event_to_variant().
 *
 * Returns: A newly allocated #ZeitgeistEvent filled with the metadata,
 *          subjects, and payload described by @event. The returned event will
 *          have a floating reference which will be consumed if you pass the
 *          event to any of the methods provided by this library. If you
 *          do not do that then you must free the event yourself with
 *          g_object_unref()
 */
ZeitgeistEvent*
zeitgeist_event_new_from_variant (GVariant *event)
{
  ZeitgeistEvent   *result;
  ZeitgeistSubject *subject;
  GVariantIter     *event_data, *subjects, *payload_data, *subject_data;
  gchar            *str;
  GByteArray       *payload;
  gint              payload_size;
  guchar            payload_byte;

  g_return_val_if_fail (event != NULL, NULL);

  g_variant_ref_sink (event);

  result = zeitgeist_event_new ();
  g_variant_get (event, ZEITGEIST_EVENT_VARIANT_SIGNATURE,
                 &event_data, &subjects, &payload_data);

  /* Parse event data */
  if (g_variant_iter_n_children (event_data) < 5)
    {
      g_critical ("Event data truncated at length %lu",
                  g_variant_iter_n_children (event_data));
      goto cleanup;
    }

  // FIXME: Use g_variant_get_string() to cause less reallocations
  g_variant_iter_next (event_data, "s", &str);
  zeitgeist_event_set_id (result, g_ascii_strtoull (str, NULL, 0));
  g_free (str);

  g_variant_iter_next (event_data, "s", &str);
  zeitgeist_event_set_timestamp (result,  g_ascii_strtoll (str, NULL, 0));
  g_free (str);

  g_variant_iter_next (event_data, "s", &str);
  zeitgeist_event_set_interpretation (result,  str[0] == '\0' ? NULL : str);
  g_free (str);

  g_variant_iter_next (event_data, "s", &str);
  zeitgeist_event_set_manifestation (result,  str[0] == '\0' ? NULL : str);
  g_free (str);

  g_variant_iter_next (event_data, "s", &str);
  zeitgeist_event_set_actor (result,  str[0] == '\0' ? NULL : str);
  g_free (str);

  if (g_variant_iter_loop (event_data, "s", &str))
    zeitgeist_event_set_origin (result, str[0] == '\0' ? NULL : str);

  /* Build the list of subjects */
  while (g_variant_iter_loop (subjects, "as", &subject_data))
  {
    /* Parse subject data */
    if (g_variant_iter_n_children (subject_data) < 7)
      {
        g_critical ("Subject data truncated at length %lu",
                    g_variant_iter_n_children (subject_data));
        goto cleanup;
      }

    subject = g_object_new (ZEITGEIST_TYPE_SUBJECT, NULL);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_uri (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_interpretation (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_manifestation (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_origin (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_mimetype (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_text (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    g_variant_iter_next (subject_data, "s", &str);
    zeitgeist_subject_set_storage (subject, str[0] == '\0' ? NULL : str);
    g_free (str);

    if (g_variant_iter_loop (subject_data, "s", &str))
      zeitgeist_subject_set_current_uri (subject, str[0] == '\0' ? NULL : str);

    zeitgeist_event_add_subject (result, subject);
  }

  /* Construct the event payload if necessary */
  payload_size = g_variant_iter_n_children (payload_data);
  if (payload_size > 0)
  {
    payload = g_byte_array_sized_new (payload_size);
    while (g_variant_iter_next (payload_data, "y", &payload_byte))
    {
      // FIXME: Implicit guchar <-> uint8 conversion..?
      g_byte_array_append (payload, &payload_byte, 1);
    }
    zeitgeist_event_set_payload (result, payload);
  }

  cleanup:
    g_variant_iter_free (event_data);
    g_variant_iter_free (subjects);
    g_variant_iter_free (payload_data);
    g_variant_unref (event);

  return result;
}

/**
 * zeitgeist_event_to_variant:
 * @event: The #ZeitgeistEvent to serialize to a #GVariant
 *
 * Serialize a #ZeitgeistEvent to a #GVariant. The reverse operation
 * is zeitgeist_event_new_from_variant().
 *
 * Returns: A floating reference to a #GVariant with signature
 *          #ZEITGEIST_EVENT_VARIANT_SIGNATURE
 */
GVariant*
zeitgeist_event_to_variant (ZeitgeistEvent *event)
{
  GVariant         *var;
  GVariantBuilder   b;
  gchar            *buf;
  const gchar      *bif;
  int               i, n_subjects;
  ZeitgeistSubject *su;
  GByteArray       *payload;

  g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);

  g_variant_builder_init (&b, ZEITGEIST_EVENT_VARIANT_TYPE);

  /* Build event data */
  g_variant_builder_open (&b, G_VARIANT_TYPE ("as"));
  if (zeitgeist_event_get_id (event) == 0)
    buf = g_strdup ("");
  else
    buf = g_strdup_printf ("%"G_GUINT32_FORMAT, zeitgeist_event_get_id (event));
  g_variant_builder_add (&b, "s", buf);
  g_free (buf);
  if (zeitgeist_event_get_timestamp (event) == 0)
    buf = g_strdup ("");
  else
    buf = g_strdup_printf ("%"G_GINT64_FORMAT, zeitgeist_event_get_timestamp (event));
  g_variant_builder_add (&b, "s", buf);
  g_free (buf);
  g_variant_builder_add (&b, "s", (bif = zeitgeist_event_get_interpretation(event), bif ? bif : ""));
  g_variant_builder_add (&b, "s", (bif = zeitgeist_event_get_manifestation(event), bif ? bif : ""));
  g_variant_builder_add (&b, "s", (bif = zeitgeist_event_get_actor (event), bif ? bif : ""));
  g_variant_builder_add (&b, "s", (bif = zeitgeist_event_get_origin (event), bif ? bif : ""));
  g_variant_builder_close (&b);

  /* Build subjects */
  g_variant_builder_open (&b, G_VARIANT_TYPE ("aas"));
  n_subjects = zeitgeist_event_num_subjects(event);
  for (i = 0; i < n_subjects; i++)
    {
      su = zeitgeist_event_get_subject (event, i);
      g_variant_builder_open (&b, G_VARIANT_TYPE ("as"));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_uri(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_interpretation(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_manifestation(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_origin(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_mimetype(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_text(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_storage(su), bif ? bif : ""));
      g_variant_builder_add (&b, "s", (bif = zeitgeist_subject_get_current_uri(su), bif ? bif : ""));
      g_variant_builder_close (&b);
    }
  g_variant_builder_close (&b);

  /* Build payload */
  g_variant_builder_open (&b, G_VARIANT_TYPE ("ay"));
  payload = zeitgeist_event_get_payload (event);
  if (payload != NULL)
    {
      for (i = 0; i < payload->len; i++)
        {
          g_variant_builder_add (&b, "y", payload->data[i]);
        }
    }
  g_variant_builder_close (&b);

  return g_variant_builder_end (&b);
}

/**
 * zeitgeist_events_to_variant:
 * @events: A #GPtrArray of #ZeitgeistEvent<!-- -->s. If the events has
 *          floating references they will be consumed. Furthermore the
 *          reference on the #GPtrArray itself will also be stolen and its
 *          @free_func set to %NULL
 *
 * Convert a set of #ZeitgeistEvent<-- -->s to a #GVariant with signature
 * as an array of #ZEITGEIST_EVENT_VARIANT_SIGNATURE.
 *
 * Returns: A floating reference to a #GVariant as described above. Unless the
 *          floating reference is consumed somewhere you must free it with
 *          g_variant_unref().
 */
GVariant*
zeitgeist_events_to_variant (GPtrArray *events)
{
  GVariantBuilder  b;
  ZeitgeistEvent  *event;
  GVariant        *vevent;
  int              i;

  g_return_val_if_fail (events != NULL, NULL);

  g_variant_builder_init (&b,
                          G_VARIANT_TYPE ("a"ZEITGEIST_EVENT_VARIANT_SIGNATURE));

  for (i = 0; i < events->len; i++)
    {
      event = ZEITGEIST_EVENT (g_ptr_array_index (events, i));
      g_object_ref_sink (event);
      vevent = zeitgeist_event_to_variant (event);
      g_variant_builder_add_value (&b, vevent);
      g_object_unref (event);
    }

  /* We need to unset the free func because we can't risk double unreffing
   * the events held in it, quite ugly but there is no other way */
  g_ptr_array_set_free_func (events, NULL);
  g_ptr_array_unref (events);

  return g_variant_builder_end (&b);
}

/**
 * zeitgeist_events_from_variant:
 * @events: A #GVariant  with signature as an array of
 *          #ZEITGEIST_EVENT_VARIANT_SIGNATURE. If @event is floating this
 *          method will consume the floating reference.
 *
 * Returns: A reference to a #GPtrArray of #ZeitgeistEvent<!-- -->s.
 *          All the events will be floating references, and the
 *          #GPtrArray<!-- -->'s @free_func will be set to g_object_unref().
 */
GPtrArray*
zeitgeist_events_from_variant (GVariant *events)
{
  GPtrArray      *result;
  GVariant       *vevent;
  ZeitgeistEvent *event;
  int             i, n_events;

  g_return_val_if_fail (events != NULL, NULL);

  g_variant_ref_sink (events);

  n_events = g_variant_n_children (events);
  result = g_ptr_array_sized_new (n_events);
  g_ptr_array_set_free_func (result, (GDestroyNotify) g_object_unref);

  for (i = 0; i < n_events; i++)
    {
      vevent = g_variant_get_child_value (events, i);
      event = zeitgeist_event_new_from_variant (vevent);
      g_variant_unref (vevent);
      g_ptr_array_add (result, event);
    }

  g_variant_unref (events);

  return result;
}

/**
 * zeitgeist_events_from_valist:
 * @events: a #va_list of #ZeitgeistEvent<!-- -->s.
 *
 * Utility function to convert a #va_list of #ZeitgeistEvent<!-- --> into a
 * #GPtrArray containing these events.
 *
 * Returns: A #GPtrArray of #ZeitgeistEvent<!-- -->s. The events are not further
 *          reffed and will still be floating if they where so when you called
 *          this method. The pointer array will have its @free_func set to
 *          g_object_unref()
 */
GPtrArray*
zeitgeist_events_from_valist (va_list events)
{
  ZeitgeistEvent *event = NULL;
  GPtrArray *result;

  result = g_ptr_array_new_with_free_func ((GDestroyNotify) g_object_unref);

  event = va_arg (events, ZeitgeistEvent*);
  while (event != NULL)
    {
      g_return_val_if_fail (ZEITGEIST_IS_EVENT (event), NULL);
      g_ptr_array_add (result, event);
      event = va_arg (events, ZeitgeistEvent*);
    }

  return result;
}
