/**
 * @file libgeis/geis_subscription.c
 * @brief implementation of the GEIS v2.0 API subscription module
 *
 * Copyright 2010, 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"
#include "geis_subscription.h"

#include "geis/geis.h"
#include "geis_atomic.h"
#include "geis_backend_token.h"
#include "geis_filter.h"
#include "geis_logging.h"
#include "geis_private.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


struct _GeisSubscription
{
  GeisRefCount          sub_refcount;
  GeisInteger           sub_id;
  Geis                  sub_geis;
  GeisString            sub_name;
  GeisSubscriptionFlags sub_flags;
  GeisBoolean           sub_activated;
  GeisBackendToken      sub_backend_token;
  GeisFilterBag         sub_filters;
  GeisPointer           sub_data;
};

struct _GeisSubBag
{
  GeisSubscription *sub_store;
  GeisSize          sub_store_size;
};

static const float sub_bag_growth_constant = 1.5f;

static void _subscription_destroy(GeisSubscription);

/*
 * Increments the reference count on a subscirption object.
 */
static inline GeisSubscription
_subscription_ref(GeisSubscription sub)
{
  geis_atomic_ref(&sub->sub_refcount);
  return sub;
}


/*
 * Decrements the reference count of a subscription object and possibly destroys
 * the object.
 */
static void
_subscription_unref(GeisSubscription sub)
{
  if (0 == geis_atomic_unref(&sub->sub_refcount))
  {
    _subscription_destroy(sub);
  }
}


/**
 * Creates a new subsciption container.
 */
GeisSubBag
geis_subscription_bag_new(GeisSize size_hint)
{
  GeisSubBag bag = calloc(1, sizeof(struct _GeisSubBag));
  if (!bag)
  {
    geis_error("failed to allocate subscription container");
    goto error_exit;
  }

  bag->sub_store_size = size_hint > 2 ? size_hint : 2;
  bag->sub_store = calloc(bag->sub_store_size, sizeof(GeisSubscription));
  if (!bag->sub_store)
  {
    geis_error("failed to allocate subscription container store");
    free(bag);
    bag = NULL;
  }

error_exit:
  return bag;
}

/**
 * Destroys a subsciption container and free any associated resources.
 *
 * Any contained subsciptions that are still valid are also destroyed.
 */
void
geis_subscription_bag_delete(GeisSubBag bag)
{
  geis_subscription_bag_empty(bag);
  free(bag->sub_store);
  free(bag);
}


/**
 * Gets the number of (valid) subsciptions in the subsciption container.
 */
GeisSize
geis_subscription_bag_count(GeisSubBag bag)
{
  GeisSize count = 0;
  GeisSize i;
  for (i = 0; i < bag->sub_store_size; ++i)
  {
    if (bag->sub_store[i])
      ++count;
  }
  return count;
}


/*
 * Gets an iterator initialized to the first subscription held in a bag.
 */
GeisSubBagIterator
geis_subscription_bag_begin(GeisSubBag bag)
{
  if (bag->sub_store_size > 0 && bag->sub_store[0])
    return &bag->sub_store[0];
  return geis_subscription_bag_end(bag);
}


/*
 * Increments the subscrition bag iterator.
 */
GeisSubBagIterator
geis_subscription_bag_iterator_next(GeisSubBag         bag,
                                    GeisSubBagIterator iter)
{
  for (++iter;
       (GeisSize)(iter - bag->sub_store) < bag->sub_store_size;
       ++iter)
  {
    if (*iter)
      return iter;
  }
  return geis_subscription_bag_end(bag);
}


/*
 * Gets an iterator indicating one-past-the-last sub in a bag.
 */
GeisSubBagIterator
geis_subscription_bag_end(GeisSubBag bag GEIS_UNUSED)
{
  return NULL;
}


/**
 * Inserts a subscription in the subscription container.
 */
GeisSize
geis_subscription_bag_insert(GeisSubBag       bag,
                             GeisSubscription sub)
{
  GeisSize index = -1;
  for (index = 0; index <  bag->sub_store_size; ++index)
  {
    if (!bag->sub_store[index])
    {
      bag->sub_store[index] = _subscription_ref(sub);
      goto final_exit;
    }
  }

  GeisSize new_store_size = ceilf(bag->sub_store_size * sub_bag_growth_constant);
  GeisSubscription *new_store = realloc(bag->sub_store,
           new_store_size * sizeof(struct _GeisSubBag));
  if (!new_store)
  {
    geis_error("failed to reallocate sub bag");
    index = -1;
    goto final_exit;
  }
  index = bag->sub_store_size;
  memset(&new_store[index], 0,
         (new_store_size - index) * sizeof(struct _GeisSubBag));

  bag->sub_store = new_store;
  bag->sub_store_size = new_store_size;
  bag->sub_store[index] = _subscription_ref(sub);

final_exit:
  return index;
}


void
geis_subscription_bag_remove(GeisSubBag       bag,
                             GeisSubscription sub)
{
  if (bag->sub_store[sub->sub_id])
  {
    bag->sub_store[sub->sub_id] = NULL;
    _subscription_unref(sub);
  }
  geis_debug("subscription \"%s\" removed", sub->sub_name);
}


/*
 * Removes all subscriptions from a subscription container.
 */
void
geis_subscription_bag_empty(GeisSubBag bag)
{
  GeisSize i;
  for (i = 0; i < bag->sub_store_size; ++i)
  {
    if (bag->sub_store[i])
    {
      _subscription_unref(bag->sub_store[i]);
      bag->sub_store[i] = NULL;
    }
  }
}


void
geis_subscription_bag_invalidate(GeisSubBag bag)
{
  GeisSize i;
  for (i = 0; i < bag->sub_store_size; ++i)
  {
    if (bag->sub_store[i])
      geis_subscription_invalidate(bag->sub_store[i]);
  }
}


/**
 * Finds a subsciption by ID.
 */
GeisSubscription
geis_subscription_bag_find(GeisSubBag bag, GeisInteger sub_id)
{
  GeisSubscription sub = NULL;
  GeisSize i;
  for (i = 0; i < bag->sub_store_size; ++i)
  {
    if (bag->sub_store[i] && bag->sub_store[i]->sub_id == sub_id)
    {
      sub = bag->sub_store[i];
      break;
    }
  }
  return sub;
}


/*
 * Activates a subscirption.
 */
GeisStatus
geis_subscription_activate(GeisSubscription sub)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisSize i;
  GeisBoolean is_system = sub->sub_flags & GEIS_SUBSCRIPTION_CONT;
  GeisBoolean is_grab = sub->sub_flags & GEIS_SUBSCRIPTION_GRAB;

  /* reset any existing token */
  geis_subscription_deactivate(sub);
  if (geis_filter_bag_count(sub->sub_filters))
  {
    sub->sub_backend_token = geis_backend_token_new(sub->sub_geis,
                                                    GEIS_BACKEND_TOKEN_INIT_NONE);
  }
  else
  {
    sub->sub_backend_token = geis_backend_token_new(sub->sub_geis,
                                                    GEIS_BACKEND_TOKEN_INIT_ALL);
  }

  /* create a new token from the filter tokens and activate it */
  for (i = 0; i < geis_filter_bag_count(sub->sub_filters); ++i)
  {
    GeisFilter filter = geis_filter_bag_filter(sub->sub_filters, i);
    geis_backend_token_compose(sub->sub_backend_token, geis_filter_token(filter));
  }

  geis_filterable_attribute_foreach(sub->sub_geis,
                                    GEIS_FILTER_SPECIAL,
                                    sub->sub_backend_token,
                                    GEIS_GESTURE_TYPE_SYSTEM,
                                    GEIS_FILTER_OP_EQ,
                                    &is_system);

  geis_filterable_attribute_foreach(sub->sub_geis,
                                    GEIS_FILTER_SPECIAL,
                                    sub->sub_backend_token,
                                    "GRAB",
                                    GEIS_FILTER_OP_EQ,
                                    &is_grab);

  status = geis_backend_token_activate(sub->sub_backend_token, sub);
  return status;
}


/*
 * Deactivates a subscirption.
 */
GeisStatus
geis_subscription_deactivate(GeisSubscription sub)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (sub->sub_backend_token)
  {
    status = geis_backend_token_deactivate(sub->sub_backend_token, sub);
    sub->sub_backend_token = NULL;
  }
  return status;
}


void
geis_subscription_invalidate(GeisSubscription sub)
{
  geis_subscription_deactivate(sub);
  geis_filter_bag_delete(sub->sub_filters);
  sub->sub_filters = NULL;
  geis_unref(sub->sub_geis);
  sub->sub_geis = NULL;
}


/**
 * Creates a new subsciption on an API instance.
 */
GeisSubscription
geis_subscription_new(Geis                  geis,
                      GeisString            sub_name,
                      GeisSubscriptionFlags sub_flags)
{
  GeisSubscription sub = calloc(1, sizeof(struct _GeisSubscription));
  if (!sub)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating subscription");
    goto final_exit;
  }

  if (sub_name)
  {
    sub->sub_name = strdup(sub_name);
  }
  else
  {
    sub->sub_name = strdup("");
  }
  if (!sub->sub_name)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating subscription name");
    goto unwind_sub;
  }

  sub->sub_filters = geis_filter_bag_new();
  if (!sub->sub_filters)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating subscription filters");
    goto unwind_name;
  }

  sub->sub_id = geis_add_subscription(geis, sub);
  if (sub->sub_id < 0)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error storing subscription");
    goto unwind_filters;
  }

  sub->sub_geis = geis_ref(geis);
  sub->sub_flags = sub_flags;
  sub->sub_backend_token = NULL;
  _subscription_ref(sub);
  geis_debug("created subscription \"%s\" id %d", sub->sub_name, sub->sub_id);
  goto final_exit;

unwind_filters:
  geis_filter_bag_delete(sub->sub_filters);
unwind_name:
  free((char *)sub->sub_name);
unwind_sub:
  free(sub);
  sub = NULL;
final_exit:
  return sub;
}


void
_subscription_destroy(GeisSubscription sub)
{
  geis_debug("destroying subscription \"%s\" id %d", sub->sub_name, sub->sub_id);
  geis_subscription_deactivate(sub);

  if (sub->sub_backend_token)
  {
    geis_backend_token_free_subscription_pdata(sub->sub_backend_token, sub);
    geis_backend_token_delete(sub->sub_backend_token);
    sub->sub_backend_token = NULL;
  }

  if (sub->sub_geis != NULL)
  {
    geis_unref(sub->sub_geis);
  }
  if (sub->sub_filters != NULL)
  {
    geis_filter_bag_delete(sub->sub_filters);
  }
  free((char *)sub->sub_name);
  free(sub);
}


/**
 * Frees all resources allocated for the subscription and destroys the
 * subscription.  Use of the subscription object after this call will result in
 * undefined behaviour.
 */
GeisStatus
geis_subscription_delete(GeisSubscription sub)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (!sub)
  {
    status = GEIS_STATUS_BAD_ARGUMENT;
    goto error_exit;
  }

  _subscription_unref(sub);
  status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}


/**
 * Retrieves the name used on creation of the subscrition.
 */
GeisString
geis_subscription_name(GeisSubscription subscription)
{
  return subscription->sub_name;
}


/**
 * Retrieves the identifier assigned to the subscrition on creation.
 * The identifier is only unique for a given API instance and identifiers may be
 * reused after the subscription they identify has been deleted.
 */
GeisInteger
geis_subscription_id(GeisSubscription subscription)
{
  return subscription->sub_id;
}


/**
 * Adds a filter to a subscription.
 *
 * @todo implement this function after the filter module is implemented
 */
GeisStatus
geis_subscription_add_filter(GeisSubscription sub,
                             GeisFilter       filter)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (!sub)
  {
    status = GEIS_STATUS_BAD_ARGUMENT;
    goto error_exit;
  }

  status = geis_filter_bag_insert(sub->sub_filters, filter);
  if (status != GEIS_STATUS_SUCCESS)
  {
    geis_error_push(sub->sub_geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error adding filter to subscription");
    goto error_exit;
  }
  else
  {
    /* since we're taking ownership of the filter, we're removing the initial
       reference that belonged to the API user, created in geis_filter_new().
       Now the only remaining reference is the one held by our geis_filter_bag */
    geis_filter_unref(filter);
  }

error_exit:
  return status;
}


/**
 * Removes a filter from a subscription.
 *
 * @todo implement this function after the filter module is implemented
 */
GeisStatus
geis_subscription_remove_filter(GeisSubscription sub,
                                GeisFilter       filter)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (!sub)
  {
    status = GEIS_STATUS_BAD_ARGUMENT;
    goto error_exit;
  }

  status = geis_filter_bag_remove(sub->sub_filters, filter);
  if (status != GEIS_STATUS_SUCCESS)
  {
    geis_error_push(sub->sub_geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error removing filter from subscription");
    goto error_exit;
  }

error_exit:
  return status;
}


/*
 * Gets the numvber of filters in a subscirption.
 */
GeisSize
geis_subscription_filter_count(GeisSubscription sub)
{
  return geis_filter_bag_count(sub->sub_filters);
}


/*
 * Gets an indicated filter from a subscription.
 */
GeisFilter
geis_subscription_filter(GeisSubscription sub, GeisSize index)
{
  return geis_filter_bag_filter(sub->sub_filters, index);
}

/*
 * Gets an iterator initialized to the first filter on a subscription.
 */
GeisFilterIterator
geis_subscription_filter_begin(GeisSubscription sub)
{
  return geis_filter_bag_begin(sub->sub_filters);
}

/*
 * Gets an iterator initialized to the one-past-the-last filter on a subscription.
 */
GeisFilterIterator
geis_subscription_filter_end(GeisSubscription sub)
{
  return geis_filter_bag_end(sub->sub_filters);
}

/*
 * Gets the next filter in sequence.
 */
GeisFilterIterator
geis_subscription_filter_next(GeisSubscription sub, GeisFilterIterator iter)
{
  return geis_filter_iterator_next(sub->sub_filters, iter);
}


/*
 * Gets an named filter from a subscription.
 */
GeisFilter
geis_subscription_filter_by_name(GeisSubscription sub,
                                 GeisString       name)
{
  return geis_filter_bag_filter_by_name(sub->sub_filters, name);
}

/*
 * Gets the operational flags for the subscription.
 */
GeisSubscriptionFlags
geis_subscription_flags(GeisSubscription sub)
{
  return sub->sub_flags;
}


/*
 * Sets the operational flags for the subscription.
 */
GeisStatus
geis_subscription_set_flags(GeisSubscription sub,
                            GeisSubscriptionFlags flags)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;
  if (!sub->sub_activated)
  {
    sub->sub_flags = flags;
    status = GEIS_STATUS_SUCCESS;
  }
  return status;
}


/*
 * Gets an associated datum on the subscription.
 */
GeisPointer
geis_subscription_pdata(GeisSubscription sub)
{
  return sub->sub_data;
}


/*
 * Sets an associated datum on the subscription.
 */
void
geis_subscription_set_pdata(GeisSubscription sub, GeisPointer data)
{
  sub->sub_data = data;
}


/*
 * Gets a subscription-level configuration item.
 */
GeisStatus
geis_subscription_get_configuration(GeisSubscription subscription,
                                    GeisString       config_item_name,
                                    GeisPointer      config_item_value)
{
  GeisStatus retval = geis_get_sub_configuration(subscription->sub_geis,
                                                 subscription,
                                                 config_item_name,
                                                 config_item_value);
  return retval;
}


/*
 * Sets a subscription-level configuration item.
 */
GeisStatus
geis_subscription_set_configuration(GeisSubscription subscription,
                                    GeisString       config_item_name,
                                    GeisPointer      config_item_value)
{
  GeisStatus retval = geis_set_sub_configuration(subscription->sub_geis,
                                                 subscription,
                                                 config_item_name,
                                                 config_item_value);
  return retval;
}


