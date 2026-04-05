/**
 * @file geis_class.h
 * @brief internal Geis Gesture Class module private interface
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
#ifndef GEIS_CLASS_H_
#define GEIS_CLASS_H_

#include "geis/geis.h"

/**
 * @defgroup geis_gesture_class_container A Gesture Class Container
 * @{
 */

/**
 * An unsorted container for holding classs.
 */
typedef struct _GeisGestureClassBag *GeisGestureClassBag;

/**
 * Creates a new class bag,
 */
GeisGestureClassBag geis_gesture_class_bag_new();

/**
 * Destroys a gesture class bag.
 *
 * @param[in] bag The gesture class bag,
 */
void geis_gesture_class_bag_delete(GeisGestureClassBag bag);

/**
 * Gets the number of gesture classs in the bag.
 *
 * @param[in] bag The gesture class bag,
 */
GeisSize geis_gesture_class_bag_count(GeisGestureClassBag bag);

/**
 * Gets an indicated gesture class from a bag.
 *
 * @param[in] bag   The gesture class bag.
 * @param[in] index The index.
 */
GeisGestureClass geis_gesture_class_bag_gesture_class(GeisGestureClassBag bag,
                                                      GeisSize            index);

/**
 * Inserts a gesture class in the bag.
 *
 * @param[in] bag           The gesture class bag.
 * @param[in] gesture_class The gesture class to insert.
 */
GeisStatus geis_gesture_class_bag_insert(GeisGestureClassBag bag,
                                         GeisGestureClass    gesture_class);

/**
 * Remoes a gesture class from the bag.
 *
 * @param[in] bag           The gesture class bag.
 * @param[in] gesture_class The gesture class to remove.
 */
GeisStatus geis_gesture_class_bag_remove(GeisGestureClassBag bag,
                                         GeisGestureClass    gesture_class);

/** @} */

/**
 * @defgroup geis_gesture_class Internal Gesture Class Functions
 * @{
 */

/**
 * Creates a new gesture class.
 *
 * @param[in] name A system-specific gesture class name.
 * @param[in] id   A system-specific gesture class identifier.
 */
GeisGestureClass geis_gesture_class_new(GeisString name, GeisInteger id);

/**
 * Inserts an attr into a gesture class.
 *
 * @param[in] gesture_class  A gesture class.
 * @param[in] attr           An attr.
 */
GeisStatus geis_gesture_class_add_attr(GeisGestureClass gesture_class,
                                       GeisAttr         attr);

/** @} */

#endif /* GEIS_CLASS_H_ */
