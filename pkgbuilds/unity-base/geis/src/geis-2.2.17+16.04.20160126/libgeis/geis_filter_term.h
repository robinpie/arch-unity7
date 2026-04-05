/**
 * @file geis_filter_term.h
 * @brief internal Geis filter term module private interface
 *
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
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_FILTER_TERM_H_
#define GEIS_FILTER_TERM_H_

#include "geis/geis.h"

/**
 * @defgroup geis_filter_term Filter Terms.
 * @{
 */

/**
 * One of the terms of a filter.
 *
 * A term belong to a facility (eg. device, gesture type, region) and is used to
 * compare (using an operation) a named attribute with a given value.
 */
typedef struct _GeisFilterTerm *GeisFilterTerm;

/**
 * Creates a new filter term.
 *
 * @param[in] facility  The term's facility.
 * @param[in] operation The term's operation.
 * @param[in] attr      The term's attr name, type, and value
 */
GeisFilterTerm geis_filter_term_new(GeisFilterFacility  facility,
                                    GeisFilterOperation operation,
                                    GeisAttr            attr);

/**
 * Increments the filter term's reference count.
 *
 * @param[in] term  The filter term.
 */
GeisFilterTerm geis_filter_term_ref(GeisFilterTerm term);

/**
 * Decrements the filter term's reference count and maybe destroys the term.
 *
 * @param[in] term  The filter term.
 *
 * The number of references to the term is reduced by 1 and when it reaches 0,
 * the term is destroyed.
 */
void geis_filter_term_unref(GeisFilterTerm term);

/**
 * Gets a filter term's facility.
 *
 * @param[in] term  The filter term.
 */
GeisFilterFacility geis_filter_term_facility(GeisFilterTerm term);

/**
 * Gets a filter term's operation.
 *
 * @param[in] term  The filter term.
 */
GeisFilterOperation geis_filter_term_operation(GeisFilterTerm term);

/**
 * Gets a filter term's attributes.
 *
 * @param[in] term  The filter term.
 */
GeisAttr geis_filter_term_attr(GeisFilterTerm term);

/**
 * Indicates if a filter term matches a GEIS event.
 *
 * @param[in] term  The filter term.
 * @param[in] event A GEIS event.
 */
GeisBoolean geis_filter_term_match_event(GeisFilterTerm term, GeisEvent event);

/**
 * Indicates a filter passes a device.
 *
 * @param[in] term   The filter term.
 * @param[in] device A GEIS device.
 *
 * @returns GEIS_TRUE if the device is passed by the filter, GEIS_FALSE
 * otherwise.
 */
GeisBoolean geis_filter_term_match_device(GeisFilterTerm term, GeisDevice device);

/** @} */

/**
 * @defgroup geis_filter_term_container A Filter Term Container
 * @{
 */

/**
 * An unsorted container for holding filter terms.
 *
 * This implementation is physically unsorted, but terms are logically grouped
 * by facility.
 *
 * This is a purely internal structure, as is the filter term.  The bag "owns"
 * the term.
 */
typedef struct _GeisFilterTermBag *GeisFilterTermBag;

/**
 * Creates a new, empty filter term bag,
 *
 * @param[in] store_size The initial filter store size. A value of 0 will
 *                       allocate a reasonable default size.
 */
GeisFilterTermBag geis_filter_term_bag_new(GeisSize store_size);

/**
 * Creates a new filter term bag by deep-copying an existing filter term bag.
 *
 * @param[in] original  The original fiter term bag.
 */
GeisFilterTermBag geis_filter_term_bag_clone(GeisFilterTermBag original);

/**
 * Destroys a filter term bag.
 *
 * @param[in] bag The filter term bag.
 */
void geis_filter_term_bag_delete(GeisFilterTermBag bag);

/**
 * Gets the number of filter terms in the bag.
 *
 * @param[in] bag      The filter term bag.
 */
GeisSize geis_filter_term_bag_count(GeisFilterTermBag  bag);

/**
 * Gets an indicated filter term from a bag.
 *
 * @param[in] bag      The filter bag.
 * @param[in] index    The index.
 */
GeisFilterTerm geis_filter_term_bag_term(GeisFilterTermBag bag, GeisSize index);

/**
 * Inserts a filter in the bag.
 *
 * @param[in] bag    The filter bag.
 * @param[in] term   The filter to insert.
 */
GeisStatus geis_filter_term_bag_insert(GeisFilterTermBag bag,
                                       GeisFilterTerm    term);

/** @} */

#endif /* GEIS_FILTER_TERM_H_ */
