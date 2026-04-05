/**
 * @file geis_dbus_proxy_box.h
 * @brief Interface for a storage facility for GEIS DBus server client proxies.
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
#ifndef GEIS_DBUS_CLIENT_BOX_H_
#define GEIS_DBUS_CLIENT_BOX_H_

#include "geis_dbus_client_proxy.h"


/**
 * A Container for GeisDBusProxy objects.
 */
typedef struct GeisDBusProxyBox *GeisDBusProxyBox;

/**
 * An iterator for traversing and accessing client proxies contained in a box.
 *
 * Client proxy box iterators are not threadsafe and should be conidered
 * invalidated if the box is updated during a traversal.
 */
typedef struct GeisDBusProxyBoxNode *GeisDBusProxyBoxIterator;


/**
 * Constructs a %GeisDBusProxyBox.
 *
 * @returns a valid %GeisDBusProxyBox or NULL on failure.
 */
GeisDBusProxyBox
geis_dbus_proxy_box_new();

/**
 * Destroys a %GeisDBusProxyBox.
 *
 * @param[in] box  A %GeisDBusProxyBox.
 */
void
geis_dbus_proxy_box_delete(GeisDBusProxyBox box);

/**
 * Inserts a %GeisDBusClientProxy in to a %GeisDBusProxyBox.
 *
 * @param[in] box    A %GeisDBusProxyBox.
 * @param[in] proxy  The %GeisDBusClientProxy to insert.
 */
void
geis_dbus_proxy_box_insert(GeisDBusProxyBox box, GeisDBusClientProxy proxy);

/**
 * Removes a %GeisDBusClientProxy from a %GeisDBusProxyBox.
 *
 * @param[in] box    A %GeisDBusProxyBox.
 * @param[in] proxy  The %GeisDBusClientProxy to remove.
 */
void
geis_dbus_proxy_box_remove(GeisDBusProxyBox box, GeisDBusClientProxy proxy);

/**
 * Gets an iterator initialized to the first client proxy in a box.
 *
 * @param[in] box    A %GeisDBusProxyBox.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_begin(GeisDBusProxyBox box);

/**
 * Gets an iterator initialized to one-past-the-end of a client proxy box.
 *
 * @param[in] box    A %GeisDBusProxyBox.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_end(GeisDBusProxyBox box);

/**
 * Gets the current client proxy from a client proxy box iterator.
 *
 * @param[in] iter The iterator.
 */
GeisDBusClientProxy
geis_dbus_proxy_box_iter_value(GeisDBusProxyBoxIterator iter);

/**
 * Advances a client proxy box iterator.
 *
 * @param[in] box   A %GeisDBusProxyBox.
 * @param[in] iter  The iterator.
 *
 * @returns the next iterator in sequence.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_iter_next(GeisDBusProxyBox         box,
                              GeisDBusProxyBoxIterator iter);

#endif /* GEIS_DBUS_CLIENT_BOX_H_ */
