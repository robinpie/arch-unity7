/**
 * @file geis_filter.h
 * @brief internal Geis filter module private interface
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
#ifndef GEIS_FILTER_H_
#define GEIS_FILTER_H_

#include "geis/geis.h"
#include "geis_backend_token.h"
#include "geis_filter_term.h"

/**
 * @defgroup geis_filter_container A Filter Container
 * @{
 */

/**
 * An unsorted container for holding filters.
 */
typedef struct _GeisFilterBag *GeisFilterBag;

/**
 * An iterator over a %GeisFilterBag.
 */
typedef GeisFilter *GeisFilterIterator;

/**
 * Creates a new filter bag,
 */
GeisFilterBag geis_filter_bag_new();

/**
 * Destroys a filter bag.
 *
 * @param[in] bag The filter bag,
 */
void geis_filter_bag_delete(GeisFilterBag bag);

/**
 * Gets the number of filters in the bag.
 *
 * @param[in] bag The filter bag,
 */
GeisSize geis_filter_bag_count(GeisFilterBag bag);

/**t
 * Gets an indicated filter from a bag.
 *
 * @param[in] bag   The filter bag.
 * @param[in] index The index.
 */
GeisFilter geis_filter_bag_filter(GeisFilterBag bag, GeisSize index);

/**
 * Gets an indicated filter from a bag.
 *
 * @param[in] bag   The filter bag.
 * @param[in] name  The name by which to find the filter.
 *
 * Returns a NULL pointer if the named filter is not found.
 */
GeisFilter geis_filter_bag_filter_by_name(GeisFilterBag bag, GeisString name);

/**
 * Gets an iterator initialized to the first filter contained in the bag.
 *
 * @param[in] bag   The filter bag.
 */
GeisFilterIterator
geis_filter_bag_begin(GeisFilterBag bag);

/**
 * Gets an iterator initialized to one-past-the-end of the filter bag.
 *
 * @param[in] bag   The filter bag.
 */
GeisFilterIterator
geis_filter_bag_end(GeisFilterBag bag);

/**
 * Advances the iterator to the next filter in the bag.
 *
 * @param[in] iter  A %GeisFilterIterator.
 *
 * @returns an iterator pointing to the next filter in the filter bag.
 */
GeisFilterIterator
geis_filter_iterator_next(GeisFilterBag bag, GeisFilterIterator iter);

/**
 * Inserts a filter in the bag.
 *
 * @param[in] bag    The filter bag.
 * @param[in] filter The filter to insert.
 */
GeisStatus geis_filter_bag_insert(GeisFilterBag bag, GeisFilter filter);

/**
 * Remoes a filter from the bag.
 *
 * @param[in] bag    The filter bag.
 * @param[in] filter The filter to remove.
 */
GeisStatus geis_filter_bag_remove(GeisFilterBag bag, GeisFilter filter);

/** @} */

/**
 * @defgroup geis_filter_internals Filter Internals
 * @{
 */

/**
 * Increments the reference count on a filter.
 *
 * @param[in] filter The filter.
 *
 * @returns the same GeisFilter instance with an incremented reference count.
 */
GeisFilter geis_filter_ref(GeisFilter filter);

/**
 * Decrements and returns the referemce count on a filter.
 *
 * @param[in] filter The filter.
 *
 * @returns the current reference count.
 */
GeisSize geis_filter_unref(GeisFilter filter);

/**
 * Gets the number of terms in the filter.
 *
 * @param[in] filter   The filter.
 */
GeisSize geis_filter_term_count(GeisFilter filter);

/**
 * Gets the indicated term in the filter.
 *
 * @param[in] filter   The filter.
 * @param[in] index    Indicates which term.
 */
GeisFilterTerm geis_filter_term(GeisFilter filter, GeisSize index);

/**
 * Adds an already-created filter term (for internal unmarshalling).
 *
 * @param[in] filter   The filter.
 * @param[in] term     A filter term.
 */
GeisStatus geis_filter_add_term_internal(GeisFilter filter, GeisFilterTerm term);

/**
 * Gets the back end token from the filter.
 *
 * @param[in] filter   The filter.
 */
GeisBackendToken geis_filter_token(GeisFilter filter);

/**
 * Performs the filter function on a Geis event.
 *
 * @param[in] filter   The filter.
 * @param[in] event    An event.
 *
 * The point of a filter is to pass or reject events.  This function does just
 * that.
 *
 * @return true of the event is passed by the filter, false otherwise.
 */
GeisBoolean geis_filter_pass_event(GeisFilter filter, GeisEvent event);

/** @} */

#endif /* GEIS_FILTER_H_ */
