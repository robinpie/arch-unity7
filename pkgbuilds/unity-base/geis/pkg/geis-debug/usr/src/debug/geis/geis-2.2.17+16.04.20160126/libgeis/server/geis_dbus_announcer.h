/**
 * @file geis_dbus_announcer.h
 * @brief Interface for the GEIS DBus announcer.
 *
 * The GEIS DBus announcer makes the location of the GEIS DBus server available
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
#ifndef GEIS_DBUS_ANNOUNCER_H_
#define GEIS_DBUS_ANNOUNCER_H_

#include "geis/geis.h"
#include "geis_dbus_server.h"


typedef struct GeisDBusAnnouncer *GeisDBusAnnouncer;


/**
 * Creates a new GeisDBusAnouncer object.
 *
 * @param[in]  server  The GEIS DBus server to announce.
 */
GeisDBusAnnouncer
geis_dbus_announcer_new(GeisDBusServer server);

/**
 * Destroys a %GeisDBusAnnouncer object.
 *
 * @param[in]  announcer  A GeisDBusAnnouncer.
 */
void
geis_dbus_announcer_delete(GeisDBusAnnouncer announcer);

#endif /* GEIS_DBUS_ANNOUNCER_H_ */
