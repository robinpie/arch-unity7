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
 * Authored by Mikkel Kamstrup Erlandnsen <mikkel.kamstrup@canonical.com>
 */

/**
 * SECTION:zeitgeist-subject 
 * @short_description: #ZeitgeistSubject
 * @include: zeitgeist-subject.h
 *
 * #ZeitgeistSubject.
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-subject.h"

G_DEFINE_TYPE (ZeitgeistSubject, zeitgeist_subject, G_TYPE_INITIALLY_UNOWNED);
#define ZEITGEIST_SUBJECT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_SUBJECT, ZeitgeistSubjectPrivate))

typedef enum
{
  ZEITGEIST_SUBJECT_URI,
  ZEITGEIST_SUBJECT_INTERPRETATION,
  ZEITGEIST_SUBJECT_MANIFESTATION,
  ZEITGEIST_SUBJECT_MIMETYPE,
  ZEITGEIST_SUBJECT_ORIGIN,
  ZEITGEIST_SUBJECT_TEXT,
  ZEITGEIST_SUBJECT_STORAGE,
  ZEITGEIST_SUBJECT_CURRENT_URI,
} ZeitgeistSubjectDataOffset;

typedef struct
{
  gchar *uri;
  gchar *interpretation;
  gchar *manifestation;
  gchar *mimetype;
  gchar *origin;
  gchar *text;
  gchar *storage;
  gchar *current_uri;
} ZeitgeistSubjectPrivate;

const gchar*
zeitgeist_subject_get_uri (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->uri;
}

void   
zeitgeist_subject_set_uri (ZeitgeistSubject *subject, 
                           const gchar *uri)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (uri);

  if (priv->uri)
    {
      g_free (priv->uri);
    }
  
  priv->uri = copy;
}

const gchar* 
zeitgeist_subject_get_interpretation (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->interpretation;
}

void   
zeitgeist_subject_set_interpretation (ZeitgeistSubject *subject, 
                           const gchar *interpretation)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (interpretation);

  if (priv->interpretation)
    {
      g_free (priv->interpretation);
    }
  
  priv->interpretation = copy;
}

const gchar* 
zeitgeist_subject_get_manifestation (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->manifestation;
}

void   
zeitgeist_subject_set_manifestation (ZeitgeistSubject *subject, 
                           const gchar *manifestation)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (manifestation);

  if (priv->manifestation)
    {
      g_free (priv->manifestation);
    }
  
  priv->manifestation = copy;
}

const gchar* 
zeitgeist_subject_get_mimetype (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->mimetype;
}

void   
zeitgeist_subject_set_mimetype (ZeitgeistSubject *subject, 
                           const gchar *mimetype)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (mimetype);

  if (priv->mimetype)
    {
      g_free (priv->mimetype);
    }
  
  priv->mimetype = copy;
}

const gchar* 
zeitgeist_subject_get_origin (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->origin;
}

void   
zeitgeist_subject_set_origin (ZeitgeistSubject *subject, 
                           const gchar *origin)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (origin);

  if (priv->origin)
    {
      g_free (priv->origin);
    }
  
  priv->origin = copy;
}

const gchar* 
zeitgeist_subject_get_text (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->text;
}

void   
zeitgeist_subject_set_text (ZeitgeistSubject *subject, 
                            const gchar *text)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (text);

  if (priv->text)
    {
      g_free (priv->text);
    }
  
  priv->text = copy;
}

const gchar* 
zeitgeist_subject_get_storage (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->storage;
}

void   
zeitgeist_subject_set_storage (ZeitgeistSubject *subject, 
                               const gchar *storage)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));
  
  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  
  gchar* copy = g_strdup (storage);

  if (priv->storage)
    {
      g_free (priv->storage);
    }
  
  priv->storage = copy;
}

/**
 * zeitgeist_subject_get_current_uri:
 * @subject: The subject to get the current_uri from
 *
 * Get the current_uri of a subject.
 *
 * This is the updated URI taking into account possible relocations of
 * the resource. It is illegal to insert an event with subjects having
 * a `current_uri' different than their `uri'.
 *
 * A special case is events with #ZEITGEIST_ZG_MOVE_EVENT interpretation.
 * In this case, `current_uri' is the destination to which `uri' is being
 * moved, and they are expected to be different.
 *
 * Returns: The current_uri of @subject.
 *
 * Since: 0.3.14
 */
const gchar*
zeitgeist_subject_get_current_uri (ZeitgeistSubject *subject)
{
  g_return_val_if_fail (ZEITGEIST_IS_SUBJECT (subject), NULL);

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);
  return priv->current_uri;
}

/**
 * zeitgeist_subject_set_current_uri:
 * @subject: The subject to set the current_uri for
 * @current_uri: The current_uri to set
 *
 * Set the current_uri of a subject.
 *
 * This is the updated URI taking into account possible relocations of
 * the resource. It is illegal to insert an event with subjects having
 * a `current_uri' different than their `uri'.
 *
 * A special case is events with #ZEITGEIST_ZG_MOVE_EVENT interpretation.
 * In this case, `current_uri' is the destination to which `uri' is being
 * moved, and they are expected to be different.
 *
 * Since: 0.3.14
 */
void
zeitgeist_subject_set_current_uri (ZeitgeistSubject *subject,
                                   const gchar *current_uri)
{
  g_return_if_fail (ZEITGEIST_IS_SUBJECT (subject));

  ZeitgeistSubjectPrivate *priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);

  gchar* copy = g_strdup (current_uri);

  if (priv->current_uri)
    {
      g_free (priv->current_uri);
    }

  priv->current_uri = copy;
}

static void
zeitgeist_subject_init (ZeitgeistSubject *object)
{
  ZeitgeistSubjectPrivate *priv;

  priv =  ZEITGEIST_SUBJECT_GET_PRIVATE (object);
  priv->uri = NULL;
  priv->interpretation = NULL;
  priv->manifestation = NULL;
  priv->mimetype = NULL;
  priv->origin = NULL;
  priv->text = NULL;
  priv->storage = NULL;
  priv->current_uri = NULL;
}

static void
zeitgeist_subject_finalize (GObject *object)
{
  ZeitgeistSubject *subject = ZEITGEIST_SUBJECT (object);
  ZeitgeistSubjectPrivate *priv;
  
  priv = ZEITGEIST_SUBJECT_GET_PRIVATE (subject);

  zeitgeist_subject_set_uri (subject, NULL);
  zeitgeist_subject_set_interpretation (subject, NULL);
  zeitgeist_subject_set_manifestation (subject, NULL);
  zeitgeist_subject_set_mimetype (subject, NULL);
  zeitgeist_subject_set_origin (subject, NULL);
  zeitgeist_subject_set_text (subject, NULL);
  zeitgeist_subject_set_storage (subject, NULL);
  zeitgeist_subject_set_current_uri (subject, NULL);
  
  G_OBJECT_CLASS (zeitgeist_subject_parent_class)->finalize (object);
}

static void
zeitgeist_subject_class_init (ZeitgeistSubjectClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize = zeitgeist_subject_finalize;

  g_type_class_add_private (object_class, sizeof (ZeitgeistSubjectPrivate));
}

/** 
 * zeitgeist_subject_new:
 * 
 * Create a new empty subject structure
 *
 * Returns: A newly create #ZeitgeistSubject instance. The returned subject will
 *          have a floating reference which will be consumed if you pass the
 *          event to any of the methods provided by this library (like
 *          adding it to an event). If you  do not do that then you must free
 *          the subject youself with g_object_unref()
 */
ZeitgeistSubject*
zeitgeist_subject_new (void)
{
  return g_object_new (ZEITGEIST_TYPE_SUBJECT, NULL);
}

// FIXME: add `current_uri' parameter
/** 
 * zeitgeist_subject_new_full:
 * @uri: The URI or URL of the subject
 * @interpretation: The interpretation type of the subject. See the list of
 *                  <link linkend="zeitgeist-1.0-Interpretation-Ontology">interpretation types</link>
 * @manifestation: The manifestation type of the subject. See the list of
 *                  <link linkend="zeitgeist-1.0-Manifestation-Ontology">manifestation types</link>
 * @mimetype: The mimetype of the subject. Eg. <emphasis>text/plain</emphasis>
 * @origin: The origin of the subject. See zeitgeist_subject_set_origin()
 *          for details
 * @text: A small textual representation of the subject suitable for display
 * @storage: String identifier for the storage medium the subject is on.
 *           Se zeitgeist_subject_set_storage() for details
 * Create a new subject structure with predefined data
 *
 * Returns: A newly create #ZeitgeistSubject instance. The returned subject will
 *          have a floating reference which will be consumed if you pass the
 *          event to any of the methods provided by this library (like
 *          adding it to an event). If you  do not do that then you must free
 *          the subject youself with g_object_unref()
 */
ZeitgeistSubject* 
zeitgeist_subject_new_full (const gchar *uri,
                            const gchar *interpretation,
                            const gchar *manifestation,
                            const gchar *mimetype,
                            const gchar *origin,
                            const gchar *text,
                            const gchar *storage)
{
  ZeitgeistSubject *self;

  self = zeitgeist_subject_new ();
  zeitgeist_subject_set_uri (self, uri);
  zeitgeist_subject_set_interpretation (self, interpretation);
  zeitgeist_subject_set_manifestation (self, manifestation);
  zeitgeist_subject_set_mimetype (self, mimetype);
  zeitgeist_subject_set_origin (self, origin);
  zeitgeist_subject_set_text (self, text);
  zeitgeist_subject_set_storage (self, storage);

  return self;
}
