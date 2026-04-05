/**
 * @file libgeis/geis.c
 * @brief implementation of the GEIS v2.0 API instance
 *
 * The GEIS API object encapsulates an entire persistent session with the
 * gesture recognition engine.  An application should be able to support
 * multiple simultaneous instances of the GEIS API abject (although it would be
 * foolish to design an application that way).
 */

/*
 * Copyright 2010-2013 Canonical Ltd.
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "geis_config.h"
#include "geis_private.h"

#include <errno.h>
#include "geis_atomic.h"
#include "geis_attr.h"
#include "geis_backend.h"
#include "geis_class.h"
#include "server/geis_dbus_server.h" /* @TODO replace me */
#include "geis_device.h"
#include "geis_backend_multiplexor.h"
#include "geis_error.h"
#include "geis_event.h"
#include "geis_event_queue.h"
#include "geis_gesture_flick.h"
#include "geis_logging.h"
#include "geis_test_api.h"
#include <stdarg.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>


/** Default timeout for synchronous init (in seconds */
#define GEIS_DEFAULT_INIT_TIMEOUT 5

/**
 * States the GEIS instance could be in.
 *
 * @def GEIS_STATE_INITIALIZING
 * The instance has not yet completed its initialization sequence: some
 * functions will not yield valid results.
 *
 * @def GEIS_STATE_RUNNING
 * Initialization has completed, the instance is up and running normally.
 */
typedef enum _GeisState
{
  GEIS_STATE_INITIALIZING,
  GEIS_STATE_RUNNING,
  GEIS_STATE_INIT_FAIL
} _GeisState;


/*
 * An internal structure to track processing callbacks in some order.
 */
typedef struct GeisProcessingEntry *GeisProcessingEntry;

struct GeisProcessingEntry 
{
  GeisProcessingEntry     next;
  int                     priority;
  GeisProcessingCallback  event_callback;
  void                   *context;
};


/*
 * The API instance.
 */
struct _Geis
{
  GeisRefCount            refcount;
  _GeisState              state;
  GeisErrorStack          error_stack;
  GeisSubBag              subscription_bag;
  GeisBackendMultiplexor  backend_multiplexor;
  GeisBackend             backend;
  GeisBoolean             backend_pending;
  GeisBoolean             backend_use_fallback;
  GeisDBusServer          server; /* @TODO: replace me */
  GeisEventQueue          input_event_queue;
  int                     input_event_signal_pipe[2];
  GeisProcessingEntry     processing_callbacks;
  GeisEventQueue          output_event_queue;
  GeisEventCallback       output_event_callback;
  void                   *output_event_callback_context;
  GeisEventCallback       class_event_callback;
  void                   *class_event_callback_context;
  FilterableAttributeBag  class_filterable_attributes;
  GeisGestureClassBag     gesture_classes;
  GeisEventCallback       device_event_callback;
  void                   *device_event_callback_context;
  FilterableAttributeBag  device_filterable_attributes;
  GeisDeviceBag           devices;
  FilterableAttributeBag  region_filterable_attributes;
  FilterableAttributeBag  special_filterable_attributes;
  GeisGestureFlick        flick;
  GeisBoolean             use_synchronous_start;
  GeisBoolean             use_atomic_gestures;
  GeisBoolean             send_tentative_events;
  GeisBoolean             send_synchronous_events;
  GeisBoolean             ignore_device_events;
};


/*
 * The default event callback -- just pushes events on the internal queue
 */
static void
_default_output_event_callback(Geis       geis,
                               GeisEvent  event,
                               void      *context GEIS_UNUSED)
{
  geis_debug("posting output event");
  geis_event_queue_enqueue(geis->output_event_queue, event);
}


/*
 * Handles device events coming in from the back end.
 */
static GeisBoolean
_device_event_handler(Geis geis, GeisEvent event)
{
  GeisBoolean handled = GEIS_FALSE;
  GeisAttr attr = NULL;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  if (!attr)
  {
    geis_warning("invalid device event received from back end.");
    handled = GEIS_TRUE;
    goto final_exit;
  }
  
  GeisDevice device = geis_attr_value_to_pointer(attr);

  GeisEventType event_type = geis_event_type(event);
  if (event_type == GEIS_EVENT_DEVICE_AVAILABLE)
  {
    geis_device_bag_insert(geis->devices, device);
    geis_backend_activate_device(geis->backend, device);
  }

  if (geis->device_event_callback != GEIS_DEFAULT_EVENT_CALLBACK)
  {
    geis->device_event_callback(geis, event, geis->device_event_callback_context);
    handled = GEIS_TRUE;
  }

  if (event_type == GEIS_EVENT_DEVICE_UNAVAILABLE)
  {
    geis_backend_deactivate_device(geis->backend, device);
    geis_device_bag_remove(geis->devices, device);
  }

final_exit:
  return handled;
}


/*
 * Handles class events coming in from the back end.
 */
static GeisBoolean
_class_event_handler(Geis geis, GeisEvent event)
{
  GeisBoolean handled = GEIS_FALSE;
  GeisAttr attr = NULL;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
  if (!attr)
  {
    geis_warning("invalid class event received from back end.");
    handled = GEIS_TRUE;
    goto final_exit;
  }
  
  GeisGestureClass gesture_class = geis_attr_value_to_pointer(attr);

  GeisEventType event_type = geis_event_type(event);
  if (event_type == GEIS_EVENT_CLASS_AVAILABLE)
  {
    geis_gesture_class_bag_insert(geis->gesture_classes, gesture_class);
  }
  else if (event_type == GEIS_EVENT_CLASS_CHANGED)
  {
    /** @todo implement GEIS_EVENT_CLASS_CHANGED */
  }
  else if (event_type == GEIS_EVENT_CLASS_UNAVAILABLE)
  {
    geis_gesture_class_bag_remove(geis->gesture_classes, gesture_class);
  }

  if (geis->class_event_callback != GEIS_DEFAULT_EVENT_CALLBACK)
  {
    geis->class_event_callback(geis, event, geis->class_event_callback_context);
    handled = GEIS_TRUE;
  }

final_exit:
  return handled;
}


static void
_geis_invoke_backend_fallback(Geis geis)
{
  if (geis->backend)
    geis_backend_delete(geis->backend);
  geis_error_clear(geis);

  geis->backend = geis_backend_by_name(geis, GEIS_INIT_GRAIL_BACKEND);
  if (!geis->backend)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("can not create back end");
    geis->state = GEIS_STATE_INIT_FAIL;
  }
  geis->backend_use_fallback = GEIS_FALSE;
}

/*
 * Filters and transforms raw gesture events into cooked gesture events.
 */
static void
_input_event_handler(int                             fd,
                     GeisBackendMultiplexorActivity  activity,
                     void                           *context)
{
  Geis geis = (Geis)context;
  GeisProcessingEntry cb;

  if (activity & GEIS_BE_MX_READ_AVAILABLE)
  {
    GeisEvent event;

    /* clear the input event signal */
    char buf[2];
    if (read(fd, buf, 1) != 1)
    {
      geis_warning("unexpected number of bytes read from signal pipe");
    }

    geis_debug("input event available");
    event = geis_event_queue_dequeue(geis->input_event_queue);
    if (event)
    {
      GeisBoolean handled = 0;
      switch (geis_event_type(event))
      {
	case GEIS_EVENT_DEVICE_AVAILABLE:
	case GEIS_EVENT_DEVICE_UNAVAILABLE:
	  handled = _device_event_handler(geis, event);
	  break;

	case GEIS_EVENT_CLASS_AVAILABLE:
	case GEIS_EVENT_CLASS_CHANGED:
	case GEIS_EVENT_CLASS_UNAVAILABLE:
	  handled = _class_event_handler(geis, event);
	  break;

        case GEIS_EVENT_INIT_COMPLETE:
          geis->state = GEIS_STATE_RUNNING;
          geis->backend_pending = GEIS_FALSE;
          break;

        case GEIS_EVENT_ERROR:
          if (geis->backend_pending && geis->backend_use_fallback)
          {
            _geis_invoke_backend_fallback(geis);
            geis->backend_pending = GEIS_FALSE;
            handled = GEIS_TRUE;
          }
          else
          {
            geis->state = GEIS_STATE_INIT_FAIL;
          }
          break;

	default:
	  break;
      }

      for (cb = geis->processing_callbacks; cb; cb = cb->next)
      {
	switch (cb->event_callback(event, cb->context))
	{
	  case GEIS_PROCESSING_DISPOSE_EVENT:
	    geis_event_delete(event);
	    goto final_exit;
	    break;
	  case GEIS_PROCESSING_COMPLETE:
	    goto processing_complete;
	    break;
	  case GEIS_PROCESSING_FAIL:
	    geis_warning("processig error in event handler");
	    break;
	  default:
	    break;
	}
      }

processing_complete:
      if (!handled)
      {
	geis->output_event_callback(geis,
	                            event,
	                            geis->output_event_callback_context);
      }
    }
  }

final_exit:
  return;
}


/*
 * Applies the back end callback to the back end token for each filterable
 * attribute with a name matching the argument.
 */
static GeisStatus
_filterable_attribute_bag_foreach(FilterableAttributeBag  bag,
                                  GeisBackendToken        token,
                                  GeisString              name,
                                  GeisFilterOperation     op,
                                  void                   *value)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;
  GeisFilterableAttributeBagIter it;
  for (it = geis_filterable_attribute_bag_begin(bag);
       it != geis_filterable_attribute_bag_end(bag);
       it = geis_filterable_attribute_bag_next(bag, it))
  {
    if (0 == strcmp(it->name, name) && it->add_term_callback)
    {
      status = it->add_term_callback(token,
                                     it->add_term_context,
                                     name, op, value);
    }
  }
  return status;
}


/*
 * Runs filters for each regostered token in a facility.
 */
GeisStatus
geis_filterable_attribute_foreach(Geis geis,
                                  GeisFilterFacility   facility,
                                  GeisBackendToken     token,
                                  GeisString           name,
                                  GeisFilterOperation  op,
                                  void                *value)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  switch (facility)
  {
    case GEIS_FILTER_CLASS:
      status = _filterable_attribute_bag_foreach(geis->class_filterable_attributes,
                                                 token, name, op, value);
      break;
    case GEIS_FILTER_DEVICE:
      status = _filterable_attribute_bag_foreach(geis->device_filterable_attributes,
                                                 token, name, op, value);
      break;
    case GEIS_FILTER_REGION:
      status = _filterable_attribute_bag_foreach(geis->region_filterable_attributes,
                                                 token, name, op, value);
      break;
    case GEIS_FILTER_SPECIAL:
      status = _filterable_attribute_bag_foreach(geis->special_filterable_attributes,
                                                 token, name, op, value);
      break;
    default:
      break;
  }
  return status;
}


/**
 * Creates a new empty Geis API instance.
 */
static Geis
geis_new_empty()
{
  geis_error_clear(NULL);
  Geis geis = calloc(1, sizeof(struct _Geis));
  if (!geis)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("calloc failed");
    goto final_exit;
  }

  geis->state = GEIS_STATE_INITIALIZING;

  geis->subscription_bag = geis_subscription_bag_new(1);
  if (!geis->subscription_bag)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of subscroption bag failed");
    free(geis);
    geis = NULL;
    goto unwind_geis;
  }

  geis->backend_multiplexor = geis_backend_multiplexor_new();
  if (!geis->backend_multiplexor)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of back end multiplexor failed");
    goto unwind_subscription_bag;
  }

  geis->input_event_queue = geis_event_queue_new();
  if (!geis->input_event_queue)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of input event queue failed");
    goto unwind_backend_mux;
  }
  if (pipe(geis->input_event_signal_pipe) < 0)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error %d creating input event signal pipe: %s",
               errno, strerror(errno));
    goto unwind_input_queue;
  }
  geis_backend_multiplexor_add_fd(geis->backend_multiplexor,
                                  geis->input_event_signal_pipe[0],
                                  GEIS_BE_MX_READ_AVAILABLE,
                                  _input_event_handler,
                                  geis);

  geis->output_event_queue = geis_event_queue_new();
  if (!geis->output_event_queue)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of output event queue failed");
    goto unwind_input_signal_pipe;
  }
  geis->output_event_callback = _default_output_event_callback;

  geis->class_filterable_attributes = geis_filterable_attribute_bag_new();
  if (!geis->class_filterable_attributes)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of geis gesture class bag failed");
    goto unwind_output_queue;
  }

  geis->gesture_classes = geis_gesture_class_bag_new();
  if (!geis->gesture_classes)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of geis gesture class bag failed");
    goto unwind_class_attrs;
  }
  geis->class_event_callback = _default_output_event_callback;

  geis->device_filterable_attributes = geis_filterable_attribute_bag_new();
  if (!geis->device_filterable_attributes)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of geis device bag failed");
    goto unwind_class_bag;
  }

  geis->devices = geis_device_bag_new();
  if (!geis->devices)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("creation of geis device bag failed");
    goto unwind_device_attrs;
  }
  geis->device_event_callback = _default_output_event_callback;

  geis->region_filterable_attributes = geis_filterable_attribute_bag_new();
  if (!geis->region_filterable_attributes)
  {
    goto unwind_device_bag;
  }

  geis->special_filterable_attributes = geis_filterable_attribute_bag_new();
  if (!geis->special_filterable_attributes)
  {
    goto unwind_region_attrs;
  }

  geis->use_synchronous_start = GEIS_FALSE;
  geis->use_atomic_gestures = GEIS_TRUE;
  geis->send_tentative_events = GEIS_FALSE;
  geis->send_synchronous_events = GEIS_FALSE;
  geis->ignore_device_events = GEIS_FALSE;
  goto final_exit;

unwind_region_attrs:
  geis_filterable_attribute_bag_delete(geis->region_filterable_attributes);
unwind_device_bag:
  geis_device_bag_delete(geis->devices);
unwind_device_attrs:
  geis_filterable_attribute_bag_delete(geis->device_filterable_attributes);
unwind_class_bag:
  geis_gesture_class_bag_delete(geis->gesture_classes);
unwind_class_attrs:
  geis_filterable_attribute_bag_delete(geis->class_filterable_attributes);
unwind_output_queue:
  geis_event_queue_delete(geis->output_event_queue);
unwind_input_signal_pipe:
  close(geis->input_event_signal_pipe[0]);
  close(geis->input_event_signal_pipe[1]);
unwind_input_queue:
  geis_event_queue_delete(geis->input_event_queue);
unwind_backend_mux:
  geis_backend_multiplexor_delete(geis->backend_multiplexor);
unwind_subscription_bag:
  geis_subscription_bag_delete(geis->subscription_bag);
unwind_geis:
  free(geis);
  geis = NULL;

final_exit:
  return geis;
}


typedef enum _BackendType
{
  BACK_END_TYPE_NONE,
  BACK_END_TYPE_MOCK_ENGINE,
  BACK_END_TYPE_DBUS,
  BACK_END_TYPE_GRAIL,
  BACK_END_TYPE_XCB
} BackendType;

/**
 * Sets optional parts of a Geis API instance from a variable argument list.
 */
static GeisBoolean
_set_valist(Geis geis, GeisString init_arg_name, va_list varargs)
{
  GeisBoolean status = GEIS_TRUE;
  BackendType back_end_type = BACK_END_TYPE_NONE;

  while (init_arg_name)
  {
    if (0 == strcmp(init_arg_name, GEIS_INIT_SERVICE_PROVIDER))
    {
      geis->server = geis_dbus_server_new(geis);
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_TRACK_DEVICES))
    {
      /* no longer supported */
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_TRACK_GESTURE_CLASSES))
    {
      /* no longer supported */
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_SYNCHRONOUS_START))
    {
      geis->use_synchronous_start = GEIS_TRUE;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_NO_ATOMIC_GESTURES))
    {
      geis->use_atomic_gestures = GEIS_FALSE;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_SEND_TENTATIVE_EVENTS))
    {
      geis->send_tentative_events = GEIS_TRUE;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_SEND_SYNCHRONOS_EVENTS))
    {
      geis->send_synchronous_events = GEIS_TRUE;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_MOCK_BACKEND))
    {
      if (back_end_type != BACK_END_TYPE_NONE)
      {
	geis_error("multiple back ends requested, only using last request");
      }
      back_end_type = BACK_END_TYPE_MOCK_ENGINE;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_DBUS_BACKEND))
    {
      if (back_end_type != BACK_END_TYPE_NONE)
      {
	geis_error("multiple back ends requested, only using last request");
      }
      back_end_type = BACK_END_TYPE_DBUS;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_GRAIL_BACKEND))
    {
      if (back_end_type != BACK_END_TYPE_NONE)
      {
	geis_error("multiple back ends requested, only using last request");
      }
      back_end_type = BACK_END_TYPE_GRAIL;
    }
    else if (0 == strcmp(init_arg_name, GEIS_INIT_XCB_BACKEND))
    {
      if (back_end_type != BACK_END_TYPE_NONE)
      {
	geis_error("multiple back ends requested, only using last request");
      }
      back_end_type = BACK_END_TYPE_XCB;
    }
    else if(0 == strcmp(init_arg_name, GEIS_CONFIG_DISCARD_DEVICE_MESSAGES))
    {
      geis->ignore_device_events = GEIS_TRUE;
    }

    init_arg_name = va_arg(varargs, GeisString);
  }

  if (back_end_type == BACK_END_TYPE_MOCK_ENGINE)
  {
    geis->backend = geis_backend_by_name(geis, GEIS_INIT_MOCK_BACKEND);
  }
  else if (back_end_type == BACK_END_TYPE_DBUS)
  {
    geis->backend = geis_backend_by_name(geis, GEIS_INIT_DBUS_BACKEND);
    geis->backend_pending = GEIS_TRUE;
  }
  else if (back_end_type == BACK_END_TYPE_GRAIL)
  {
    geis->backend = geis_backend_by_name(geis, GEIS_INIT_GRAIL_BACKEND);
  }
  else if (back_end_type == BACK_END_TYPE_XCB)
  {
    geis->backend = geis_backend_by_name(geis, GEIS_INIT_XCB_BACKEND);
  }
  else
  {
    geis_warning("back end not specified, defaulting to DBus");
    geis->backend = geis_backend_by_name(geis, GEIS_INIT_DBUS_BACKEND);
    geis->backend_pending = GEIS_TRUE;
    geis->backend_use_fallback = GEIS_TRUE;
  }
  if (!geis->backend)
  {
    if (geis->backend_pending & geis->backend_use_fallback)
    {
      _geis_invoke_backend_fallback(geis);
    }
    else
    {
      geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
      geis_error("can not create back end");
      status = GEIS_FALSE;
      geis->state = GEIS_STATE_INIT_FAIL;
    }
  }

  return status;
}


/**
 * Performs a blocking wait for a GEIS_INIT message.
 */
static GeisStatus
_geis_wait_for_init(Geis geis)
{
  geis_debug("waiting for initialization to complete...");
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  int geis_fd = geis_backend_multiplexor_fd(geis->backend_multiplexor);

  while (1)
  {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(geis_fd, &read_fds);
    struct timeval timeout = { GEIS_DEFAULT_INIT_TIMEOUT, 0 };

    int sstat = select(geis_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (sstat < 0)
    {
      geis_error("error %d in select(): %s", errno, strerror(errno));
      break;
    }
    else if (sstat == 0)
    {
      geis_error("failed to get init response");
      break;
    }

    if (FD_ISSET(geis_fd, &read_fds))
    {
      geis_dispatch_events(geis);
      if (geis->state == GEIS_STATE_RUNNING)
      {
        status = GEIS_STATUS_SUCCESS;
        break;
      }
      else if (geis->state == GEIS_STATE_INIT_FAIL)
      {
        break;
      }
    }
  }

  geis_debug("... initialization complete, status=%d", status);
  return status;
}


/**
 * Creates an initialized Geis API instance.
 */
Geis
geis_new(GeisString init_arg_name, ...)
{
  GeisBoolean success = GEIS_FALSE;
  Geis geis = geis_new_empty();
  if (geis)
  {
    geis_ref(geis);

    va_list varargs;
    va_start(varargs, init_arg_name);
    success = _set_valist(geis, init_arg_name, varargs);
    va_end(varargs);
  }

  if (!success)
  {
    geis_error_push(NULL, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("can not initialize GEIS API");
    geis_delete(geis);
    geis = NULL;
    goto final_exit;
  }

  if (geis->use_synchronous_start)
  {
    GeisStatus status = _geis_wait_for_init(geis);
    if (status != GEIS_STATUS_SUCCESS)
    {
      geis_delete(geis);
      geis = NULL;
      goto final_exit;
    }
  }

  geis->flick = geis_gesture_flick_new(geis);

final_exit:
  return geis;
}


static void _geis_destroy(Geis geis)
{
  GeisProcessingEntry cb;

  cb = geis->processing_callbacks;
  while (cb)
  {
    GeisProcessingEntry cb_next = cb->next;
    free(cb);
    cb = cb_next;
  }

  geis_gesture_flick_delete(geis->flick);
  if (geis->backend)
    geis_backend_delete(geis->backend);
  if (geis->server)
    geis_dbus_server_delete(geis->server);
  geis_filterable_attribute_bag_delete(geis->special_filterable_attributes);
  geis_filterable_attribute_bag_delete(geis->region_filterable_attributes);
  geis_device_bag_delete(geis->devices);
  geis_filterable_attribute_bag_delete(geis->device_filterable_attributes);
  geis_gesture_class_bag_delete(geis->gesture_classes);
  geis_filterable_attribute_bag_delete(geis->class_filterable_attributes);
  geis_event_queue_delete(geis->output_event_queue);
  close(geis->input_event_signal_pipe[0]);
  close(geis->input_event_signal_pipe[1]);
  geis_event_queue_delete(geis->input_event_queue);
  geis_backend_multiplexor_delete(geis->backend_multiplexor);
  if (geis->subscription_bag)
    geis_subscription_bag_delete(geis->subscription_bag);
  free(geis);
}

/*
 * Disposes of a Geis API instance.
 */
GeisStatus
geis_delete(Geis geis)
{
  if (geis == NULL)
  {
    return GEIS_STATUS_BAD_ARGUMENT;
  }

  /* break circular dependencies */
  if (geis->subscription_bag)
  {
    geis_subscription_bag_delete(geis->subscription_bag);
    geis->subscription_bag = NULL;
  }
  if (geis->backend)
  {
    geis_backend_delete(geis->backend);
    geis->backend = NULL;
  }

  geis_unref(geis);
  return GEIS_STATUS_SUCCESS;
}

/*
 * Increases the reference count of an API instance object.
 */
Geis
geis_ref(Geis geis)
{
  geis_atomic_ref(&geis->refcount);
  return geis;
}


/*
 * Decremenets the reference count of an API instance object.
 */
void
geis_unref(Geis geis)
{
  if (0 == geis_atomic_unref(&geis->refcount))
  {
    _geis_destroy(geis);
  }
}


/**
 * Gets a named configuration item.
 */
GeisStatus
geis_get_configuration(Geis        geis, 
                       GeisString  configuration_item_name,
                       void       *configuration_item_value)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;

  if (0 == strcmp(configuration_item_name, GEIS_CONFIGURATION_FD))
  {
    *(int*)configuration_item_value
        = geis_backend_multiplexor_fd(geis->backend_multiplexor);
    status = GEIS_STATUS_SUCCESS; 
  }
  else if (0 == strcmp(configuration_item_name, GEIS_CONFIG_MAX_EVENTS))
  {
    *(int*)configuration_item_value
        = geis_backend_multiplexor_max_events_per_pump(geis->backend_multiplexor);
    status = GEIS_STATUS_SUCCESS; 
  }
  else if (0 == strcmp(configuration_item_name, GEIS_CONFIG_ATOMIC_GESTURES))
  {
    *(GeisBoolean*)configuration_item_value = geis->use_atomic_gestures;
    status = GEIS_STATUS_SUCCESS; 
  }

  else if (0 == strcmp(configuration_item_name, GEIS_CONFIG_SEND_TENTATIVE_EVENTS))
  {
    *(GeisBoolean*)configuration_item_value = geis->send_tentative_events;
    status = GEIS_STATUS_SUCCESS; 
  }

  else if (0 == strcmp(configuration_item_name, GEIS_CONFIG_SEND_SYNCHRONOS_EVENTS))
  {
    *(GeisBoolean*)configuration_item_value = geis->send_synchronous_events;
    status = GEIS_STATUS_SUCCESS; 
  }

  else if (0 == strcmp(configuration_item_name, GEIS_CONFIG_DISCARD_DEVICE_MESSAGES))
  {
    *(GeisBoolean*)configuration_item_value = geis->ignore_device_events;
    status = GEIS_STATUS_SUCCESS; 
  }
  else
  {
    status = geis_get_sub_configuration(geis,
                                        NULL,
                                        configuration_item_name,
                                        configuration_item_value);
  }

  return status;
}


/*
 * Gets a feature configuration value from the current back end.
 */
GeisStatus
geis_get_sub_configuration(Geis              geis, 
                           GeisSubscription  sub,
                           GeisString        item_name,
                           void             *item_value)
{
  return geis_backend_get_configuration(geis->backend,
                                        sub,
                                        item_name,
                                        item_value);
}


/*
 * gets a feature configuration value in the current back end.
 */
GeisStatus
geis_set_sub_configuration(Geis              geis,
                           GeisSubscription  sub,
                           GeisString        item_name, 
                           void             *item_value)
{
  return geis_backend_set_configuration(geis->backend,
                                        sub,
                                        item_name,
                                        item_value);
}

/*
 * Sets a named configuration item.
 */
GeisStatus
geis_set_configuration(Geis        geis,
                       GeisString  configuration_item_name,
                       void       *configuration_item_value)
{
  GeisStatus status = GEIS_STATUS_NOT_SUPPORTED;

  if (0 == strcmp(configuration_item_name, GEIS_CONFIG_MAX_EVENTS))
  {
    int max_events = *(int *)configuration_item_value;
    geis_backend_multiplexor_set_max_events_per_pump(geis->backend_multiplexor,
                                                     max_events);
    status = GEIS_STATUS_SUCCESS; 
  }
  else if (0 == strcmp(configuration_item_name,
                       GEIS_CONFIG_DISCARD_DEVICE_MESSAGES))
  {
    geis->ignore_device_events = *(GeisBoolean*)configuration_item_value;
    status = GEIS_STATUS_SUCCESS; 
  }
  else
  {
    status = geis_set_sub_configuration(geis,
                                        NULL,
                                        configuration_item_name,
                                        configuration_item_value);
  }
  return status;
}


/*
 * Registers a callback to receive device change notifications.
 */
void
geis_register_device_callback(Geis               geis,
                              GeisEventCallback  event_callback,
                              void              *context)
{
  geis->device_event_callback = event_callback;
  geis->device_event_callback_context = context;
}


/*
 * Registers a callback to receive gesture class change notifications.
 */
void
geis_register_class_callback(Geis               geis,
                             GeisEventCallback  event_callback,
                             void              *context)
{
  geis->class_event_callback = event_callback;
  geis->class_event_callback_context = context;
}


/*
 * Registers an event-processing callback.
 */
void
geis_register_processing_callback(Geis                    geis,
                                  int                     priority,
                                  GeisProcessingCallback  event_callback,
                                  void                   *context)
{
  GeisProcessingEntry new_entry = calloc(1, sizeof(struct GeisProcessingEntry));
  new_entry->priority = priority;
  new_entry->event_callback = event_callback;
  new_entry->context = context;

  if (!geis->processing_callbacks)
  {
    geis->processing_callbacks = new_entry;
  }
  else
  {
    GeisProcessingEntry cb = NULL;
    GeisProcessingEntry prev = NULL;
    for (cb = geis->processing_callbacks; cb; cb = cb->next)
    {
      if (priority < cb->priority)
      {
	if (cb == geis->processing_callbacks)
	{
	  geis->processing_callbacks = new_entry;
	}
	else
	{
	  new_entry->next = prev->next;
	  prev->next = new_entry;
	}
	break;
      }
      prev = cb;
    }
    if (!cb)
    {
      prev->next = new_entry;
    }
  }
}


/*
 * Removes all matching events from all event queues.
 */
void
geis_remove_matching_events(Geis            geis,
                            GeisEventMatch  matching,
                            void           *context)
{
  geis_event_queue_remove_if(geis->input_event_queue, matching, context);
  geis_event_queue_remove_if(geis->output_event_queue, matching, context);
}


/*
 * Registers an application-supplied event callback.
 */
void 
geis_register_event_callback(Geis               geis,
                             GeisEventCallback  output_event_callback,
                             void              *context)
{
  if (output_event_callback == GEIS_DEFAULT_EVENT_CALLBACK)
  {
    geis->output_event_callback = _default_output_event_callback;
  }
  else
  {
    geis->output_event_callback = output_event_callback;
  }
  geis->output_event_callback_context = context;
}


/*
 * Pumps the GEIS v2 event loop.
 */
GeisStatus
geis_dispatch_events(Geis geis)
{
  GeisStatus status = geis_backend_multiplexor_pump(geis->backend_multiplexor);
  return status;
}


/*
 * Posts an event through the API.
 *
 * Pushes the new event onto the input event queue and signals that a new event
 * has arrived.
 */
void
geis_post_event(Geis geis, GeisEvent event)
{
  geis_event_queue_enqueue(geis->input_event_queue, event);
  if (write(geis->input_event_signal_pipe[1], "1", 1) != 1)
  {
    geis_error("error %d writing input event signal: %s", errno, strerror(errno));
  }
}


/*
 * Pulls the next event off the queue.
 */
GeisStatus
geis_next_event(Geis geis, GeisEvent *event)
{
  GeisStatus status = GEIS_STATUS_EMPTY;
  *event = geis_event_queue_dequeue(geis->output_event_queue);
  if (*event)
  {
    status = geis_event_queue_is_empty(geis->output_event_queue)
           ? GEIS_STATUS_SUCCESS
           : GEIS_STATUS_CONTINUE;
  }

  return status;
}


/*
 * Adds a back end file descriptor to multiplex.
 */
void
geis_multiplex_fd(Geis                            geis,
                  int                             fd,
                  GeisBackendMultiplexorActivity  activity,
                  GeisBackendFdEventCallback      callback,
                  void                           *context)
{
  geis_backend_multiplexor_add_fd(geis->backend_multiplexor,
                                  fd,
                                  activity,
                                  callback,
                                  context);
}


/*
 * Modifies a multiplexed back end file descriptor.
 */
void
geis_remultiplex_fd(Geis                            geis,
                    int                             fd,
                    GeisBackendMultiplexorActivity  activity)
{
  geis_backend_multiplexor_modify_fd(geis->backend_multiplexor,
                                     fd,
                                     activity);
}


/*
 * Removes a back end file descriptor from the multiplex.
 */
void
geis_demultiplex_fd(Geis geis, int fd)
{
  geis_backend_multiplexor_remove_fd(geis->backend_multiplexor, fd);
}


GeisErrorStack *
geis_error_stack(Geis geis)
{
  return &geis->error_stack;
}


GeisSize
geis_add_subscription(Geis geis, GeisSubscription subscription)
{
  return geis_subscription_bag_insert(geis->subscription_bag, subscription);
}


/*
 * Creates a new backend token.
 */
GeisBackendToken
geis_backend_token_new(Geis geis, GeisBackendTokenInitState init_state)
{
  return geis_backend_create_token(geis->backend, init_state);
}


/*
 * Resgisters a new gesture class with the API.
 */
void
geis_register_gesture_class(Geis                    geis,
                            GeisGestureClass        gesture_class,
                            GeisSize                filterable_attr_count,
                            GeisFilterableAttribute filterable_attrs)
{
  GeisAttr attr = NULL;
  GeisEvent event = NULL;
  GeisSize i;

  /* Add the filterable attributes. */
  for (i = 0; i < filterable_attr_count; ++i)
  {
    geis_filterable_attribute_bag_insert(geis->class_filterable_attributes,
                                         &filterable_attrs[i]);
  }

  geis_debug("registering class \"%s\" id %d",
             geis_gesture_class_name(gesture_class),
             geis_gesture_class_id(gesture_class));

  /* Add a "new gesture class" event to the processing queue. */
  event = geis_event_new(GEIS_EVENT_CLASS_AVAILABLE);
  attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_CLASS,
                       GEIS_ATTR_TYPE_POINTER,
                       gesture_class);
  geis_event_add_attr(event, attr);
  geis_post_event(geis, event);
}


GeisGestureClassBag
geis_gesture_classes(Geis geis)
{
  return geis->gesture_classes;
}


/*
 * Registers a new input device with the API.
 */
void
geis_register_device(Geis                    geis,
                     GeisDevice              device,
                     GeisSize                filterable_attr_count,
                     GeisFilterableAttribute filterable_attrs)
{
  /* Add the filterable attributes. */
  for (GeisSize i = 0; i < filterable_attr_count; ++i)
  {
    geis_filterable_attribute_bag_insert(geis->device_filterable_attributes,
                                     &filterable_attrs[i]);
  }

  if (device)
  {
    geis_debug("registering device \"%s\" id %d",
               geis_device_name(device),
               geis_device_id(device));

    /* Add a "new device" event to the processing queue. */
    GeisEvent event = geis_event_new(GEIS_EVENT_DEVICE_AVAILABLE);
    GeisAttr attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_DEVICE,
                                  GEIS_ATTR_TYPE_POINTER,
                                  device);
    geis_device_ref(device);
    geis_attr_set_destructor(attr, (GeisAttrDestructor)geis_device_unref);

    geis_event_add_attr(event, attr);
    geis_post_event(geis, event);
  }
}

/*
 * Unregisters an existing input device with the API.
 */
void
geis_unregister_device(Geis geis, GeisDevice device)
{
  GeisEvent event = NULL;
  GeisAttr attr = NULL;

  /* Add a "remove device" event to the processing queue. */
  event = geis_event_new(GEIS_EVENT_DEVICE_UNAVAILABLE);
  attr = geis_attr_new(GEIS_EVENT_ATTRIBUTE_DEVICE,
                       GEIS_ATTR_TYPE_POINTER,
                       device);

  geis_device_ref(device);
  geis_attr_set_destructor(attr, (GeisAttrDestructor)geis_device_unref);

  geis_event_add_attr(event, attr);
  geis_post_event(geis, event);
}

GeisDeviceBag
geis_devices(Geis geis)
{
  return geis->devices;
}


/**
 * A check to see if a device has the given name.
 */
static GeisBoolean
_device_has_name(GeisDevice          device,
                 GeisFilterOperation op,
                 GeisString          name)
{
  GeisBoolean names_match = (0 == strcmp(geis_device_name(device), name));
  return (op == GEIS_FILTER_OP_EQ && names_match)
      || (op == GEIS_FILTER_OP_NE && !names_match);
}


/**
 * A check to see if a device has the named boolean attribute.
 */
static GeisBoolean
_device_has_boolean_attr(GeisDevice          device,
                         GeisFilterOperation op,
                         GeisString          attr_name,
                         GeisBoolean         match_value)
{
  GeisBoolean has_attr = GEIS_FALSE;
  GeisAttr device_attr = geis_device_attr_by_name(device, attr_name);
  if (device_attr)
  {
    GeisBoolean attr_value = geis_attr_value_to_boolean(device_attr);
    has_attr = (op == GEIS_FILTER_OP_EQ && attr_value == match_value)
            || (op == GEIS_FILTER_OP_NE && attr_value != match_value);
  }
  return has_attr;
}


/**
 * A check to see if a device has the named integer attribute.
 */
static GeisBoolean
_device_has_integer_attr(GeisDevice          device,
                         GeisFilterOperation op,
                         GeisString          attr_name,
                         GeisInteger         match_value)
{
  GeisBoolean has_attr = GEIS_FALSE;
  GeisAttr device_attr = geis_device_attr_by_name(device, attr_name);
  if (device_attr)
  {
    GeisInteger attr_value = geis_attr_value_to_integer(device_attr);
    has_attr = (op == GEIS_FILTER_OP_EQ && attr_value == match_value)
            || (op == GEIS_FILTER_OP_NE && attr_value != match_value)
            || (op == GEIS_FILTER_OP_GT && attr_value > match_value)
            || (op == GEIS_FILTER_OP_GE && attr_value >= match_value)
            || (op == GEIS_FILTER_OP_LE && attr_value <= match_value)
            || (op == GEIS_FILTER_OP_LT && attr_value < match_value);
  }
  return has_attr;
}


/*
 * Selects devices that pass the given filter terms.
 *
 * If there are no device terms in the filter at all, then ALL devices match.
 *
 * If there are any device terms in the filter but no known devices match, then
 * NONE devices match.
 *
 * Otherwise, there is at least one device that matches and at least one device
 * filter term, so SOME devices match.
 */
GeisSelectResult
geis_select_devices(Geis geis, GeisFilter filter, GeisDeviceBag bag)
{
  GeisSelectResult result = GEIS_SELECT_RESULT_ALL;
  GeisBoolean device_terms_present = GEIS_FALSE;
  GeisBoolean devices_found = GEIS_FALSE;
  GeisSize    device_count = geis_device_bag_count(geis->devices);

  const GeisSize term_count = geis_filter_term_count(filter);
  for (GeisSize i = 0; i < term_count; ++i)
  {
    GeisFilterTerm term = geis_filter_term(filter, i);
    if (geis_filter_term_facility(term) == GEIS_FILTER_DEVICE)
    {
      device_terms_present = GEIS_TRUE;

      GeisAttr            attr         = geis_filter_term_attr(term);
      GeisString          name         = geis_attr_name(attr);
      GeisFilterOperation operation    = geis_filter_term_operation(term);

      if (0 == strcmp(name, GEIS_DEVICE_ATTRIBUTE_NAME))
      {
        GeisString device_name = geis_attr_value_to_string(attr);
        for (GeisSize i = 0; i < device_count; ++i)
        {
          GeisDevice device =  geis_device_bag_device(geis->devices, i);
          if (_device_has_name(device, operation, device_name))
          {
            devices_found = GEIS_TRUE;
            geis_device_bag_insert(bag, device);
          }
        }
      }
      else if (0 == strcmp(name, GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH)
            || 0 == strcmp(name, GEIS_DEVICE_ATTRIBUTE_INDEPENDENT_TOUCH))
      {
        GeisBoolean value = geis_attr_value_to_boolean(attr);
        for (GeisSize i = 0; i < device_count; ++i)
        {
          GeisDevice device =  geis_device_bag_device(geis->devices, i);
          if (_device_has_boolean_attr(device, operation, name, value))
          {
            devices_found = GEIS_TRUE;
            geis_device_bag_insert(bag, device);
          }
        }
      }
      else if (0 == strcmp(name, GEIS_DEVICE_ATTRIBUTE_ID)
            || 0 == strcmp(name, GEIS_DEVICE_ATTRIBUTE_TOUCHES))
      {
        GeisInteger value = geis_attr_value_to_integer(attr);
        /* legacy special case for device ID == 0:  means ALL devices in v1 */
        if (value == 0)
        {
          device_terms_present = FALSE;
          break;
        }
        for (GeisSize i = 0; i < device_count; ++i)
        {
          GeisDevice device =  geis_device_bag_device(geis->devices, i);
          if (_device_has_integer_attr(device, operation, name, value))
          {
            devices_found = GEIS_TRUE;
            geis_device_bag_insert(bag, device);
          }
        }
      }
    }
  }

  if (device_terms_present)
  {
    if (devices_found)
    {
      result = GEIS_SELECT_RESULT_SOME;
    }
    else
    {
      result = GEIS_SELECT_RESULT_NONE;
    }
  }
  return result;
}


/*
 * Resgisters a new region with the API.
 */
void
geis_register_region(Geis                    geis,
                     GeisRegion              region GEIS_UNUSED,
                     GeisSize                filterable_attr_count,
                     GeisFilterableAttribute filterable_attrs)
{
  GeisSize i;

  geis_debug("registering region");

  /* Add the filterable attributes. */
  for (i = 0; i < filterable_attr_count; ++i)
  {
    geis_filterable_attribute_bag_insert(geis->region_filterable_attributes,
                                         &filterable_attrs[i]);
  }
}


/*
 * Gets an iterator to the first registered filterable region attribute.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_begin(Geis geis)
{
  return geis_filterable_attribute_bag_begin(geis->region_filterable_attributes);
}


/*
 * Advances an iterator to the next registered filterable region attribute.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_next(Geis                           geis,
                              GeisFilterableAttributeBagIter iter)
{
  return geis_filterable_attribute_bag_next(geis->region_filterable_attributes,
                                            iter);
}


/*
 * Gets an iterator to the on-past-the-last registered filterable region
 * attribute.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_end(Geis geis)
{
  return geis_filterable_attribute_bag_end(geis->region_filterable_attributes);
}


/*
 * Resgisters a new special filter with the API.
 */
void
geis_register_special(Geis                    geis,
                      GeisSize                filterable_attr_count,
                      GeisFilterableAttribute filterable_attrs)
{
  GeisSize i;

  geis_debug("registering feature");

  /* Add the filterable attributes. */
  for (i = 0; i < filterable_attr_count; ++i)
  {
    geis_filterable_attribute_bag_insert(geis->special_filterable_attributes,
                                         &filterable_attrs[i]);
  }
}


GeisAttrType
geis_get_device_attr_type(Geis geis, GeisString attr_name)
{
  FilterableAttributeBag bag = geis->device_filterable_attributes;
  GeisFilterableAttributeBagIter fa;
  for (fa = geis_filterable_attribute_bag_begin(bag);
       fa != geis_filterable_attribute_bag_end(bag);
       fa = geis_filterable_attribute_bag_next(bag, fa))
  {
    if (0 == strcmp(attr_name, fa->name))
    {
      return fa->type;
    }
  }
  return GEIS_ATTR_TYPE_UNKNOWN;
}


GeisAttrType
geis_get_class_attr_type(Geis geis, GeisString attr_name)
{
  FilterableAttributeBag bag = geis->class_filterable_attributes;
  GeisFilterableAttributeBagIter fa;
  for (fa = geis_filterable_attribute_bag_begin(bag);
       fa != geis_filterable_attribute_bag_end(bag);
       fa = geis_filterable_attribute_bag_next(bag, fa))
  {
    if (0 == strcmp(attr_name, fa->name))
    {
      return fa->type;
    }
  }
  return GEIS_ATTR_TYPE_UNKNOWN;
}


GeisAttrType
geis_get_region_attr_type(Geis geis, GeisString attr_name)
{
  FilterableAttributeBag bag = geis->region_filterable_attributes;
  GeisFilterableAttributeBagIter fa;
  for (fa = geis_filterable_attribute_bag_begin(bag);
       fa != geis_filterable_attribute_bag_end(bag);
       fa = geis_filterable_attribute_bag_next(bag, fa))
  {
    if (0 == strcmp(attr_name, fa->name))
    {
      return fa->type;
    }
  }
  return GEIS_ATTR_TYPE_UNKNOWN;
}



GeisAttrType
geis_get_special_attr_type(Geis geis, GeisString attr_name)
{
  FilterableAttributeBag bag = geis->special_filterable_attributes;
  GeisFilterableAttributeBagIter fa;
  for (fa = geis_filterable_attribute_bag_begin(bag);
       fa != geis_filterable_attribute_bag_end(bag);
       fa = geis_filterable_attribute_bag_next(bag, fa))
  {
    if (0 == strcmp(attr_name, fa->name))
    {
      return fa->type;
    }
  }
  return GEIS_ATTR_TYPE_UNKNOWN;
}

GeisDevice
geis_get_device(Geis geis, GeisInteger device_id)
{
  GeisDevice device = NULL;
  for (GeisSize i = 0; i < geis_device_bag_count(geis->devices); ++i)
  {
    GeisDevice d = geis_device_bag_device(geis->devices, i);
    if (geis_device_id(d) == device_id)
    {
      device = d;
      break;
    }
  }
  return device;
}


/*
 * Marks a gesture as accepted.
 */
GeisStatus
geis_gesture_accept(Geis geis, GeisGroup group, GeisGestureId gesture_id)
{
  return geis_backend_gesture_accept(geis->backend, group, gesture_id);
}


/*
 * Marks a gesture as rejected.
 */
GeisStatus
geis_gesture_reject(Geis geis, GeisGroup group, GeisGestureId gesture_id)
{
  return geis_backend_gesture_reject(geis->backend, group, gesture_id);
}

