/**
 * @file geis_dbus_client.h
 * @brief Interface for the GEIS DBus client.
 *
 * The GEIS DBus client offers remote GEIS functionality over a managed
 * DBus connection.
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
#ifndef GEIS_DBUS_CLIENT_H_
#define GEIS_DBUS_CLIENT_H_

#include "geis/geis.h"
#include "geis_dbus_dispatcher.h"


typedef struct GeisDBusClient *GeisDBusClient;


/**
 * Creates a new %GeisDBusClient object.
 */
GeisDBusClient
geis_dbus_client_new(Geis geis);

/**
 * Destroys a %GeisDBusClient.
 *
 * @param[in]  client  A GeisDBusClient.
 */
void
geis_dbus_client_delete(GeisDBusClient client);

/**
 * Gets the client dispatcher.
 *
 * @param[in]  client  A GeisDBusClient.
 */
GeisDBusDispatcher
geis_dbus_client_dispatcher(GeisDBusClient client);

/**
 * Signals the client the server has been located.
 *
 * @param[in]  client  A GeisDBusClient.
 */
void
geis_dbus_client_server_located(GeisDBusClient client);

/**
 * Signals the client the server has been dislocated.
 *
 * @param[in]  client  A GeisDBusClient.
 */
void
geis_dbus_client_server_dislocated(GeisDBusClient client);

/**
 * Cerates a subscription on the remote end.
 *
 * @param[in] client       The client-side of the DBus connection.
 * @param[in] subscription The local subscription object.
 */
GeisStatus
geis_dbus_client_subscribe(GeisDBusClient   client,
                           GeisSubscription subscription);

/**
 * Deactivates a subscription on the remote end.
 *
 * @param[in] client       The client-side of the DBus connection.
 * @param[in] subscription The local subscription object.
 */
GeisStatus
geis_dbus_client_unsubscribe(GeisDBusClient   client,
                             GeisSubscription subscription);

/**
 * Asks the remote server to accept a gesture.
 *
 * @param[in] client     The client side of the DBus connection.
 * @param[in] group      The gesture group to which the gesture belongs.
 * @param[in] gesture_id The gesture to accept.
 *
 * @returns GEIS_STATUS_SUCCESS on success, some other value otherwise.
 */
GeisStatus
geis_dbus_client_accept_gesture(GeisDBusClient client,
                                GeisGroup      group,
                                GeisGestureId  gesture_id);

/**
 * Asks the remote server to reject a gesture.
 *
 * @param[in] client     The client side of the DBus connection.
 * @param[in] group      The gesture group to which the gesture belongs.
 * @param[in] gesture_id The gesture to reject.
 *
 * @returns GEIS_STATUS_SUCCESS on success, some other value otherwise.
 */
GeisStatus
geis_dbus_client_reject_gesture(GeisDBusClient client,
                                GeisGroup      group,
                                GeisGestureId  gesture_id);

#endif /* GEIS_DBUS_CLIENT_H_ */
