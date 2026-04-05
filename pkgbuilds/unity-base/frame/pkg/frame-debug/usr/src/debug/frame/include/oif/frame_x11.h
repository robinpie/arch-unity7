/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2013 Canonical Ltd.
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
 *
 ****************************************************************************/

#ifndef FRAME_OIF_FRAME_X11_H_
#define FRAME_OIF_FRAME_X11_H_

#include "oif/frame.h"
#include <X11/Xlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup x11 X11
 * @{
 */

/**
 * Create a new frame context for an X11 window server
 *
 * @param [in] display The X11 server connection
 * @param [out] handle The object for the new frame instance
 * @return UFStatusSuccess, UFStatusErrorResources, or UFStatusErrorGeneric
 */
FRAME_PUBLIC
UFStatus frame_x11_new(Display *display, UFHandle *handle);

/**
 * Delete an X11 frame instance
 *
 * @param [in] handle The object for the frame instance
 */
FRAME_PUBLIC
void frame_x11_delete(UFHandle handle);

/**
 * Process an X11 input event into the frame instance
 *
 * @param [in] handle The frame context
 * @param [in] event The X11 generic input event cookie
 * @return UFStatusSuccess or UFStatusErrorGeneric
 *
 * The frame library can process XIDeviceEvent and
 * XIHierarchyEvent events. Processing these events requires additional event
 * data. This data is obtained by calling XGetEventData on the XEvent. See the
 * XGetEventData and XFreeEventData man pages for more details.
 *
 * This function will silently ignore any events other than those listed above.
 */
FRAME_PUBLIC
UFStatus frame_x11_process_event(UFHandle handle, XGenericEventCookie *xcookie);

/**
 * Accept ownership of a touch
 *
 * @deprecated Use frame_accept_touch instead.
 * @param [in] device The device object for the touch (const)
 * @param [in] window The window to accept the touch for
 * @param [in] touch_id The touch ID object for the touch
 * @return UFStatusSuccess, UFStatusErrorInvalidTouch
 */
FRAME_PUBLIC
UFStatus frame_x11_accept_touch(UFDevice device, UFWindowId window,
                                UFTouchId touch_id);

/**
 * Reject ownership of a touch
 *
 * @deprecated Use frame_reject_touch instead.
 * @param [in] device The device object for the touch (const)
 * @param [in] window The window to reject the touch for
 * @param [in] touch_id The touch ID object for the touch
 * @return UFStatusSuccess, UFStatusErrorInvalidTouch
 */
FRAME_PUBLIC
UFStatus frame_x11_reject_touch(UFDevice device, UFWindowId window,
                                UFTouchId touch_id);

/**
 * Get the X11 Window ID of a frame window
 *
 * @param [in] window_id The frame window ID object (const)
 * @return The X11 window ID
 */
FRAME_PUBLIC
Window frame_x11_get_window_id(UFWindowId window_id);

/**
 * Create a new frame window ID object for an X11 window ID
 *
 * @param [in] id The X11 ID of the window
 * @return The new frame window ID object
 */
FRAME_PUBLIC
UFWindowId frame_x11_create_window_id(Window id);

/**
 * Get the X11 touch ID of a frame touch
 *
 * @param [in] touch_id The frame touch ID object (const)
 * @return The X11 touch ID
 */
FRAME_PUBLIC
unsigned int frame_x11_get_touch_id(UFTouchId touch_id);

/**
 * Create a new frame touch ID object for an X11 touch ID
 *
 * @param [in] id The X11 ID of the touch
 * @return The new frame touch ID object
 */
FRAME_PUBLIC
UFTouchId frame_x11_create_touch_id(unsigned int id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // FRAME_OIF_FRAME_X11_H_
