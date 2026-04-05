/**
 * @file geis_bag.h
 * @brief generic bag container interface
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
#ifndef GEIS_UTIL_BAG_H
#define GEIS_UTIL_BAG_H

#include "geis/geis.h"


/**
 * @struct GeisBag
 * @brief A handle for a generic value-storage bag.
 * @ingroup geis_util
 *
 * A bag is a container that holds zero or more copies of a data structure of
 * homogeneous type.  It is implemented as a contiguous store of data that grows
 * when needed using a predetermined growth factor to give amortized constant
 * insertion and removal rates.
 *
 * Uses of this structure should be wrapped by more type-specific functions.
 *
 * @typedef GeisBag
 * @brief opaque pointer to a GeisBag object.
 */
typedef struct GeisBag *GeisBag;

struct GeisBag
{
  GeisSize   store_size;           /**< The size of data_store in datum_size
                                     *  units. */
  GeisFloat  store_growth_factor;  /**< The storage growth factor. */
  GeisSize   datum_size;           /**< The number of bytes in each datum. */
  GeisSize   data_count;           /**< The number of data in data_store. */
  void      *data_store;           /**< The the data store. */
};


#define geis_bag_default_init_alloc    2
#define geis_bag_default_growth_factor 1.5f


/**
 * Creates a new, empty bag.
 * @memberof GeisBag
 *
 * @param[in] data_size      The size (in bytes) of the stored datum.
 * @param[in] init_alloc     The initial number of data to allocate space for.
 * @param[in] growth_factor  The growth factor for extending the bag.
 *
 * Constructs a generic homogenous container that is designed to hold data with
 * a size of @p data_size (in bytes).  Space for @p init_alloc data is initially
 * allocated, and subsequent expansions will be done by a factor of
 * @p growth_factor (taking the ceiling of the current allocation times the @p
 * growth_factor).
 *
 * @returns a new, empty GeisBag on success or a NULL on failure.
 */
GeisBag
geis_bag_new(GeisSize data_size, GeisSize init_alloc, GeisFloat growth_factor);

/**
 * Destroys a bag.
 * @memberof GeisBag
 *
 * @param[in]  bag  The bag to destroy.
 *
 * Frees all resources used by the bag itself:  does not destroy any store data.
 */
void
geis_bag_delete(GeisBag bag);

/**
 * Gets the number of data contained in the bag.
 * @memberof GeisBag
 *
 * @param[in]  bag    The bag.
 *
 * @returns the count of the data contained in the bag.
 */
GeisSize
geis_bag_count(GeisBag bag);

/**
 * Appends a new datam to the store.
 * @memberof GeisBag
 *
 * @param[in]  bag    The bag to append to.
 * @param[in]  datum  The datum to append.
 *
 * @returns GEIS_STATUS_UNKNOWN_ERROR if @p datum failed to be appended (for
 * example, memory allocation failure during bag expansion), GEIS_STATUS_SUCCESS
 * otherwise.
 */
GeisStatus
geis_bag_append(GeisBag bag, void *datum);

/**
 * Gets a pointer to a datum by index.
 * @memberof GeisBag
 *
 * @param[in]  bag    The bag.
 * @param[in]  index  Indicates the datum to retrieve.
 *
 * @returns NULL if @p index is out of range, a pointer to the indictaed datum
 * otherwise.
 */
void*
geis_bag_at(GeisBag bag, GeisSize index);

/**
 * Removes an indicated datum from the bag.
 * @memberof GeisBag
 *
 * @param[in]  bag    The bag.
 * @param[in]  index  Indicates the datum to remove.
 *
 * This function removes the datum and if necessary fills in the 'hole' in the
 * store by moving other data around.  It will invalidate any pointers into the
 * bag:  avoid using 'em.  Does not call any deleter functions, so plan
 * accordingly.
 *
 * @returns GEIS_STATUS_BAD_ARGUMENT if @p index is out of range,
 * GEIS_STATUS_SUCCESS otherwise.
 */
GeisStatus
geis_bag_remove(GeisBag bag, GeisSize index);

#endif /* GEIS_UTIL_BAG_H */
