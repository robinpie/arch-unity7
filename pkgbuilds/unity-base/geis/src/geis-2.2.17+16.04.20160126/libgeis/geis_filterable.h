/**
 * @file geis_filterable.h
 * @brief internal Geis filterable entities private interface
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
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_FILTERABLE_H_
#define GEIS_FILTERABLE_H_

#include "geis/geis.h"
#include "geis_backend_token.h"


/**
 * A callback to add a term to a back end filter token.
 *
 * The callback receives the back end filter token, a back end callback context,
 * the name of the attribute, a filter operation, and the filter value.
 *
 * @param[in] token   The back-end filter token.
 * @param[in] context The back-end callback context.
 * @param[in] name    The filterable attribute name.
 * @param[in] op      The filter operation.
 * @param[in] value   The filter term value.
 */
typedef GeisStatus (*AddTermCallback)(GeisBackendToken     token,
                                      void                *context,
                                      GeisString           name,
                                      GeisFilterOperation  op,
                                      void                *value);

/**
 * The description of an attribute that can be used in subscirption filter
 * terms.
 *
 * When a facility (class, device, or region) registers the attributes that can
 * be used to build subscription filter terms, it passes a list of these structs
 * and the ABI instance maintains a collection of them.  The facility just needs
 * to indicate the name and type of the attribute and provide a callback and
 * context for creating the filter term.
 */
typedef struct GeisFilterableAttribute
{
  GeisString       name;
  GeisAttrType     type;
  AddTermCallback  add_term_callback;
  void            *add_term_context;
} *GeisFilterableAttribute;


typedef struct FilterableAttributeBag *FilterableAttributeBag;
typedef GeisFilterableAttribute GeisFilterableAttributeBagIter;

/**
 * Handy sugar.
 */
static inline void
geis_filterable_attribute_init(GeisFilterableAttribute fa,
                               GeisString              n,
                               GeisAttrType            t,
                               AddTermCallback         ac,
                               void                   *acc)
{
  fa->name = n;
  fa->type = t;
  fa->add_term_callback = ac;
  fa->add_term_context = acc;
}


/**
 * Constructs a new filterable attribute bag.
 */
FilterableAttributeBag
geis_filterable_attribute_bag_new();

/**
 * Destroys a filterable attribute bag.
 */
void
geis_filterable_attribute_bag_delete(FilterableAttributeBag bag);

GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_begin(FilterableAttributeBag  bag);

GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_end(FilterableAttributeBag  bag);

GeisFilterableAttributeBagIter
geis_filterable_attribute_bag_next(FilterableAttributeBag         bag,
                                   GeisFilterableAttributeBagIter iter);

void
geis_filterable_attribute_bag_insert(FilterableAttributeBag  bag,
                                     GeisFilterableAttribute fa);

void
geis_filterable_attribute_copy(GeisFilterableAttribute src,
                               GeisFilterableAttribute dst);

#endif /* GEIS_FILTERABLE_H_ */
