/**
 * @file geis_grail_backend.c
 * @brief GEIS grail client back end
 */

/*
 * Copyright 2011-2013 Canonical Ltd.
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
#include "geis_grail_backend.h"

#include <float.h>
#include "geis_attr.h"
#include "geis_backend.h"
#include "geis_backend_protected.h"
#include "geis_bag.h"
#include "geis_event.h"
#include "geis_grail_token.h"
#include "geis_grail_window_grab.h"
#include "geis_grail_xsync.h"
#include "geis_group.h"
#include "geis_logging.h"
#include "geis_private.h"
#include "geis_subscription.h"
#include "geis_test_api.h"
#include "geis_touch.h"
#include "geis_ugsubscription_store.h"
#include <math.h>
#include <string.h>
#include <oif/frame_x11.h>
#include <oif/grail.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xlib.h>

#define GBE_MAX_TOUCHES 5

static inline GeisSize
_min(GeisSize a, GeisSize b)
{
  return (a < b) ? a : b;
}


static inline GeisSize
_max(GeisSize a, GeisSize b)
{
  return (a > b) ? a : b;
}


struct _GeisSliceState
{
  unsigned int slice_id;
  uint64_t     timestamp;
  GeisFloat    angle;
  GeisFloat    position_x;
  GeisFloat    position_y;
  GeisFloat    radius;
  unsigned int num_touches;
  unsigned int touch_ids[GBE_MAX_TOUCHES];
};


/**
 * @addtogroup geis_backend_grail GEIS Grail Back End
 * @ingroup geis_backends
 *
 * A GEIS Back End that wraps a grail instance.
 *
 * @{
 */

/** The less opaque Grail Back End structure. */
struct GeisGrailBackend
{
  Geis                      geis;         /**< The parent GEIS instance. */
  Display                  *display;      /**< The X11 connection. */
  Window                    root_window;  /**< The X11 root window. */
  GeisGrailXSync            xsync;        /**< XSync handler for setting timers. */
  UFHandle                  frame;        /**< A frame instance. */
  UGHandle                  grail;        /**< A grail instance. */
  GeisBag                   devices;      /**< The list of known devices. */
  GeisGrailWindowGrabStore  window_grabs; /**< A collection of window inputs */
  GeisBag                   slice_states; /**< Gesture slice states */
  GeisSubBag                subscription_bag;

  GeisGestureClass          drag_class;
  GeisGestureClass          pinch_class;
  GeisGestureClass          rotate_class;
  GeisGestureClass          tap_class;
  GeisGestureClass          touch_class;

  GeisBoolean               send_tentative_events;
  GeisBoolean               send_synchronous_events;
};

/**
  Holds backend-specific information regarding a GeisSubscription
 */
struct GeisGrailSubscriptionData
{
  GeisUGSubscriptionStore ugstore;

  /* configuration options
     NULL if not set by user, in which case grail defaults are used */
  uint64_t *drag_timeout;
  float *drag_threshold;
  uint64_t *pinch_timeout;
  float *pinch_threshold;
  uint64_t *rotate_timeout;
  float *rotate_threshold;
  uint64_t *tap_timeout;
  float *tap_threshold;
};



static GeisStatus
_grail_be_activate_for_device(GeisGrailBackend gbe,
                              GeisFilter       filter,
                              GeisDevice       device,
                              GeisSubscription subscription);

/**
 * Hashes an ID from a UFDevice.
 *
 * @todo this may need to be honed a little more.
 */
static inline GeisInteger
_grail_be_device_id_from_ufdevice(UFDevice ufdevice)
{
  return (intptr_t)ufdevice & 0xffff;
}


/**
 * Gets the UFDevice associated with an identified device hash.
 */
static UFDevice
_grail_be_ufdevice_from_device_id(GeisGrailBackend gbe, GeisInteger device_id)
{
  UFDevice ufdevice = NULL;
  for (GeisSize i = 0; i < geis_bag_count(gbe->devices); ++i)
  {
    UFDevice *d = geis_bag_at(gbe->devices, i);
    if (device_id == _grail_be_device_id_from_ufdevice(*d))
    {
      ufdevice = *d;
      break;
    }
  }
  return ufdevice;
}


/**
 * A temporary error handler for X for when the default error handler is
 * abort().
 */
static int
_grail_be_x_error_handler(Display*     display,
                          XErrorEvent* event)
{
  char buffer[512];
  XGetErrorText(display, (int)event->error_code, buffer, 511);
  geis_error("error %u in X detected: %s", (unsigned)event->error_code, buffer);
  return 0;
}


/**
 * Extracts or calculates various gesture attributes.
 *
 * @param[in]  gbe         the grail back end instance
 * @param[in]  slice       the grail slice received
 * @param[in]  slice_state the current grail slice state
 * @param[in]  delta_t     the time since the previous frame
 * @param[out] frame       the gesture frame being created
 */
static void
_grail_be_extract_gesture_attrs(GeisGrailBackend        gbe,
                                UGSlice                 slice,
                                struct _GeisSliceState *slice_state,
                                GeisFloat               delta_t,
                                GeisFrame               frame)
{
  UGGestureTypeMask ugmask = grail_slice_get_recognized(slice);
  const UGTransform *C = grail_slice_get_cumulative_transform(slice);

  geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME,
                                           GEIS_ATTR_TYPE_STRING,
                                           "n/a"));

  geis_frame_set_matrix(frame, &(*C)[0][0]);
  if (ugmask & UGGestureTypeDrag)
  {
    geis_frame_set_is_class(frame, gbe->drag_class);

    GeisFloat position_x = (*C)[0][2] + grail_slice_get_original_center_x(slice);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_POSITION_X,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &position_x));

    GeisFloat position_y = (*C)[1][2] + grail_slice_get_original_center_y(slice);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_POSITION_Y,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &position_y));

    GeisFloat delta_x = position_x - slice_state->position_x;
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_DELTA_X,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &delta_x));
    slice_state->position_x = position_x;

    GeisFloat delta_y = position_y - slice_state->position_y;
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_DELTA_Y,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &delta_y));
    slice_state->position_y = position_y;

    if (delta_t > 0.0f)
    {
      GeisFloat x_velocity = delta_x / delta_t;
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_VELOCITY_X,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &x_velocity));

      GeisFloat y_velocity = delta_y / delta_t;
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &y_velocity));
    }
  }
  if (ugmask & UGGestureTypePinch)
  {
    geis_frame_set_is_class(frame, gbe->pinch_class);

    GeisFloat ca = (*C)[0][0];
    GeisFloat cb = (*C)[0][1];
    GeisFloat r = sqrtf(ca*ca + cb*cb);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_RADIUS,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &r));

    GeisFloat dr = r / slice_state->radius;
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &dr));
    slice_state->radius = r;

    if (delta_t > 0.0f)
    {
      GeisFloat vr = (delta_t > 0.0f) ? dr / delta_t : 0.0f;
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_RADIAL_VELOCITY,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &vr));
    }
  }
  if (ugmask & UGGestureTypeRotate)
  {
    geis_frame_set_is_class(frame, gbe->rotate_class);

    GeisFloat centre_x = grail_slice_get_center_of_rotation_x(slice);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_CENTROID_X,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &centre_x));

    GeisFloat centre_y = grail_slice_get_center_of_rotation_y(slice);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_CENTROID_Y,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &centre_y));
    GeisFloat angle = atan2((*C)[0][1], (*C)[0][0]);
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_ANGLE,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &angle));

    GeisFloat da = angle - slice_state->angle;
    geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_ANGLE_DELTA,
                                             GEIS_ATTR_TYPE_FLOAT,
                                             &da));
    slice_state->angle = angle;

    if (delta_t > 0.0f)
    {
      GeisFloat va = (delta_t > 0.0f) ? da / delta_t : 0.0f;
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_ANGULAR_VELOCITY,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &va));
    }
  }
  if (ugmask & UGGestureTypeTap)
  {
    geis_frame_set_is_class(frame, gbe->tap_class);
  }
  if (ugmask & UGGestureTypeTouch)
  {
    geis_frame_set_is_class(frame, gbe->touch_class);
  }
}


/**
 * Returns a grail slice state, given its id.
 *
 * @param[in] gbe       the grail back end
 * @param[in] slice_id  the id of a grail slice
 *
 * @returns a pointer to a grail slice state or NULL if there is no
 * grail slice state for a slice with the given id.
 */
struct _GeisSliceState *
_grail_be_slice_state_from_id(GeisGrailBackend gbe,
                              unsigned int slice_id)
{
  struct _GeisSliceState *slice_state = NULL;
  for (GeisSize i = 0; i < geis_bag_count(gbe->slice_states); ++i)
  {
    struct _GeisSliceState *s = geis_bag_at(gbe->slice_states, i);
    if (s->slice_id == slice_id)
    {
      slice_state = s;
      break;
    }
  }
  return slice_state;
}


/**
 * Gets the previous slice state (if any) for a grail slice.
 *
 * @param[in] gbe    the grail back end
 * @param[in] slice  a grail slice
 *
 * @returns a pointer to a grail slice state or NULL if there is no previous
 * grail slice state.
 */
struct _GeisSliceState *
_grail_be_slice_state_from_ugslice(GeisGrailBackend gbe,
                                   UGSlice          slice)
{
  return _grail_be_slice_state_from_id(gbe,
                                       grail_slice_get_id(slice));
}


/**
 * Constructs a new slice state for a grail slise.
 *
 * @param[in] gbe        the grail back end
 * @param[in] slice      a grail slice
 *
 * @returns a pointer to a grail slice state or NULL on failure.
 */
struct _GeisSliceState *
_grail_be_slice_state_new(GeisGrailBackend gbe,
                          UGSlice          slice)
{
  const UGTransform *C = grail_slice_get_cumulative_transform(slice);
  struct _GeisSliceState new_slice_state = {
    .slice_id = grail_slice_get_id(slice),
    .timestamp = 0, 
    .angle = 0.0f,
    .position_x = grail_slice_get_original_center_x(slice) + (*C)[0][2],
    .position_y = grail_slice_get_original_center_y(slice) + (*C)[1][2],
    .radius = 1.0f,
    .num_touches = grail_slice_get_num_touches(slice)
  };

  /* fill up the touch_ids array */
  UFTouchId touch_id;
  UGStatus status;
  for (unsigned int i = 0; i < new_slice_state.num_touches; ++i)
  {
    status = grail_slice_get_touch_id(slice, i, &touch_id);
    if (status == UGStatusSuccess)
    {
      new_slice_state.touch_ids[i] = touch_id;
    }
    else
    {
      geis_error("failed to get id of touch of index %u from slice with id %u",
        i, new_slice_state.slice_id);

      /* zero is a valid id but it's still better than leaving an aleatory
         number here */
      new_slice_state.touch_ids[i] = 0;
    }
  }

  geis_bag_append(gbe->slice_states, &new_slice_state);
  return _grail_be_slice_state_from_ugslice(gbe, slice);
}

/**
 * Update a slice state for a grail slice.
 *
 * @param[in] slice_state the geis slice state
 * @param[in] slice       a grail slice
 *
 * This function resets the slice state if the number of touches has changed.
 */
static void
_grail_be_slice_state_update(struct _GeisSliceState *slice_state, UGSlice slice)
{
  
  if (slice_state->num_touches != grail_slice_get_num_touches(slice))
  {
    const UGTransform *C = grail_slice_get_cumulative_transform(slice);

    slice_state->angle = 0.0f;
    slice_state->position_x = grail_slice_get_original_center_x(slice) +
                              (*C)[0][2];
    slice_state->position_y = grail_slice_get_original_center_y(slice) +
                              (*C)[1][2];
    slice_state->radius = 1.0f;
    slice_state->num_touches = grail_slice_get_num_touches(slice);
  }
}


/**
 * Creates a geis event from a grail slice.
 *
 * @param[in] gbe          the grail back end
 * @param[in] slice        the grail slice
 * @param[in] clice_state  the slice state (may be NULL)
 *
 * @returns a new geis event or NULL if no valid event can be created for the
 * slice.
 */
static GeisEvent
_grail_be_geis_event_from_ugslice(GeisGrailBackend        gbe,
                                  UGSlice                 slice,
                                  struct _GeisSliceState *slice_state)
{
  GeisEvent geis_event = NULL;

  /* Retrieve the slice's recognized gestures. */
  UGGestureTypeMask ugmask = grail_slice_get_recognized(slice);

  switch (grail_slice_get_state(slice))
  {
    case UGGestureStateBegin:
      if (ugmask)
        geis_event = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
      else if (gbe->send_tentative_events)
        geis_event = geis_event_new(GEIS_EVENT_TENTATIVE_BEGIN);
      break;
    case UGGestureStateUpdate:
      if (ugmask)
        if (slice_state)
          geis_event = geis_event_new(GEIS_EVENT_GESTURE_UPDATE);
        else
          geis_event = geis_event_new(GEIS_EVENT_GESTURE_BEGIN);
      else if (gbe->send_tentative_events)
        geis_event = geis_event_new(GEIS_EVENT_TENTATIVE_UPDATE);
      break;
    case UGGestureStateEnd:
      if (slice_state)
        geis_event = geis_event_new(GEIS_EVENT_GESTURE_END);
      else if (gbe->send_tentative_events)
        geis_event = geis_event_new(GEIS_EVENT_TENTATIVE_END);
      break;
  }

  return geis_event;
}

static void
_grail_be_set_x11_timeout(GeisGrailBackend gbe)
{
  /* Set a timeout on the X server, if necessary. */
  uint64_t timeout = grail_next_timeout(gbe->grail);
  if (timeout)
    geis_grail_xsync_set_timeout(gbe->xsync, timeout);
}

/**
 * Creates a GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED corresponding to the
 * "construction finished" property of the given slice.
 */
static GeisAttr
_grail_be_create_construction_attr(UGSlice slice)
{
  int slice_construction_finished = 0;

  UGStatus status = grail_slice_get_property(slice,
      UGSlicePropertyConstructionFinished,
      &slice_construction_finished);
  if (status != UGStatusSuccess)
  {
    geis_error("could not retrieve \"construction finished\" property from "
               "grail slice");
    return 0;
  }

  GeisBoolean attr_value;
  if (slice_construction_finished)
    attr_value = GEIS_TRUE;
  else
    attr_value = GEIS_FALSE;

  GeisAttr construction_attr = geis_attr_new(
      GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED,
      GEIS_ATTR_TYPE_BOOLEAN,
      &attr_value);

  return construction_attr;
}

static void
_grail_be_grail_fd_callback(int                             fd GEIS_UNUSED,
                            GeisBackendMultiplexorActivity  ev GEIS_UNUSED,
                            void                           *ctx)
{
  GeisGrailBackend gbe = (GeisGrailBackend)ctx;
  UGEvent event;
  while (grail_get_event(gbe->grail, &event) == UGStatusSuccess)
  {
    /* Extract the grail slice from the grail event. */
    if (UGEventTypeSlice == grail_event_get_type(event))
    {
      UGSlice slice;
      UGStatus ugstatus = grail_event_get_property(event,
                                                   UGEventPropertySlice,
                                                   &slice);
      if (ugstatus != UGStatusSuccess)
      {
        geis_error("could not retrieve slice from grail event");
        goto next_event;
      }

      /* Get any previous slice state for the grail slice. */
      struct _GeisSliceState *slice_state = NULL;
      slice_state = _grail_be_slice_state_from_ugslice(gbe, slice);

      /* Translate the grail slice into a geis event, maybe. */
      GeisEvent geis_event = _grail_be_geis_event_from_ugslice(gbe,
                                                               slice,
                                                               slice_state);
      if (!geis_event)
      {
        goto next_event;
      }

      /* A gesture needs to start out with an initial state. */
      if (GEIS_EVENT_GESTURE_BEGIN == geis_event_type(geis_event))
      {
        slice_state = _grail_be_slice_state_new(gbe, slice);
        if (!slice_state)
        {
          geis_error("logic error");
          geis_event_delete(geis_event);
          goto next_event;
        }
      }

      /* If the client doesn't want tentative events, tap events must be sent as
       * a one-shot update event. Transform begin events into update events, and
       * any discard any further events. */
      if (!gbe->send_tentative_events &&
          UGGestureTypeTap == grail_slice_get_recognized(slice))
      {
        if (GEIS_EVENT_GESTURE_BEGIN == geis_event_type(geis_event))
          geis_event_override_type(geis_event, GEIS_EVENT_GESTURE_UPDATE);
        else
        {
          geis_event_delete(geis_event);
          goto next_event;
        }
      }

      /* Updated any saved gesture state. */
      uint64_t timestamp = grail_event_get_time(event);
      GeisFloat time_delta = 0.0f;
      if (slice_state)
      {
        uint64_t dt = timestamp - slice_state->timestamp;
        /* Skip synchronous events if not requested to send them. */
        if (GEIS_EVENT_GESTURE_UPDATE == geis_event_type(geis_event)
          && !gbe->send_synchronous_events
          && dt <= 0)
        {
          geis_event_delete(geis_event);
          goto next_event;
        }
        time_delta = dt;
        slice_state->timestamp = timestamp;
      }

      _grail_be_slice_state_update(slice_state, slice);

      GeisGroupSet  groupset = geis_groupset_new();
      GeisAttr      group_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_GROUPSET,
                                               GEIS_ATTR_TYPE_POINTER,
                                               groupset);
      geis_attr_set_destructor(group_attr,
                               (GeisAttrDestructor)geis_groupset_delete);
      GeisGroup group = geis_group_new(1);
      geis_groupset_insert(groupset, group);

      GeisTouchSet  touchset = geis_touchset_new();
      GeisAttr      touch_attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_TOUCHSET,
                                               GEIS_ATTR_TYPE_POINTER,
                                               touchset);
      geis_attr_set_destructor(touch_attr,
                               (GeisAttrDestructor)geis_touchset_delete);

      GeisFrame frame = geis_frame_new(grail_slice_get_id(slice));
      geis_group_insert_frame(group, frame);

      GeisInteger ts = timestamp;
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_TIMESTAMP,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &ts));

      UFFrame ufframe = grail_slice_get_frame(slice);
      UFDevice ufdevice = frame_frame_get_device(ufframe);
      GeisInteger geis_device_id = _grail_be_device_id_from_ufdevice(ufdevice);
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_DEVICE_ID,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &geis_device_id));

      GeisBoolean is_touchscreen = GEIS_FALSE;
      GeisDevice geis_device = geis_get_device(gbe->geis, geis_device_id);
      if (!geis_device)
      {
        geis_warning("unrecognized device %d reported", geis_device_id);
      }
      else
      {
        GeisAttr attr = geis_device_attr_by_name(geis_device,
                                   GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH);
        is_touchscreen = (attr && geis_attr_value_to_boolean(attr));
      }

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_ROOT_WINDOW_ID,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &gbe->root_window));

      GeisInteger window = frame_x11_get_window_id(frame_frame_get_window_id(ufframe));
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &window));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_CHILD_WINDOW_ID,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &window));

      GeisInteger num_touches = grail_slice_get_num_touches(slice);
      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_TOUCHES,
                                               GEIS_ATTR_TYPE_INTEGER,
                                               &num_touches));

      GeisFloat focus_x = 0.0f;
      GeisFloat focus_y = 0.0f;
      if (is_touchscreen)
      {
        const UGTransform *C = grail_slice_get_cumulative_transform(slice);
        focus_x = (*C)[0][2] + grail_slice_get_original_center_x(slice);
        focus_y = (*C)[1][2] + grail_slice_get_original_center_y(slice);
      }

      GeisFloat bboxMinX = FLT_MAX;
      GeisFloat bboxMinY = FLT_MAX;
      GeisFloat bboxMaxX = 0.0f;
      GeisFloat bboxMaxY = 0.0f;
      for (GeisInteger t = 0; t < num_touches; ++t)
      {
        GeisTouch touch = geis_touch_new(t);
        if (!touch)
        {
          geis_error("can not create GEIS touch object");
          continue;
        }

        UFTouchId touch_id;
        ugstatus = grail_slice_get_touch_id(slice, t, &touch_id);
        GeisInteger touch_slot = touch_id;
        geis_touch_add_attr(touch, geis_attr_new(GEIS_TOUCH_ATTRIBUTE_ID,
                                                 GEIS_ATTR_TYPE_INTEGER,
                                                 &touch_slot));

        UFTouch uftouch;
        UFStatus ufstatus;
        ufstatus = frame_frame_get_touch_by_id(ufframe, touch_id, &uftouch);
        if (ufstatus != UFStatusSuccess)
        {
          geis_error("can not retrieve touch %ld from slice frame", (long)touch_id);
          continue;
        }

        if (!is_touchscreen)
        {
          focus_x = frame_touch_get_window_x(uftouch);
          focus_y = frame_touch_get_window_y(uftouch);
        }

        GeisFloat touch_x = 0.0f, touch_y = 0.0f;
        if (is_touchscreen)
        {
          touch_x = frame_touch_get_window_x(uftouch);
          touch_y = frame_touch_get_window_y(uftouch);
        }
        else
        {
          touch_x = frame_touch_get_device_x(uftouch);
          touch_y = frame_touch_get_device_y(uftouch);
        }

        bboxMinX = fmin(bboxMinX, touch_x);
        bboxMaxX = fmax(bboxMaxX, touch_x);
        geis_touch_add_attr(touch, geis_attr_new(GEIS_TOUCH_ATTRIBUTE_X,
                                                 GEIS_ATTR_TYPE_FLOAT,
                                                 &touch_x));

        bboxMinY = fmin(bboxMinY, touch_y);
        bboxMaxY = fmax(bboxMaxY, touch_y);
        geis_touch_add_attr(touch, geis_attr_new(GEIS_TOUCH_ATTRIBUTE_Y,
                                                 GEIS_ATTR_TYPE_FLOAT,
                                                 &touch_y));

        geis_touchset_insert(touchset, touch);
        geis_frame_add_touchid(frame, geis_touch_id(touch));
      }

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_FOCUS_X,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &focus_x));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_FOCUS_Y,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &focus_y));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X1,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &bboxMinX));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y1,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &bboxMinY));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_X2,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &bboxMaxX));

      geis_frame_add_attr(frame, geis_attr_new(GEIS_GESTURE_ATTRIBUTE_BOUNDINGBOX_Y2,
                                               GEIS_ATTR_TYPE_FLOAT,
                                               &bboxMaxY));

      _grail_be_extract_gesture_attrs(gbe,
                                      slice, slice_state,
                                      time_delta,
                                      frame);

      GeisAttr construction_attr = _grail_be_create_construction_attr(slice);

      geis_event_add_attr(geis_event, group_attr);
      geis_event_add_attr(geis_event, touch_attr);
      geis_event_add_attr(geis_event, construction_attr);
      geis_post_event(gbe->geis, geis_event);

      /* Destroy any saved state after the a gesture has ended. */
      if (UGGestureStateEnd == grail_slice_get_state(slice))
      {
        for (GeisSize i = 0; i < geis_bag_count(gbe->slice_states); ++i)
        {
          if (slice_state == geis_bag_at(gbe->slice_states, i))
          {
            geis_bag_remove(gbe->slice_states, i);
            break;
          }
        }
      }
    }

next_event:
    grail_event_unref(event);
  }

  _grail_be_set_x11_timeout(gbe);
}


/**
 * Activates active subscriptions for a newly-seen device.
 */
static void
_grail_be_subscribe_new_device(GeisGrailBackend gbe, GeisDevice device)
{
  for (GeisSubBagIterator it = geis_subscription_bag_begin(gbe->subscription_bag);
       it != geis_subscription_bag_end(gbe->subscription_bag);
       it = geis_subscription_bag_iterator_next(gbe->subscription_bag, it))
  {
    for (GeisFilterIterator fit = geis_subscription_filter_begin(*it);
         fit != geis_subscription_filter_end(*it);
         fit = geis_subscription_filter_next(*it, fit))
    {
      GeisBoolean device_applies = GEIS_TRUE;
      for (GeisSize tindex = 0; tindex < geis_filter_term_count(*fit); ++tindex)
      {
        GeisFilterTerm term = geis_filter_term(*fit, tindex);
        if (geis_filter_term_facility(term) == GEIS_FILTER_DEVICE)
        {
          if (!geis_filter_term_match_device(term, device))
          {
            device_applies = GEIS_FALSE;
            break;
          }
        }
      }

      if (device_applies)
      {
        _grail_be_activate_for_device(gbe, *fit, device, *it);
      }
    }
  }
}


/**
 * Removes active subscriptions for recently-removed devices.
 */
static void
_grail_be_unsubscribe_removed_device(GeisGrailBackend gbe, UFDevice device)
{
  for (GeisSubBagIterator it = geis_subscription_bag_begin(gbe->subscription_bag);
       it != geis_subscription_bag_end(gbe->subscription_bag);
       it = geis_subscription_bag_iterator_next(gbe->subscription_bag, it))
  {
   struct GeisGrailSubscriptionData *subscription_data = geis_subscription_pdata(*it);
   GeisUGSubscriptionStore ugstore = subscription_data->ugstore;
   for (GeisFilterIterator fit = geis_subscription_filter_begin(*it);
         fit != geis_subscription_filter_end(*it);
         fit = geis_subscription_filter_next(*it, fit))
    {
      geis_ugsubscription_release_for_device(ugstore, *fit, device, gbe->window_grabs);
    }
  }
}


/**
 * Adds the axis exten attributes for a device, if available.
 */
static void
_gbe_add_device_axis_attributes(UFDevice frame_device, GeisDevice geis_device)
{
  UFStatus  status;
  GeisFloat fval;

  UFAxis x_axis;
  status = frame_device_get_axis_by_type(frame_device, UFAxisTypeX, &x_axis);
  if (status != UFStatusSuccess)
  {
    geis_warning("failed to get X axis property from device '%s'",
                 geis_device_name(geis_device));
  }
  else
  {
    fval = frame_axis_get_minimum(x_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_MIN_X,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));

    fval = frame_axis_get_maximum(x_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_MAX_X,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));

    fval = frame_axis_get_resolution(x_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_RES_X,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));
  }

  UFAxis y_axis;
  status = frame_device_get_axis_by_type(frame_device, UFAxisTypeY, &y_axis);
  if (status != UFStatusSuccess)
  {
    geis_warning("failed to get Y axis property from device '%s'",
                 geis_device_name(geis_device));
  }
  else
  {
    fval = frame_axis_get_minimum(y_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_MIN_Y,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));

    fval = frame_axis_get_maximum(y_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_MAX_Y,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));

    fval = frame_axis_get_resolution(y_axis);
    geis_device_add_attr(geis_device, geis_attr_new(GEIS_DEVICE_ATTRIBUTE_RES_Y,
                                                    GEIS_ATTR_TYPE_FLOAT,
                                                    &fval));
  }
}


/**
 * Reports an X11 device to the front end as a GEIS device.
 */
static void
_geis_grail_add_device(GeisGrailBackend gbe, UFDevice frame_device)
{
  UFStatus status;

  GeisBoolean discard_device_messages = GEIS_FALSE;
  geis_get_configuration(gbe->geis,
                         GEIS_CONFIG_DISCARD_DEVICE_MESSAGES,
                         &discard_device_messages);
  if (discard_device_messages)
  {
    geis_debug("device message discarded because of configuration setting");
    return;
  }

  geis_bag_append(gbe->devices, &frame_device);
  GeisInteger device_id = _grail_be_device_id_from_ufdevice(frame_device);

  char *device_name = NULL;
  status = frame_device_get_property(frame_device,
                                     UFDevicePropertyName,
                                     &device_name);
  if (status != UFStatusSuccess)
  {
    geis_error("failed to get 'name' property from device");
    goto final_exit;
  }

  GeisDevice geis_device = geis_device_new(device_name, device_id);
  if (!geis_device)
  {
    geis_error("failed to create GEIS device");
    goto final_exit;
  }

  int ival;
  status = frame_device_get_property(frame_device,
                                     UFDevicePropertyMaxTouches,
                                     &ival);
  if (status != UFStatusSuccess)
  {
    geis_warning("failed to get 'touches' property from device '%s'",
                 device_name);
  }
  else
  {
    GeisAttr attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_TOUCHES,
                                  GEIS_ATTR_TYPE_INTEGER,
                                  &ival);
    geis_device_add_attr(geis_device, attr);
  }

  status = frame_device_get_property(frame_device,
                                     UFDevicePropertyDirect,
                                     &ival);
  if (status != UFStatusSuccess)
  {
    geis_warning("failed to get 'direct' property from device '%s'",
                 device_name);
  }
  else
  {
    GeisAttr attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,
                                  GEIS_ATTR_TYPE_BOOLEAN,
                                  &ival);
    geis_device_add_attr(geis_device, attr);
  }

  status = frame_device_get_property(frame_device,
                                     UFDevicePropertyIndependent,
                                     &ival);
  if (status != UFStatusSuccess)
  {
    geis_warning("failed to get 'independent' property from device '%s'",
                 device_name);
  }
  else
  {
    GeisAttr attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH,
                                  GEIS_ATTR_TYPE_BOOLEAN,
                                  &ival);
    geis_device_add_attr(geis_device, attr);
  }

  _gbe_add_device_axis_attributes(frame_device, geis_device);

  /* Report the device as a filterable entity. */
  static struct GeisFilterableAttribute attrs[] = {
    { GEIS_DEVICE_ATTRIBUTE_NAME,              GEIS_ATTR_TYPE_STRING,  0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_ID,                GEIS_ATTR_TYPE_INTEGER, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_TOUCHES,           GEIS_ATTR_TYPE_INTEGER, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,      GEIS_ATTR_TYPE_BOOLEAN, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH, GEIS_ATTR_TYPE_BOOLEAN, 0, NULL },
  };
  static GeisSize attr_count = sizeof(attrs)
                             / sizeof(struct GeisFilterableAttribute);

  geis_register_device(gbe->geis, geis_device, attr_count, attrs);

  /* We are not going to hold a pointer to this geis_device ourselves  */
  geis_device_unref(geis_device);

final_exit:
  return;
}


static void
_geis_grail_remove_device(GeisGrailBackend gbe, UFDevice frame_device)
{
  GeisBoolean discard_device_messages = GEIS_FALSE;
  geis_get_configuration(gbe->geis,
                         GEIS_CONFIG_DISCARD_DEVICE_MESSAGES,
                         &discard_device_messages);
  if (discard_device_messages)
  {
    geis_debug("device message discarded because of configuration setting");
    return;
  }

  _grail_be_unsubscribe_removed_device(gbe, frame_device);

  for (GeisSize i = 0; i < geis_bag_count(gbe->devices); ++i)
  {
    UFDevice *device = geis_bag_at(gbe->devices, i);
    GeisInteger device_id = _grail_be_device_id_from_ufdevice(*device);
    if (device_id == _grail_be_device_id_from_ufdevice(frame_device))
    {
      GeisDevice geis_device = geis_get_device(gbe->geis, device_id);
      if (!geis_device)
      {
        geis_warning("unrecognized device %d has been removed", device_id);
      }
      else
      {
        geis_unregister_device(gbe->geis, geis_device);
      }
      break;
    }
  }
}


/**
 * Processes all the frame events until the queue is empty.
 *
 * @param[in] gbe        the grail back end
 */
static void
_geis_be_flush_frame_events(GeisGrailBackend gbe)
{
  UFEvent frame_event;
  while (frame_get_event(gbe->frame, &frame_event) == UFStatusSuccess)
  {
    UFEventType event_type = frame_event_get_type(frame_event);
    if (event_type == UFEventTypeDeviceAdded)
    {
      UFDevice frame_device;
      UFStatus status = frame_event_get_property(frame_event,
                                                 UFEventPropertyDevice,
                                                 &frame_device);
      if (status != UFStatusSuccess)
      {
        geis_warning("can not get device from device-added frame event");
      }
      else
      {
        _geis_grail_add_device(gbe, frame_device);
      }
    }
    else if (event_type == UFEventTypeDeviceRemoved)
    {
      UFDevice frame_device;
      UFStatus status = frame_event_get_property(frame_event,
                                                 UFEventPropertyDevice,
                                                 &frame_device);
      if (status != UFStatusSuccess)
      {
        geis_warning("can not get device from device-removed frame event");
      }
      else
      {
        _geis_grail_remove_device(gbe, frame_device);
      }
    }
    grail_process_frame_event(gbe->grail, frame_event);
    frame_event_unref(frame_event);

    _grail_be_set_x11_timeout(gbe);
  }
}


/**
 * Callback invoked when frame has events in the queue.
 *
 * @param[in] fd   The frame file descriptor (not used).
 * @param[in] ev   The type of FD event (not used).
 * @param[in] ctx  The context data (the GEIS grail back end).
 */
static void
_grail_be_frame_fd_callback(int                             fd GEIS_UNUSED,
                            GeisBackendMultiplexorActivity  ev GEIS_UNUSED,
                            void                           *ctx)
{
  GeisGrailBackend gbe = (GeisGrailBackend)ctx;
  _geis_be_flush_frame_events(gbe);
}


/**
 * Pushes X11 events into the frame filter.
 *
 * @param[in] fd   The X11 file descriptor (not used).
 * @param[in] ev   The type of FD event (not used).
 * @param[in] ctx  The context data (the GEIS grail back end).
 */
static void
_x11_fd_callback(int                             fd GEIS_UNUSED,
                 GeisBackendMultiplexorActivity  ev GEIS_UNUSED,
                 void                           *ctx)
{
  GeisGrailBackend gbe = (GeisGrailBackend)ctx;
  XEvent event;
  while (XPending(gbe->display))
  {
    XNextEvent(gbe->display, &event);

    if (geis_grail_xsync_is_timeout(gbe->xsync, &event))
    {
      uint64_t server_time = geis_grail_xsync_get_server_time(&event);
      if (server_time)
        grail_update_time(gbe->grail, server_time);
      continue;
    }

    XGenericEventCookie *xcookie = &event.xcookie;
    if (!XGetEventData(gbe->display, xcookie))
    {
      geis_warning("failed to get X generic event data");
      continue;
    }

    UFStatus frame_status = frame_x11_process_event(gbe->frame, xcookie);
    if (frame_status != UFStatusSuccess)
    {
      geis_warning("failed to inject X11 event");
    }

    XFreeEventData(gbe->display, xcookie);
  }
}


/**
 * Checks if the X11 server supports the appropriate version of the XInput
 * extension.
 *
 * @param[in] gbe  The grail back end
 *
 * @returns GEIS_TRUE if the X11 server provides the XInput extension of a
 * sufficiently recent vintage, GEIS_FALSE otherwise.
 */
static GeisBoolean
_geis_grail_x11_has_xi2(GeisGrailBackend gbe)
{
  GeisBoolean has_xi2 = GEIS_FALSE;

  /* check if the XInput extension is available at all */
  int opcode;
  int event;
  int error;
  if (!XQueryExtension(gbe->display, "XInputExtension", &opcode, &event, &error))
  {
    geis_error("XInput extension is not available");
    goto final_exit;
  }

  /* check if it's the right version */
  int major = 2;
  int minor = 2;
  if (XIQueryVersion(gbe->display, &major, &minor) == BadRequest)
  {
    geis_error("XI2 is unavailable, X Server supports only %d.%d", major, minor);
    goto final_exit;
  }
  has_xi2 = GEIS_TRUE;

final_exit:
  return has_xi2;
}

/**
 * Connects to the X11 server and sets up processing on X11 events.
 *
 * @param[in] gbe  The grail back end
 *
 * @returns GEIS_TRUE if a conneciton to X11 server has been successfully
 * extablished and it provides the minimum required functionality,
 * GEIS_FALSE otherwise.
 */
static GeisBoolean
_geis_grail_open_x11_connection(GeisGrailBackend gbe)
{
  XErrorHandler old_x_handler;
  GeisBoolean success = GEIS_FALSE;
  gbe->display = XOpenDisplay(NULL);
  if (!gbe->display)
  {
    geis_error("error connecting to X server");
    goto final_exit;
  }

  /* verify all the necessary extensions are available */
  if (!_geis_grail_x11_has_xi2(gbe))
  {
    goto unwind_x11;
  }

  gbe->root_window = DefaultRootWindow(gbe->display);

  /* install an X11 event callback */
  geis_multiplex_fd(gbe->geis,
                    ConnectionNumber(gbe->display),
                    GEIS_BE_MX_READ_AVAILABLE,
                    _x11_fd_callback,
                    gbe);

  success = GEIS_TRUE;
  goto final_exit;

unwind_x11:
  old_x_handler = XSetErrorHandler(_grail_be_x_error_handler);
  XCloseDisplay(gbe->display);
  XSetErrorHandler(old_x_handler);
final_exit:
  return success;
}


/**
 * Asks the X server for device added or removed events.
 */
static void
_geis_grail_subscribe_x11_device_events(GeisGrailBackend gbe)
{
  XIEventMask mask = {
    .deviceid = XIAllDevices,
    .mask_len = XIMaskLen(XI_LASTEVENT),
    .mask     = calloc(XIMaskLen(XI_LASTEVENT), sizeof(char))
  };
  XISetMask(mask.mask, XI_HierarchyChanged);
  Status status = XISelectEvents(gbe->display, gbe->root_window, &mask, 1);
  if (status != Success)
  {
    geis_error("error %d selecting device-changed events on X server", status);
  }
  free(mask.mask);
}


/**
 * Reports available filterable device attrs to the front end.
 */
static void
_geis_grail_report_devices(GeisGrailBackend gbe)
{
  static struct GeisFilterableAttribute attrs[] = {
    { GEIS_DEVICE_ATTRIBUTE_NAME,              GEIS_ATTR_TYPE_STRING,  0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_ID,                GEIS_ATTR_TYPE_INTEGER, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_TOUCHES,           GEIS_ATTR_TYPE_INTEGER, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH,      GEIS_ATTR_TYPE_BOOLEAN, 0, NULL },
    { GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH, GEIS_ATTR_TYPE_BOOLEAN, 0, NULL },
  };
  static GeisSize attr_count = sizeof(attrs)
                             / sizeof(struct GeisFilterableAttribute);

  geis_register_device(gbe->geis, NULL, attr_count, attrs);
}


/**
 * Reports available gesture classes to the front end.
 */
static void
_geis_grail_report_classes(GeisGrailBackend gbe)
{
  static struct GeisFilterableAttribute attrs[] = {
    {
      GEIS_CLASS_ATTRIBUTE_NAME,
      GEIS_ATTR_TYPE_STRING,
      0,
      NULL
    },
    {
      GEIS_CLASS_ATTRIBUTE_ID,
      GEIS_ATTR_TYPE_INTEGER,
      0,
      NULL
    },
    {
      GEIS_GESTURE_ATTRIBUTE_TOUCHES,
      GEIS_ATTR_TYPE_INTEGER,
      0,
      NULL
    }
  };
  GeisSize attr_count = sizeof(attrs)
                      / sizeof(struct GeisFilterableAttribute);

  gbe->drag_class = geis_gesture_class_new(GEIS_GESTURE_DRAG,
                                           GEIS_GESTURE_PRIMITIVE_DRAG);
  geis_register_gesture_class(gbe->geis, gbe->drag_class, attr_count, attrs);

  gbe->pinch_class = geis_gesture_class_new(GEIS_GESTURE_PINCH,
                                            GEIS_GESTURE_PRIMITIVE_PINCH);
  geis_register_gesture_class(gbe->geis, gbe->pinch_class, attr_count, attrs);

  gbe->rotate_class = geis_gesture_class_new(GEIS_GESTURE_ROTATE,
                                             GEIS_GESTURE_PRIMITIVE_ROTATE);
  geis_register_gesture_class(gbe->geis, gbe->rotate_class, attr_count, attrs);

  gbe->tap_class = geis_gesture_class_new(GEIS_GESTURE_TAP,
                                          GEIS_GESTURE_PRIMITIVE_TAP);
  geis_register_gesture_class(gbe->geis, gbe->tap_class, attr_count, attrs);

  gbe->touch_class = geis_gesture_class_new(GEIS_GESTURE_TOUCH,
                                            GEIS_GESTURE_PRIMITIVE_TOUCH);
  geis_register_gesture_class(gbe->geis, gbe->touch_class, attr_count, attrs);
}


/**
 * Reports available regions to the front end.
 */
static void
_geis_grail_report_regions(GeisGrailBackend gbe)
{
  static struct GeisFilterableAttribute attrs[] = {
    { 
      GEIS_REGION_ATTRIBUTE_WINDOWID,
      GEIS_ATTR_TYPE_INTEGER,
      0,
      NULL
    },
  };
  static const GeisSize attr_count = sizeof(attrs)
                                   / sizeof(struct GeisFilterableAttribute);

  geis_register_region(gbe->geis, NULL, attr_count, attrs);
}


/**
 * Reports a successful initialization to the front end.
 */
static void
_geis_grail_report_init_complete(GeisGrailBackend gbe)
{
  geis_post_event(gbe->geis, geis_event_new(GEIS_EVENT_INIT_COMPLETE));
}


/**
 * Constructs a Grail back end.
 *
 * @param[in] mem
 * @param[in] geis
 */
static void
_geis_grail_backend_construct(void *mem, Geis geis)
{
  GeisGrailBackend gbe = (GeisGrailBackend)mem;
  gbe->geis = geis;

  if (_geis_grail_open_x11_connection(gbe))
  {
    gbe->xsync = geis_grail_xsync_new(gbe->display);
    if (!gbe->xsync)
    {
      geis_error("failed to create xsync instance");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_x11;
    }

    _geis_grail_subscribe_x11_device_events(gbe);

    UFStatus frame_status = frame_x11_new(gbe->display, &gbe->frame);
    if (frame_status != UFStatusSuccess)
    {
      geis_error("failed to create frame instance");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_xsync;
    }
    geis_multiplex_fd(gbe->geis,
                      frame_get_fd(gbe->frame),
                      GEIS_BE_MX_READ_AVAILABLE,
                      _grail_be_frame_fd_callback,
                      gbe);

    UGStatus grail_status = grail_new(&gbe->grail);
    if (grail_status != UGStatusSuccess)
    {
      geis_error("failed to create grail instance");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_frame;
    }
    geis_multiplex_fd(gbe->geis,
                      grail_get_fd(gbe->grail),
                      GEIS_BE_MX_READ_AVAILABLE,
                      _grail_be_grail_fd_callback,
                      gbe);

    gbe->devices = geis_bag_new(sizeof(UFDevice),
                                geis_bag_default_init_alloc,
                                geis_bag_default_growth_factor);
    if (!gbe->devices)
    {
      geis_error("failed to create UFDevices store");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_grail;
    }

    gbe->window_grabs = geis_grail_window_grab_store_new(gbe->display);
    if (!gbe->window_grabs)
    {
      geis_error("failed to create window grabs store");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_devices;
    }

    gbe->slice_states = geis_bag_new(sizeof(struct _GeisSliceState),
                                     geis_bag_default_init_alloc,
                                     geis_bag_default_growth_factor);
    if (!gbe->slice_states)
    {
      geis_error("failed to create slice times store");
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_grabs;
    }

    gbe->subscription_bag = geis_subscription_bag_new(1);
    if (!gbe->subscription_bag)
    {
      geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
      goto unwind_slice_states;
    }

    geis_get_configuration(gbe->geis,
                           GEIS_CONFIG_SEND_TENTATIVE_EVENTS,
                           &gbe->send_tentative_events);
    geis_get_configuration(gbe->geis,
                           GEIS_CONFIG_SEND_SYNCHRONOS_EVENTS,
                           &gbe->send_synchronous_events);

    _geis_grail_report_devices(gbe);
    _geis_grail_report_classes(gbe);
    _geis_grail_report_regions(gbe);
    _geis_grail_report_init_complete(gbe);
    goto final_exit;

unwind_slice_states:
    geis_bag_delete(gbe->slice_states);
unwind_grabs:
    geis_grail_window_grab_store_delete(gbe->window_grabs);
unwind_devices:
    geis_bag_delete(gbe->devices);
unwind_grail:
    geis_demultiplex_fd(gbe->geis, grail_get_fd(gbe->grail));
    grail_delete(gbe->grail);
unwind_frame:
    geis_demultiplex_fd(gbe->geis, frame_get_fd(gbe->frame));
    frame_x11_delete(gbe->frame);
unwind_xsync:
    geis_grail_xsync_delete(gbe->xsync);
unwind_x11:
    geis_demultiplex_fd(gbe->geis, ConnectionNumber(gbe->display));
    XErrorHandler old_x_handler = XSetErrorHandler(_grail_be_x_error_handler);
    XCloseDisplay(gbe->display);
    XSetErrorHandler(old_x_handler);
  }
  else
  {
    geis_error("no XInput connection established");
    geis_error_push(gbe->geis, GEIS_STATUS_UNKNOWN_ERROR);
  }

final_exit:
  return;
}


/**
 * Deconstructs a Grail back end.
 *
 * @param[in] be  A %GeisGrailBackend.
 */
static void 
_geis_grail_backend_finalize(GeisBackend be)
{
  GeisGrailBackend gbe = (GeisGrailBackend)be;
  geis_subscription_bag_delete(gbe->subscription_bag);
  geis_bag_delete(gbe->slice_states);
  geis_grail_window_grab_store_delete(gbe->window_grabs);
  geis_bag_delete(gbe->devices);
  geis_demultiplex_fd(gbe->geis, grail_get_fd(gbe->grail));
  grail_delete(gbe->grail);
  geis_demultiplex_fd(gbe->geis, frame_get_fd(gbe->frame));
  frame_x11_delete(gbe->frame);
  geis_grail_xsync_delete(gbe->xsync);
  geis_demultiplex_fd(gbe->geis, ConnectionNumber(gbe->display));

  XErrorHandler old_x_handler = XSetErrorHandler(_grail_be_x_error_handler);
  XCloseDisplay(gbe->display);
  XSetErrorHandler(old_x_handler);
}

/*
 * Identify events from gestures that overlap the one that got accepted.
 */
static GeisBoolean
_grail_be_match_overlapping_gesture_event(GeisEvent event, void *context)
{
  struct _GeisSliceState *slice_state = (struct _GeisSliceState *)context;

  GeisEventType event_type = geis_event_type(event);
  if (event_type != GEIS_EVENT_GESTURE_BEGIN
      && event_type != GEIS_EVENT_GESTURE_UPDATE
      && event_type != GEIS_EVENT_GESTURE_END)
    return GEIS_FALSE;

  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  if (!attr)
  {
    geis_error("can not get touchset from event");
    return GEIS_FALSE;
  }
  GeisTouchSet touchset = (GeisTouchSet)geis_attr_value_to_pointer(attr);

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  if (!attr)
  {
    geis_error("can not get groupset from event");
    return GEIS_FALSE;
  }
  GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);

  /* we filter only single-gesture events (i.e. events with a single group
     containing a single frame) */
  if (geis_groupset_group_count(groupset) != 1)
    return GEIS_FALSE;

  GeisGroup group = geis_groupset_group(groupset, 0);
  if (!group)
  {
    geis_error("can not get group 0 in groupset of event");
    return GEIS_FALSE;
  }

  if (geis_group_frame_count(group) != 1)
    return GEIS_FALSE;

  for (GeisSize i = 0; i < geis_touchset_touch_count(touchset); ++i)
  {
    GeisTouch touch = geis_touchset_touch(touchset, i);
    attr = geis_touch_attr_by_name(touch, GEIS_TOUCH_ATTRIBUTE_ID);
    unsigned int touch_id = geis_attr_value_to_integer(attr);

    for (GeisSize j = 0; j < slice_state->num_touches; ++j)
    {
      if (touch_id == slice_state->touch_ids[j])
        return GEIS_TRUE;
    }
  }

  /* There were no touch ids in common with slice_state */
  return GEIS_FALSE;
}

/*
 * Asks grail to accept an identified gesture.
 */
GeisStatus
_grail_be_accept_gesture(GeisBackend   be,
                         GeisGroup     group GEIS_UNUSED,
                         GeisGestureId gesture_id)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisGrailBackend gbe = (GeisGrailBackend)be;
  unsigned id = gesture_id;
  UGStatus ugstatus = grail_accept_gesture(gbe->grail, id);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("accept failed for gesture %u", id);
  }
  else
  {
    geis_debug("gesture %u accepted", id);
    status = GEIS_STATUS_SUCCESS;
  }

  struct _GeisSliceState *slice_state = _grail_be_slice_state_from_id(gbe, id);
  /* the corresponding grail gesture might have already ended, in which case we
     no longer hold information over its state. */
  if (slice_state)
    geis_remove_matching_events(gbe->geis,
                                _grail_be_match_overlapping_gesture_event,
                                slice_state);
  return status;
}


/*
 * Predicate to identify events to remove on rejection.
 */
static GeisBoolean
_grail_be_match_gesture_event(GeisEvent event, void *context)
{
  GeisGestureId gesture_id = *(GeisGestureId *)context;
  GeisEventType event_type = geis_event_type(event);
  if (event_type == GEIS_EVENT_GESTURE_BEGIN
   || event_type == GEIS_EVENT_GESTURE_UPDATE
   || event_type == GEIS_EVENT_GESTURE_END)
  {
    GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
    if (!attr)
    {
      geis_error("can not get groupset from event");
      return GEIS_FALSE;
    }
    GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
    for (GeisSize i = 0; i < geis_groupset_group_count(groupset); ++i)
    {
      GeisGroup group = geis_groupset_group(groupset, i);
      if (!group)
      {
        geis_error("can not get group %zu in groupset of event", i);
        return GEIS_FALSE;
      }

      for (GeisSize j = 0; j < geis_group_frame_count(group); ++j)
      {
        GeisFrame frame = geis_group_frame(group, j);
        if (!frame)
        {
          geis_error("can not get frame %zu in group %zu of event", j, i);
          return GEIS_FALSE;
        }

        return gesture_id == geis_frame_id(frame);
      }
    }
  }
  return GEIS_FALSE;
}


/*
 * Asks grail to reject an identified gesture.
 */
GeisStatus
_grail_be_reject_gesture(GeisBackend   be,
                         GeisGroup     group GEIS_UNUSED,
                         GeisGestureId gesture_id)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisGrailBackend gbe = (GeisGrailBackend)be;
  unsigned id = gesture_id;
  UGStatus ugstatus = grail_reject_gesture(gbe->grail, id);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("rejection failed for gesture %u", id);
  }
  else
  {
    geis_debug("gesture %u rejected", id);
    status = GEIS_STATUS_SUCCESS;
  }

  geis_remove_matching_events(gbe->geis,
                              _grail_be_match_gesture_event,
                              &gesture_id);
  return status;
}


/**
 * Sets a given property from the first ugsub in a GEIS subscription.
 *
 * If there is more than one grail subscription mapped to a geis subscription,
 * only the property value from the first is retrieved assuming all the mapped
 * subscriptions have the same property value setting.
 */
static GeisStatus
_grail_be_get_ugsub_property(GeisUGSubscriptionStore ugstore,
                             UGSubscriptionProperty grail_property,
                             GeisPointer            grail_value)
{
  GeisStatus retval = GEIS_STATUS_UNKNOWN_ERROR;
  if (ugstore)
  {
    for (GeisSize i = 0; i < geis_ugsubscription_count(ugstore); ++i)
    {
      UGSubscription ugsub = geis_ugsubscription_get_ugsubscription_at(ugstore,
                                                                       i);
      if (UGStatusSuccess == grail_subscription_get_property(ugsub,
                                                             grail_property,
                                                             grail_value))
      {
        retval = GEIS_STATUS_SUCCESS;
        break;
      }
    }
  }
  return retval;
}


/**
 * Sets a given grail property to a specified value.
 */
static GeisStatus
_grail_be_set_ugsub_property(GeisUGSubscriptionStore ugstore,
                             UGSubscriptionProperty grail_property,
                             GeisPointer            grail_value)
{
  /* OBS: it's still a success if there's no grail subscription */
  GeisStatus retval = GEIS_STATUS_SUCCESS;
  for (GeisSize i = 0; i < geis_ugsubscription_count(ugstore); ++i)
  {
    UGSubscription ugsub = geis_ugsubscription_get_ugsubscription_at(ugstore,
                                                                     i);
    if (UGStatusSuccess == grail_subscription_set_property(ugsub,
                                                           grail_property,
                                                           grail_value))
    {
      retval = GEIS_STATUS_SUCCESS;
    }
    else
    {
      retval = GEIS_STATUS_UNKNOWN_ERROR;
    }
  }
  return retval;
}

static GeisStatus
_grail_be_get_integer_configuration(GeisUGSubscriptionStore ugstore,
                                    uint64_t *prop,
                                    UGSubscriptionProperty grail_prop,
                                    GeisPointer geis_value)
{
  if (prop)
  {
    *((GeisInteger*)geis_value) = *prop;
    return GEIS_STATUS_SUCCESS;
  }
  else
  {
    return _grail_be_get_ugsub_property(ugstore, grail_prop, geis_value);
  }

}

static GeisStatus
_grail_be_get_float_configuration(GeisUGSubscriptionStore ugstore,
                                  float *prop,
                                  UGSubscriptionProperty grail_prop,
                                  GeisPointer geis_value)
{
  if (prop)
  {
    *((GeisFloat*)geis_value) = *prop;
    return GEIS_STATUS_SUCCESS;
  }
  else
  {
    return _grail_be_get_ugsub_property(ugstore, grail_prop, geis_value);
  }

}

/*
 * Dispatches the get-configuration call.
 */
static GeisStatus
_grail_be_get_configuration(GeisBackend      be GEIS_UNUSED,
                            GeisSubscription subscription,
                            GeisString       item_name,
                            GeisPointer      item_value)
{
  GeisStatus retval = GEIS_STATUS_NOT_SUPPORTED;

  struct GeisGrailSubscriptionData *subscription_data =
    geis_subscription_pdata(subscription);
  if (!subscription_data)
  {
    return retval;
  }

  if (0 == strcmp(item_name, GEIS_CONFIG_NUM_ACTIVE_SUBSCRIPTIONS))
  {
    struct GeisGrailSubscriptionData *sub_data = geis_subscription_pdata(subscription);
    *((GeisSize*)item_value) = geis_ugsubscription_count(sub_data->ugstore);
    retval =  GEIS_STATUS_SUCCESS;
  }

  #define GEIS_GRAIL_CHECK_GESTURE_CONFIG(gesture, Gesture, GESTURE) \
  if (strcmp(item_name, GEIS_CONFIG_##GESTURE##_TIMEOUT) == 0) \
  { \
    retval = _grail_be_get_integer_configuration( \
        subscription_data->ugstore, \
        subscription_data->gesture##_timeout, \
        UGSubscriptionProperty##Gesture##Timeout, \
        item_value); \
  } \
  else if (strcmp(item_name, GEIS_CONFIG_##GESTURE##_THRESHOLD) == 0) \
  { \
    retval = _grail_be_get_float_configuration( \
        subscription_data->ugstore, \
        subscription_data->gesture##_threshold, \
        UGSubscriptionProperty##Gesture##Threshold, \
        item_value); \
  }

  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(drag, Drag, DRAG)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(pinch, Pinch, PINCH)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(rotate, Rotate, ROTATE)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(tap, Tap, TAP)

  #undef GEIS_GRAIL_CHECK_GESTURE_CONFIG

  return retval;
}

static GeisStatus
_grail_be_set_integer_configuration(GeisUGSubscriptionStore ugstore,
                                    uint64_t **prop,
                                    UGSubscriptionProperty grail_prop,
                                    GeisPointer geis_value)
{
  if (!*prop)
    *prop = malloc(sizeof(uint64_t));

  **prop = *((GeisInteger*)geis_value);

  if (ugstore)
    return _grail_be_set_ugsub_property(ugstore, grail_prop, *prop);
  else
    return GEIS_STATUS_SUCCESS;
}

static GeisStatus
_grail_be_set_float_configuration(GeisUGSubscriptionStore ugstore,
                                  float **prop,
                                  UGSubscriptionProperty grail_prop,
                                  GeisPointer geis_value)
{
  if (!*prop)
    *prop = malloc(sizeof(float));

  **prop = *((GeisFloat*)geis_value);

  if (ugstore)
    return _grail_be_set_ugsub_property(ugstore, grail_prop, *prop);
  else
    return GEIS_STATUS_SUCCESS;
}

/*
 * Dispatches the set-configuration call.
 */
static GeisStatus
_grail_be_set_configuration(GeisBackend      be GEIS_UNUSED,
                            GeisSubscription subscription,
                            GeisString       item_name,
                            GeisPointer      item_value)
{
  GeisStatus retval = GEIS_STATUS_NOT_SUPPORTED;

  struct GeisGrailSubscriptionData *subscription_data =
    geis_subscription_pdata(subscription);
  if (!subscription_data)
  {
    subscription_data = calloc(1, sizeof(struct GeisGrailSubscriptionData));
    geis_subscription_set_pdata(subscription, subscription_data);
  }

  #define GEIS_GRAIL_CHECK_GESTURE_CONFIG(gesture, Gesture, GESTURE) \
  if (strcmp(item_name, GEIS_CONFIG_##GESTURE##_TIMEOUT) == 0) \
  { \
    retval = _grail_be_set_integer_configuration( \
        subscription_data->ugstore, \
        &(subscription_data->gesture##_timeout), \
        UGSubscriptionProperty##Gesture##Timeout, \
        item_value); \
  } \
  else if (strcmp(item_name, GEIS_CONFIG_##GESTURE##_THRESHOLD) == 0) \
  { \
    retval = _grail_be_set_float_configuration( \
        subscription_data->ugstore, \
        &(subscription_data->gesture##_threshold), \
        UGSubscriptionProperty##Gesture##Threshold, \
        item_value); \
  }

  GEIS_GRAIL_CHECK_GESTURE_CONFIG(drag, Drag, DRAG)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(pinch, Pinch, PINCH)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(rotate, Rotate, ROTATE)
  else GEIS_GRAIL_CHECK_GESTURE_CONFIG(tap, Tap, TAP)

  #undef GEIS_GRAIL_CHECK_GESTURE_CONFIG

  return retval;
}

static GeisStatus
_grail_be_activate_device(GeisBackend be, GeisDevice device)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  GeisGrailBackend gbe = (GeisGrailBackend)be;
  _grail_be_subscribe_new_device(gbe, device);
  return status;
}


static GeisStatus
_grail_be_deactivate_device(GeisBackend be, GeisDevice device)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  GeisGrailBackend gbe = (GeisGrailBackend)be;
  UFDevice ufdevice = _grail_be_ufdevice_from_device_id(gbe,
                                                geis_device_id(device));
  _grail_be_unsubscribe_removed_device(gbe, ufdevice);
  return status;
}


static struct GeisBackendVtable gbe_vtbl = {
  _geis_grail_backend_construct,
  _geis_grail_backend_finalize,
  geis_grail_token_new,
  _grail_be_accept_gesture,
  _grail_be_reject_gesture,
  _grail_be_activate_device,
  _grail_be_deactivate_device,
  _grail_be_get_configuration,
  _grail_be_set_configuration
};

static GeisStatus
_geis_grail_filter_gestures(GeisGrailBackend gbe,
                            GeisFilter       filter,
                            GeisSubscription sub,
                            UGSubscription   ugsub)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisSubscriptionFlags sub_flags = geis_subscription_flags(sub);

  /* default mask is ALL gesture (for now) */
  UGGestureTypeMask default_ugmask = UGGestureTypeDrag
                                   | UGGestureTypePinch
                                   | UGGestureTypeRotate
                                   | UGGestureTypeTap
                                   | UGGestureTypeTouch;
  UGGestureTypeMask ugmask = 0;
  unsigned int start_touches = 1;
  unsigned int min_touches = 1;
  unsigned int max_touches = 5;

  for (GeisSize i = 0; filter && i < geis_filter_term_count(filter); ++i)
  {
    GeisFilterTerm term = geis_filter_term(filter, i);
    if (GEIS_FILTER_CLASS == geis_filter_term_facility(term))
    {
      GeisAttr            attr      = geis_filter_term_attr(term);
      GeisString          name      = geis_attr_name(attr);
      GeisFilterOperation operation = geis_filter_term_operation(term);

      if (0 == strcmp(name, GEIS_CLASS_ATTRIBUTE_NAME)
          && operation == GEIS_FILTER_OP_EQ)
      {
        GeisString class_name = geis_attr_value_to_string(attr);
        if (0 == strcmp(class_name, geis_gesture_class_name(gbe->drag_class)))
          ugmask |= UGGestureTypeDrag;
        else if (0 == strcmp(class_name, geis_gesture_class_name(gbe->pinch_class)))
          ugmask |= UGGestureTypePinch;
        else if (0 == strcmp(class_name, geis_gesture_class_name(gbe->rotate_class)))
          ugmask |= UGGestureTypeRotate;
        else if (0 == strcmp(class_name, geis_gesture_class_name(gbe->tap_class)))
          ugmask |= UGGestureTypeTap;
        else if (0 == strcmp(class_name, geis_gesture_class_name(gbe->touch_class)))
          ugmask |= UGGestureTypeTouch;
      }
      else if (0 == strcmp(name, GEIS_CLASS_ATTRIBUTE_ID)
          && operation == GEIS_FILTER_OP_EQ)
      {
        GeisInteger class_id = geis_attr_value_to_integer(attr);
        if (class_id == geis_gesture_class_id(gbe->drag_class))
          ugmask |= UGGestureTypeDrag;
        else if (class_id == geis_gesture_class_id(gbe->pinch_class))
          ugmask |= UGGestureTypePinch;
        else if (class_id == geis_gesture_class_id(gbe->rotate_class))
          ugmask |= UGGestureTypeRotate;
        else if (class_id == geis_gesture_class_id(gbe->tap_class))
          ugmask |= UGGestureTypeTap;
        else if (class_id == geis_gesture_class_id(gbe->touch_class))
          ugmask |= UGGestureTypeTouch;
      }
      else if (0 == strcmp(name, GEIS_GESTURE_ATTRIBUTE_TOUCHES))
      {
        /*
         * Starting touches: filter terms are ANDed: multiple TOUCHES
         * terms are ANDed using the MAX operator.
         */
        unsigned int val = geis_attr_value_to_integer(attr);
        switch (operation)
        {
          case GEIS_FILTER_OP_EQ:
            start_touches = _max(start_touches, val);
            min_touches = _max(min_touches, start_touches);
            max_touches = _min(max_touches, start_touches);
            break;
          case GEIS_FILTER_OP_NE:
            geis_error("unsupported comparison");
            break;
          case GEIS_FILTER_OP_GE:
            start_touches = _max(start_touches, val);
            min_touches = _max(min_touches, start_touches);
            max_touches = _min(max_touches, 5);
            break;
          case GEIS_FILTER_OP_GT:
            start_touches = _max(start_touches, val+1);
            min_touches = _max(min_touches, start_touches);
            max_touches = _min(max_touches, 5);
            break;
          case GEIS_FILTER_OP_LE:
            start_touches = _max(start_touches, val);
            min_touches = _max(min_touches, 0);
            max_touches = _min(max_touches, start_touches);
            break;
          case GEIS_FILTER_OP_LT:
            start_touches = _max(start_touches, val-1);
            min_touches = _max(min_touches, 0);
            max_touches = _min(max_touches, start_touches);
            break;
        }
      }
    }
  }

  if (!ugmask)
    ugmask = default_ugmask;

  /* A tap gesture for a non-tentative client only sends one update event when
   * the tap finishes. The Recognition occurs at the very end of the gesture,
   * and the last grail slice is transformed into the update event. If the min
   * touches allows for down to one touch, the end slice will probably only have
   * one touch. As a work around, only allow for continuous touch gestures for
   * non-tap, non-tentative subscriptions.
   *
   * This puts the onus on the subscriber not to mix tap and non-tap continuous
   * touch gestures. In practice, this would likely only occur for geis v1
   * clients, but geis v1 subscriptions are already split into separate backend
   * subscriptions, so this shouldn't be a problem.
   */
  if (sub_flags & GEIS_SUBSCRIPTION_CONT && !gbe->send_tentative_events &&
      !(ugmask & UGGestureTypeTap))
  {
    min_touches = 1;
    max_touches = start_touches;
  }

  geis_debug("mask=0x%08x start=%u min=%u max=%u",
             ugmask, start_touches, min_touches, max_touches);

  UGStatus ugstatus;
  ugstatus = grail_subscription_set_property(ugsub,
                                             UGSubscriptionPropertyMask,
                                             &ugmask);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription mask");
    goto final_exit;
  }

  ugstatus = grail_subscription_set_property(ugsub,
                                             UGSubscriptionPropertyTouchesStart,
                                             &start_touches);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription start touches");
    goto final_exit;
  }

  ugstatus = grail_subscription_set_property(ugsub,
                                             UGSubscriptionPropertyTouchesMinimum,
                                             &min_touches);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription min touches");
    goto final_exit;
  }

  ugstatus = grail_subscription_set_property(ugsub,
                                             UGSubscriptionPropertyTouchesMaximum,
                                             &max_touches);
  if (ugstatus != UGStatusSuccess)
  {
    geis_error("failed to set UGSubscription max touches");
    goto final_exit;
  }

  status = GEIS_STATUS_SUCCESS;

final_exit:
  return status;
}


static void
_geis_grail_set_ugsubscription_properties(GeisGrailBackend gbe,
                                          UGSubscription ugsub,
                                          GeisSubscription subscription)
{
  struct GeisGrailSubscriptionData *subscription_data = geis_subscription_pdata(subscription);

  GeisBoolean geis_use_atomic_gestures = GEIS_FALSE;
  geis_get_configuration(gbe->geis,
                         GEIS_CONFIG_ATOMIC_GESTURES,
                         &geis_use_atomic_gestures);
  int grail_use_atomic_gestures = (geis_use_atomic_gestures == GEIS_TRUE);
  grail_subscription_set_property(ugsub,
                                  UGSubscriptionPropertyAtomicGestures,
                                  &grail_use_atomic_gestures);


  #define GEIS_GRAIL_SYNC_GESTURE_PROPERTIES(gesture, Gesture) \
  if (subscription_data->gesture##_timeout) \
  { \
    grail_subscription_set_property(ugsub, \
                                    UGSubscriptionProperty##Gesture##Timeout, \
                                    subscription_data->gesture##_timeout); \
  } \
  if (subscription_data->gesture##_threshold) \
  { \
    grail_subscription_set_property(ugsub, \
                                    UGSubscriptionProperty##Gesture##Threshold, \
                                    subscription_data->gesture##_threshold); \
  }

  GEIS_GRAIL_SYNC_GESTURE_PROPERTIES(drag, Drag);
  GEIS_GRAIL_SYNC_GESTURE_PROPERTIES(pinch, Pinch);
  GEIS_GRAIL_SYNC_GESTURE_PROPERTIES(rotate, Rotate);
  GEIS_GRAIL_SYNC_GESTURE_PROPERTIES(tap, Tap);

  #undef GEIS_GRAIL_SYNC_GESTURE_PROPERTIES
}

/**
 * Activates a subscription for a (device, region).
 */
static GeisStatus
_geis_grail_activate_for_device_region(GeisGrailBackend gbe,
                                       GeisFilter       filter,
                                       GeisDevice       device,
                                       GeisInteger      window_id,
                                       GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisInteger device_id = geis_device_id(device);
  UFDevice ufdevice = _grail_be_ufdevice_from_device_id(gbe, device_id);
  struct GeisGrailSubscriptionData *subscription_data = geis_subscription_pdata(subscription);
  GeisUGSubscriptionStore ugstore = subscription_data->ugstore;
  UGSubscription ugsub = geis_ugsubscription_get_ugsubscription(ugstore,
                                                                filter,
                                                                ufdevice,
                                                                window_id);

  if (!ugsub)
  {
    ugsub = geis_ugsubscription_create_ugsubscription(ugstore,
                                                      filter,
                                                      ufdevice,
                                                      window_id);
    _geis_grail_set_ugsubscription_properties(gbe, ugsub, subscription);
  }

  if (!ugsub)
  {
    geis_error("can not retrieve UGSubscription for (device, window)");
    goto final_exit;
  }

  status = _geis_grail_filter_gestures(gbe, filter, subscription, ugsub);

  if (filter)
    geis_debug("subscription='%s' filter='%s' device=%d '%s' window=0x%08x "
               "ugsub=%p",
               geis_subscription_name(subscription),
               geis_filter_name(filter),
               device_id, geis_device_name(device),
               window_id,
               (void *)ugsub);
  else
    geis_debug("subscription='%s' no-filter device=%d '%s' window=0x%08x "
               "ugsub=%p",
               geis_subscription_name(subscription),
               device_id, geis_device_name(device),
               window_id,
               (void *)ugsub);

  status = geis_grail_window_grab_store_grab(gbe->window_grabs, window_id);
  if (status != GEIS_STATUS_SUCCESS)
  {
    geis_error("failed to grab input on window 0x%08x", window_id);
    goto final_exit;
  }

  UGStatus ugstatus = grail_subscription_activate(gbe->grail, ugsub);
  if (ugstatus != UGStatusSuccess)
  {
    status = GEIS_STATUS_UNKNOWN_ERROR;
    geis_error("failed to activate UGSubscription");
  }

final_exit:
  return status;
}


static GeisStatus
_grail_be_activate_for_device(GeisGrailBackend gbe,
                              GeisFilter       filter,
                              GeisDevice       device,
                              GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisBoolean has_region_term = GEIS_FALSE;
  if (filter)
  {
    for (GeisSize i = 0; i < geis_filter_term_count(filter); ++i)
    {
      GeisFilterTerm term = geis_filter_term(filter, i);
      if (GEIS_FILTER_REGION == geis_filter_term_facility(term))
      {
        GeisInteger         window_id = 0;
        GeisAttr            attr      = geis_filter_term_attr(term);
        GeisString          name      = geis_attr_name(attr);
        GeisFilterOperation operation = geis_filter_term_operation(term);

        if (0 == strcmp(name, GEIS_REGION_ATTRIBUTE_WINDOWID)
            && operation == GEIS_FILTER_OP_EQ)
        {
          window_id = geis_attr_value_to_integer(attr);
          has_region_term = GEIS_TRUE;
          status = _geis_grail_activate_for_device_region(gbe,
              filter,
              device,
              window_id,
              subscription);
          if (status != GEIS_STATUS_SUCCESS)
          {
            goto final_exit;
          }
        }
        else
        {
          geis_warning("unhandled region filter term");
        }
      }
    }

    /* If no region terms were found, or there's no filter, use the root
       window. */
    if (!has_region_term)
    {
      status = _geis_grail_activate_for_device_region(gbe,
                                                      filter,
                                                      device,
                                                      gbe->root_window,
                                                      subscription);
    }
  }

final_exit:
  return status;
}


/**
 * Activates a subscription for a list of devices.
 */
static GeisStatus
_geis_grail_activate_for_devices(GeisGrailBackend gbe,
                                 GeisFilter       filter,
                                 GeisDeviceBag    device_bag,
                                 GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  for (GeisSize d = 0; d < geis_device_bag_count(device_bag); ++d)
  {
    GeisDevice device = geis_device_bag_device(device_bag, d);
    status = _grail_be_activate_for_device(gbe, filter, device, subscription);
    if (status != GEIS_STATUS_SUCCESS)
      break;
  }

  return status;
}

static GeisStatus
_geis_grail_activate_with_filters(GeisGrailBackend gbe,
                                  GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;

  for (GeisFilterIterator it = geis_subscription_filter_begin(subscription);
       it != geis_subscription_filter_end(subscription);
       it = geis_subscription_filter_next(subscription, it))
  {
    GeisFilter filter = *it;
    GeisDeviceBag selected_devices = geis_device_bag_new();
    GeisSelectResult device_matches = geis_select_devices(gbe->geis,
                                                          filter,
                                                          selected_devices);
    if (device_matches == GEIS_SELECT_RESULT_ALL)
    {
      geis_debug("filter %p '%s' matches ALL devices",
                 (void *)filter,
                 geis_filter_name(filter));

      /* Special case: no devices succeeds, in case one gets added later. */
      if (geis_device_bag_count(geis_devices(gbe->geis)) == 0)
        status = GEIS_STATUS_SUCCESS;
      else
        status = _geis_grail_activate_for_devices(gbe,
                                                  filter,
                                                  geis_devices(gbe->geis),
                                                  subscription);
    }
    else if (device_matches == GEIS_SELECT_RESULT_SOME)
    {
      geis_debug("filter %p '%s' matches %zu devices",
                 (void *)filter,
                 geis_filter_name(filter),
                 geis_device_bag_count(selected_devices));
      status = _geis_grail_activate_for_devices(gbe,
                                                filter,
                                                selected_devices,
                                                subscription);
    }
    else
    {
      geis_debug("filter %p '%s' matches NO devices",
                 (void *)filter,
                 geis_filter_name(filter));
      status = GEIS_STATUS_SUCCESS;
    }
    geis_device_bag_delete(selected_devices);
    if (status != GEIS_STATUS_SUCCESS)
      break;
  }

  return status;
}

/*
 * Activates a GEIS subscription on the back end.
 */
GeisStatus
geis_grail_backend_activate_subscription(GeisGrailBackend gbe,
                                         GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;

  if (geis_subscription_bag_find(gbe->subscription_bag,
                                 geis_subscription_id(subscription)))
  {
    geis_warning("subscription is already activated");
    goto final_exit;
  }

  struct GeisGrailSubscriptionData *subscription_data =
    geis_subscription_pdata(subscription);
  if (!subscription_data)
  {
    subscription_data = calloc(1, sizeof(struct GeisGrailSubscriptionData));
    geis_subscription_set_pdata(subscription, subscription_data);
  }

  if (subscription_data->ugstore == NULL)
  {
    subscription_data->ugstore = geis_ugsubscription_store_new();
    if (!subscription_data->ugstore)
    {
      geis_error("error creating grail subscription store");
      goto final_exit;
    }
  }

  if (geis_subscription_filter_count(subscription) > 0)
  {
    status = _geis_grail_activate_with_filters(gbe, subscription);
  }
  else
  {
    status = _geis_grail_activate_for_devices(gbe,
        NULL, /* no filter */
        geis_devices(gbe->geis),
        subscription);
  }

  geis_subscription_bag_insert(gbe->subscription_bag, subscription);

final_exit:
  return status;
}


/*
 * Deactivates a GEIS subscription on the back end.
 *
 * @todo replace this stub
 */
GeisStatus
geis_grail_backend_deactivate_subscription(GeisGrailBackend gbe,
                                           GeisSubscription subscription)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;

  if (!geis_subscription_bag_find(gbe->subscription_bag,
                                 geis_subscription_id(subscription)))
  {
    geis_warning("deactivating a subscription that is not active");
    goto final_exit;
  }

  struct GeisGrailSubscriptionData *subscription_data =
    geis_subscription_pdata(subscription);
  GeisUGSubscriptionStore ugstore = subscription_data->ugstore;
  if (ugstore)
  {
    for (GeisSize i = 0; i < geis_ugsubscription_count(ugstore); ++i)
    {
      UGSubscription ugsub = geis_ugsubscription_get_ugsubscription_at(ugstore,
                                                                       i);

      UFWindowId ufwindow;
      UGStatus ugstatus;
      ugstatus = grail_subscription_get_property(ugsub,
                                                 UGSubscriptionPropertyWindow,
                                                 &ufwindow);
      if (ugstatus != UGStatusSuccess)
      {
        geis_warning("error %d getting subscription window", ugstatus);
      }
      else
      {
        Window window_id = frame_x11_get_window_id(ufwindow);
        geis_grail_window_grab_store_ungrab(gbe->window_grabs, window_id);
      }

      grail_subscription_deactivate(gbe->grail, ugsub);
      geis_subscription_bag_remove(gbe->subscription_bag, subscription);

      status = GEIS_STATUS_SUCCESS;
    }
  }

final_exit:
  return status;
}

/*
  Frees the memory allocated for the GEIS subscription private data
 */
void
geis_grail_backend_free_subscription_pdata(GeisGrailBackend gbe GEIS_UNUSED,
                                           GeisSubscription subscription)
{
  struct GeisGrailSubscriptionData *subscription_data =
    geis_subscription_pdata(subscription);
  if (!subscription_data)
    return;

  if (subscription_data->ugstore)
    geis_ugsubscription_delete(subscription_data->ugstore);

  free(subscription_data->drag_timeout);
  free(subscription_data->drag_threshold);
  free(subscription_data->pinch_timeout);
  free(subscription_data->pinch_threshold);
  free(subscription_data->rotate_timeout);
  free(subscription_data->rotate_threshold);
  free(subscription_data->tap_timeout);
  free(subscription_data->tap_threshold);

  free(subscription_data);

  geis_subscription_set_pdata(subscription, NULL);
}

/**
 * Registers the back end with the GEIS back end registry.
 */
static void __attribute__((constructor))
_register_grail_backend(void)
{
  geis_register_backend(GEIS_INIT_GRAIL_BACKEND,
                        sizeof(struct GeisGrailBackend),
                        &gbe_vtbl);
}


/** A dummy routine to force linkage of this module without dlopening it */
void
geis_include_grail_backend(void)
{
}

/** @} */
