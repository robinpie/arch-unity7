/**
 * @file oif/frame_backend.h
 * API for creating objects.
 */

#ifndef FRAME_OIF_FRAME_BACKEND_H_
#define FRAME_OIF_FRAME_BACKEND_H_

/* front end definitions */
#include <oif/frame.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Handle for a device to be used on the backend API */
typedef struct UFBackendDevice_* UFBackendDevice;

/** Handle for a frame to be used on the backend API */
typedef struct UFBackendFrame_* UFBackendFrame;

/** Handle for a touch to be used on the backend API */
typedef struct UFBackendTouch_* UFBackendTouch;

/********************************************************************
 * Event
 ********************************************************************/

/**
 * Creates a new event with a reference count of one.
 */
FRAME_PUBLIC
UFEvent frame_event_new();

/**
 * Sets the type of the given event
 */
FRAME_PUBLIC
void frame_event_set_type(UFEvent event, UFEventType type);

/**
 * Sets the device property of the given event
 *
 * It increases the reference count of the corresponding UFDevice by one.
 */
FRAME_PUBLIC
void frame_event_set_device(UFEvent event, UFBackendDevice device);

/**
 * Sets the frame property of the given event
 *
 * It increases the reference count of the corresponding UFFrame by one.
 */
FRAME_PUBLIC
void frame_event_set_frame(UFEvent event, UFBackendFrame frame);

/**
 * Sets the time of the given event
 */
FRAME_PUBLIC
void frame_event_set_time(UFEvent event, uint64_t time);

/********************************************************************
 * Device
 ********************************************************************/

/**
 * Creates a new UFDevice and returns its backend handle.
 */
FRAME_PUBLIC
UFBackendDevice frame_backend_device_new();

/**
 * Returns a UFDevice instance given its backend handle.
 */
FRAME_PUBLIC
UFDevice frame_backend_device_get_device(UFBackendDevice device);

/**
 * Sets the "Name" property of the given device
 */
FRAME_PUBLIC
void frame_backend_device_set_name(UFBackendDevice device, const char *name);

/**
 * Sets the "Direct" property of the given device
 */
FRAME_PUBLIC
void frame_backend_device_set_direct(UFBackendDevice device, int direct);

/**
 * Sets the "Independent" property of the given device
 */
FRAME_PUBLIC
void frame_backend_device_set_independent(UFBackendDevice device, int independent);

/**
 * Sets the "SemiMT" property of the given device
 */
FRAME_PUBLIC
void frame_backend_device_set_semi_mt(UFBackendDevice device, int semi_mt);

/**
 * Sets the "MaxTouches" property of the given device
 */
FRAME_PUBLIC
void frame_backend_device_set_max_touches(UFBackendDevice device, unsigned int max_touches);

/**
 * Sets the "WindowResolutionX" and "WindowResolutionY" properties of the
 * given device.
 */
FRAME_PUBLIC
void frame_backend_device_set_window_resolution(UFBackendDevice device, float x, float y);

/**
 * Adds an axis to the device
 */
FRAME_PUBLIC
void frame_backend_device_add_axis(UFBackendDevice device,
                                   UFAxisType type,
                                   float min, float max, float resolution);

/**
 * Deletes the backend handle of a UFDevice, decreasing its reference count by one.
 */
FRAME_PUBLIC
void frame_backend_device_delete(UFBackendDevice device);

/********************************************************************
 * Frame
 ********************************************************************/

/**
 * Creates a new, empty, UFFrame and returns its backend handle.
 *
 * Usually you will use this method only for the very first frame. For all
 * subsequent ones it will be safer and more convinent to use
 * frame_backend_frame_create_next().
 */
FRAME_PUBLIC
UFBackendFrame frame_backend_frame_new();

/**
 * Creates a new UFFrame that is a continuation of the given one.
 *
 * Touches that had a "begin" state on the given frame will be hard-copied and
 * have an "update" state on the new frame.
 *
 * Touches that had an "update" state will be lazily copied to the new frame.
 *
 * Touches that had a "end" state on the given frame won't be present
 * on the new frame.
 *
 * The "ActiveTouches" property is automatically set to match the number
 * of UFTouches present or remaining.
 */
FRAME_PUBLIC
UFBackendFrame frame_backend_frame_create_next(UFBackendFrame frame);

/**
 * Returns a UFFrame instance given its backend handle.
 */
FRAME_PUBLIC
UFFrame frame_backend_frame_get_frame(UFBackendFrame frame);

/**
 * Gets a UFBackendTouch for the UFTouch that has the given id.
 *
 * The underlying UFTouch is moved from the given frame to the returned UFBackendTouch.
 * Once done modifying the touch you're expected to return it to the frame via
 * frame_backend_frame_give_touch().
 *
 * If the underlying UFTouch is a lazy copy (likely from a touch in the previous frame), a hard copy
 * will be made upon the first change made to it.
 *
 * Possible errors: UFStatusErrorInvalidTouch
 */
FRAME_PUBLIC
UFStatus frame_backend_frame_borrow_touch_by_id(UFBackendFrame frame,
                                                UFTouchId id,
                                                UFBackendTouch *touch);

/**
 * Sets the "Device" property of the given frame
 */
FRAME_PUBLIC
void frame_backend_frame_set_device(UFBackendFrame frame, UFBackendDevice device);

/**
 * Sets the "WindowId" property of the given frame
 */
FRAME_PUBLIC
void frame_backend_frame_set_window_id(UFBackendFrame frame, UFWindowId window_id);

/**
 * Sets the "ActiveTouches" property of the given frame
 *
 * If unset this property returns the number of touches.
 */
FRAME_PUBLIC
void frame_backend_frame_set_active_touches(UFBackendFrame frame, unsigned int active_touches);

/**
 * Gives a UFTouch to the specified frame.
 *
 * Gives the underlying UFTouch to the specified frame. The UFBackendTouch
 * is deleted and no longer valid after this call.
 *
 * A frame is a snapshot of the state of all touches at a given point in time.
 * Therefore it must not have multiple UFTouch instances with the same touch ID.
 * Attempting to do so will result in a UFStatusErrorTouchIdExists being returned.
 *
 * Possible errors: UFStatusErrorTouchIdExists
 */
FRAME_PUBLIC
UFStatus frame_backend_frame_give_touch(UFBackendFrame frame, UFBackendTouch *touch);

/**
 * Deletes the backend handle of a UFFrame,
 * decreasing its reference count by one.
 */
FRAME_PUBLIC
void frame_backend_frame_delete(UFBackendFrame frame);

/********************************************************************
 * Touch
 ********************************************************************/

/**
 * Creates a new UFTouch and returns its backend handle.
 *
 * Its state will be set to "Begin".
 *
 * After filled out, it should be given to a frame via frame_backend_frame_give_touch()
 */
FRAME_PUBLIC
UFBackendTouch frame_backend_touch_new();

/**
 * Returns a UFTouch instance given its backend handle.
 */
FRAME_PUBLIC
UFTouch frame_backend_touch_get_touch(UFBackendTouch touch);

/**
 * Sets the "Id" property of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_id(UFBackendTouch touch, UFTouchId id);

/**
 * Sets the "State" property of the given touch to "End"
 */
FRAME_PUBLIC
void frame_backend_touch_set_ended(UFBackendTouch touch);

/**
 * Sets the "WindowX" and "WindowY" properties of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_window_pos(UFBackendTouch touch, float x, float y);

/**
 * Sets the "Time" property of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_time(UFBackendTouch touch, uint64_t time);

/**
 * Sets the "StartTime" property of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_start_time(UFBackendTouch touch, uint64_t start_time);

/**
 * Sets the "Owned" property of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_owned(UFBackendTouch touch, int owned);

/**
 * Sets the "PendingEnd" property of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_pending_end(UFBackendTouch touch, int pending_end);

/**
 * Sets the value of an axis of the given touch
 */
FRAME_PUBLIC
void frame_backend_touch_set_value(UFBackendTouch touch, UFAxisType type, float value);

#ifdef __cplusplus
}
#endif

#endif /* FRAME_OIF_FRAME_BACKEND_H_ */
