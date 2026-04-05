/**
 * @file geis_dbus_server.h
 * @brief Interface for the GEIS DBus server.
 *
 * The GEIS DBus server offers remote GEIS functionality over a set of managed
 * DBus connections.
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
#ifndef GEIS_DBUS_SERVER_H_
#define GEIS_DBUS_SERVER_H_

#include "geis/geis.h"
#include "geis_dbus_dispatcher.h"


typedef struct GeisDBusServer *GeisDBusServer;
typedef struct GeisDBusClientProxy *GeisDBusClientProxy;


/**
 * Creates a new %GeisDBusServer object.
 */
GeisDBusServer
geis_dbus_server_new(Geis geis);

/**
 * Destroys a %GeisDBusServer.
 *
 * @param[in]  server  A GeisDBusServer.
 */
void
geis_dbus_server_delete(GeisDBusServer server);

/**
 * Gets the address of a %GeisDBusServer.
 *
 * @param[in]  server  A GeisDBusServer.
 */
char *
geis_dbus_server_address(GeisDBusServer server);

/**
 * Gets the server dispatcher.
 *
 * @param[in]  server  A GeisDBusServer.
 */
GeisDBusDispatcher
geis_dbus_server_dispatcher(GeisDBusServer server);

/**
 * Gets the server geis instance.
 *
 * @param[in]  server  A %GeisDBusServer.
 */
Geis
geis_dbus_server_geis(GeisDBusServer server);

/**
 * Shuts down and disconnects a client.
 *
 * @param[in]  server  A %GeisDBusServer.
 * @param[in]  proxy   A %GeisDBusClientProxy.
 */
void
geis_dbus_server_client_disconnect(GeisDBusServer      server,
                                   GeisDBusClientProxy proxy);

#endif /* GEIS_DBUS_SERVER_H_ */
