/**
 * @file geis_backend_token.h
 * @brief internal GEIS back end token public interface
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
#ifndef GEIS_BACKEND_TOKEN_H_
#define GEIS_BACKEND_TOKEN_H_

#include "geis/geis.h"

/**
 * An opaque token representing a set of filter terms in a backend-specific way.
 */
typedef struct GeisBackendToken *GeisBackendToken;

/**
 * The states in which a backend token may be initialized.  The choices are:
 * all gestures or no gestures.
 */
typedef enum GeisBackendTokenInitState
{
  GEIS_BACKEND_TOKEN_INIT_NONE,
  GEIS_BACKEND_TOKEN_INIT_ALL
} GeisBackendTokenInitState;


/**
 * Creates a new backend token.
 *
 * @param[in] geis       The API instance for which the token is created.
 * @param[in] init_state The initial state the token should be in.
 *
 * @returns a new %GeisBackendToken initialized according to @p init_state or
 * NULL on failure.
 */
GeisBackendToken
geis_backend_token_new(Geis geis, GeisBackendTokenInitState init_state);

/**
 * Clones a new backend token from an existing one.
 * @param[in] token  The orignal backend token.
 *
 * @returns a new %GeisBackendToken that is an identical copy of @p token or
 * NULL on failure.  Failure is not really an option.
 */
GeisBackendToken
geis_backend_token_clone(GeisBackendToken token);

/**
 * Destroys a backend token.
 *
 * @param[in] token  The backend token.
 */
void
geis_backend_token_delete(GeisBackendToken token);

/**
 * Composes two backend tokens into a new backend token.
 *
 * @param[in] lhs  A backend token.
 * @param[in] rhs  Another backend token.
 *
 * Composed tokens are effectively ANDed.
 */
void
geis_backend_token_compose(GeisBackendToken lhs, GeisBackendToken rhs);

/**
 * Activates a token in the back end.
 *
 * @param[in] token         A backend token.
 * @param[in] subscription  The subscription under which the token will be
 *                          activated.
 */
GeisStatus
geis_backend_token_activate(GeisBackendToken token,
                            GeisSubscription subscription);

/**
 * Deactivates a token in the back end.
 *
 * @param[in] token          The backend token.
 * @param[in] subscription  The subscription for which the token will be
 *                          deactivated.
 */
GeisStatus
geis_backend_token_deactivate(GeisBackendToken token,
                              GeisSubscription subscription);

/**
 * Frees the private data that the backend allocated for a given subscription
 *
 * @param[in] token          The backend token.
 * @param[in] subscription  The subscription that will have its backend private data
 *                          freed
 */
void
geis_backend_token_free_subscription_pdata(GeisBackendToken token,
                                           GeisSubscription subscription);

#endif /* GEIS_BACKEND_TOKEN_H_ */
