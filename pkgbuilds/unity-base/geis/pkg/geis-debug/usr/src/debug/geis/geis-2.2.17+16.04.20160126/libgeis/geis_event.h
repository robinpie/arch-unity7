/**
 * @file geis_event.c
 * @brief Internal interface of the GeisEvent module
 *
 * Copyright 2010 Canonical Ltd.
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_EVENT_H_
#define GEIS_EVENT_H_

#include "geis/geis.h"

/**
 * Creates a new, empty event of type @p type.
 *
 * @param[in] type  The type of the event.
 */
GeisEvent geis_event_new(GeisEventType type);

/**
 * Overrides the type of the event.
 *
 * @param[in] event The event.
 * @param[in] type  The type of the event.
 */
void geis_event_override_type(GeisEvent event, GeisEventType type);

/**
 * Adds an attr to an event.
 *
 * @param[in] event  The event.
 * @param[in] attr   The attr.
 */
GeisStatus geis_event_add_attr(GeisEvent event, GeisAttr attr);

#endif /* GEIS_EVENT_H_ */
