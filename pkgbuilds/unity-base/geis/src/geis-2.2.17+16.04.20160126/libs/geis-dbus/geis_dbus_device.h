/**
 * @file geis_dbus_device.h
 * @brief Interface for the GEIS DBus device transport.
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GEIS_DBUS_DEVICE_H_
#define GEIS_DBUS_DEVICE_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * Creates a Dbus "device available" message from a GEIS device.
 *
 * @param[in] device  A GEIS device.
 */
DBusMessage *
geis_dbus_device_available_message_from_device(GeisDevice device);

/**
 * Creates GEIS device from a DBus "device available" message.
 *
 * @param[in] message  A DBus message.
 */
GeisDevice
geis_dbus_device_device_from_available_message(DBusMessage *message);

/**
 * Creates a Dbus "device unavailable" message from a GEIS device.
 *
 * @param[in] device  A GEIS device.
 */
DBusMessage *
geis_dbus_device_unavailable_message_from_device(GeisDevice device);

/**
 * Creates GEIS device from a DBus "device unavailable" message.
 *
 * @param[in] message  A DBus message.
 */
GeisDevice
geis_dbus_device_device_from_unavailable_message(DBusMessage *message);

#endif /* GEIS_DBUS_DEVICE_H_ */
