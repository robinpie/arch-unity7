/**
 * @file geis_ugsubscription_store.cpp
 * @brief grail subscription storage
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
#include "geis_config.h"
#include "geis_ugsubscription_store.h"

#include <assert.h>
#include "geis_logging.h"
#include <oif/frame_x11.h>
#include <X11/Xlib.h>


typedef struct GeisUGSubscription
{
  GeisFilter     filter;
  UFDevice       device;
  GeisInteger    region_id;
  UGSubscription ugsub;
} *GeisUGSubscription;

static const GeisSize _geis_grail_ugsubscription_store_default_size = 2;
static const GeisFloat _geis_grail_ugsubscription_store_growth_factor = 1.7;


/*
 * Creates a new, empty grail subscription store.
 */
GeisUGSubscriptionStore
geis_ugsubscription_store_new()
{
  GeisUGSubscriptionStore store = geis_bag_new(sizeof(struct GeisUGSubscription),
                                  _geis_grail_ugsubscription_store_default_size,
                                  _geis_grail_ugsubscription_store_growth_factor);
  return store;
}


/*
 * Destroys a grail subscription store.
 */
void
geis_ugsubscription_delete(GeisUGSubscriptionStore store)
{
  for (GeisSize i = 0; i < geis_bag_count(store); ++i)
  {
    struct GeisUGSubscription * s = (struct GeisUGSubscription *)geis_bag_at(store, i);
    grail_subscription_delete(s->ugsub);
  }
  geis_bag_delete(store);
}


/*
 * Gets a count of the number of ugsubs in the store.
 */
GeisSize
geis_ugsubscription_count(GeisUGSubscriptionStore store)
{
  return geis_bag_count(store);
}


/*
 * Gets a UGSubscription from the store.
 */
UGSubscription
geis_ugsubscription_get_ugsubscription_at(GeisUGSubscriptionStore store,
                                          GeisSize                index)
{
  assert(index < geis_bag_count(store));
  return ((GeisUGSubscription)geis_bag_at(store, index))->ugsub;
}

/*
 * Creates a new UGSubscription and adds it to the store.
 */
UGSubscription
geis_ugsubscription_create_ugsubscription(GeisUGSubscriptionStore store,
                                          GeisFilter              filter,
                                          UFDevice                device,
                                          GeisInteger             region_id)
{
  UGSubscription ugsub = NULL;

  UGStatus ugstatus = grail_subscription_new(&ugsub);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to create grail subscription");
    goto final_exit;
  }

  ugstatus = grail_subscription_set_property(ugsub,
      UGSubscriptionPropertyDevice,
      &device);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription device property");
  }

  UFWindowId window_id = frame_x11_create_window_id(region_id);
  ugstatus = grail_subscription_set_property(ugsub,
      UGSubscriptionPropertyWindow,
      &window_id);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription window property");
  }

  struct GeisUGSubscription s = {
    .filter    = filter,
    .device    = device,
    .region_id = region_id,
    .ugsub     = ugsub
  };
  geis_bag_append(store, &s);

 final_exit:
  return ugsub;
}


/*
 * Gets a UGSubscription from the store.
 */
UGSubscription
geis_ugsubscription_get_ugsubscription(GeisUGSubscriptionStore store,
                                       GeisFilter              filter,
                                       UFDevice                device,
                                       GeisInteger             region_id)
{
  UGSubscription ugsub = NULL;

  for (GeisSize i = 0; i < geis_bag_count(store); ++i)
  {
    GeisUGSubscription s = (GeisUGSubscription)geis_bag_at(store, i);
    if (s->filter == filter && s->device == device && s->region_id == region_id)
    {
      ugsub = s->ugsub;
      break;
    }
  }

  return ugsub;
}


void
geis_ugsubscription_release_for_device(GeisUGSubscriptionStore  store,
                                       GeisFilter               filter,
                                       UFDevice                 device,
                                       GeisGrailWindowGrabStore window_grabs)
{
  GeisSize i = 0;
  while (i < geis_bag_count(store))
  {
    GeisUGSubscription s = (GeisUGSubscription)geis_bag_at(store, i);
    if (s->filter == filter && s->device == device)
    {
      UFWindowId ufwindow;
      UGStatus ugstatus;
      ugstatus = grail_subscription_get_property(s->ugsub,
                                                 UGSubscriptionPropertyWindow,
                                                 &ufwindow);
      if (ugstatus != UGStatusSuccess)
      {
        geis_warning("error %d getting subscription window", ugstatus);
      }
      else
      {
        Window window_id = frame_x11_get_window_id(ufwindow);
        geis_grail_window_grab_store_ungrab(window_grabs, window_id);
      }

      grail_subscription_delete(s->ugsub);
      geis_bag_remove(store, i);
    }
    else
    {
      ++i;
    }
  }
}


/*
 * Releses a UGSubscription from the store.
 */
void
geis_ugsubscription_release_ugsubscription(GeisUGSubscriptionStore store GEIS_UNUSED,
                                           UGSubscription          ugsub GEIS_UNUSED)
{
}


