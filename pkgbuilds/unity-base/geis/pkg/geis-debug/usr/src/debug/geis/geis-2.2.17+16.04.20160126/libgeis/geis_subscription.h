/**
 * @file geis_subscription.h
 * @brief internal Geis subscription modul private interface
 *
 * Copyright 2010, 2011 Canonical Ltd.
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
#ifndef GEIS_SUBSCRIPTION_H_
#define GEIS_SUBSCRIPTION_H_

#include <geis/geis.h>
#include "geis_filter.h"


/**
 * @defgroup geis_sub_container A Subscription Container
 * @{
 */

/**
 * A container for subscriptions.
 */
typedef struct _GeisSubBag *GeisSubBag;

typedef GeisSubscription* GeisSubBagIterator;

/**
 * Creates a new Geis Subscription container.
 *
 * @param[in] hint A hint as to how many subscriptions to initially allocate in
 *                 the new container.
 */
GeisSubBag geis_subscription_bag_new(GeisSize size_hint);

/**
 * Destroys a Geis Subscription container.
 *
 * @param[in] bag The bag.
 */
void geis_subscription_bag_delete(GeisSubBag bag);

/**
 * Tells how any entires in a Geis Subscription container.
 *
 * @param[in] bag The bag.
 */
GeisSize geis_subscription_bag_count(GeisSubBag bag);

/**
 * Gets an iterator initialized to the first subscription held in a bag.
 *
 * @param[in] bag The bag.
 */
GeisSubBagIterator geis_subscription_bag_begin(GeisSubBag bag);

/**
 * Increments the subscrition bag iterator.
 *
 * @param[in] bag   The bag.
 * @param[in] iter  The iterator.
 */
GeisSubBagIterator geis_subscription_bag_iterator_next(GeisSubBag         bag,
                                                       GeisSubBagIterator iter);

/**
 * Gets an iterator indicating one-past-the-last sub in a bag.
 *
 * @param[in] bag The bag.
 */
GeisSubBagIterator geis_subscription_bag_end(GeisSubBag bag);

/**
 * Creates a new subscription object in a subscription container.
 *
 * @param[in] bag The container.
 * @param[in] sub The subscription to be added.
 *
 * @returns the index of the newly inserted subscription
 */
GeisSize geis_subscription_bag_insert(GeisSubBag       bag,
                                      GeisSubscription sub);

/**
 * Removes a subscription from a subscription container.
 *
 * @param[in] bag The subscription container.
 * @param[in] sub The subscription to be removed.
 */
void geis_subscription_bag_remove(GeisSubBag       bag,
                                  GeisSubscription sub);

/**
 * Removes all subscriptions from a subscription container.
 *
 * @param[in] bag The subscription container.
 */
void geis_subscription_bag_empty(GeisSubBag bag);

/**
 * Marks all subscriptions in a bag as invalid.
 *
 * @param[in] bag The subscription container.
 *
 * See geis_subscription_invalidate.
 */
void geis_subscription_bag_invalidate(GeisSubBag bag);

/**
 * Looks for an subscription in an subscription container.
 *
 * @param[in] bag The bag.
 */
GeisSubscription geis_subscription_bag_find(GeisSubBag bag, GeisInteger sub_id);

/* @} */

/**
 * Gets the numvber of filters in a subscirption.
 *
 * @param[in] sub  The subscription.
 */
GeisSize geis_subscription_filter_count(GeisSubscription sub);

/**
 * Gets an indicated filter from a subscription.
 *
 * @param[in] sub   The subscription.
 * @param[in] index Indicates which filter to retrieve.
 */
GeisFilter geis_subscription_filter(GeisSubscription sub, GeisSize index);

/**
 * Gets an iterator initialized to the first filter on a subscription.
 *
 * @param[in] sub   The subscription.
 */
GeisFilterIterator
geis_subscription_filter_begin(GeisSubscription sub);

/**
 * Gets an iterator initialized to the one-past-the-last filter on a subscription.
 *
 * @param[in] sub   The subscription.
 */
GeisFilterIterator
geis_subscription_filter_end(GeisSubscription sub);

/**
 * Gets the next filter in sequence.
 *
 * @param[in] sub   The subscription.
 * @param[in] iter  A filter iterator.
 *
 * @returns an interator initialized to the next filter in sequence in the
 * subscription, or an iterator that is equal to geis_subscription_filter_end().
 */
GeisFilterIterator
geis_subscription_filter_next(GeisSubscription sub, GeisFilterIterator iter);

/**
 * Invalidates a subscription.
 *
 * @param[in] sub   The subscription.
 *
 * A subscription becomes invalid when its owning Geis instance has been
 * destroyed but the GeisSubscription instance has not.  This can occur because
 * lifetime of both objects is under external control.
 */
void geis_subscription_invalidate(GeisSubscription sub);

/**
 * Sets the operational flags for the subscription.
 *
 * @param[in] sub   The subscription.
 * @param[in] flags The subscription flags.
 *
 * Changes which flags are set during construction of the subscription instance.
 * It is inappropriate to change the operational flags of an activated
 * subscription.
 *
 * @retval GEIS_STATUS_SUCCESS       Normal successful completion.
 * @retval GEIS_STATUS_NOT_SUPPORTED Call made to an activated subcription.
 */
GeisStatus geis_subscription_set_flags(GeisSubscription sub,
                                       GeisSubscriptionFlags flags);

/**
 * Gets the operational flags for the subscription.
 *
 * @param[in] sub   The subscription.
 */
GeisSubscriptionFlags geis_subscription_flags(GeisSubscription sub);

/**
 * Gets an associated datum from the subscription.
 *
 * @param[in] sub   The subscription.
 */
GeisPointer geis_subscription_pdata(GeisSubscription subscription);

/**
 * Sets an associated datum on the subscription.
 *
 * @param[in] sub   The subscription.
 * @param[in] data  Some data.
 */
void geis_subscription_set_pdata(GeisSubscription subscription, GeisPointer data);

#endif /* GEIS_SUBSCRIPTION_H_ */
