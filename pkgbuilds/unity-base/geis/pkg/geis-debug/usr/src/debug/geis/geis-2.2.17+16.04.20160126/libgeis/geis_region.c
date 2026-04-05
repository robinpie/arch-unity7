/**
 * @file libgeis/geis_region.c
 * @brief implementation of the GEIS v2.0 API region module
 *
 * Copyright 2010 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"
#include "geis_region.h"

#include "geis_atomic.h"
#include "geis_error.h"
#include "geis_logging.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>


struct _GeisRegion
{
  GeisRefCount refcount;
  GeisString   type;
  GeisString   name;
  union {
    int windowid;
  } data;
};

struct _GeisRegionBag
{
  GeisRegion *region_store;
  GeisSize    region_store_size;
  GeisSize    region_count;
};

static const int region_bag_growth_constant = 2;


/*
 * Constructs a region bag.
 */
GeisRegionBag
geis_region_bag_new()
{
  GeisRegionBag bag = calloc(1, sizeof(struct _GeisRegionBag));
  if (!bag)
  {
    geis_error("failed to allocate region bag");
    goto final_exit;
  }

  bag->region_store_size = 3;
  bag->region_count = 0;
  bag->region_store = calloc(bag->region_store_size, sizeof(GeisRegion));
  if (!bag->region_store)
  {
    geis_error("failed to allocate region bag store");
    goto unwind_bag;
  }
  goto final_exit;

unwind_bag:
  free(bag);
final_exit:
  return bag;
}


/*
 * Destroys a region bag.
 */
void
geis_region_bag_delete(GeisRegionBag bag)
{
  GeisSize i;
  for (i = bag->region_count; i > 0; --i)
  {
    geis_region_delete(bag->region_store[i-1]);
  }
  free(bag);
}


/*
 * Gets the number of regions held in a bag.
 */
GeisSize
geis_region_bag_count(GeisRegionBag bag)
{
  return bag->region_count;
}


/*
 * Gets an indicated region from a bag.
 */
GeisRegion
geis_region_bag_region(GeisRegionBag bag, GeisSize index)
{
  GeisRegion region = NULL;
  if (index < bag->region_count)
  {
    region = bag->region_store[index];
  }
  return region;
}


/*
 * Puts a region into a bag.
 */
GeisStatus
geis_region_bag_insert(GeisRegionBag bag, GeisRegion region)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->region_count >= bag->region_store_size)
  {
    GeisSize new_store_size = bag->region_store_size * region_bag_growth_constant;
    GeisRegion *new_store = realloc(bag->region_store,
             new_store_size * sizeof(struct _GeisRegion));
    if (!new_store)
    {
      geis_error("failed to reallocate region bag");
      goto error_exit;
    }
    bag->region_store = new_store;
    bag->region_store_size = new_store_size;
  }
  bag->region_store[bag->region_count++] = region;
  status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}


/**
 * Takes a region out of a bag.
 */
GeisStatus
geis_region_bag_remove(GeisRegionBag bag, GeisRegion region)
{
  GeisSize i;
  GeisStatus status = GEIS_STATUS_SUCCESS;
  for (i = 0; i < bag->region_count; ++i)
  {
    if (bag->region_store[i] == region)
    {
      GeisSize j;
      geis_region_delete(bag->region_store[i]);
      --bag->region_count;
      for (j = i; j < bag->region_count; ++j)
      {
	bag->region_store[j] = bag->region_store[j+1];
      }
      break;
    }
  }
  return status;
}


/*
 * Constructs a region.
 */
GeisRegion
geis_region_new(Geis       geis,
                GeisString name,
                GeisString init_arg_name, ...)
{
  GeisRegion region = NULL;
  va_list    varargs;

  region = calloc(1, sizeof(struct _GeisRegion));
  if (!region)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating region");
    goto final_exit;
  }

  va_start(varargs, init_arg_name);
  while (init_arg_name)
  {
    if (0 == strcmp(init_arg_name, GEIS_REGION_X11_ROOT))
    {
      if (region->type)
      {
	geis_warning("multiple region types requested, only using the first");
	break;
      }
      region->type = strdup(init_arg_name);
      geis_debug("using X11 root");
    }
    else if (0 == strcmp(init_arg_name, GEIS_REGION_X11_WINDOWID))
    {
      if (region->type)
      {
	geis_warning("multiple region types requested, only using the first");
	break;
      }
      region->type = strdup(init_arg_name);
      region->data.windowid = va_arg(varargs, int);
      geis_debug("using X11 windowid 0x%08x", region->data.windowid);
    }

    init_arg_name = va_arg(varargs, GeisString);
  }
  va_end(varargs);

  ++region->refcount;
  region->name = strdup(name);

final_exit:
  return region;
}


/*
 * Destroys a GEIS v2.0 region.
 */
GeisStatus
geis_region_delete(GeisRegion region)
{
  if (0 == geis_atomic_unref(&region->refcount))
  {
    free((char *)region->name);
    free((char *)region->type);
    free(region);
  }
  return GEIS_STATUS_SUCCESS;
}


/*
 * Gets the name of a GEIS v2.0 region.
 */
GeisString
geis_region_name(GeisRegion region)
{
  return region->name;
}


/*
 * Adds a reference to a region.
 */
void
geis_region_ref(GeisRegion region)
{
  geis_atomic_ref(&region->refcount);
}


/*
 * Gets the type of the region.
 */
GeisString
geis_region_type(GeisRegion region)
{
  return region->type;
}


/*
 * Gets the data (if any) associated with the region type.
 */
void *
geis_region_data(GeisRegion region)
{
  return &region->data;
}


