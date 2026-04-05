/**
 * @file geis_dbus_attr.h
 * @brief Interface for the GEIS DBus attr transport.
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
#ifndef GEIS_DBUS_ATTR_H_
#define GEIS_DBUS_ATTR_H_

#include <dbus/dbus.h>
#include "geis/geis.h"


/**
 * The DBus type signature for a GEIS attrlist entry.
 */
#define GEIS_DBUS_TYPE_SIGNATURE_ATTR \
               DBUS_STRUCT_BEGIN_CHAR_AS_STRING \
               DBUS_TYPE_STRING_AS_STRING \
               DBUS_TYPE_VARIANT_AS_STRING \
               DBUS_STRUCT_END_CHAR_AS_STRING

/**
 * Marshalls a single GEIS attr to an open DBus message container iterator.
 *
 * @param[in] attr  The GEIS attr.
 * @param[in] iter  The DBus message iterator.
 */
void 
geis_dbus_attr_marshall(GeisAttr attr, DBusMessageIter *iter);

/**
 * Unmarshalls a single GEIS attr from a DBus message iterator.
 *
 * @param[in] iter  The DBus message iterator.
 *
 * @returns a GEIS attribute or NULL on error.
 */
GeisAttr
geis_dbus_attr_unmarshall(DBusMessageIter *iter);

#endif /* GEIS_DBUS_ATTR_H_ */
