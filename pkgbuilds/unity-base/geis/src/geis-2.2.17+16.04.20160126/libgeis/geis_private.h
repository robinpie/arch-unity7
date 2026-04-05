/**
 * @file geis_internal.h
 * @brief internal interface of the GEIS API
 *
 * This file is the internal interface to the GEIS API object.  It provides the
 * implementation hooks for plugins (back ends, servers, and extensions) to pass
 * information through the API.
 */

/*
 * Copyright 2010, 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GEIS_PRIVATE_H_
#define GEIS_PRIVATE_H_

#include <geis/geis.h>

#include "geis_error.h"
#include "geis_backend.h"
#include "geis_backend_token.h"
#include "geis_backend_multiplexor.h"
#include "geis_class.h"
#include "geis_device.h"
#include "geis_event_queue.h"
#include "geis_filterable.h"
#include "geis_region.h"
#include "geis_subscription.h"


/**
 * Increases the reference count of an API instance object.
 *
 * @param[in] geis  The API instance.
 *
 * @returns the same API instance.
 */
Geis geis_ref(Geis geis);

/**
 * Decremenets the reference count of an API instance object.
 *
 * @param[in] geis  The API instance.
 *
 * If the reference count on the API instance object drops to zero, the object
 * is destroyed.
 */
void geis_unref(Geis geis);

/**
 * Gets the error stack from the geis object.
 *
 * @param[in] geis  The API instance.
 */
GeisErrorStack *geis_error_stack(Geis geis);

/**
 * Adds a subscription to the API instance.
 *
 * @param[in] geis          The API instance.
 * @param[in] subscription  A GEIS subscirption.
 */
GeisSize geis_add_subscription(Geis geis, GeisSubscription subscription);

/**
 * Adds a back end file descriptor to multiplex.
 *
 * @param[in] geis     The API instance.
 * @param[in] fd       The file descriptor to add.
 * @param[in] activity The file descriptor activity(ies) to monitor.
 * @param[in] callback The file descriptor event callback.
 * @param[in] context  A contextual datum.
 */
void geis_multiplex_fd(Geis                            geis,
                       int                             fd,
                       GeisBackendMultiplexorActivity  activity,
                       GeisBackendFdEventCallback      callback,
                       void                           *context);

/**
 * Modifies a multiplexed back end file descriptor.
 *
 * @param[in] geis     The API instance.
 * @param[in] fd       The file descriptor to add.
 * @param[in] activity The file descriptor activity(ies) to monitor.
 */
void geis_remultiplex_fd(Geis                            geis,
                         int                             fd,
                         GeisBackendMultiplexorActivity  activity);

/**
 * Removes a back end file descriptor from the multiplex.
 *
 * @param[in] geis     The API instance.
 * @param[in] fd       The file descriptor to remove.
 */
void geis_demultiplex_fd(Geis geis, int fd);

/**
 * Posts a new event through the API.
 *
 * @param[in] geis  The API instance.
 * @param[in] event The GEIS event.
 */
void geis_post_event(Geis geis, GeisEvent event);

typedef enum GeisProcessingResult {
  GEIS_PROCESSING_IGNORED       =  0,
  GEIS_PROCESSING_DISPOSE_EVENT = 10,
  GEIS_PROCESSING_COMPLETE      = 20,
  GEIS_PROCESSING_FAIL          = 99
} GeisProcessingResult;

typedef GeisProcessingResult (*GeisProcessingCallback)(GeisEvent  event,
                                                       void      *context);
/**
 * Registers an event-processing callback.
 *
 * @param[in] geis            The API instance.
 * @param[in] priority        The event-handling priority.
 * @param[in] event_callback  The event-handling callback.
 * @param[in] context         The context to supply to the callback.
 */
void geis_register_processing_callback(Geis                    geis,
                                       int                     priority,
                                       GeisProcessingCallback  event_callback,
                                       void                   *context);

/**
 * Removes all matching events from all event queues.
 *
 * @param[in] geis      The API instance.
 * @param[in] matching  A unary predicate function to indicate the events to be
 *                      removed.
 * @param[in] context   An application-specific context value to be passed to
 *                      the matching function.
 *
 * Removes any and all queued events that are indicated by the passed matching
 * function.
 */
void geis_remove_matching_events(Geis            geis,
                                 GeisEventMatch  matching,
                                 void           *context);



/**
 * @defgroup geis_v2_plugin Plugin Interfaces
 * @ingroup geis_v2
 * Interfaces for plugin implementors.
 *
 * The Advanced API provides an extension capability through a defined plugin
 * interface.
 *
 * @{
 */

/**
 * Resgisters a new gesture class with the API.
 *
 * @param[in] geis                  The API instance to which the new gesture
 *                                  class will be added.
 * @param[in] gesture_class         The new gesture class.
 * @param[in] filterable_attr_count The number of filterable attributes in the
 *                                  following structure.
 * @param[in] filterable_attrs      The set of attributes that may be used if
 *                                  filter terms for this class.
 *
 * An extension must register a gesture class if it will be providing new
 * gesture class events.  Any filterable attributes (other than gesture class
 * name, which is mandatory) must be provided.  A filterable attribute for the
 * class name will be automatically added.
 */
void geis_register_gesture_class(Geis                    geis,
                                 GeisGestureClass        gesture_class,
                                 GeisSize                filterable_attr_count,
                                 GeisFilterableAttribute filterable_attrs);

GeisGestureClassBag geis_gesture_classes(Geis geis);

/**
 * Registers a new input device with the API.
 *
 * @param[in] geis                  The API instance to which the new input
 *                                  device will be added.
 * @param[in] device                The new input device.
 * @param[in] filterable_attr_count The number of filterable attributes in the
 *                                  following structure.
 * @param[in] filterable_attrs      The set of attributes that may be used if
 *                                  filter terms for this device.
 */
void geis_register_device(Geis                    geis,
                          GeisDevice              device,
                          GeisSize                filterable_attr_count,
                          GeisFilterableAttribute filterable_attrs);

/**
 * Unregisters an existing input device with the API.
 *
 * @param[in] geis                  The API instance to which the new input
 *                                  device will be added.
 * @param[in] device                The input device to unregister.
 */
void geis_unregister_device(Geis geis, GeisDevice device);

GeisDeviceBag geis_devices(Geis geis);

/**
 * The possible selection results for a filter.
 */
typedef enum GeisSelectResult
{
  GEIS_SELECT_RESULT_NONE,   /**< Nothing was selected. */
  GEIS_SELECT_RESULT_SOME,   /**< A proper subset of data was selected. */
  GEIS_SELECT_RESULT_ALL     /**< All data was selected. */
} GeisSelectResult;

/**
 * Selects devices that pass the given filter terms.
 *
 * @param[in]  geis    The API instance with a list of known devices.
 * @param[in]  filter  The filter used to select devices.
 * @param[out] bag     The bag in which to place the selected devices.
 *
 * @retval GEIS_SELECT_RESULT_NONE  No devices match the filter.  The contents
 *                                  of @p bag remains unchanged.
 * @retval GEIS_SELECT_RESULT_SOME  Some device matches were found and added to
 *                                  @p bag.
 * @retval GEIS_SELECT_RESULT_ALL   All devices match the filetr (there were
 *                                  probably no filter terms in the device
 *                                  facility).  The contents of @p bag remain
 *                                  unchanged.
 */
GeisSelectResult
geis_select_devices(Geis geis, GeisFilter filter, GeisDeviceBag bag);

/**
 * @defgoup geis_v2_region_filters
 * @{
 */
/**
 * Registers a new region (type) with the API.
 *
 * @param[in] geis                  The API instance to which the new region
 *                                  will be added.
 * @param[in] region                The new region.
 * @param[in] filterable_attr_count The number of filterable attributes in the
 *                                  following structure.
 * @param[in] filterable_attrs      The set of attributes that may be used if
 *                                  filter terms for this class.
 */
void geis_register_region(Geis                    geis,
                          GeisRegion              region,
                          GeisSize                filterable_attr_count,
                          GeisFilterableAttribute filterable_attrs);

/**
 * Gets an iterator to the first registered filterable region attribute.
 *
 * @param[in] geis  The API instance.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_begin(Geis geis);

/**
 * Advances an iterator to the next registered filterable region attribute.
 *
 * @param[in] geis  The API instance.
 * @param[in] iter  A filterable attribute iterator.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_next(Geis                           geis,
                              GeisFilterableAttributeBagIter iter);

/**
 * Gets an iterator to the on-past-the-last registered filterable region
 * attribute.
 *
 * @param[in] geis  The API instance.
 *
 * It is an error to dereference the returned iterator.
 */
GeisFilterableAttributeBagIter
geis_region_filter_attrs_end(Geis geis);

/** @} */

/**
 * Resgisters a new special filters with the API.
 *
 * @param[in] geis                  The API instance to which the new region
 *                                  will be added.
 * @param[in] filterable_attr_count The number of filterable attributes in the
 *                                  following structure.
 * @param[in] filterable_attrs      The set of attributes that may be used if
 *                                  filter terms for this class.
 */
void geis_register_special(Geis                    geis,
                           GeisSize                filterable_attr_count,
                           GeisFilterableAttribute filterable_attrs);

/**
 * Invokes the filterable attribute's add_term callback for a named attribute.
 *
 * @param[in] geis     The API instance.
 * @param[in] facility The facility.
 * @param[in] token    The back end filter token.
 * @param[in] name     The filter attribute name.
 * @param[in] op       The filter operation.
 * @param[in] value    The filter value.
 */
GeisStatus geis_filterable_attribute_foreach(Geis                geis,
                                             GeisFilterFacility  facility,
                                             GeisBackendToken    token,
                                             GeisString          name,
                                             GeisFilterOperation op,
                                             void               *value);

/**
 * Gets the type of a named device attr.
 *
 * @param[in]  geis      The API instance.
 * @param[in]  attr_name The name of the device attr.
 *
 * Gets the type of a device attr by name, assuming the attr is known.
 *
 * There is a basic assumption that all device attrs of the same name are of the
 * same type.
 *
 * @returns the type of the attr, GEIS_ATTR_TYPE_UNKNOWN if the attr is unknown.
 */
GeisAttrType geis_get_device_attr_type(Geis geis, GeisString attr_name);

/**
 * Gets the type of a named class attr.
 *
 * @param[in]  geis      The API instance.
 * @param[in]  attr_name The name of the class attr.
 *
 * Gets the type of a class attr by name, assuming the attr is known.
 *
 * There is a basic assumption that all class attrs of the same name are of the
 * same type.
 *
 * @returns the type of the attr, GEIS_ATTR_TYPE_UNKNOWN if the attr is unknown.
 */
GeisAttrType geis_get_class_attr_type(Geis geis, GeisString attr_name);

/**
 * Gets the type of a named region attr.
 *
 * @param[in]  geis      The API instance.
 * @param[in]  attr_name The name of the region attr.
 *
 * Gets the type of a region attr by name, assuming the attr is known.
 *
 * There is a basic assumption that all region attrs of the same name are of the
 * same type.
 *
 * @returns the type of the attr, GEIS_ATTR_TYPE_UNKNOWN if the attr is unknown.
 */
GeisAttrType geis_get_region_attr_type(Geis geis, GeisString attr_name);

/**
 * Gets the type of a named special attr.
 *
 * @param[in]  geis      The API instance.
 * @param[in]  attr_name The name of the special attr.
 *
 * Gets the type of a special attr by name, assuming the attr is known.
 *
 * There is a basic assumption that all special attrs of the same name are of the
 * same type.
 *
 * @returns the type of the attr, GEIS_ATTR_TYPE_UNKNOWN if the attr is unknown.
 */
GeisAttrType geis_get_special_attr_type(Geis geis, GeisString attr_name);

/**
 * Gets a feature configuration value.
 *
 * @param[in]  geis       An opaque GEIS API object.
 * @param[in]  sub        A subscription.  May be NULL for global configuration
 *                        items.
 * @param[in]  item_name  Selects the configuration value to return.
 * @param[out] item_value Points to a buffer to contain the output value.  The
 *                        actual type of this buffer depends on the 
 *                        @p configuration_value_name.
 *
 * @retval GEIS_STATUS_BAD_ARGUMENT   an invalid argument value was passed
 * @retval GEIS_STATUS_NOT_SUPPORTED  the configuration value is not supported
 * @retval GEIS_STATUS_SUCCESS        normal successful completion
 */
GeisStatus geis_get_sub_configuration(Geis              geis, 
                                      GeisSubscription  sub,
                                      GeisString        item_name,
                                      void             *item_value);

/**
 * Sets a feature configuration value.
 *
 * @param[in] geis       An opaque GEIS API object.
 * @param[in] sub        A subscription.  May be NULL for global configuration
 *                       items.
 * @param[in] item_name  Selects the configuration value to return.
 * @param[in] item_value Points to a buffer to contain the output configuration
 *                       value.  The actual type of this buffer depends on the
 *                       @p configuration_value_name.
 *
 * @retval GEIS_STATUS_BAD_ARGUMENT   an invalid argument value was passed
 * @retval GEIS_STATUS_NOT_SUPPORTED  the configuration value is not supported
 * @retval GEIS_STATUS_SUCCESS        normal successful completion
 */
GeisStatus geis_set_sub_configuration(Geis              geis,
                                      GeisSubscription  sub,
                                      GeisString        item_name, 
                                      void             *item_value);


/* @} */

#endif /* GEIS_PRIVATE_H_ */
