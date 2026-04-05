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
 * Authored by:
 *               Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

/**
 * SECTION:zeitgeist-simple-result-set
 * @short_description: Internal API do not use
 *
 * Simple implementation of a #ZeitgeistResultSet on top of a #GptrArray
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "zeitgeist-simple-result-set.h"

static void zeitgeist_simple_result_set_result_set_iface_init (ZeitgeistResultSetIface *iface);
G_DEFINE_TYPE_WITH_CODE (ZeitgeistSimpleResultSet,
                         zeitgeist_simple_result_set,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ZEITGEIST_TYPE_RESULT_SET,
                                                zeitgeist_simple_result_set_result_set_iface_init))

#define ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE(obj, ZEITGEIST_TYPE_SIMPLE_RESULT_SET, ZeitgeistSimpleResultSetPrivate))

typedef struct
{
  GPtrArray *events;
  guint      estimated_matches;
  guint      cursor;
} ZeitgeistSimpleResultSetPrivate;

/* GObject Init */
static void
zeitgeist_simple_result_set_finalize (GObject *object)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (object);
  
  if (priv->events)
    g_ptr_array_unref (priv->events);

  G_OBJECT_CLASS (zeitgeist_simple_result_set_parent_class)->finalize (object);
}

static void
zeitgeist_simple_result_set_class_init (ZeitgeistSimpleResultSetClass *klass)
{
  GObjectClass  *obj_class = G_OBJECT_CLASS (klass);

  obj_class->finalize     = zeitgeist_simple_result_set_finalize;  

  /* Add private data */
  g_type_class_add_private (obj_class, sizeof (ZeitgeistSimpleResultSetPrivate));
}

static void
zeitgeist_simple_result_set_init (ZeitgeistSimpleResultSet *self)
{

}

static guint
zeitgeist_simple_result_set_size (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), 0);
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  return priv->events->len;
}

static guint
zeitgeist_simple_result_set_estimated_matches (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), 0);
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  return priv->estimated_matches;
}

static ZeitgeistEvent*
zeitgeist_simple_result_set_next (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  ZeitgeistEvent *next;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), NULL);
  g_return_val_if_fail (zeitgeist_result_set_has_next (self), NULL);
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  next = zeitgeist_result_set_peek (self);
  priv->cursor++;
  return next;
}

static gboolean
zeitgeist_simple_result_set_has_next (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  ZeitgeistEvent *next;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), FALSE);
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  return priv->cursor < priv->events->len;
}

static ZeitgeistEvent*
zeitgeist_simple_result_set_peek (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  ZeitgeistEvent *next;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), NULL);
  
  /* Invariant: priv->cursor < priv->events->len. Ensured by next() */
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  return ZEITGEIST_EVENT (g_ptr_array_index (priv->events, priv->cursor));
}

static void
zeitgeist_simple_result_set_seek (ZeitgeistResultSet *self,
                                  guint               pos)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  ZeitgeistEvent *next;
  
  g_return_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self));
  g_return_if_fail (pos < zeitgeist_result_set_size (self));
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);  
  priv->cursor = pos;
}

static guint
zeitgeist_simple_result_set_tell (ZeitgeistResultSet *self)
{
  ZeitgeistSimpleResultSetPrivate *priv;
  
  g_return_val_if_fail (ZEITGEIST_IS_SIMPLE_RESULT_SET (self), 0);
  
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  return priv->cursor;
}

static void
zeitgeist_simple_result_set_result_set_iface_init (ZeitgeistResultSetIface *iface)
{
  iface->size              = zeitgeist_simple_result_set_size;
  iface->estimated_matches = zeitgeist_simple_result_set_estimated_matches;
  iface->next              = zeitgeist_simple_result_set_next;
  iface->has_next          = zeitgeist_simple_result_set_has_next;
  iface->peek              = zeitgeist_simple_result_set_peek;
  iface->seek              = zeitgeist_simple_result_set_seek;
  iface->tell              = zeitgeist_simple_result_set_tell;
}

/* Internal constructor. Steals the ref to @events */
ZeitgeistResultSet*
_zeitgeist_simple_result_set_new (GPtrArray *events,
                                  guint      estimated_matches)
{
  GObject                         *self;
  ZeitgeistSimpleResultSetPrivate *priv;

  self = g_object_new (ZEITGEIST_TYPE_SIMPLE_RESULT_SET, NULL);
  priv = ZEITGEIST_SIMPLE_RESULT_SET_GET_PRIVATE (self);
  priv->events = events;
  priv->estimated_matches = estimated_matches;
  
  return (ZeitgeistResultSet*)self;
}
