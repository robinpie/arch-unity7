/**
 * @file geis_touch.h
 * @brief Geis touch module internal interface
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
#ifndef GEIS_TOUCH_H_
#define GEIS_TOUCH_H_

#include "geis/geis.h"

/**
 * @defgroup geis_touchset A Touch Container
 * @{
 */

/**
 * Creates a new, empty touch set.
 */
GeisTouchSet geis_touchset_new();

/**
 * Destroys a touch set and all touches contained in it.
 *
 * @param[in] touchset  The touch set to destroy.
 */
void geis_touchset_delete(GeisTouchSet touchset);

/**
 * Inserts a touch in to a touch set.
 *
 * @param[in] touchset  A touch set.
 * @param[in] touch     A touch.
 *
 * The set takes ownership of the touch.
 */
GeisStatus geis_touchset_insert(GeisTouchSet touchset, GeisTouch touch);

/** @} */

/**
 * @defgroup geis_touch A Gesture Touch
 * @{
 */

/**
 * Creates a new touch.
 *
 * @param[in] id  Identifier for the new touch.
 */
GeisTouch geis_touch_new(GeisTouchId id);

/**
 * Destroys a touch.
 *
 * @param[in] touch  The touch to destroy.
 */
void geis_touch_delete(GeisTouch touch);

/**
 * Inserts touch attr.
 *
 * @param[in] touch  The touch.
 * @param[in] attr   The attr.
 */
GeisStatus geis_touch_add_attr(GeisTouch touch, GeisAttr attr);

/** @} */

#endif /* GEIS_TOUCH_H_ */
