/**
 * @file geis_attr.h
 * @brief internal GeisAttr facilities
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
#ifndef GEIS_ATTR_H_
#define GEIS_ATTR_H_

#include <geis/geis.h>


typedef struct _GeisAttrBag *GeisAttrBag;

typedef void (*GeisAttrDestructor)(void *);

/**
 * Creates a new Geis Attribute container.
 */
GeisAttrBag geis_attr_bag_new(GeisSize size_hint);

/**
 * Destroys a Geis Attribute container.
 */
void geis_attr_bag_delete(GeisAttrBag bag);

/**
 * Tells how any entires in a Geis Attribute container.
 */
GeisSize geis_attr_bag_count(GeisAttrBag bag);

/**
 * Pulls an indicated attr out of a bag.
 */
GeisAttr geis_attr_bag_attr(GeisAttrBag bag, GeisSize index);

/**
 * Inserts an attribute in an attribute container.
 */
GeisStatus geis_attr_bag_insert(GeisAttrBag bag, GeisAttr attr);

/**
 * Replaces an attribute in an attribute container.
 */
GeisStatus geis_attr_bag_replace(GeisAttrBag bag, GeisAttr attr);

/**
 * Looks for an attribute in an attribute container.
 */
GeisAttr geis_attr_bag_find(GeisAttrBag bag, GeisString attr_name);

/**
 * Creates a Geis Attribute object.
 */
GeisAttr geis_attr_new(GeisString attr_name, GeisAttrType attr_type, void* attr_value);

/**
 * Destroys a Geis Attribute object.
 */
void geis_attr_delete(GeisAttr attr);

/**
 * Gets the (generic) attribute value.
 */
void *geis_attr_value(GeisAttr attr);

/**
 * Set a destructor for a pointer attr.
 *
 * @param[in] attr       An attr.
 * @param[in] destructor A destrutor function for a pointer attr value.
 */
void geis_attr_set_destructor(GeisAttr attr, GeisAttrDestructor destructor);

/**
 * Compares two attributes using a filter operation.
 *
 * @param[in] lhs  The attr on the left-hand side of the comparison.
 * @param[in] rhs  The attr on the right-hand side of the comparison.
 * @param[in] op   The filter operation.
 *
 * Returns GEIS_TRUE if the comparison is true, GEIS_FALSE otherwise.
 */
GeisBoolean geis_attr_compare(GeisAttr lhs, GeisAttr rhs, GeisFilterOperation op);

#endif /* GEIS_ATTR_H_ */
