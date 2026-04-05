/**
 * @file geis_group.h
 * @brief Geis gesture group module internal interface
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
#ifndef GEIS_GROUP_H_
#define GEIS_GROUP_H_

#include "geis/geis.h"
#include "geis_frame.h"

/**
 * @defgroup geis_groupset A Gesture Group Container
 * @{
 */

/**
 * Creates a new, empty group set.
 */
GeisGroupSet geis_groupset_new();

/**
 * Destroys a group set and all groups contained in it.
 *
 * @param[in] groupset  The group set to destroy.
 */
void geis_groupset_delete(GeisGroupSet groupset);

/**
 * Inserts a gesture group into a group set.
 *
 * @param[in] groupset  A group set.
 * @param[in] group     A gesture group.
 *
 * The set takes ownership of the gesture group.
 */
GeisStatus geis_groupset_insert(GeisGroupSet groupset, GeisGroup group);

/** @} */

/**
 * @defgroup geis_group A Gesture Group
 * @{
 */

/**
 * Creates a new gesture group.
 *
 * @param[in] id  Identifier for the new group.
 */
GeisGroup geis_group_new(GeisInteger id);

/**
 * Destroys a gesture group and all gesture frames contained in it.
 *
 * @param[in] group  The gesture group to destroy.
 */
void geis_group_delete(GeisGroup group);

/**
 * Adds a gesture frame to a gesture group.
 *
 * @param[in] group  A gesture group.
 * @param[in] frame  A gesture frame.
 *
 * The group takes ownership of the frame object.
 */
GeisStatus geis_group_insert_frame(GeisGroup group, GeisFrame frame);

/** @} */

#endif /* GEIS_GROUP_H_ */
