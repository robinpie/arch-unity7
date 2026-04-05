/**
 * @file geis_backend_token.c
 * @brief internal GEIS back end token implementation
 *
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
#include "geis_backend_token.h"

#include "geis_backend_protected.h"
#include "geis_logging.h"


/*
 * Clones a new backend token.
 */
GeisBackendToken
geis_backend_token_clone(GeisBackendToken token)
{
  return token->vtbl->clone(token);
}


/*
 * Destroys a backend token.
 */
void
geis_backend_token_delete(GeisBackendToken token)
{
  token->vtbl->finalize(token);
}


/*
 * Composes two backend tokens into a new backend token.
 */
void
geis_backend_token_compose(GeisBackendToken lhs,
                           GeisBackendToken rhs)
{
  lhs->vtbl->compose(lhs, rhs);
}


/*
 * Activates a token in the back end.
 */
GeisStatus
geis_backend_token_activate(GeisBackendToken token,
                            GeisSubscription subscription)
{
  geis_debug("called");
  return token->vtbl->activate(token, subscription);
}


/**
 * Deactivates a token in the back end.
 */
GeisStatus
geis_backend_token_deactivate(GeisBackendToken token,
                              GeisSubscription subscription)
{
  geis_debug("called");
  return token->vtbl->deactivate(token, subscription);
}

/**
 * Frees the private data that the backend allocated for a given subscription
 */
void
geis_backend_token_free_subscription_pdata(GeisBackendToken token,
                                           GeisSubscription subscription)
{
  token->vtbl->free_subscription_pdata(token, subscription);
}
