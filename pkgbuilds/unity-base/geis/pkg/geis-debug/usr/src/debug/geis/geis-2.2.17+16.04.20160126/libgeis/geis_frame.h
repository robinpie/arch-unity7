/**
 * @file geis_frame.h
 * @brief uFrame Geis frame module internal interface
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
#ifndef GEIS_FRAME_H_
#define GEIS_FRAME_H_

#include "geis/geis.h"

/**
 * @defgroup uframe_geis_frameset A Frame Container
 * @{
 */

typedef struct _GeisFrameSet *GeisFrameSet;

/**
 * Creates a new, empty frame set.
 */
GeisFrameSet geis_frameset_new();

/**
 * Destroys a frame set and all framees contained in it.
 *
 * @param[in] frameset  The frame set to destroy.
 */
void geis_frameset_delete(GeisFrameSet frameset);

/**
 * Inserts a frame in to a frame set.
 *
 * @param[in] frameset  A frame set.
 * @param[in] frame     A frame.
 *
 * The set takes ownership of the frame.
 */
GeisStatus geis_frameset_insert(GeisFrameSet frameset, GeisFrame frame);

/**
 * Gets the number of framees in a frameset.
 *
 * @param[in] frameset  The frameset,
 */
GeisSize geis_frameset_frame_count(GeisFrameSet frameset);

/**
 * Gets an indicated frame from a frameset.
 *
 * @param[in] frameset  The frameset.
 * @param[in] index     Indicates which frame to retrieve.
 */
GeisFrame geis_frameset_frame(GeisFrameSet frameset, GeisSize index);

/** @} */

/**
 * @defgroup uframe_geis_frame A Gesture Frame
 * @{
 */

/**
 * Creates a new frame.
 *
 * @param[in] id  Identifier for the new frame.
 */
GeisFrame geis_frame_new(GeisInteger id);

/**
 * Destroys a frame.
 *
 * @param[in] frame  The frame to destroy.
 */
void geis_frame_delete(GeisFrame frame);

/**
 * Inserts a frame attr.
 *
 * @param[in] frame  The gesture frame.
 * @param[in] attr   The gesture attr.
 */
GeisStatus geis_frame_add_attr(GeisFrame frame, GeisAttr attr);

/**
 * Inserts a touch IDinto a gesture frame.
 *
 * @param[in] frame    The gesture frame.
 * @param[in] touchid  The index of the frame in the related frame set.
 */
GeisStatus geis_frame_add_touchid(GeisFrame frame, GeisTouchId touchid);

/**
 * Classifies the frame.
 *
 * @param[in] frame         The gesture frame.
 * @param[in] gesture-class The class to which the frame belongs.
 *
 * A frame may belong to one or more gesture classes.
 */
GeisStatus geis_frame_set_is_class(GeisFrame        frame,
                                   GeisGestureClass gesture_class);

/**
 * Sets the frame transform matrix.
 *
 * @param[in] frame  The gesture frame.
 * @param[in] matrix The transform matrix.  This is assumed to be a 4x4 matrix
 *                   in row-major form.  No bounds checking or validation is
 *                   performed.
 *
 * This function performs a depp copy of the transform matrix.
 */
void geis_frame_set_matrix(GeisFrame frame, const GeisFloat *matrix);

/**
 * Indicates if a gesture frame belongs to a gesture class, by class name.
 *
 * @param[in] frame       The gesture frame.
 * @param[in] class_name  The gesture class name.
 *
 * @returns true if the gesture can currently be classified by the gesture class
 * name, false otherwise.
 */
GeisBoolean geis_frame_is_class_by_name(GeisFrame   frame,
                                        GeisString  class_name);

/**
 * Gets the number of gesture classes to which the frame belongs.
 *
 * @param[in] frame       The gesture frame.
 */
GeisSize geis_frame_class_count(GeisFrame frame);

/**
 * Gets a gesture class to which the frame belongs.
 *
 * @param[in] frame  The gesture frame.
 * @param[in] index  Indicates the class to retrieve.
 */
GeisGestureClass geis_frame_class(GeisFrame frame, GeisSize index);

/** @} */

#endif /* GEIS_FRAME_H_ */
