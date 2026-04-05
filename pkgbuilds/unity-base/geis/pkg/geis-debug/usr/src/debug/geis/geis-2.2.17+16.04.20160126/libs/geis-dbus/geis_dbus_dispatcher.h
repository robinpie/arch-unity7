/**
 * @file geis_dbus_dispatcher.h
 * @brief Interface for the GEIS DBus dispatcher.
 *
 * The GEIS DBus dispatcher provides a central dispatch point for all DBus
 * events used internally by GEIS.
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
#ifndef GEIS_DBUS_DISPATCHER_H_
#define GEIS_DBUS_DISPATCHER_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * The %GeisDBusDispatcher centralizes all dispatch for all DBus events used
 * internally by the GEIS client-server mechanism.
 *
 * Geis implements single-threaded ansynchronous dispatch for DBus events, in
 * case a single-threaded client is making use of GEIS.  This also simplifies a
 * lot of the internal design of the GEIS DBus service, since no locking or
 * other forms of synchronization are required.
 */
typedef struct GeisDBusDispatcher *GeisDBusDispatcher;

/**
 * A callback type for the handler function dispatched on DBus events.
 *
 * @todo the parameters of the GeisDispatchCallback must be defined.
 */
typedef void (*GeisDispatchCallback)(void *context);


/**
 * Creates a new GEIS DBus dispatcher.
 *
 * @param[in] geis  A GEIS instance.
 *
 * Creates a new %GeisDBusDispatcher and registers it with the GEIS API instance
 * so it will be multiplexed and receive event notification.
 *
 * @returns a new %GeisDBusDispatcher object or NULL on failure.
 */
GeisDBusDispatcher
geis_dbus_dispatcher_new(Geis geis);

/**
 * Destroys an existing %GeisDBusDispatcher object.
 *
 * @param[in] dispatcher  A %GeisDBusDispatcher object.
 */
void
geis_dbus_dispatcher_delete(GeisDBusDispatcher dispatcher);

/**
 * Registers a new DBusWatch with a %GeisDBusDispatcher object.
 *
 * @param[in] dispatcher  A %GeisDBusDispatcher object.
 * @param[in] connection  A DBus connection.
 * @param[in] watch       A DBusWatch om the connection.
 *
 * The @p watch will be registered with the @p dispatcher for future DBus
 * events.  The @p watch may or may not be activated, depending on its @a
 * enabled state at the time of registration.
 */
void
geis_dbus_dispatcher_register(GeisDBusDispatcher  dispatcher,
                              DBusConnection     *connection,
                              DBusWatch          *watch);

/**
 * Unregisters a DBusWatch for events.
 *
 * @param[in] dispatcher  A %GeisDBusDispatcher object.
 * @param[in] watch       A DBusWatch.
 */
void
geis_dbus_dispatcher_unregister(GeisDBusDispatcher  dispatcher,
                                DBusWatch          *watch);

/**
 * Marks a DBusWatch as active or not, depending on its state.
 *
 * @param[in] dispatcher  A %GeisDBusDispatcher object.
 * @param[in] watch       A pointer to a DBusWatch object.
 *
 * The @p dispatcher will listen for events on the @p watch if its @a is_enabled
 * state is true or not.
 */
void
geis_dbus_dispatcher_toggle_watch(GeisDBusDispatcher  dispatcher,
                                  DBusWatch          *watch);



#endif /* GEIS_DBUS_DISPATCHER_H_ */
