/**
 * @file geis_ugsubscription_store.h
 * @brief grail subscritpion storage
 */
/*
 * Copyright 2012 Canonical Ltd.
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
#ifndef GEIS_BACKEND_GRAIL_UGSUBSCRIPTION_STORE_H_
#define GEIS_BACKEND_GRAIL_UGSUBSCRIPTION_STORE_H_

#include "geis/geis.h"
#include "geis_bag.h"
#include "geis_grail_window_grab.h"
#include <oif/grail.h>


/**
 * A place to keep grail subscription objects.
 *
 * This is an opaque pointer.
 *
 * The store is keyed by (device_id, window_id).
 */
typedef GeisBag GeisUGSubscriptionStore;

/**
 * Creates a new, empty grail subscription store.
 *
 * @returns a pointer to a valid store, or NULL to indicate failure.
 */
GeisUGSubscriptionStore
geis_ugsubscription_store_new();

/**
 * Destroys a grail subscription store.
 * @param[in] store   The grail subscription store.
 */
void
geis_ugsubscription_delete(GeisUGSubscriptionStore store);

/**
 * Gets a count of the number of ugsubs in the store.
 * @param[in] store     The grail subscription store.
 *
 * @returns the number of UGSubscriptions stored within.
 */
GeisSize
geis_ugsubscription_count(GeisUGSubscriptionStore store);

/**
 * Gets a UGSubscription from the store.
 * @param[in] store    The grail subscription store.
 * @param[in] index    Indicates a ugsub.
 *
 * Gets a UGSubscription from the store at the given index.
 *
 * @returns a valid UGSubscription or NULL to indicate failure.
 */
UGSubscription
geis_ugsubscription_get_ugsubscription_at(GeisUGSubscriptionStore store,
                                          GeisSize                index);

/*
 * Creates a new UGSubscription and adds it to the store.
 * @param[in] store     The grail subscription store.
 * @param[in] filter    Identifies a filter.
 * @param[in] device    Identifies a device.
 * @param[in] region_id Identifies a region.
 *
 * Creates a new UGSubscription for the identified filter, device,
 * and region. It's added to the store. A separate grail subscription
 * is required for each (filter, device, window) because filters on a
 * geis subscription are ORed, meaning each filter may have a different
 * minimum touch requirement.
 *
 * @returns a valid UGSubscription or NULL to indicate failure.
 */
UGSubscription
geis_ugsubscription_create_ugsubscription(GeisUGSubscriptionStore store,
                                          GeisFilter              filter,
                                          UFDevice                device,
                                          GeisInteger             region_id);

/**
 * Gets a UGSubscription from the store.
 * @param[in] store     The grail subscription store.
 * @param[in] filter    Identifies a filter.
 * @param[in] device    Identifies a device.
 * @param[in] region_id Identifies a region.
 *
 * Gets a UGSubscription from the store for the identified filter, device,
 * and region.  A separate grail subscription is required for each (filter,
 * device, window) because filters on a geis subscription are ORed, meaning each
 * filter may have a different minimum touch requirement.
 *
 * @returns a valid UGSubscription or NULL to indicate failure.
 */
UGSubscription
geis_ugsubscription_get_ugsubscription(GeisUGSubscriptionStore store,
                                       GeisFilter              filter,
                                       UFDevice                device,
                                       GeisInteger             region_id);

/**
 * Releases all UGSubscriptions for a (filter, device).
 * @param[in] store        The grail subscription store.
 * @param[in] filter       Identifies a filter.
 * @param[in] device       Identifies a device.
 * @param[in] window_grabs A window grab collection.
 */
void
geis_ugsubscription_release_for_device(GeisUGSubscriptionStore  store,
                                       GeisFilter               filter,
                                       UFDevice                 device,
                                       GeisGrailWindowGrabStore window_grabs);

/**
 * Releses a UGSubscription from the store.
 * @param[in] store  The grail subscription store.
 * @param[in] ugsub  The UGSubscription to release.
 */
void
geis_ugsubscription_release_ugsubscription(GeisUGSubscriptionStore store,
                                           UGSubscription          ugsub);
#endif /* GEIS_BACKEND_GRAIL_UGSUBSCRIPTION_STORE_H_ */

