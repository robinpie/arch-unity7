/**
 * @file geis_device.h
 * @brief internal Geis Input Device module private interface
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
#ifndef GEIS_DEVICE_H_
#define GEIS_DEVICE_H_

#include "geis/geis.h"

/**
 * @defgroup geis_device_container A Device Container
 * @{
 */

/**
 * An unsorted container for holding devices.
 */
typedef struct _GeisDeviceBag *GeisDeviceBag;

/**
 * Creates a new device bag,
 */
GeisDeviceBag geis_device_bag_new();

/**
 * Destroys a device bag.
 *
 * @param[in] bag The device bag,
 */
void geis_device_bag_delete(GeisDeviceBag bag);

/**
 * Gets the number of devices in the bag.
 *
 * @param[in] bag The device bag,
 */
GeisSize geis_device_bag_count(GeisDeviceBag bag);

/**
 * Gets an indicated device from a bag.
 *
 * @param[in] bag   The device bag.
 * @param[in] index The index.
 */
GeisDevice geis_device_bag_device(GeisDeviceBag bag, GeisSize index);

/**
 * Inserts a device in the bag.
 *
 * @param[in] bag    The device bag.
 * @param[in] device The device to insert.
 */
GeisStatus geis_device_bag_insert(GeisDeviceBag bag, GeisDevice device);

/**
 * Remoes a device from the bag.
 *
 * @param[in] bag    The device bag.
 * @param[in] device The device to remove.
 */
GeisStatus geis_device_bag_remove(GeisDeviceBag bag, GeisDevice device);

/** @} */

/**
 * @defgroup geis_device Internal Device Functions
 * @{
 */

/**
 * Creates a new device.
 *
 * @param[in] name A system-specific device name.
 * @param[in] id   A system-specific device identifier.
 */
GeisDevice geis_device_new(GeisString name, GeisInteger id);

/**
 * Inserts an attr into a device.
 *
 * @param[in] device  A device.
 * @param[in] attr    An attr.
 */
void geis_device_add_attr(GeisDevice device, GeisAttr attr);

/* @} */

#endif /* GEIS_DEVICE_H_ */
