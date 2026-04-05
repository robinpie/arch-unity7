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

/**
 * SECTION:zeitgeist-symbols
 * @short_description: Used to get information about Interpretation and/or
 *                     Manifestation symbols
 * @title: Symbol comprehension
 * @include: zeitgeist.h
 */

#include "zeitgeist-ontology-interpretations.h"
#include "zeitgeist-ontology-manifestations.h"
#include "zeitgeist-symbols.h"

static void _ensure_symbols_loaded   (void);

static gboolean    symbols_loaded = FALSE;
static GHashTable *symbol_uris    = NULL;

typedef struct
{
  gchar *uri;
  GSList *parents;
  GSList *children;
  GSList *all_children;
} SymbolInfo;

static SymbolInfo*
symbol_info_new (gchar *uri,
                 GStrv parents,
                 GStrv children,
                 GStrv all_children)
{
  gchar *iter;
  SymbolInfo *i = g_slice_new0 (SymbolInfo);
  i->uri = g_strdup (uri);

  while (*parents)
    {
      i->parents = g_slist_append (i->parents,
                                   GINT_TO_POINTER (g_quark_from_string (*parents)));
      parents++;
    }

  while (*children)
    {
      i->children = g_slist_append (i->children,
                                    GINT_TO_POINTER (g_quark_from_string (*children)));
      children++;
    }

  while (*all_children)
    {
      i->all_children = g_slist_append (i->all_children,
                                        GINT_TO_POINTER (g_quark_from_string (*all_children)));
      all_children++;
    }

  return i;
}

static void 
zeitgeist_register_symbol (SymbolInfo *i)
{
  if (symbol_uris == NULL)
    symbol_uris = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_insert (symbol_uris, i->uri, i);
}

/**
 * zeitgeist_symbol_get_parents:
 * @symbol: A symbol.
 * 
 * Gets list of immediate parents of the specified symbol.
 *
 * Returns: A newly allocated list of immediate parents of this symbol.
 *          The data elements of the list contain strings which you
 *          do not own, use g_strdup() if you intend to work with them.
 *          Free the list with a call to g_list_free().
 */
GList*
zeitgeist_symbol_get_parents (const gchar *symbol)
{
  GSList *iter;
  GList *result = NULL;
  g_return_val_if_fail (symbol != NULL, NULL);

  _ensure_symbols_loaded ();

  SymbolInfo *info = (SymbolInfo*) g_hash_table_lookup (symbol_uris, symbol);
  g_return_val_if_fail (info != NULL, NULL);

  for (iter = info->parents; iter; iter = iter->next)
  {
    result = g_list_prepend (result,
                             (gchar*) g_quark_to_string (GPOINTER_TO_INT (iter->data)));
  }

  return g_list_reverse (result);
}

/**
 * zeitgeist_symbol_get_children:
 * @symbol: A symbol.
 * 
 * Gets list of immediate children of the specified symbol.
 *
 * Returns: A newly allocated list of immediate children of this symbol.
 *          The data elements of the list contain strings which you
 *          do not own, use g_strdup() if you intend to work with them.
 *          Free the list with a call to g_list_free().
 */
GList*
zeitgeist_symbol_get_children (const gchar *symbol)
{
  GSList *iter;
  GList *result = NULL;
  g_return_val_if_fail (symbol != NULL, NULL);

  _ensure_symbols_loaded ();

  SymbolInfo *info = (SymbolInfo*) g_hash_table_lookup (symbol_uris, symbol);
  g_return_val_if_fail (info != NULL, NULL);

  for (iter = info->children; iter; iter = iter->next)
  {
    result = g_list_prepend (result,
                             (gchar*) g_quark_to_string (GPOINTER_TO_INT (iter->data)));
  }

  return g_list_reverse (result);
}

/**
 * zeitgeist_symbol_get_all_children:
 * @symbol: A symbol.
 * 
 * Gets list of all children of the specified symbol.
 *
 * Returns: A newly allocated list of all children of this symbol (includes
 *          also children of children recursively).
 *          The data elements of the list contain strings which you
 *          do not own, use g_strdup() if you intend to work with them.
 *          Free the list with a call to g_list_free().
 */
GList*
zeitgeist_symbol_get_all_children (const gchar *symbol)
{
  GSList *iter;
  GList *result = NULL;
  g_return_val_if_fail (symbol != NULL, NULL);

  _ensure_symbols_loaded ();

  SymbolInfo *info = (SymbolInfo*) g_hash_table_lookup (symbol_uris, symbol);
  g_return_val_if_fail (info != NULL, NULL);

  for (iter = info->all_children; iter; iter = iter->next)
  {
    result = g_list_prepend (result,
                             (gchar*) g_quark_to_string (GPOINTER_TO_INT (iter->data)));
  }

  return g_list_reverse (result);
}

/**
 * zeitgeist_symbol_is_a:
 * @symbol: A symbol.
 * @parent: Parent symbol.
 * 
 * Determines if @symbol is child of @parent.
 *
 * Returns: True if @symbol is child of @parent (or equal to @parent), 
 *          FALSE otherwise.
 */
gboolean
zeitgeist_symbol_is_a (const gchar *symbol, const gchar *parent)
{
  if (parent == NULL || symbol == NULL) return FALSE;

  _ensure_symbols_loaded ();

  SymbolInfo *info = (SymbolInfo*) g_hash_table_lookup (symbol_uris, parent);
  if (info == NULL) return FALSE;

  GQuark symbol_quark = g_quark_try_string (symbol);
  if (symbol_quark == 0) return FALSE;
  // catch equal symbols
  if (symbol_quark == g_quark_try_string (parent)) return TRUE;

  return g_slist_find (info->all_children, GINT_TO_POINTER (symbol_quark)) != NULL;
}

static void
_ensure_symbols_loaded (void)
{
  gchar **parents;
  gchar **children;
  gchar **all_children;
  gchar *uri;
  SymbolInfo *info;

  if (symbols_loaded) return;

  uri = ZEITGEIST_NFO_EXECUTABLE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_SPREADSHEET;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_MODIFY_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_TRASH;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DATA_CONTAINER;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_CURSOR;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_RASTER_IMAGE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_WEBSITE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_TODO;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_CREATE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_MEDIA_LIST;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 2);
  children[0] = ZEITGEIST_NMM_MUSIC_ALBUM;
  children[1] = NULL;
  all_children = g_new (char*, 2);
  all_children[0] = ZEITGEIST_NMM_MUSIC_ALBUM;
  all_children[1] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_FILESYSTEM;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DATA_CONTAINER;
  parents[1] = NULL;
  children = g_new (char*, 2);
  children[0] = ZEITGEIST_NFO_FILESYSTEM_IMAGE;
  children[1] = NULL;
  all_children = g_new (char*, 2);
  all_children[0] = ZEITGEIST_NFO_FILESYSTEM_IMAGE;
  all_children[1] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_MIND_MAP;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_PERSON_CONTACT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NCO_CONTACT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_CALENDAR;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_CONTACT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NCO_PERSON_CONTACT;
  children[1] = ZEITGEIST_NCO_ORGANIZATION_CONTACT;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NCO_ORGANIZATION_CONTACT;
  all_children[1] = ZEITGEIST_NCO_PERSON_CONTACT;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_RASTER_IMAGE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_IMAGE;
  parents[1] = NULL;
  children = g_new (char*, 2);
  children[0] = ZEITGEIST_NFO_CURSOR;
  children[1] = NULL;
  all_children = g_new (char*, 2);
  all_children[0] = ZEITGEIST_NFO_CURSOR;
  all_children[1] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_EVENT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_LEAVE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_ARCHIVE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DATA_CONTAINER;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_ACCESS_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_VECTOR_IMAGE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_IMAGE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_IMMESSAGE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NMO_MESSAGE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_HTML_DOCUMENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_VIDEO;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_VISUAL;
  parents[1] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NMM_TVSHOW;
  children[1] = ZEITGEIST_NMM_MOVIE;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NMM_TVSHOW;
  all_children[1] = ZEITGEIST_NMM_MOVIE;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_BOOKMARK_FOLDER;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_ACCEPT_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_BOOKMARK;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_EXPIRE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_FONT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_DATA_CONTAINER;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 5);
  children[0] = ZEITGEIST_NFO_FILESYSTEM;
  children[1] = ZEITGEIST_NFO_ARCHIVE;
  children[2] = ZEITGEIST_NFO_FOLDER;
  children[3] = ZEITGEIST_NFO_TRASH;
  children[4] = NULL;
  all_children = g_new (char*, 6);
  all_children[0] = ZEITGEIST_NFO_FILESYSTEM_IMAGE;
  all_children[1] = ZEITGEIST_NFO_ARCHIVE;
  all_children[2] = ZEITGEIST_NFO_FOLDER;
  all_children[3] = ZEITGEIST_NFO_TRASH;
  all_children[4] = ZEITGEIST_NFO_FILESYSTEM;
  all_children[5] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_IMAGE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_VISUAL;
  parents[1] = NULL;
  children = g_new (char*, 4);
  children[0] = ZEITGEIST_NFO_ICON;
  children[1] = ZEITGEIST_NFO_VECTOR_IMAGE;
  children[2] = ZEITGEIST_NFO_RASTER_IMAGE;
  children[3] = NULL;
  all_children = g_new (char*, 5);
  all_children[0] = ZEITGEIST_NFO_ICON;
  all_children[1] = ZEITGEIST_NFO_VECTOR_IMAGE;
  all_children[2] = ZEITGEIST_NFO_CURSOR;
  all_children[3] = ZEITGEIST_NFO_RASTER_IMAGE;
  all_children[4] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_TEXT_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_SOURCE_CODE;
  children[1] = ZEITGEIST_NFO_HTML_DOCUMENT;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NFO_SOURCE_CODE;
  all_children[1] = ZEITGEIST_NFO_HTML_DOCUMENT;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 12);
  children[0] = ZEITGEIST_ZG_DENY_EVENT;
  children[1] = ZEITGEIST_ZG_ACCESS_EVENT;
  children[2] = ZEITGEIST_ZG_EXPIRE_EVENT;
  children[3] = ZEITGEIST_ZG_LEAVE_EVENT;
  children[4] = ZEITGEIST_ZG_CREATE_EVENT;
  children[5] = ZEITGEIST_ZG_MOVE_EVENT;
  children[6] = ZEITGEIST_ZG_ACCEPT_EVENT;
  children[7] = ZEITGEIST_ZG_SEND_EVENT;
  children[8] = ZEITGEIST_ZG_MODIFY_EVENT;
  children[9] = ZEITGEIST_ZG_DELETE_EVENT;
  children[10] = ZEITGEIST_ZG_RECEIVE_EVENT;
  children[11] = NULL;
  all_children = g_new (char*, 12);
  all_children[0] = ZEITGEIST_ZG_DENY_EVENT;
  all_children[1] = ZEITGEIST_ZG_MODIFY_EVENT;
  all_children[2] = ZEITGEIST_ZG_EXPIRE_EVENT;
  all_children[3] = ZEITGEIST_ZG_LEAVE_EVENT;
  all_children[4] = ZEITGEIST_ZG_CREATE_EVENT;
  all_children[5] = ZEITGEIST_ZG_MOVE_EVENT;
  all_children[6] = ZEITGEIST_ZG_ACCEPT_EVENT;
  all_children[7] = ZEITGEIST_ZG_SEND_EVENT;
  all_children[8] = ZEITGEIST_ZG_ACCESS_EVENT;
  all_children[9] = ZEITGEIST_ZG_DELETE_EVENT;
  all_children[10] = ZEITGEIST_ZG_RECEIVE_EVENT;
  all_children[11] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_SEND_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_APPLICATION;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_SOFTWARE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_DOCUMENT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 5);
  children[0] = ZEITGEIST_NFO_TEXT_DOCUMENT;
  children[1] = ZEITGEIST_NFO_PRESENTATION;
  children[2] = ZEITGEIST_NFO_MIND_MAP;
  children[3] = ZEITGEIST_NFO_SPREADSHEET;
  children[4] = NULL;
  all_children = g_new (char*, 9);
  all_children[0] = ZEITGEIST_NFO_SOURCE_CODE;
  all_children[1] = ZEITGEIST_NFO_SPREADSHEET;
  all_children[2] = ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT;
  all_children[3] = ZEITGEIST_NFO_TEXT_DOCUMENT;
  all_children[4] = ZEITGEIST_NFO_HTML_DOCUMENT;
  all_children[5] = ZEITGEIST_NFO_MIND_MAP;
  all_children[6] = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  all_children[7] = ZEITGEIST_NFO_PRESENTATION;
  all_children[8] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMM_MUSIC_PIECE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_AUDIO;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_MAILBOX;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_DENY_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_TEXT_DOCUMENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT;
  children[1] = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  children[2] = NULL;
  all_children = g_new (char*, 5);
  all_children[0] = ZEITGEIST_NFO_SOURCE_CODE;
  all_children[1] = ZEITGEIST_NFO_HTML_DOCUMENT;
  all_children[2] = ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT;
  all_children[3] = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  all_children[4] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_FILESYSTEM_IMAGE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_FILESYSTEM;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_JOURNAL;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_ICON;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_IMAGE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_PRESENTATION;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_ORGANIZATION_CONTACT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NCO_CONTACT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMM_MUSIC_ALBUM;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_MEDIA_LIST;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_SOURCE_CODE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_PLAIN_TEXT_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_VISUAL;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_MEDIA;
  parents[1] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_IMAGE;
  children[1] = ZEITGEIST_NFO_VIDEO;
  children[2] = NULL;
  all_children = g_new (char*, 9);
  all_children[0] = ZEITGEIST_NMM_TVSHOW;
  all_children[1] = ZEITGEIST_NFO_VECTOR_IMAGE;
  all_children[2] = ZEITGEIST_NFO_VIDEO;
  all_children[3] = ZEITGEIST_NMM_MOVIE;
  all_children[4] = ZEITGEIST_NFO_CURSOR;
  all_children[5] = ZEITGEIST_NFO_ICON;
  all_children[6] = ZEITGEIST_NFO_IMAGE;
  all_children[7] = ZEITGEIST_NFO_RASTER_IMAGE;
  all_children[8] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_EMAIL;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NMO_MESSAGE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_PAGINATED_TEXT_DOCUMENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_TEXT_DOCUMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMM_TVSHOW;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_VIDEO;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_CONTACT_LIST;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMM_TVSERIES;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_AUDIO;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_MEDIA;
  parents[1] = NULL;
  children = g_new (char*, 2);
  children[0] = ZEITGEIST_NMM_MUSIC_PIECE;
  children[1] = NULL;
  all_children = g_new (char*, 2);
  all_children[0] = ZEITGEIST_NMM_MUSIC_PIECE;
  all_children[1] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_CONTACT_GROUP;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_OPERATING_SYSTEM;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_SOFTWARE;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_SOFTWARE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_APPLICATION;
  children[1] = ZEITGEIST_NFO_OPERATING_SYSTEM;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NFO_OPERATING_SYSTEM;
  all_children[1] = ZEITGEIST_NFO_APPLICATION;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMM_MOVIE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_VIDEO;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_MESSAGE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NMO_EMAIL;
  children[1] = ZEITGEIST_NMO_IMMESSAGE;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NMO_EMAIL;
  all_children[1] = ZEITGEIST_NMO_IMMESSAGE;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_MOVE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_TIMEZONE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_ALARM;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_FOLDER;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_DATA_CONTAINER;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_MIME_ENTITY;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_FREEBUSY;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_MEDIA;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_VISUAL;
  children[1] = ZEITGEIST_NFO_AUDIO;
  children[2] = NULL;
  all_children = g_new (char*, 12);
  all_children[0] = ZEITGEIST_NMM_TVSHOW;
  all_children[1] = ZEITGEIST_NMM_MUSIC_PIECE;
  all_children[2] = ZEITGEIST_NFO_AUDIO;
  all_children[3] = ZEITGEIST_NFO_VECTOR_IMAGE;
  all_children[4] = ZEITGEIST_NFO_VIDEO;
  all_children[5] = ZEITGEIST_NMM_MOVIE;
  all_children[6] = ZEITGEIST_NFO_VISUAL;
  all_children[7] = ZEITGEIST_NFO_CURSOR;
  all_children[8] = ZEITGEIST_NFO_ICON;
  all_children[9] = ZEITGEIST_NFO_IMAGE;
  all_children[10] = ZEITGEIST_NFO_RASTER_IMAGE;
  all_children[11] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_DELETE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_RECEIVE_EVENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_INTERPRETATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_HARD_DISK_PARTITION;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_ARCHIVE_ITEM;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_EMBEDDED_FILE_DATA_OBJECT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_FILE_DATA_OBJECT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 4);
  children[0] = ZEITGEIST_NFO_EMBEDDED_FILE_DATA_OBJECT;
  children[1] = ZEITGEIST_NFO_DELETED_RESOURCE;
  children[2] = ZEITGEIST_NFO_REMOTE_DATA_OBJECT;
  children[3] = NULL;
  all_children = g_new (char*, 6);
  all_children[0] = ZEITGEIST_NFO_EMBEDDED_FILE_DATA_OBJECT;
  all_children[1] = ZEITGEIST_NFO_ARCHIVE_ITEM;
  all_children[2] = ZEITGEIST_NFO_DELETED_RESOURCE;
  all_children[3] = ZEITGEIST_NCAL_ATTACHMENT;
  all_children[4] = ZEITGEIST_NFO_REMOTE_DATA_OBJECT;
  all_children[5] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_SOFTWARE_SERVICE;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_SCHEDULED_ACTIVITY;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_MEDIA_STREAM;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_EMBEDDED_FILE_DATA_OBJECT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_FILE_DATA_OBJECT;
  parents[1] = NULL;
  children = g_new (char*, 3);
  children[0] = ZEITGEIST_NFO_ARCHIVE_ITEM;
  children[1] = ZEITGEIST_NFO_ATTACHMENT;
  children[2] = NULL;
  all_children = g_new (char*, 3);
  all_children[0] = ZEITGEIST_NFO_ARCHIVE_ITEM;
  all_children[1] = ZEITGEIST_NCAL_ATTACHMENT;
  all_children[2] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_ATTACHMENT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_ATTACHMENT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_HEURISTIC_ACTIVITY;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_REMOTE_DATA_OBJECT;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_FILE_DATA_OBJECT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 6);
  children[0] = ZEITGEIST_ZG_USER_ACTIVITY;
  children[1] = ZEITGEIST_ZG_SYSTEM_NOTIFICATION;
  children[2] = ZEITGEIST_ZG_HEURISTIC_ACTIVITY;
  children[3] = ZEITGEIST_ZG_SCHEDULED_ACTIVITY;
  children[4] = ZEITGEIST_ZG_WORLD_ACTIVITY;
  children[5] = NULL;
  all_children = g_new (char*, 6);
  all_children[0] = ZEITGEIST_ZG_USER_ACTIVITY;
  all_children[1] = ZEITGEIST_ZG_HEURISTIC_ACTIVITY;
  all_children[2] = ZEITGEIST_ZG_WORLD_ACTIVITY;
  all_children[3] = ZEITGEIST_ZG_SCHEDULED_ACTIVITY;
  all_children[4] = ZEITGEIST_ZG_SYSTEM_NOTIFICATION;
  all_children[5] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NMO_MAILBOX_DATA_OBJECT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCAL_CALENDAR_DATA_OBJECT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NCO_CONTACT_LIST_DATA_OBJECT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_WEB_DATA_OBJECT;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_USER_ACTIVITY;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_REMOTE_PORT_ADDRESS;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_WORLD_ACTIVITY;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_SOFTWARE_ITEM;
  parents = g_new (char*, 1);
  parents[0] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_NFO_DELETED_RESOURCE;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_NFO_FILE_DATA_OBJECT;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);

  uri = ZEITGEIST_ZG_SYSTEM_NOTIFICATION;
  parents = g_new (char*, 2);
  parents[0] = ZEITGEIST_ZG_EVENT_MANIFESTATION;
  parents[1] = NULL;
  children = g_new (char*, 1);
  children[0] = NULL;
  all_children = g_new (char*, 1);
  all_children[0] = NULL;
  info = symbol_info_new (uri, parents, children, all_children);
  zeitgeist_register_symbol (info);
  g_free (parents);
  g_free (children);
  g_free (all_children);


  symbols_loaded = TRUE;
}

