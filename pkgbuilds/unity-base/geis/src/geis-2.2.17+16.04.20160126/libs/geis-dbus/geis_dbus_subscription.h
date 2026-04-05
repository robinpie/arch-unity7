/**
 * @file geis_dbus_subscription.h
 * @brief Interface for the GEIS DBus subscription transport.
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
#ifndef GEIS_DBUS_SUBSCRIPTION_H_
#define GEIS_DBUS_SUBSCRIPTION_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_CREATE method call.
 *
 * @param[in] message  A DBus message.
 *
 * This function is used on the server side to identify if a received message is
 * a GEIS_DBUS_SUBSCRIPTION_CREATE method call.
 *
 * @returns GEIS_TRUE if the message is GEIS_DBUS_SUBSCRIPTION_CREATE,
 *          GEIS_FALSE otherwise.
 */
GeisBoolean
geis_dbus_message_is_subscription_create_call(DBusMessage *message);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_CREATE method call message.
 *
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the client side to create a
 * GEIS_DBUS_SUBSCRIPTION_CREATE method call message from a local
 * %GeisSubscription object.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_create_call_message(GeisSubscription subscription);

/**
 * Creates a %GeisSubscription from a GEIS_DBUS_SUBSCRIPTION_CREATE method call
 * message.
 *
 * @param[in] geis     A %Geis instance.
 * @param[in] message  A DBus message.
 *
 * This function is used on the server side to create a subscription object to
 * proxy the client-side subscription object.
 *
 * @returns a %GeisSubscription or NULL on failure.
 */
GeisSubscription
geis_dbus_subscription_from_create_call_message(Geis geis, DBusMessage *message);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_CREATE method return message.
 *
 * @param[in] message       The DBUs method_call message to reply to.
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the server side to create a response to a received
 * GEIS_DBUS_SUBSCRIPTION_CREATE method call message.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_create_return_message(DBusMessage      *message,
                                             GeisSubscription  subscription);

/**
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_ACTIVATE message.
 *
 * @param[in] message  A DBus message.
 *
 * @returns GEIS_TRUE if the message is GEIS_DBUS_SUBSCRIPTION_ACTIVATE,
 *          GEIS_FALSE otherwise.
 */
GeisBoolean
geis_dbus_message_is_subscription_activate_call(DBusMessage *message);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_ACTIVATE method call message.
 *
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the client side to create a
 * GEIS_DBUS_SUBSCRIPTION_ACTIVATE method call message from a local
 * %GeisSubscription object.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_activate_call_message(GeisSubscription subscription);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_ACTIVATE method return message.
 *
 * @param[in] message       The DBUs method_call message to reply to.
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the server side to create a response to a received
 * GEIS_DBUS_SUBSCRIPTION_ACTIVATE method call message.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_activate_return_message(DBusMessage      *message,
                                               GeisSubscription  subscription);

/**
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_DEACTIVATE message.
 *
 * @param[in] message  A DBus message.
 *
 * @returns GEIS_TRUE if the message is GEIS_DBUS_SUBSCRIPTION_DEACTIVATE,
 *          GEIS_FALSE otherwise.
 */
GeisBoolean
geis_dbus_message_is_subscription_deactivate_call(DBusMessage *message);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_DEACTIVATE method return message.
 *
 * @param[in] message       The DBUs method_call message to reply to.
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the server side to create a response to a received
 * GEIS_DBUS_SUBSCRIPTION_DEACTIVATE method call message.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_deactivate_return_message(DBusMessage      *message,
                                                 GeisSubscription  subscription);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_DEACTIVATE method call message.
 *
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the client side to create a
 * GEIS_DBUS_SUBSCRIPTION_DEACTIVATE method call message from a local
 * %GeisSubscription object.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_deactivate_call_message(GeisSubscription subscription);

/**
 * Indicates if a DBus message is a GEIS_DBUS_SUBSCRIPTION_DESTROY message.
 *
 * @param[in] message  A DBus message.
 *
 * @returns GEIS_TRUE if the message is GEIS_DBUS_SUBSCRIPTION_DESTROY,
 *          GEIS_FALSE otherwise.
 */
GeisBoolean
geis_dbus_message_is_subscription_destroy_call(DBusMessage *message);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_DESTROY method call message.
 *
 * @param[in] subscription  A GEIS subscription.
 *
 * This function is used on the client side to create a
 * GEIS_DBUS_SUBSCRIPTION_DESTROY method call message from a local
 * %GeisSubscription object.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_destroy_call_message(GeisSubscription subscription);

/**
 * Creates a GEIS_DBUS_SUBSCRIPTION_DESTROY method return message.
 *
 * @param[in] message       The DBUs method_call message to reply to.
 *
 * This function is used on the server side to create a response to a received
 * GEIS_DBUS_SUBSCRIPTION_DESTROY method call message.
 *
 * @returns A DBus message object.
 */
DBusMessage *
geis_dbus_subscription_destroy_return_message(DBusMessage *message);


#endif /* GEIS_DBUS_SUBSCRIPTION_H_ */
