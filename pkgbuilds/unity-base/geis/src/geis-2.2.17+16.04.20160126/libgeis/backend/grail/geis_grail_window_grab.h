/**
 * @file geis_grail_window_grab.h
 * @brief window grab handling for the GEIS grail back end
 */
/*
 * Copyright 2011 Canonical Ltd.
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
#ifndef GEIS_BACKEND_GRAIL_WINDOW_GRAB_H_
#define GEIS_BACKEND_GRAIL_WINDOW_GRAB_H_

#include "geis/geis.h"
#include <X11/Xlib.h>


/**
 * The opaque Grail Window Grab Store.
 *
 * A "window grab" in this context refers to a passive grab of all multi-touch
 * input on a specified window.  There should be only a single grab of a window
 * with the X server, but it's possible for multiple GEIS subscriptions to exist
 * for the same region (window), so they need to multiplexed and refcounted
 * through a subscription-independent means.
 *
 * This is that means.
 */
typedef struct GeisGrailWindowGrabStore *GeisGrailWindowGrabStore;

/**
 * Constructs a new window grab store.
 */
GeisGrailWindowGrabStore
geis_grail_window_grab_store_new(Display *display);

/**
 * Destroys a window grab store.
 *
 * @param store  A window grab store.
 */
void
geis_grail_window_grab_store_delete(GeisGrailWindowGrabStore store);

/**
 * Grabs a window through a window grab store.
 *
 * @param store  A window grab store.
 * @param window The window to grab.
 *
 * @returns GEIS_STATUS_SUCCESS if the multi-touch for the window was grabbed
 * successfully, GEIS_STATUS_UNKNOWN_ERROR otherwise.
 */
GeisStatus
geis_grail_window_grab_store_grab(GeisGrailWindowGrabStore store,
                                  Window window);

/**
 * Ungrabs a window through a window grab store.
 *
 * @param store  A window grab store.
 * @param window The window to ungrab.
 */
void
geis_grail_window_grab_store_ungrab(GeisGrailWindowGrabStore store,
                                    Window window);

#endif /* GEIS_BACKEND_GRAIL_WINDOW_GRAB_H_ */

