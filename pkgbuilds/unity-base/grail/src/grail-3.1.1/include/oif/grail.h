/*****************************************************************************
 *
 * grail - Multitouch Gesture Recognition Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

/**
 * @file oif/grail.h
 * Definitions of the main and platform-generic API
 */

#ifndef GRAIL_OIF_GRAIL_H_
#define GRAIL_OIF_GRAIL_H_

/* Macros that set symbol visibilities in shared libraries properly.
 * Adapted from http://gcc.gnu.org/wiki/Visibility
 */

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_GRAIL
    #define GRAIL_PUBLIC __declspec(dllexport)
  #else
    #define GRAIL_PUBLIC __declspec(dllimport)
  #endif
#else
  #if defined __GNUC__
    #define GRAIL_PUBLIC __attribute__ ((visibility("default")))
  #else
    #pragma message ("Compiler does not support symbol visibility.")
    #define GRAIL_PUBLIC
  #endif
#endif

#include <stdint.h>

#include <oif/frame.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup v3 Grail 3.x
 * @{
 */

/** An object for the context of the grail instance */
typedef struct UGHandle_* UGHandle;
/** An object for a gesture subscription */
typedef struct UGSubscription_* UGSubscription;
/** An object for an event */
typedef struct UGEvent_* UGEvent;
/** An object for a gesture state in time */
typedef const struct UGSlice_* UGSlice;

/** The status code denoting the result of a function call */
typedef enum UGStatus {
  UGStatusSuccess = 0, /**< The call was successful */
  UGStatusErrorGeneric, /**< A platform-dependent error occurred */
  UGStatusErrorResources, /**< An error occurred due to insufficient resources */
  UGStatusErrorNoEvent, /**< No events were available to get */
  UGStatusErrorUnknownProperty, /**< The requested property value was not set */
  UGStatusErrorInvalidValue, /**< The property value passed in is invalid */
  UGStatusErrorInvalidDevice, /**< The requested device does not exist */
  UGStatusErrorInvalidSubscription, /**< The subscription is invalid */
  UGStatusErrorInvalidGesture, /**< The requested gesture does not exist */
  UGStatusErrorInvalidIndex, /**< The requested touch index is invalid */
  UGStatusErrorAtomicity, /**< The subscription has a different value for 
                               UGSubscriptionPropertyAtomicGestures than other
                               subscriptions active on the window */
} UGStatus;

/** Subscription properties */
typedef enum UGSubscriptionProperty {
  /**
   * Device to subscribe to gesture events for
   *
   * Value type: UFDevice
   */
  UGSubscriptionPropertyDevice,
  /**
   * Window to subscribe to gesture events for
   *
   * Value type: UFWindowId
   */
  UGSubscriptionPropertyWindow,
  /**
   * Gesture types to subscribe for
   *
   * Value type: UGGestureTypeMask
   */
  UGSubscriptionPropertyMask,
  /**
   * Number of touches required to begin gesture
   *
   * Value type: unsigned int
   * Default value: 2 touches
   */
  UGSubscriptionPropertyTouchesStart,
  /**
   * Minimum number of touches for gesture
   *
   * Value type: unsigned int
   * Default value: 2 touches
   */
  UGSubscriptionPropertyTouchesMinimum,
  /**
   * Maximum number of touches for gesture
   *
   * Value type: unsigned int
   * Default value: 2 touches
   */
  UGSubscriptionPropertyTouchesMaximum,
  /**
   * Timeout for recognizing a drag gesture
   *
   * Value type: 64-bit unsigned integer
   * Default value: 300 ms
   */
  UGSubscriptionPropertyDragTimeout,
  /**
   * Threshold value for recognizing a drag gesture
   *
   * Value type: float
   * Default value: 0.0026 m
   *
   * The value is in units of meters.
   */
  UGSubscriptionPropertyDragThreshold,
  /**
   * Timeout for recognizing a pinch gesture
   *
   * Value type: 64-bit unsigned integer
   * Default value: 300 ms
   */
  UGSubscriptionPropertyPinchTimeout,
  /**
   * Threshold value for recognizing a pinch gesture
   *
   * Value type: float
   * Default value: 1.1
   *
   * The value is a proportionality representing how much a group of touches
   * have moved closer or farther apart. For example, a threshold of 1.1 would
   * be met if two touches moved from 1000 pixels apart to either 1100 or 909
   * pixels apart.
   */
  UGSubscriptionPropertyPinchThreshold,
  /**
   * Timeout for recognizing a rotate gesture
   *
   * Value type: 64-bit unsigned integer
   * Default value: 500 ms
   */
  UGSubscriptionPropertyRotateTimeout,
  /**
   * Threshold value for recognizing a rotate gesture
   *
   * Value type: float
   * Default value: 0.125663706 (1/50th of a revolution)
   *
   * The value is in units of radians.
   */
  UGSubscriptionPropertyRotateThreshold,
  /**
   * Timeout for recognizing a tap gesture
   *
   * Value type: 64-bit unsigned integer
   * Default value: 300 ms
   */
  UGSubscriptionPropertyTapTimeout,
  /**
   * Threshold value for recognizing a tap gesture
   *
   * Value type: float
   * Default value: 0.0026 m
   *
   * For a tap to be recognized, the touches must not move more than the
   * threshold value in any direction.
   */
  UGSubscriptionPropertyTapThreshold,
  /**
   * Only support one gesture at a time
   *
   * Value type: int with boolean semantics
   * Default value: False
   *
   * The first version of grail supported only one gesture at a time. When this
   * property is true, grail will mimic this behavior. This results in the
   * following:
   *
   * - The grail client must not attempt to accept or reject a gesture
   * - If a gesture is active for a maximum of N touches, the addition of
   *   another touch will end the gesture. A new gesture is begun if another
   *   subscription's TouchesStart property equals the new number of touches.
   * - All subscriptions for a window must have the same value for this
   *   property. If a client attempts to activate a subscription with a
   *   different value for this property than the already activated
   *   subscriptions for the window, UGStatusErrorAtomicity will be returned.
   * - Gestures from multiple subscriptions may be active at the same time.
   *
   * There is one key difference between grail v1 behavior and the use of this
   * option. The v1 behavior only supported one gesture per device. The use of
   * this option only supports one gesture per device per window. The beginning
   * of a gesture in a window does not inhibit gestures in other windows. It
   * also does not guarantee that there are no active touches outside the
   * window.
   */
  UGSubscriptionPropertyAtomicGestures,
} UGSubscriptionProperty;

/** Event type */
typedef enum UGEventType {
  UGEventTypeSlice = 0, /**< A new gesture slice */
} UGEventType;

/** Event properties */
typedef enum UGEventProperty {
  /**
   * Type of event
   *
   * Value type: UGEventType
   */
  UGEventPropertyType = 0,
  /**
   * Slice of a gesture
   *
   * Value type: UGSlice
   */
  UGEventPropertySlice,
  /**
   * Event time
   *
   * Value type: 64-bit unsigned int
   *
   * This property holds the time the event occurred in display server
   * timespace. The time is provided in milliseconds (ms).
   */
  UGEventPropertyTime,
} UGEventProperty;

/** Gesture type bit indices */
typedef enum UGGestureType {
  UGGestureTypeDrag = 0x1, /**< Drag gesture */
  UGGestureTypePinch = 0x2, /**< Pinch gesture */
  UGGestureTypeRotate = 0x4, /**< Rotate gesture */
  UGGestureTypeTap = 0x8, /**< Tap gesture */
  UGGestureTypeTouch = 0x10, /**< Touch gesture */
} UGGestureType;

/** Bit-mask of gesture types */
typedef uint32_t UGGestureTypeMask;

/** 2D affine transformation */
typedef const float UGTransform[3][3];

/** Gesture slice state */
typedef enum UGGestureState {
  UGGestureStateBegin = 0, /**< Gesture slice begin */
  UGGestureStateUpdate, /**< Gesture slice update */
  UGGestureStateEnd, /**< Gesture slice end */
} UGGestureState;

/**
 * Gesture slice properties
 *
 * The coordinate system for gesture properties is determined by the device
 * type. Direct devices provide screen coordinates. Indirect devices provide
 * device coordinates.
 */
typedef enum UGSliceProperty {
  /**
   * Gesture ID
   *
   * Value type: unsigned int
   */
  UGSlicePropertyId = 0,
  /**
   * Gesture set state
   *
   * Value type: UGGestureState
   */
  UGSlicePropertyState,
  /**
   * Gesture subscription
   *
   * Value type: UGSubscription
   */
  UGSlicePropertySubscription,
  /**
   * Recognized gestures
   *
   * Value type: UGGestureTypeMask
   */
  UGSlicePropertyRecognized,
  /**
   * Number of touches
   *
   * Value type: unsigned int
   */
  UGSlicePropertyNumTouches,
  /**
   * Touch frame
   *
   * Value type: UFFrame
   */
  UGSlicePropertyFrame,
  /**
   * Original gesture center along the X axis
   *
   * Value type: float
   *
   * This value represents the original geometric center of the touches.
   */
  UGSlicePropertyOriginalCenterX,
  /**
   * Original gesture center along the Y axis
   *
   * Value type: float
   *
   * This value represents the original geometric center of the touches.
   */
  UGSlicePropertyOriginalCenterY,
  /**
   * Original radius of touches
   *
   * Value type: float
   *
   * This value represents the average of the square of the euclidean distance
   * from the geometric center of the original touches to each touch.
   */
  UGSlicePropertyOriginalRadius,
  /**
   * Best-fit 2D affine transformation of previous to current touch locations
   *
   * Value type: pointer to UGTransform
   *
   * The transformation is relative to the previous geometric center of the
   * touches.
   */
  UGSlicePropertyTransform,
  /**
   * Best-fit 2D affine transformation of original to current touch locations
   *
   * Value type: pointer to UGTransform
   *
   * The transformation is relative to the original geometric center of the
   * touches.
   */
  UGSlicePropertyCumulativeTransform,
  /**
   * Best-fit instant center of rotation along the X axis
   *
   * Value type: float
   */
  UGSlicePropertyCenterOfRotationX,
  /**
   * Best-fit instant center of rotation along the Y axis
   *
   * Value type: float
   */
  UGSlicePropertyCenterOfRotationY,
  /**
   * Whether the construction of all gestures containing the same touches is
   * finished
   *
   * Value type: int with boolean semantics
   *
   * Grail events are serial. This property allows the client to determine if
   * all the possible gestures from the set of touches in this gesture have been
   * sent. When this value is true, the client will have received all the
   * information needed to make a gesture accept and reject decision based on
   * potentially overlapping gestures. An example is when both one and two touch
   * gestures are subscribed on the same window with the same gesture types and
   * thresholds. When this property is true for one touch gesture events, the
   * client can be sure there are no other touches unless a two touch gesture
   * event has already been sent.
   */
  UGSlicePropertyConstructionFinished,
} UGSliceProperty;

/**
 * Create a new grail context
 *
 * @param [out] handle The new grail context object
 * @return UGStatusSuccess or UGStatusErrorResources
 */
GRAIL_PUBLIC
UGStatus grail_new(UGHandle *handle);

/**
 * Delete a grail context
 *
 * @param [in] handle The grail context object
 */
GRAIL_PUBLIC
void grail_delete(UGHandle handle);

/**
 * Get the event file descriptor for the frame context
 *
 * @param [in] handle The grail context object
 * @return A file descriptor for the context
 *
 * When events are available for processing, the file descriptor will be
 * readable. Perform an 8-byte read from the file descriptor to clear the state.
 * Refer to the EVENTFD(2) man page for more details.
 */
GRAIL_PUBLIC
int grail_get_fd(UGHandle handle);

/**
 * Create a new subscription object
 *
 * @param [out] subscription The new subscription object
 * @return UGStatusSuccess or UGStatusErrorResources
 */
GRAIL_PUBLIC
UGStatus grail_subscription_new(UGSubscription* subscription);

/**
 * Delete a subscription object
 *
 * @param [in] subscription The subscription object
 */
GRAIL_PUBLIC
void grail_subscription_delete(UGSubscription subscription);

/**
 * Set a subscription property
 *
 * @param [in] subscription The subscription object
 * @param [in] property The subscription property
 * @param [in] value The new value of the property
 * @return UGStatusSuccess or UGStatusInvalidProperty
 */
GRAIL_PUBLIC
UGStatus grail_subscription_set_property(UGSubscription subscription,
                                         UGSubscriptionProperty property,
                                         const void* value);

/**
 * Get a subscription property
 *
 * @param [in] subscription The subscription object
 * @param [in] property The subscription property
 * @param [out] value The value of the property
 * @return UGStatusSuccess or UGStatusInvalidProperty
 */
GRAIL_PUBLIC
UGStatus grail_subscription_get_property(UGSubscription subscription,
                                         UGSubscriptionProperty property,
                                         void* value);

/**
 * Activate a subscription
 *
 * @param [in] handle The context object
 * @param [in] subscription The subscription object
 * @return UGStatusSuccess or UGStatusErrorInvalidDevice
 */
GRAIL_PUBLIC
UGStatus grail_subscription_activate(UGHandle handle, UGSubscription subscription);

/**
 * Deactivate a subscription
 *
 * @param [in] handle The context object
 * @param [in] subscription The subscription object
 */
GRAIL_PUBLIC
void grail_subscription_deactivate(UGHandle handle,
                                   UGSubscription subscription);

/**
 * Process a frame event
 *
 * @param [in] handle The context object
 * @param [in] event The frame event
 */
GRAIL_PUBLIC
void grail_process_frame_event(UGHandle handle, const UFEvent event);

/**
 * Get an event from the grail context
 *
 * @param [in] handle The context object
 * @param [out] event The retrieved event
 * @return UGStatusSuccess or UGStatusErrorNoEvent
 */
GRAIL_PUBLIC
UGStatus grail_get_event(UGHandle handle, UGEvent *event);

/**
 * Update the grail state for the given server time
 *
 * @param [in] handle The context object
 * @param [in] time The current server time
 *
 * The recognizer uses timeouts for deciding whether to accept or reject
 * touches. Calling this function will perform any pending decisions based on
 * the current server time.
 */
GRAIL_PUBLIC
void grail_update_time(UGHandle handle, uint64_t time);

/**
 * Get the next timeout at which to update the grail state
 *
 * @param [in] handle The context object
 * @return The next server time at which the grail state should be updated
 *
 * To update the grail state, call grail_update_time().
 */
GRAIL_PUBLIC
uint64_t grail_next_timeout(UGHandle handle);

/**
 * Increment the reference count of an event
 *
 * @param [in] event The event object
 */
GRAIL_PUBLIC
void grail_event_ref(UGEvent event);

/**
 * Decrement the reference count of an event
 *
 * @param [in] event The event object
 *
 * When the reference count reaches zero, the event is freed.
 */
GRAIL_PUBLIC
void grail_event_unref(UGEvent event);

/**
 * Get the value of a property of an event
 *
 * @param [in] event The event object
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UGStatusSuccess or UGStatusErrorUnknownProperty
 */
GRAIL_PUBLIC
UGStatus grail_event_get_property(const UGEvent event, UGEventProperty property,
                                  void *value);

/**
 * Get the touch ID of a touch in a slice
 *
 * @param [in] slice The gesture slcie object
 * @param [in] index The index of the touch in the slice
 * @param [out] touch_id The touch ID of the touch
 */
GRAIL_PUBLIC
UGStatus grail_slice_get_touch_id(const UGSlice slice, unsigned int index,
                                  UFTouchId *touch_id);

/**
 * Get the value of a property of a gesture slice
 *
 * @param [in] slice The gesture slice object
 * @param [in] property The property to retrieve a value for
 * @param [out] value The value retrieved
 * @return UGStatusSuccess or UGStatusErrorUnknownProperty
 */
GRAIL_PUBLIC
UGStatus grail_slice_get_property(const UGSlice slice, UGSliceProperty property,
                                  void *value);

/**
 * Accept gesture associated with gesture slice
 *
 * @param [in] handle The context object
 * @param [in] id The ID of the gesture to accept
 * @return UGStatusSuccess or UGStatusErrorInvalidGesture
 */
GRAIL_PUBLIC
UGStatus grail_accept_gesture(UGHandle handle, unsigned int id);

/**
 * Reject gesture associated with gesture slice
 *
 * @param [in] handle The context object
 * @param [in] id The ID of the gesture to reject
 * @return UGStatusSuccess or UGStatusErrorInvalidGesture
 */
GRAIL_PUBLIC
UGStatus grail_reject_gesture(UGHandle handle, unsigned int id);

/**
 * @defgroup v3-helpers Helper Functions
 * These helper functions may be used in place of the generic property getters.
 * They are limited to properties that are guaranteed to exist in all instances
 * of the objects.
 * @{
 */

/**
 * Get the type of an event
 *
 * @param [in] event The event object
 * @return The type of the event
 */
GRAIL_PUBLIC
UGEventType grail_event_get_type(const UGEvent event);

/**
 * Get the time of an event
 *
 * @param [in] event The event object
 * @return The time of the event
 */
GRAIL_PUBLIC
uint64_t grail_event_get_time(const UGEvent event);

/**
 * Get the ID of a gesture from a slice
 *
 * @param [in] slice The gesture slice object
 * @return The ID of the gesture
 */
GRAIL_PUBLIC
unsigned int grail_slice_get_id(const UGSlice slice);

/**
 * Get the state of a gesture in a slice
 *
 * @param [in] slice The gesture slice object
 * @return The state of the gesture in the slice
 */
GRAIL_PUBLIC
UGGestureState grail_slice_get_state(const UGSlice slice);

/**
 * Get the subscription for the gesture from the slice
 *
 * @param [in] slice The gesture slice object
 * @return The subscription
 */
GRAIL_PUBLIC
UGSubscription grail_slice_get_subscription(const UGSlice slice);

/**
 * Get the gestures recognized through the slice
 *
 * @param [in] slice The gesture slice object
 * @return The recognized gestures
 */
GRAIL_PUBLIC
UGGestureTypeMask grail_slice_get_recognized(const UGSlice slice);

/**
 * Get the current number of touches in the slice
 *
 * @param [in] slice The gesture slice object
 * @return The number of touches
 */
GRAIL_PUBLIC
unsigned int grail_slice_get_num_touches(const UGSlice slice);

/**
 * Get the original centroid position of the gesture along the X axis
 *
 * @param [in] slice The gesture slice object
 * @return The position
 */
GRAIL_PUBLIC
float grail_slice_get_original_center_x(const UGSlice slice);

/**
 * Get the original centroid position of the gesture along the Y axis
 *
 * @param [in] slice The gesture slice object
 * @return The position
 */
GRAIL_PUBLIC
float grail_slice_get_original_center_y(const UGSlice slice);

/**
 * Get the original radius of the gesture
 *
 * @param [in] slice The gesture slice object
 * @return The position
 */
GRAIL_PUBLIC
float grail_slice_get_original_radius(const UGSlice slice);

/**
 * Get the instantaneous center of rotation of the gesture along the X axis
 *
 * @param [in] slice The gesture slice object
 * @return The position
 */
GRAIL_PUBLIC
float grail_slice_get_center_of_rotation_x(const UGSlice slice);

/**
 * Get the instantaneous center of rotation of the gesture along the Y axis
 *
 * @param [in] slice The gesture slice object
 * @return The position
 */
GRAIL_PUBLIC
float grail_slice_get_center_of_rotation_y(const UGSlice slice);

/**
 * Get the best-fit instantaneous 2D affine transformation for the gesture slice
 *
 * @param [in] slice The gesture slice object
 * @return the transformation
 *
 * The returned transformation is owned by the gesture slice.
 */
GRAIL_PUBLIC
const UGTransform *grail_slice_get_transform(const UGSlice slice);

/**
 * Get the best-fit cumulative 2D affine transformation for the gesture slice
 *
 * @param [in] slice The gesture slice object
 * @return the transformation
 *
 * The returned transformation is owned by the gesture slice.
 */
GRAIL_PUBLIC
const UGTransform *grail_slice_get_cumulative_transform(const UGSlice slice);

/**
 * Get the frame frame for the slice
 *
 * @param [in] slice The gesture slice object
 * @return the frame
 */
GRAIL_PUBLIC
const UFFrame grail_slice_get_frame(const UGSlice slice);

/**
 * Get whether construction has finished for all touches in the gesture
 *
 * @param [in] slice The gesture slice object
 * @return whether construction has finished
 */
GRAIL_PUBLIC
int grail_slice_get_construction_finished(const UGSlice slice);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif // GRAIL_OIF_GRAIL_H_
