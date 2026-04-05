/**
 * @file geis_backend_protected.h
 * @brief internal GEIS back end base class "protected" interface
 *
 * This file contains the implementation interface for the various GEIS v2 back
 * ends.
 */

/*
 * Copyright 2010-2013 Canonical Ltd.
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
#ifndef GEIS_BACKEND_PROTECTED_H_
#define GEIS_BACKEND_PROTECTED_H_

#include "geis/geis.h"
#include "geis_backend.h"
#include "geis_backend_token.h"


/**
 * The custom dispatch table for "derived" GEIS v2 backends.
 */
typedef struct GeisBackendVtable
{
  void             (* construct)(void *mem, Geis geis);
  void             (* finalize)(GeisBackend);
  GeisBackendToken (* create_token)(GeisBackend, GeisBackendTokenInitState);
  GeisStatus       (* accept_gesture)(GeisBackend, GeisGroup, GeisGestureId);
  GeisStatus       (* reject_gesture)(GeisBackend, GeisGroup, GeisGestureId);
  GeisStatus       (* activate_device)(GeisBackend, GeisDevice);
  GeisStatus       (* deactivate_device)(GeisBackend, GeisDevice);
  GeisStatus       (* get_configuration)(GeisBackend, GeisSubscription, GeisString, GeisPointer);
  GeisStatus       (* set_configuration)(GeisBackend, GeisSubscription, GeisString, GeisPointer);
} *GeisBackendVtable;


/**
 * The custom dispatch table for backend-specific tokens.
 *
 * Backend tokens are used when creating subscriptions (including filters and
 * filter terms).
 */
typedef struct GeisBackendTokenVtable
{
  GeisBackendToken (* clone)(GeisBackendToken);
  void             (* finalize)(GeisBackendToken);
  void             (* compose)(GeisBackendToken, GeisBackendToken);
  GeisStatus       (* activate)(GeisBackendToken, GeisSubscription);
  GeisStatus       (* deactivate)(GeisBackendToken, GeisSubscription);
  void             (* free_subscription_pdata)(GeisBackendToken, GeisSubscription);
} *GeisBackendTokenVtable;


struct GeisBackendToken
{
  GeisBackendTokenVtable vtbl;
};

/**
 * Registers back ends with the API.
 *
 * @param[in] name The name of the back end.
 * @param[in] size The size of the back end data structure.
 * @param[in] vtbl A pointer to a (static) function dispatch table.
 *
 * This registration function should be called from a module init routine
 * decorated with the GCC attribute((constructor)) so the back end gets
 * registered with the API.
 */
void geis_register_backend(GeisString        name,
                           GeisSize          size,
                           GeisBackendVtable vtbl);

#endif /* GEIS_BACKEND_PROTECTED_H_ */
