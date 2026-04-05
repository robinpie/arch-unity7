/**
 * @file geis_grail_token.c
 * @brief GEIS filter token for the grail-based back end
 */
/*
 * Copyright 2011-2012 Canonical Ltd.
 *
 * This library is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_config.h"
#include "geis_grail_token.h"

#include "geis_backend_token.h"
#include "geis_grail_backend.h"
#include "geis_logging.h"
#include <string.h>
#include <X11/Xlib.h>


/* The maximum nuimber of devices supported. */
#define MAX_NUM_DEVICES 10

/* The maximum nuimber of regions supported. */
#define MAX_NUM_WINDOWS 10


struct GeisGrailToken
{
  struct GeisBackendToken base;
  GeisGrailBackend        be;
  int                     device_count;
  int                     devices[MAX_NUM_DEVICES];
  int                     window_count;
  Window                  windows[MAX_NUM_WINDOWS];
};


/**
 * Converts from a GeisBackendToken to an XcbBackendToken.
 */
static inline GeisGrailToken
_geis_grail_token_from_geis_token(GeisBackendToken gbt)
{
  return (GeisGrailToken)gbt;
}


/**
 * Allocates memory for a token from a pool.
 */
static GeisGrailToken
_geis_grail_token_allocate(void)
{
  return calloc(1, sizeof(struct GeisGrailToken));
}


/**
 * Returns memory for a token to a pool.
 */
static void
_geis_grail_token_deallocate(GeisGrailToken gdt)
{
  free(gdt);
}


/**
 * Deep-copy-constructs a token.
 */
static GeisBackendToken
_geis_grail_token_clone(GeisBackendToken original)
{
  GeisGrailToken new_token = calloc(1, sizeof(struct GeisGrailToken));
  memcpy(new_token,
         _geis_grail_token_from_geis_token(original),
         sizeof(struct GeisGrailToken));
  return (GeisBackendToken)new_token;
}


/**
 * Composes one token onto another.
 *
 * @param[in,out] lhs
 * @param[in]     rhs
 */
static void             
_geis_grail_token_compose(GeisBackendToken lhs GEIS_UNUSED,
                          GeisBackendToken rhs GEIS_UNUSED)
{
}


/**
 * Activates a Grail back end token.
 *
 * @param[in] token        A %GeisGrailToken.
 * @param[in] subscription The subscrition the token will be activated on.
 */
static GeisStatus       
_geis_grail_token_activate(GeisBackendToken token, GeisSubscription sub)
{
  GeisGrailToken gdt = _geis_grail_token_from_geis_token(token);
  GeisStatus status = geis_grail_backend_activate_subscription(gdt->be, sub);
  return status;
}


/**
 * Deactivates a Grail back end token.
 *
 * @param[in] token        A %GeisGrailToken.
 */
static GeisStatus       
_geis_grail_token_deactivate(GeisBackendToken token, GeisSubscription sub)
{
  GeisGrailToken gdt = _geis_grail_token_from_geis_token(token);
  GeisStatus status = geis_grail_backend_deactivate_subscription(gdt->be, sub);
  return status;
}

/**
 * Frees the memory allocated for the GEIS subscription private data
 */
static void
_geis_grail_token_free_subscription_pdata(GeisBackendToken token, GeisSubscription sub)
{
  GeisGrailToken gdt = _geis_grail_token_from_geis_token(token);
  geis_grail_backend_free_subscription_pdata(gdt->be, sub);
}

/**
 * Creates Grail-back-end-specific back end token.
 */
GeisBackendToken
geis_grail_token_new(GeisBackend be,
                     GeisBackendTokenInitState init_state GEIS_UNUSED)
{
  static struct GeisBackendTokenVtable _token_vtbl = {
    _geis_grail_token_clone,
    geis_grail_token_delete,
    _geis_grail_token_compose,
    _geis_grail_token_activate,
    _geis_grail_token_deactivate,
    _geis_grail_token_free_subscription_pdata
  };

  GeisGrailToken token = _geis_grail_token_allocate();
  if (token)
  {
    token->base.vtbl = &_token_vtbl;
    token->be = (GeisGrailBackend)be;
  }
  return (GeisBackendToken)token;
}


/**
 * Releases resources for a token.
 *
 * @param[in] token  A %GeisGrailToken.
 */
void             
geis_grail_token_delete(GeisBackendToken token)
{
  GeisGrailToken gdt = _geis_grail_token_from_geis_token(token);
  _geis_grail_token_deallocate(gdt);
}


GeisStatus
geis_grail_token_add_class_term(GeisBackendToken     gbtoken GEIS_UNUSED,
                                void                *context GEIS_UNUSED,
                                GeisString           name GEIS_UNUSED,
                                GeisFilterOperation  op GEIS_UNUSED,
                                void                *value GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  return status;
}


GeisStatus
geis_grail_token_add_device_term(GeisBackendToken     gbtoken GEIS_UNUSED,
                                 void                *context GEIS_UNUSED,
                                 GeisString           name GEIS_UNUSED,
                                 GeisFilterOperation  op GEIS_UNUSED,
                                 void                *value GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  return status;
}


GeisStatus
geis_grail_token_add_feature_term(GeisBackendToken     gbtoken GEIS_UNUSED,
                                  void                *context GEIS_UNUSED,
                                  GeisString           name GEIS_UNUSED,
                                  GeisFilterOperation  op GEIS_UNUSED,
                                  void                *value GEIS_UNUSED)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  return status;
}


GeisStatus
geis_grail_token_add_region_term(GeisBackendToken     gbtoken,
                                 void                *context GEIS_UNUSED,
                                 GeisString           name,
                                 GeisFilterOperation  op,
                                 void                *value)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisGrailToken token = _geis_grail_token_from_geis_token(gbtoken);
  if (0 == strcmp(name, GEIS_REGION_ATTRIBUTE_WINDOWID)
     && op == GEIS_FILTER_OP_EQ)
  {
    Window window = (Window)*(GeisInteger*)value;
    geis_debug("attr name=\"%s\" windowid=0x%x", name, (unsigned int)window);
    token->windows[token->window_count++] = window;
    status = GEIS_STATUS_SUCCESS;
  }
  return status;
}


