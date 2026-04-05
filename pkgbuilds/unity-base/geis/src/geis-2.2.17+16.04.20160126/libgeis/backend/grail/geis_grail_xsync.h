/**
 * @file geis_grail_xsync.h
 * @brief Handles XSync lib usage for the GEIS grail backend
 */
/*
 * Copyright 2012 Canonical Ltd.
 *
 * This file is part of GEIS.
 *
 * GEIS is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * GEIS is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with GEIS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GEIS_BACKEND_GRAIL_XSYNC_H_
#define GEIS_BACKEND_GRAIL_XSYNC_H_

#include <X11/Xlib.h> // For Display and XEvent
#include <stdint.h> // For uint64_t
#include "geis/geisimpl.h" // For GeisBoolean

/**
 * The opaque "X Synchronization Extension Library" handler
 *
 * It facilitates and encapsulates the use of the XSync extension. Specially
 * by managing the XSyncAlarm instances needed to implement timeouts.
 */
typedef struct GeisGrailXSync *GeisGrailXSync;

/**
 * Constructs a new GeisGrailXSync instance.
 *
 * It returns NULL if the initialization wasn't successful.
 *
 * OBS: Having multiple GeisGrailXSync instances is not supported and doesn't
 * make sense.
 */
GeisGrailXSync
geis_grail_xsync_new(Display *display);

/**
 * Destroys the given GeisGrailXSync
 */
void
geis_grail_xsync_delete(GeisGrailXSync xsync);


/**
 * Creates an XSyncAlarm with the given timeout
 *
 * If there's already an XSyncAlarm with that timeout,
 * nothing is done.
 *
 * An XSyncAlarmNotifyEvent will be sent once that timeout
 * is reached on the server.
 */
void
geis_grail_xsync_set_timeout(GeisGrailXSync xsync, uint64_t timeout);

/**
 * Returns true if the given XEvent is an XSyncAlarmNotifyEvent for
 * one of the existing XSyncAlarm instances and false otherwise.
 *
 * It also takes care of destroying the XSyncAlarm that had its
 * timeout reached.
 */
GeisBoolean
geis_grail_xsync_is_timeout(GeisGrailXSync xsync, const XEvent *event);

/**
 * Gets the server time contained in the given event.
 *
 * That event must be an XSyncAlarmNotifyEvent. It will be the case when
 * geis_grail_xsync_is_timeout() returns GeisTrue for it.
 *
 */
uint64_t
geis_grail_xsync_get_server_time(const XEvent *event);

#endif // GEIS_BACKEND_GRAIL_XSYNC_H_
