/**
 * @file geis_grail_window_grab.c
 * @brief window grab handling for the GEIS grail back end
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
#include "geis_grail_window_grab.h"

#include "geis_bag.h"
#include "geis_logging.h"
#include <X11/extensions/XInput2.h>


/**
 * Tracks the number of grabs for each X window.
 */
struct GeisGrailWindowGrab
{
  Window window_id;
  int    grab_count;
};
typedef struct GeisGrailWindowGrab *GeisGrailWindowGrab;

/**
 * Stores all tracked window grabs.
 */
struct GeisGrailWindowGrabStore 
{
  Display *display;
  GeisBag  grabs;
};

static const GeisSize _geis_grail_window_grab_store_default_size = 2;
static const GeisFloat _geis_grail_window_grab_store_growth_factor = 1.7;


static GeisGrailWindowGrabStore
_window_grab_allocate()
{
  GeisGrailWindowGrabStore wgs = malloc(sizeof(struct GeisGrailWindowGrabStore));
  if (!wgs)
  {
    geis_error("failed to allocate window grab store");
  }
  return wgs;
}


static void
_window_grab_deallocate(GeisGrailWindowGrabStore wgs)
{
  free(wgs);
}


/*
 * Constructs a new window grab store.
 */
GeisGrailWindowGrabStore
geis_grail_window_grab_store_new(Display *display)
{
  GeisGrailWindowGrabStore wgs = _window_grab_allocate();
  if (wgs)
  {
    wgs->display = display;
    wgs->grabs = geis_bag_new(sizeof(struct GeisGrailWindowGrab),
                              _geis_grail_window_grab_store_default_size,
                              _geis_grail_window_grab_store_growth_factor); 
    if (!wgs->grabs)
    {
      free(wgs);
      wgs = NULL;
    }
  }
  return wgs;
}


/*
 * Destroys a window grab store.
 */
void
geis_grail_window_grab_store_delete(GeisGrailWindowGrabStore wgs)
{
  for (GeisSize i = 0; i < geis_bag_count(wgs->grabs); ++i)
  {
    /* @todo: ungrab */
  }
  geis_bag_delete(wgs->grabs);
  _window_grab_deallocate(wgs);
}


static GeisGrailWindowGrab
_window_grab_store_find(GeisGrailWindowGrabStore wgs, Window window_id)
{
  for (GeisSize i = 0; i < geis_bag_count(wgs->grabs); ++i)
  {
    GeisGrailWindowGrab grab = (GeisGrailWindowGrab)geis_bag_at(wgs->grabs, i);
    if (grab->window_id == window_id)
    {
      return grab;
    }
  }
  return NULL;
}


/*
 * Grabs a window through a window grab store.
 */
GeisStatus
geis_grail_window_grab_store_grab(GeisGrailWindowGrabStore wgs, Window window_id)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisGrailWindowGrab grab = _window_grab_store_find(wgs, window_id);
  if (!grab)
  {
    struct GeisGrailWindowGrab new_grab = { window_id, 1 };
    geis_bag_append(wgs->grabs, &new_grab);

    XIEventMask mask = {
      XIAllMasterDevices,
      XIMaskLen(XI_LASTEVENT),
      calloc(XIMaskLen(XI_LASTEVENT), sizeof(char))
    };
    XISetMask(mask.mask, XI_TouchBegin);
    XISetMask(mask.mask, XI_TouchUpdate);
    XISetMask(mask.mask, XI_TouchEnd);
    XISetMask(mask.mask, XI_TouchOwnership);
    XISetMask(mask.mask, XI_HierarchyChanged);
    XIGrabModifiers mods = { XIAnyModifier, 0 };
    int xstat = XIGrabTouchBegin(wgs->display, XIAllMasterDevices,
                                 window_id,
                                 0, &mask, 1, &mods);
    free(mask.mask);
    if (xstat)
    {
      geis_error("error %d returned from XIGrabTouchBegin()", xstat);
      goto final_exit;
    }
    else if (mods.status != XIGrabSuccess)
    {
      geis_error("status %d returned from XIGrabTouchBegin()", mods.status);
      goto final_exit;
    }
    status = GEIS_STATUS_SUCCESS;
  }
  else
  {
    ++grab->grab_count;
    status = GEIS_STATUS_SUCCESS;
  }

final_exit:
  return status;
}


/*
 * Ungrabs a window through a window grab store.
 * @param window The window to ungrab.
 */
void
geis_grail_window_grab_store_ungrab(GeisGrailWindowGrabStore wgs,
                                    Window window_id)
{
  GeisGrailWindowGrab grab = _window_grab_store_find(wgs, window_id);
  if (grab)
  {
    --grab->grab_count;
    if (0 == grab->grab_count)
    {
      XIGrabModifiers mods = { XIAnyModifier, 0 };
      Status xstat = XIUngrabTouchBegin(wgs->display,
                                        XIAllMasterDevices,
                                        window_id,
                                        1, &mods);
      if (xstat)
      {
        geis_error("error %d returned from XIUngrabTouchBegin()", xstat);
      }

      XSync(wgs->display, False);
    }
  }
}


