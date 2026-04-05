/**
 * @file geis_dbus_gesture_event.h
 * @brief Interface for the GEIS DBus gesture event transport.
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
#ifndef GEIS_DBUS_GESTURE_EVENT_H_
#define GEIS_DBUS_GESTURE_EVENT_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * Creates a Dbus "gesture event" message from a GEIS gesture event.
 *
 * @param[in] event  A GEIS event.
 */
DBusMessage *
geis_dbus_gesture_event_message_from_geis_event(GeisEvent event);

/**
 * Indicates if a DBus message is a "gesture event" message.
 *
 * @param[in] message  A DBus message.
 */
GeisBoolean
geis_dbus_message_is_gesture_event(DBusMessage *message);

/**
 * Creates GEIS event from a DBus "gesture_event" message.
 *
 * @param[in] message  A DBus message.
 */
GeisEvent
geis_dbus_gesture_event_from_message(Geis geis, DBusMessage *message);


#endif /* GEIS_DBUS_GESTURE_EVENT_H_ */

