/**
 * @file geis_bag.c
 * @brief generic bag container 
 */

/*
 * Copyright 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License version 3, as published 
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_config.h"
#include "geis_bag.h"

#include <assert.h>
#include "geis_logging.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


/**
 * Allocates data storage for a GeisBag.
 */
static void *
_bag_store_allocate(GeisSize store_size, GeisSize data_size)
{
  void *store = calloc(store_size, data_size);
  if (!store)
    geis_error("failed to allocate bag store");
  return store;
}


/*
 * Creates a new, empty bag.
 */
GeisBag
geis_bag_new(GeisSize datum_size, GeisSize init_alloc, GeisFloat growth_factor)
{
  GeisBag bag = calloc(1, sizeof(struct GeisBag));
  if (!bag)
  {
    geis_error("failed to allocate bag");
    goto final_exit;
  }

  bag->store_size = init_alloc;
  bag->store_growth_factor = growth_factor;
  bag->datum_size = datum_size;
  bag->data_count = 0;
  bag->data_store = _bag_store_allocate(bag->store_size, bag->datum_size);
  if (!bag->data_store)
  {
    free(bag);
    bag = NULL;
  }
  
final_exit:
  return bag;
}


/*
 * Destroys a bag.
 */
void
geis_bag_delete(GeisBag bag)
{
  free(bag->data_store);
  free(bag);
}


/*
 * Gets the number of data contained in the bag.
 */
GeisSize
geis_bag_count(GeisBag bag)
{
  return bag->data_count;
}


/*
 * Appends a new datam to the store.
 */
GeisStatus
geis_bag_append(GeisBag bag, void *datum)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->data_count >= bag->store_size)
  {
    GeisSize new_store_size = ceilf(bag->store_size * bag->store_growth_factor);
    void *new_data_store =  _bag_store_allocate(new_store_size, bag->datum_size);
    if (!new_data_store)
    {
      goto final_exit;
    }

    memcpy(new_data_store, bag->data_store, bag->data_count * bag->datum_size);
    free(bag->data_store);
    bag->data_store = new_data_store;
    bag->store_size = new_store_size;
  }

  memcpy((char *)bag->data_store + bag->data_count * bag->datum_size,
         datum,
         bag->datum_size);
  ++bag->data_count;
  status = GEIS_STATUS_SUCCESS;

final_exit:
  return status;
}


/*
 * Gets a pointer to a datum by index.
 */
void *
geis_bag_at(GeisBag bag, GeisSize index)
{
  void *datum = NULL;
  if (index >= bag->data_count)
  {
    assert(index < bag->data_count);
    goto final_exit;
  }

  datum = (char *)bag->data_store + bag->datum_size * index;

final_exit:
  return datum;
}


/*
 * Removes an indicated datum from the bag.
 */
GeisStatus
geis_bag_remove(GeisBag bag, GeisSize index)
{
  GeisStatus status = GEIS_STATUS_BAD_ARGUMENT;
  if (index >= bag->data_count)
  {
    assert(index < bag->data_count);
    goto final_exit;
  }

  size_t size = (bag->data_count - index - 1) * bag->datum_size;
  if (size)
  {
    char *dst = (char *)bag->data_store + bag->datum_size * index;
    char *src = dst + bag->datum_size;
    memmove(dst, src, size);
  }
  --bag->data_count;
  status = GEIS_STATUS_SUCCESS;

final_exit:
  return status;
}

