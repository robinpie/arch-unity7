/**
 * @file geis_filterable.c
 * @brief internal Geis filterable entities implementation
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor#include "geis_filterable.h"
 */
#include "geis_config.h"
#include "geis_filterable.h"

#include "geis_logging.h"
#include <string.h>


/*
 * An internal struct to collect filterable attributes for facilities.
 */
struct FilterableAttributeBag
{
  GeisFilterableAttribute store;
  GeisSize                size;
  GeisSize                count;
};


/*
 * Constructs a new filterable attribute bag.
 */
FilterableAttributeBag
geis_filterable_attribute_bag_new()
{
  FilterableAttributeBag bag = calloc(1, sizeof(struct FilterableAttributeBag));
  if (!bag)
  {
    geis_error("failed to allocate filterable attribute bag");
    goto final_exit;
  }

  bag->size = 2;
  bag->count = 0;

  bag->store = calloc(1, sizeof(struct GeisFilterableAttribute));
  if (!bag)
  {
    geis_error("failed to allocate filterable attribute bag store");
    goto unwind_bag;
  }
  goto final_exit;

unwind_bag:
  free(bag);
  bag = NULL;
final_exit:
  return bag;
}


/*
 * Destroys a filterable attribute bag.
 */
void
geis_filterable_attribute_bag_delete(FilterableAttributeBag bag)
{
  if (bag)
  {
    GeisSize i;
    for (i = 0; i < bag->count; ++i)
    {
      free((char *)bag->store[i].name);
    }
    free(bag->store);
  }
  free(bag);
}


void
geis_filterable_attribute_copy(GeisFilterableAttribute src,
                               GeisFilterableAttribute dst)
{
  dst->name = strdup(src->name);
  dst->type = src->type;
  dst->add_term_callback = src->add_term_callback;
  dst->add_term_context  = src->add_term_context;
}


GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_begin(FilterableAttributeBag bag)
{
  if (bag->count)
    return &bag->store[0];
  return geis_filterable_attribute_bag_end(bag);
}


GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_end(FilterableAttributeBag bag GEIS_UNUSED)
{
  return NULL;
}


GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_next(FilterableAttributeBag         bag,
                                   GeisFilterableAttributeBagIter iter)
{
  if (iter < bag->store + bag->count - 1)
    return ++iter;
  return geis_filterable_attribute_bag_end(bag);
}


void
geis_filterable_attribute_bag_insert(FilterableAttributeBag  bag,
                                     GeisFilterableAttribute fa)
{
  GeisSize new_count = bag->count + 1;
  if (new_count >= bag->size)
  {
    GeisSize new_size = bag->size * 2;
    GeisSize allocation_size = new_size * sizeof(struct GeisFilterableAttribute);
    GeisFilterableAttribute new_store = realloc(bag->store, allocation_size);
    if (!new_store)
    {
      geis_error("failed to reallocate filterable attribute bag store");
    }
    else
    {
      bag->store = new_store;
      bag->size = new_size;
    }
  }
  geis_filterable_attribute_copy(fa, &bag->store[bag->count]);
  bag->count = new_count;
}

