/**
 * @file geis_dbus.h
 * @brief Common definitions for the GEIS DBus module(s).
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
#ifndef GEIS_DBUS_H_
#define GEIS_DBUS_H_

#define GEIS_DBUS_SERVICE_PATH        "/com/canonical/oif/Geis"
#define GEIS_DBUS_SERVICE_INTERFACE   "com.canonical.oif.Geis"

#define GEIS_DBUS_GET_SERVER_ADDRESS  "GetServerAddress"

#define GEIS_DBUS_INIT_COMPLETE       "InitComplete"

#define GEIS_DBUS_DEVICE_AVAILABLE    "DeviceAvailable"
#define GEIS_DBUS_DEVICE_UNAVAILABLE  "DeviceUnavailable"

#define GEIS_DBUS_CLASS_AVAILABLE     "ClassAvailable"
#define GEIS_DBUS_CLASS_UNAVAILABLE   "ClassUnavailable"

#define GEIS_DBUS_REGION_AVAILABLE    "RegionAvailable"
#define GEIS_DBUS_REGION_UNAVAILABLE  "RegionUnavailable"

#define GEIS_DBUS_SUBSCRIPTION_CREATE     "SubscriptionCreate"
#define GEIS_DBUS_SUBSCRIPTION_ACTIVATE   "SubscriptionActivate"
#define GEIS_DBUS_SUBSCRIPTION_DEACTIVATE "SubscriptionDeactivate"
#define GEIS_DBUS_SUBSCRIPTION_DESTROY    "SubscriptionDestroy"

#define GEIS_DBUS_GESTURE_EVENT       "GestureEvent"

#define GEIS_DBUS_ERROR_SUBSCRIPTION_FAIL GEIS_DBUS_SERVICE_INTERFACE \
                                          ".SubscriptionFail"

#endif /* GEIS_DBUS_H_ */
