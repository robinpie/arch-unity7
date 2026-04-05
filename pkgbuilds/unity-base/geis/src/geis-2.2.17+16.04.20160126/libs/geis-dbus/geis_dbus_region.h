/**
 * @file geis_dbus_region.h
 * @brief Interface for the GEIS DBus region transport.
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
#ifndef GEIS_DBUS_REGION_H_
#define GEIS_DBUS_REGION_H_

#include <dbus/dbus.h>
#include "geis/geis.h"
#include "geis_filterable.h"


/**
 * Creates a Dbus "region available" message from a GEIS region.
 *
 * @param[in] fa  A GEIS region filterable attribute.
 */
DBusMessage *
geis_dbus_region_available_message_from_region(GeisFilterableAttribute fa);

/**
 * Creates GEIS region filterable attribute from a DBus "region available"
 * message.
 *
 * @param[in] message  A DBus message.
 */
GeisFilterableAttribute
geis_dbus_region_from_region_available_message(DBusMessage *message);

#endif /* GEIS_DBUS_REGION_H_ */
