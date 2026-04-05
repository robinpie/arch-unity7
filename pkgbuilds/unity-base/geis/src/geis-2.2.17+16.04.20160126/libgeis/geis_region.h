/**
 * @file geis_region.h
 * @brief internal Geis region module private interface
 *
 * Copyright 2010 Canonical Ltd.
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
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_REGION_H_
#define GEIS_REGION_H_

#include "geis/geis.h"

/**
 * @struct _GeisRegion
 *
 * This is a refcounted object:  using the internal function geis_region_ref()
 * will increment the reference count on the object, and geis_region_delete()
 * will decrement the reference count and destroy the object when the reference
 * count is set to zero.
 *
 * If you are handed a GeisRegion and want to keep it around, you need to ref it
 * and then delete it when you're done, otherwise it could disappear out from
 * under you.
 *
 * Because GeisRegion lifetime is affected explicitly by application action
 * (new and delete calls are under application control), the refcount mechanism
 * has been made threadsafe.
 */

/**
 * Contains regions in no particular order, with no duplicate checking.
 *
 * @note This container is not threadsafe.
 */
typedef struct _GeisRegionBag *GeisRegionBag;

/**
 * Creates a new region bag.
 */
GeisRegionBag geis_region_bag_new();

/**
 * Destroys a region bag.
 *
 * @param[in] bag  The region bag.
 */
void geis_region_bag_delete(GeisRegionBag bag);

/**
 * Gets the number of regions held in a bag.
 */
GeisSize geis_region_bag_count(GeisRegionBag bag);

/**
 * Gets an indicated region from a bag.
 */
GeisRegion geis_region_bag_region(GeisRegionBag bag, GeisSize index);

/**
 * Puts a region into a bag.
 */
GeisStatus geis_region_bag_insert(GeisRegionBag bag, GeisRegion region);

/**
 * Takes a region out of a bag.
 */
GeisStatus geis_region_bag_remove(GeisRegionBag bag, GeisRegion region);

/**
 * Adds a reference to a region.
 *
 * @param[in] region  A region.
 */
void geis_region_ref(GeisRegion);

/**
 * Gets the type of the region.
 *
 * @param[in] region  A region.
 */
GeisString geis_region_type(GeisRegion region);

/**
 * Gets the data (if any) associated with the region type.
 *
 * @param[in] region  A region.
 */
void *geis_region_data(GeisRegion region);

#endif /* GEIS_REGION_H_ */
