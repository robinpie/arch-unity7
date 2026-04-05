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

/**
 * @file oif/frame.h
 * Definitions of the main and platform-generic API
 */

#ifndef FRAME_OIF_FRAME_H_
#define FRAME_OIF_FRAME_H_

/* Macros that set symbol visibilities in shared libraries properly.
 * Adapted from http://gcc.gnu.org/wiki/Visibility
 */

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_FRAME
    #define FRAME_PUBLIC __declspec(dllexport)
  #else
    #define FRAME_PUBLIC __declspec(dllimport)
  #endif
#else
  #if defined __GNUC__
    #define FRAME_PUBLIC __attribute__ ((visibility("default")))
  #else
    #pragma message ("Compiler does not support symbol visibility.")
    #define FRAME_PUBLIC
  #endif
#endif

/* Clang provides __has_feature, but GCC does not */
#ifdef __has_feature
#if __has_feature(c_generic_selections)
#define HAS_C_GENERIC_SELECTIONS
#endif // __has_feature
#endif // __has_feature(c_generic_selections)

/* Whether the X11 backend (frame_x11.h) is available.
   FRAME_X11_BACKEND will be defined if it is and FRAME_NO_X11_BACKEND otherwise. */
#define FRAME_X11_BACKEND

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** An object for the context of the frame instance */
typedef struct UFHandle_* UFHandle;
/** An object for an event */
typedef struct UFEvent_* UFEvent;
/** An object for a frame of touches */
typedef struct UFFrame_* UFFrame;
/** An object for a touch */ 
typedef struct UFTouch_* UFTouch;
/** An object for a device */
typedef struct UFDevice_* UFDevice;
/** An object for a device axis */
typedef struct UFAxis_* UFAxis;
/** An object for a window ID */
typedef uint64_t UFWindowId;
/** An object for a touch ID */
typedef uint64_t UFTouchId;

/** The status code denoting the result of a function call */
typedef enum UFStatus {
  UFStatusSuccess = 0, /**< The call was successful */
  UFStatusErrorGeneric, /**< A platform-dependent error occurred */
  UFStatusErrorResources, /**< An error occurred due to insufficient resources */
  UFStatusErrorNoEvent, /**< No events were available to get */
  UFStatusErrorUnknownProperty, /**< The requested property value was not set */
  UFStatusErrorInvalidTouch, /**< The requested touch does not exist */
  UFStatusErrorInvalidAxis, /**< The requested axis does not exist */
  UFStatusErrorUnsupported, /**< The requested function is not supported by the
                                 window server */
  UFStatusErrorInvalidType, /**< The variable type passed as a void pointer into
                                 a property getter is invalid for the property
                             */
  UFStatusErrorTouchIdExists, /**< A touch with the same ID already exists */
} UFStatus;

/** Properties of a device */
typedef enum UFDeviceProperty {
  /**
   * The name of the device
   *
   * Value type: const char *
   *
   * The frame library owns the string. The string is valid until an event
   * notifying removal of the device is released.
   */
  UFDevicePropertyName = 0,
  /**
   * Whether the device is a direct touch device
   *
   * Value type: int with boolean semantics
   *
   * A direct touch device is a device where there is a direct transformation
   * from the touch location to the event location on the screen. An indirect
   * touch device is a device where the touch has meaning relative to a position
   * on the screen, such as the location of a cursor. A touchscreens is an
   * example of a direct device, and a trackpad is an example of an indirect
   * device.
   */
  UFDevicePropertyDirect,
  /**
   * Whether the device is an independent touch device
   *
   * Value type: int with boolean semantics
   *
   * An independent device is an indirect device whose cursor moves
   * independently of the touches on the device. A mouse with a touch area for
   * gestures is an example of an independent device, and a trackpad is an
   * example of a dependent device.
   */
  UFDevicePropertyIndependent,
  /**
   * Whether the device is a semi-multitouch device
   *
   * Value type: int with boolean semantics
   *
   * A semi-multitouch device provides a bounding box of some touches on the
   * touch surface. In contrast, a full-multitouch device provides accurate
   * locations of each individual touch.
   */
  UFDevicePropertySemiMT,
  /**
   * The maximum number of touches supported by the device
   *
   * Value type: unsigned int
   */
  UFDevicePropertyMaxTouches,
  /**
   * The number of touch axes provided by the device
   *
   * Value type: unsigned int
   */
  UFDevicePropertyNumAxes,
  /**
   * The resolution of the window coordinates of the device in the X axis
   *
   * Value type: float
   *
   * The resolution is provided in pixels per meter.
   */
  UFDevicePropertyWindowResolutionX,
  /**
   * The resolution of the window coordinates of the device in the Y axis
   *
   * Value type: float
   *
   * The resolution is provided in pixels per meter.
   */
  UFDevicePropertyWindowResolutionY,
} UFDeviceProperty;

/** Device touch axis types */
typedef enum UFAxisType {
  UFAxisTypeX = 0, /**< X coordinate */
  UFAxisTypeY, /**< Y coordinate */
  UFAxisTypeTouchMajor, /**< Width along major axis of contact area of touch */
  UFAxisTypeTouchMinor, /**< Width along minor axis of contact area of touch */
  UFAxisTypeWidthMajor, /**< Width along major axis of touch tool */
  UFAxisTypeWidthMinor, /**< Width along minor axis of touch tool */
  UFAxisTypeOrientation, /**< Orientation of major axis of contact ellipse */
  UFAxisTypeTool, /**< Tool type */
  UFAxisTypeBlobId, /**< Blob ID of group of touches */
  UFAxisTypeTrackingId, /**< Tracking ID */
  UFAxisTypePressure, /**< Pressure */
  UFAxisTypeDistance, /**< Hover distance */
} UFAxisType;

/** Event properties */
typedef enum UFEventProperty {
  /**
   * Type of event
   *
   * Value type: UFEventType
   */
  UFEventPropertyType = 0,
  /**
   * Device added or removed
   *
   * Value type: UFDevice
   *
   * This property is set only when the event type is UFEventTypeDeviceAdded
   * or UFEventTypeDeviceRemoved. The object is owned by the library and is
   * valid until an event notifying removal of the device is released.
   */
  UFEventPropertyDevice,
  /**
   * Touch frame
   *
   * Value type: UFFrame
   *
   * This property is set only when the event type is UFEventTypeFrame. The
   * object is owned by the library and is valid until the event is released.
   */
  UFEventPropertyFrame,
  /**
   * Event time
   *
   * Value type: 64-bit unsigned int
   *
   * This property holds the time the event occurred in display server
   * timespace. The time is provided in milliseconds (ms). If the event, such as
   * device addition, occurred before the frame context was created, the value
   * will be 0.
   */
  UFEventPropertyTime,
} UFEventProperty;

/** Event types */
typedef enum UFEventType {
  UFEventTypeDeviceAdded = 0, /**< A new device has been added */
  UFEventTypeDeviceRemoved, /**< An existing device has been removed */
  UFEventTypeFrame, /**< The state of one or more touches has changed */
} UFEventType;

/** Touch frame properties */
typedef enum UFFrameProperty {
  /**
   * The device for the frame
   *
   * Value type: UFDevice
   */
  UFFramePropertyDevice = 0,
  /**
   * The window server ID of the window for the frame
   *
   * Value type: UFWindowId
   */
  UFFramePropertyWindowId,
  /**
   * Number of touches in the frame
   *
   * Value type: unsigned int
   *
   * Some devices can track more touches than they can report data for. Only
   * touches with X and Y position are provided in the frame.
   */
  UFFramePropertyNumTouches,
  /**
   * Total number of active touches on the device
   *
   * Value type: unsigned int
   *
   * Some devices can track more touches than they can report data for. This
   * value includes the number of reported and unreported touches.
   */
  UFFramePropertyActiveTouches,
} UFFrameProperty;

/** State of an individual touch */
typedef enum UFTouchState {
  UFTouchStateBegin = 0, /**< The touch began */
  UFTouchStateUpdate, /**< A value or property of the touch changed */
  UFTouchStateEnd, /**< The touch ended */
} UFTouchState;

/** Touch properties */
typedef enum UFTouchProperty {
  /**
   * Window server ID of the touch
   *
   * Value type: UFTouchId
   */
  UFTouchPropertyId = 0,
  /**
   * State of the touch
   *
   * Value type: UFTouchState
   */
  UFTouchPropertyState,
  /**
   * Location along X axis of touch relative to event window
   *
   * Value type: float
   *
   * The window server may provide touch location in window coordinate space.
   * This property will be set where available.
   */
  UFTouchPropertyWindowX,
  /**
   * Location along Y axis of touch relative to event window
   *
   * Value type: float
   *
   * The window server may provide touch location in window coordinate space.
   * This property will be set where available.
   */
  UFTouchPropertyWindowY,
  /**
   * Time of last touch state change
   *
   * Value type: 64-bit unsigned int
   *
   * See UFEventPropertyTime for the semantics of the value. If the touch has
   * not changed during this frame, the value of this property will be less than
   * the value of the UFEventPropertyTime event property for this frame.
   */
  UFTouchPropertyTime,
  /**
   * Start time of touch
   *
   * Value type: 64-bit unsigned int
   *
   * See UFEventPropertyTime for the semantics of the value.
   */
  UFTouchPropertyStartTime,
  /**
   * Whether the touch is owned by the client
   *
   * Value type: int with boolean semantics
   *
   * Some window servers have the concept of touch ownership. This property
   * is only valid when the server supports touch ownership.
   */
  UFTouchPropertyOwned,
  /**
   * Whether the touch has physically ended before the touch sequence has ended
   *
   * Value type: int with boolean semantics
   *
   * Some window servers have the concept of touch ownership. If a touch has
   * ended before the client receives ownership, this property will be set to
   * true. The property will also be set to true when the touch has ended before
   * the client has accepted or rejected ownership of the touch sequence.
   */
  UFTouchPropertyPendingEnd,
} UFTouchProperty;

/**
 * Get the event file descriptor for the frame context
 *
 * @param [in] handle The frame context object
 * @return A file descriptor for the context
 *
 * When events are available for processing, the file descriptor will be
 * readable. Perform an 8-byte read from the file descriptor to clear the state.
 * Refer to the EVENTFD(2) man page for more details.
 */
FRAME_PUBLIC
int frame_get_fd(UFHandle handle);

/**
 * Get an event from the frame context
 *
 * @param [in] handle The context object
 * @param [out] event The retrieved event
 * @return UFStatusSuccess or UFStatusErrorNoEvent
 *
 * The reference count of the returned event is implicity incremented once.
 */
FRAME_PUBLIC
UFStatus frame_get_event(UFHandle handle, UFEvent *event);

/**
 * Get the value of a property of a device
 *
 * @param [in] device The device object (const)
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess or UFStatusErrorUnknownProperty
 */
#ifndef HAS_C_GENERIC_SELECTIONS /* See frame_internal.h */
FRAME_PUBLIC
UFStatus frame_device_get_property(UFDevice device, UFDeviceProperty property,
                                   void *value);
#endif

/**
 * Get a device touch axis by index
 *
 * @param [in] device The device object (const)
 * @param [in] index The index of the axis to get
 * @param [out] axis The axis retrieved
 * @return UFStatusSuccess or UFStatusErrorInvalidAxis
 *
 * The index value must be greater than or equal to 0 and less than the number
 * axes of the device.
 */
FRAME_PUBLIC
UFStatus frame_device_get_axis_by_index(UFDevice device, unsigned int index,
                                        UFAxis *axis);

/**
 * Get a device touch axis by axis type
 *
 * @param [in] device The device object (const)
 * @param [in] type The axis type
 * @param [out] axis The axis retrieved
 * @return UFStatusSuccess or UFStatusErrorInvalidAxis
 *
 * UFStatusErrorInvalidAxis is returned if the device does not have an axis of
 * the type requested.
 */
FRAME_PUBLIC
UFStatus frame_device_get_axis_by_type(UFDevice device, UFAxisType type,
                                       UFAxis *axis);

/**
 * Get the type of a touch device axis
 *
 * @param [in] axis The touch device axis (const)
 * @return The type of the axis
 */
FRAME_PUBLIC
UFAxisType frame_axis_get_type(UFAxis axis);

/**
 * Get the minimum value of a touch device axis
 *
 * @param [in] axis The touch device axis (const)
 * @return The minimum value of the axis
 */
FRAME_PUBLIC
float frame_axis_get_minimum(UFAxis axis);

/**
 * Get the maximum value of a touch device axis
 *
 * @param [in] axis The touch device axis (const)
 * @return The maximum value of the axis
 */
FRAME_PUBLIC
float frame_axis_get_maximum(UFAxis axis);

/**
 * Get the resolution of a touch device axis
 *
 * @param [in] axis The touch device axis (const)
 * @return The resolution of the axis
 */
FRAME_PUBLIC
float frame_axis_get_resolution(UFAxis axis);

/**
 * Increment the reference count of an event
 *
 * @param [in] event The event object
 */
FRAME_PUBLIC
void frame_event_ref(UFEvent event);

/**
 * Decrement the reference count of an event
 *
 * @param [in] event The event object
 *
 * When the reference count reaches zero, the event is freed.
 */
FRAME_PUBLIC
void frame_event_unref(UFEvent event);

/**
 * Get the value of a property of an event
 *
 * @param [in] event The event object (const)
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess or UFStatusErrorUnknownProperty
 */
#ifndef HAS_C_GENERIC_SELECTIONS /* See frame_internal.h */
FRAME_PUBLIC
UFStatus frame_event_get_property(UFEvent event, UFEventProperty property,
                                  void *value);
#endif

/**
 * Get the value of a property of a frame
 *
 * @param [in] frame The frame object (const)
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess or UFStatusErrorUnknownProperty
 */
#ifndef HAS_C_GENERIC_SELECTIONS /* See frame_internal.h */
FRAME_PUBLIC
UFStatus frame_frame_get_property(UFFrame frame, UFFrameProperty property,
                                  void *value);
#endif

/**
 * Get a touch of a frame by index
 *
 * @param [in] frame The frame object (const)
 * @param [in] index The index of the touch to get
 * @param [out] touch The touch retrieved
 * @return UFStatusSuccess or UFStatusErrorInvalidTouch
 *
 * The index value must be greater than or equal to 0 and less than the number
 * touches reported in the frame.
 */
FRAME_PUBLIC
UFStatus frame_frame_get_touch_by_index(UFFrame frame, unsigned int index,
                                        UFTouch *touch);

/**
 * Get a touch from a frame by the window server ID
 *
 * @param [in] frame The frame object (const)
 * @param [in] touch_id The window server ID of the touch
 *             The value type of the touch ID is window server dependent. See
 *             UFTouchPropertyId for more details.
 * @param [out] touch The touch object
 * @return UFStatusSuccess or UFStatusErrorInvalidTouch
 */
FRAME_PUBLIC
UFStatus frame_frame_get_touch_by_id(UFFrame frame, UFTouchId touch_id,
                                     UFTouch* touch);

/**
 * Get the previous value of a property of a touch
 *
 * @param [in] frame The current frame object (const)
 * @param [in] touch The current touch object (const)
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess, UFStatusErrorUnknownProperty, or
 *         UFStatusErrorInvalidTouch
 *
 * The previous value is the value of the property in the previous frame.
 * UFStatusErrorInvalidTouch is returned if the touch did not exist in the
 * previous frame.
 */
FRAME_PUBLIC
UFStatus frame_frame_get_previous_touch_property(UFFrame frame, UFTouch touch,
                                                 UFTouchProperty property,
                                                 void *value);

/**
 * Get the previous value of an axis of a touch
 *
 * @param [in] frame The current frame object (const)
 * @param [in] touch The current touch object (const)
 * @param [in] type The axis to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess, UFStatusErrorInvalidAxis, or
 *         UFStatusErrorInvalidTouch
 *
 * The previous value is the value of the axis in the previous frame.
 * UFStatusErrorInvalidTouch is returned if the touch did not exist in the
 * previous frame.
 */
FRAME_PUBLIC
UFStatus frame_frame_get_previous_touch_value(UFFrame frame, UFTouch touch,
                                              UFAxisType type, float* value);

/**
 * Get the value of a property of a touch
 *
 * @param [in] touch The touch object (const)
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess or UFStatusErrorUnknownProperty
 */
#ifndef HAS_C_GENERIC_SELECTIONS /* See frame_internal.h */
FRAME_PUBLIC
UFStatus frame_touch_get_property(UFTouch touch, UFTouchProperty property,
                                  void *value);
#endif

/**
 * Get the value of an axis of a touch
 *
 * @param [in] touch The touch object (const)
 * @param [in] type The axis to retrieve a value for
 * @param [out] value The value retrieved
 * @return UFStatusSuccess or UFStatusErrorInvalidAxis
 */
FRAME_PUBLIC
UFStatus frame_touch_get_value(UFTouch touch, UFAxisType type, float *value);

/**
 * @defgroup v2-helpers Helper Functions
 * These helper functions may be used in place of the generic property getters.
 * They are limited to properties that are guaranteed to exist in all instances
 * of the objects.
 * @{
 */

/**
 * Get the type of an event
 *
 * @param [in] event The event object (const)
 * @return The type of the event
 */
FRAME_PUBLIC
UFEventType frame_event_get_type(UFEvent event);

/**
 * Get the time of an event
 *
 * @param [in] event The event object (const)
 * @return The time of the event
 */
FRAME_PUBLIC
uint64_t frame_event_get_time(UFEvent event);

/**
 * Get the number of axes of a device
 *
 * @param [in] device The device object (const)
 * @return The number of axes
 */
FRAME_PUBLIC
unsigned int frame_device_get_num_axes(UFDevice device);

/**
 * Get The resolution of the window coordinates of the device in the X axis
 *
 * @param [in] device The device object (const)
 * @return The resolution in pixels per meter
 */
FRAME_PUBLIC
float frame_device_get_window_resolution_x(UFDevice device);

/**
 * Get The resolution of the window coordinates of the device in the Y axis
 *
 * @param [in] device The device object (const)
 * @return The resolution in pixels per meter
 */
FRAME_PUBLIC
float frame_device_get_window_resolution_y(UFDevice device);

/**
 * Get the number of touches in the frame
 *
 * @param [in] frame The frame object (const)
 * @return The number of touches
 */
FRAME_PUBLIC
uint32_t frame_frame_get_num_touches(UFFrame frame);

/**
 * Get the device of a frame
 *
 * @param [in] frame The frame context object (const)
 * return The device of the window context
 */
FRAME_PUBLIC
UFDevice frame_frame_get_device(UFFrame frame);

/**
 * Get the window ID of a frame
 *
 * @param [in] frame The frame context object (const)
 * @return The window server ID of the window of the frame
 */
FRAME_PUBLIC
UFWindowId frame_frame_get_window_id(UFFrame frame);

/**
 * Get the window server ID of a touch
 *
 * @param [in] touch The touch context object (const)
 * @return The window server ID of the touch
 */
FRAME_PUBLIC
UFTouchId frame_touch_get_id(UFTouch touch);

/**
 * Get the state of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The state of the touch
 */
FRAME_PUBLIC
UFTouchState frame_touch_get_state(UFTouch touch);

/**
 * Get the X window coordinate of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The X window coordinate of the touch
 */
FRAME_PUBLIC
float frame_touch_get_window_x(UFTouch touch);

/**
 * Get the Y window coordinate of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The Y window coordinate of the touch
 */
FRAME_PUBLIC
float frame_touch_get_window_y(UFTouch touch);

/**
 * Get the X device coordinate of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The X device coordinate of the touch
 */
FRAME_PUBLIC
float frame_touch_get_device_x(UFTouch touch);

/**
 * Get the Y device coordinate of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The Y device coordinate of the touch
 */
FRAME_PUBLIC
float frame_touch_get_device_y(UFTouch touch);

/**
 * Get the time of a touch state change
 *
 * @param [in] touch The touch object (const)
 * @return The time of the touch state change
 */
FRAME_PUBLIC
uint64_t frame_touch_get_time(UFTouch touch);

/**
 * Get the start time of a touch
 *
 * @param [in] touch The touch object (const)
 * @return The start time of the touch
 */
FRAME_PUBLIC
uint64_t frame_touch_get_start_time(UFTouch touch);

/**
 * Accept ownership of a touch
 *
 * All touches received should be eventally accepted or rejected.
 * This decision can come even after they have already ended but
 * should be done as soon as possible.
 *
 * You should accept a touch when you're going to use it and
 * reject it if you're not interested in it.
 *
 * Not all window servers have this concept. Those that do use
 * this information to pass rejected touches forward to other
 * clients that might want it.
 *
 * @param [in] device The device object for the touch (const)
 * @param [in] window The window to accept the touch for
 * @param [in] touch_id The touch ID object for the touch
 * @return UFStatusSuccess, UFStatusErrorInvalidTouch
 * @see frame_reject_touch
 */
FRAME_PUBLIC
UFStatus frame_accept_touch(UFDevice device, UFWindowId window,
                            UFTouchId touch_id);

/**
 * Reject ownership of a touch
 *
 * @param [in] device The device object for the touch (const)
 * @param [in] window The window to reject the touch for
 * @param [in] touch_id The touch ID object for the touch
 * @return UFStatusSuccess, UFStatusErrorInvalidTouch
 * @see frame_accept_touch
 */
FRAME_PUBLIC
UFStatus frame_reject_touch(UFDevice device, UFWindowId window,
                            UFTouchId touch_id);

/** @} */

#include "oif/frame_internal.h"

#ifdef __cplusplus
}
#endif

#undef HAS_C_GENERIC_SELECTIONS

#endif // FRAME_OIF_FRAME_H_
