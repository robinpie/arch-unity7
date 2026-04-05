/**
 * @file geis_backend.h
 * @brief internal GEIS back end base class public interface
 *
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_BACKEND_H_
#define GEIS_BACKEND_H_

#include "geis/geis.h"
#include "geis_backend_token.h"

/**
 * Provides a virtual "base class" for various GEIS back ends.
 *
 * The GEIS is an API that implements the facade pattern over some number of
 * actual "back end" implementations.  Most internal operations of the API are
 * performed through this "virtual base class" interface rather than through the
 * concrete back ends (isn't object-oriented programming great?).
 */
typedef struct GeisBackend *GeisBackend;

/**
 * Creates a back end by name.
 *
 * @param[in] name  Names a back end.
 */
GeisBackend geis_backend_by_name(Geis geis, GeisString name);

/**
 * Destroys the back end.
 *
 * @parameter[in] be  The back end.
 *
 * This function behaves like a virtual destructor and chains through to the
 * concrete destructor call.
 */
void geis_backend_delete(GeisBackend be);

/**
 * Gets the name of the back end.
 *
 * @parameter[in] be  The back end.
 *
 * This accessor is useful for diagnostics.
 */
GeisString geis_backend_name(GeisBackend be);

/**
 * Creates a new back end token.
 */
GeisBackendToken geis_backend_create_token(GeisBackend be,
                                           GeisBackendTokenInitState);

/**
 * Marks a gesture as accepted by the back end.
 *
 * @param[in] be          The GEIS back end.
 * @param[in] group       The gesture group containing the accepted gesture.
 * @param[in] gesture_id  Identifies the gesture.
 */
GeisStatus
geis_backend_gesture_accept(GeisBackend   be,
                            GeisGroup     group,
                            GeisGestureId gesture_id);

/**
 * Marks a gesture as rejected by the back end.
 *
 * @param[in] be          The GEIS back end.
 * @param[in] group       The gesture group containing the rejected gesture.
 * @param[in] gesture_id  Identifies the gesture.
 */
GeisStatus
geis_backend_gesture_reject(GeisBackend   be,
                            GeisGroup     group,
                            GeisGestureId gesture_id);

/**
 * Tells the back end to activate a device after it's been set up.
 *
 * @param[in] be          The GEIS back end.
 * @param[in] device      The GEIS device for which subscription wll activate.
 *
 * The back end sends and event to the middle end when a new device has been
 * detected.  The middle end sets up some structures etc., then tells the back
 * end to activate the device (which may actually cause some subscriptions to be
 * activated), then forwards the new-device event to the front end where the
 * application can be made aware of it.
 */
GeisStatus
geis_backend_activate_device(GeisBackend  be,
                             GeisDevice   device);

/**
 * Tells the back end to activate a device after it's been set up.
 *
 * @param[in] be          The GEIS back end.
 * @param[in] device      The GEIS device for which subscription wll deactivate.
 *
 * See geis_backend_activate_device for more information.
 */
GeisStatus
geis_backend_deactivate_device(GeisBackend  be,
                               GeisDevice   device);

/**
 * Gets a back end configuration value.
 *
 * @param[in]  be                       The back end.
 * @param[in]  subscription             A subscription from the back end (or NULL
 *                                      if a non-subscription configuration).
 * @param[in]  configuration_item_name  The name of a configuration item.
 * @param[out] configuration_item_value A pointer to a variable of the
 *                                      appropriate type to receive the
 *                                      configuration item value.
 *
 * @retval GEIS_STATUS_SUCCESS       The configuration item is supported and the
 *                                   item value has been successfully retrieved.
 * @retval GEIS_STATUS_NO_SUPPORTED  The configuration item is not supported on
 *                                   this backend.
 * @retval GEIS_STATUS_UNKNOWN_ERROR The configuration item is supported but an
 *                                   error occurred when attempting to get the
 *                                   item value.
 */
GeisStatus
geis_backend_get_configuration(GeisBackend      be,
                               GeisSubscription subscription,
                               GeisString       configuration_item_name,
                               GeisPointer      configuration_item_value);

/**
 * Sets a back end configuration value.
 *
 * @param[in] be                       The back end.
 * @param[in] subscription             A subscription from the back end (or NULL
 *                                     if a non-subscription configuration).
 * @param[in] configuration_item_name  The name of a configuration item.
 * @param[in] configuration_item_value A pointer to a value to set.
 *
 * @retval GEIS_STATUS_SUCCESS       The configuration item is supported and has
 *                                   been successfully set.
 * @retval GEIS_STATUS_NO_SUPPORTED  The configuration item is not supported on
 *                                   this backend.
 * @retval GEIS_STATUS_UNKNOWN_ERROR The configuration item is supported but an
 *                                   error occurred when attempting to set the
 *                                   item value.
 */
GeisStatus
geis_backend_set_configuration(GeisBackend      be,
                               GeisSubscription subscription,
                               GeisString       configuration_item_name,
                               GeisPointer      configuration_item_value);
#endif /* GEIS_BACKEND_H_ */
