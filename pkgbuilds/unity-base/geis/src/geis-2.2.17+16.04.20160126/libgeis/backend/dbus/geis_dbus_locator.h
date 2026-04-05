/**
 * @file geis_dbus_locator.h
 * @brief Interface for the GEIS DBus locator.
 *
 * The GEIS DBus locator makes the location of the GEIS DBus server available
 * over the DBus session bus.
 *
 * This header is for internal GEIS use only and contains no client
 * (externally-visible) symbols.
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
#ifndef GEIS_DBUS_LOCATOR_H_
#define GEIS_DBUS_LOCATOR_H_

#include "geis/geis.h"
#include "geis_dbus_client.h"


typedef struct GeisDBusLocator *GeisDBusLocator;


/**
 * Creates a new GeisDBusLocator object.
 *
 * @param[in]  client A GEIS DBus CLient.
 */
GeisDBusLocator
geis_dbus_locator_new(GeisDBusClient client);

/**
 * Destroys a %GeisDBusLocator object.
 *
 * @param[in]  locator  A GeisDBusLocator.
 */
void
geis_dbus_locator_delete(GeisDBusLocator locator);

/**
 * Gets the currently located server address.
 *
 * @param[in]  locator  A GeisDBusLocator.
 */
char *
geis_dbus_locator_server_address(GeisDBusLocator locator);

#endif /* GEIS_DBUS_LOCATOR_H_ */
