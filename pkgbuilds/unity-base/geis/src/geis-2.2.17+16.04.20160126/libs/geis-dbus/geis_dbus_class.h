/**
 * @file geis_dbus_class.h
 * @brief Interface for the GEIS DBus gesture class transport.
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
#ifndef GEIS_DBUS_GESTURE_CLASS_H_
#define GEIS_DBUS_GESTURE_CLASS_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * Creates a Dbus "class available" message from a GEIS class.
 *
 * @param[in] class  A GEIS gesture class.
 */
DBusMessage *
geis_dbus_class_available_message_from_class(GeisGestureClass gesture_class);

/**
 * Creates GEIS class from a DBus "class available" message.
 *
 * @param[in] message  A DBus message.
 */
GeisGestureClass
geis_dbus_class_class_from_available_message(DBusMessage *message);

#endif /* GEIS_DBUS_GESTURE_CLASS_H_ */
