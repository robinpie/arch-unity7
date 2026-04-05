/**
 * @file geis_dbus_client_proxy.h
 * @brief Interface for the GEIS DBus client proxy.
 *
 * The GEIS DBus client proxy provides client_proxy-side handling of client
 * requests.
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
#ifndef GEIS_DBUS_CLIENT_PROXY_H_
#define GEIS_DBUS_CLIENT_PROXY_H_

#include "geis/geis.h"
#include "geis_dbus_dispatcher.h"
#include "geis_dbus_server.h"


/**
 * Creates a new %GeisDBusClientProxy object.
 */
GeisDBusClientProxy
geis_dbus_client_proxy_new(GeisDBusServer server, DBusConnection *connection);

/**
 * Destroys a %GeisDBusClientProxy.
 *
 * @param[in]  client_proxy  A %GeisDBusClientProxy.
 */
void
geis_dbus_client_proxy_delete(GeisDBusClientProxy client_proxy);

/**
 * Handles a GEIS event.
 *
 * @param[in]  client_proxy  A %GeisDBusClientProxy.
 * @param[in]  event         A %GeisEvent.
 */
void
geis_dbus_client_proxy_handle_geis_event(GeisDBusClientProxy client_proxy,
                                         GeisEvent   event);

#endif /* GEIS_DBUS_CLIENT_PROXY_H_ */
