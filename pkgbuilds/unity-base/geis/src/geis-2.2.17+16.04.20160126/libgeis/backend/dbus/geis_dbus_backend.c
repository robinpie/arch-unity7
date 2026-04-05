/**
 * @file geis_dbus_backend.c
 * @brief GEIS DBus client back end
 */

/*
 * Copyright 2011-2013 Canonical Ltd.
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
#include "geis_config.h"
#include "geis_backend.h"
#include "geis_backend_protected.h"

#include "geis_dbus_client.h"
#include "geis_event.h"
#include "geis_logging.h"
#include "geis_private.h"


/**
 * @addtogroup geis_backend_dbus GEIS DBus Back End
 * @ingroup geis_backends
 *
 * A GEIS Back End that is a DBus client, for connecting to a single central
 * GEIS service offering data over the DBus.
 *
 * @{
 */

/** The opaque DBus Back End type. */
typedef struct GeisDBusBackend *GeisDBusBackend;

/** The less opaque DBus Back End structure. */
struct GeisDBusBackend
{
  Geis            geis;
  GeisDBusClient  dbus_client;
};

/** The DBus Back End token type */
typedef struct GeisDBusToken
{
  struct GeisBackendToken base;
  GeisDBusBackend         be;
} *GeisDBusToken;


/**
 * Converts from a GeisBackendToken to an XcbBackendToken.
 */
static inline GeisDBusToken
_geis_dbus_token_from_geis_token(GeisBackendToken gbt)
{
  return (GeisDBusToken)gbt;
}


/**
 * Allocates memory for a token from a pool.
 */
static GeisDBusToken
_geis_dbus_token_allocate(void)
{
  return calloc(1, sizeof(struct GeisDBusToken));
}


/**
 * Returns memory for a token to a pool.
 */
static void
_geis_dbus_token_deallocate(GeisDBusToken gdt)
{
  free(gdt);
}


/**
 * Deep-copy-constructs a token.
 */
static GeisBackendToken
_geis_dbus_token_clone(GeisBackendToken original)
{
  return original;
}


/**
 * Releases resources for a token.
 *
 * @param[in] token  A %GeisDBusToken.
 */
static void             
_geis_dbus_token_finalize(GeisBackendToken token GEIS_UNUSED)
{
  GeisDBusToken gdt = _geis_dbus_token_from_geis_token(token);
  _geis_dbus_token_deallocate(gdt);
}


/**
 * Composes one token onto another.
 *
 * @param[in,out] lhs
 * @param[in]     rhs
 */
static void             
_geis_dbus_token_compose(GeisBackendToken lhs GEIS_UNUSED,
                         GeisBackendToken rhs GEIS_UNUSED)
{
}


/**
 * Activates a DBus back end token.
 *
 * @param[in] token        A %GeisDBusToken.
 * @param[in] subscription The subscrition the token will be activated on.
 *
 * Sends a request to the server to activate a subscription with the tokenized
 * content.
 *
 * @returns GEIS_STATUS_SUCCESS.
 */
static GeisStatus       
_geis_dbus_token_activate(GeisBackendToken token, GeisSubscription subscription)
{
  GeisDBusToken gdt = _geis_dbus_token_from_geis_token(token);
  geis_dbus_client_subscribe(gdt->be->dbus_client, subscription);
  return GEIS_STATUS_SUCCESS;
}


/**
 * Deactivates a DBus back end token.
 *
 * @param[in] token        A %GeisDBusToken.
 */
static GeisStatus       
_geis_dbus_token_deactivate(GeisBackendToken token, GeisSubscription subscription)
{
  GeisDBusToken gdt = _geis_dbus_token_from_geis_token(token);
  geis_dbus_client_unsubscribe(gdt->be->dbus_client, subscription);
  return GEIS_STATUS_UNKNOWN_ERROR;
}

static void
_geis_dbus_token_free_subscription_pdata(GeisBackendToken token GEIS_UNUSED,
                                         GeisSubscription subscription GEIS_UNUSED)
{
}

static struct GeisBackendTokenVtable _token_vtbl = {
  _geis_dbus_token_clone,
  _geis_dbus_token_finalize,
  _geis_dbus_token_compose,
  _geis_dbus_token_activate,
  _geis_dbus_token_deactivate,
  _geis_dbus_token_free_subscription_pdata
};


/**
 * Constructs a DBus back end.
 *
 * @param[in] mem
 * @param[in] geis
 */
static void
_geis_dbus_backend_construct(void *mem, Geis geis)
{
  GeisDBusBackend gdb = (GeisDBusBackend)mem;
  gdb->geis = geis;

  gdb->dbus_client = geis_dbus_client_new(geis);
  if (!gdb->dbus_client)
  {
    geis_error("error creating GEIS DBus client");
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    goto final_exit;
  }

final_exit:
  return;
}


/**
 * Deconstructs a DBus back end.
 *
 * @param[in] be  A %GeisDBusBackend.
 */
static void 
_geis_dbus_backend_finalize(GeisBackend be)
{
  GeisDBusBackend gdb = (GeisDBusBackend)be;
  geis_dbus_client_delete(gdb->dbus_client);
}


/**
 * Creates DBus-back-end-specific back end token.
 */
static GeisBackendToken
_geis_dbus_backend_create_token(GeisBackend be,
                                GeisBackendTokenInitState init_state GEIS_UNUSED)
{
  GeisDBusBackend gdb = (GeisDBusBackend)be;
  GeisDBusToken token = _geis_dbus_token_allocate();
  if (token)
  {
    token->base.vtbl = &_token_vtbl;
    token->be = gdb;
  }
  return (GeisBackendToken)token;
}


static GeisStatus
_geis_dbus_accept_gesture(GeisBackend   be,
                          GeisGroup     group,
                          GeisGestureId gesture_id)
{
  GeisDBusBackend gdb = (GeisDBusBackend)be;
  return geis_dbus_client_accept_gesture(gdb->dbus_client, group, gesture_id);
}


static GeisStatus
_geis_dbus_reject_gesture(GeisBackend   be,
                          GeisGroup     group,
                          GeisGestureId gesture_id)
{
  GeisDBusBackend gdb = (GeisDBusBackend)be;
  return geis_dbus_client_reject_gesture(gdb->dbus_client, group, gesture_id);
}


static GeisStatus
_geis_dbus_get_configuration(GeisBackend      be GEIS_UNUSED,
                             GeisSubscription subscription GEIS_UNUSED,
                             GeisString       item_name GEIS_UNUSED,
                             GeisPointer      item_value GEIS_UNUSED)
{
  return GEIS_STATUS_NOT_SUPPORTED;
}


static GeisStatus
_geis_dbus_set_configuration(GeisBackend      be GEIS_UNUSED,
                             GeisSubscription subscription GEIS_UNUSED,
                             GeisString       item_name GEIS_UNUSED,
                             GeisPointer      item_value GEIS_UNUSED)
{
  return GEIS_STATUS_NOT_SUPPORTED;
}



static struct GeisBackendVtable gdb_vtbl = {
  _geis_dbus_backend_construct,
  _geis_dbus_backend_finalize,
  _geis_dbus_backend_create_token,
  _geis_dbus_accept_gesture,
  _geis_dbus_reject_gesture,
  NULL,
  NULL,
  _geis_dbus_get_configuration,
  _geis_dbus_set_configuration,
};


/**
 * Registers the back end with the GEIS back end registry.
 */
static void __attribute__((constructor))
_register_dbus_backend(void)
{
  geis_register_backend(GEIS_INIT_DBUS_BACKEND,
                        sizeof(struct GeisDBusBackend),
                        &gdb_vtbl);
}


/** A dummy routine to force linkage of this module without dlopening it */
void
geis_include_dbus_backend(void)
{
}

/** @} */
