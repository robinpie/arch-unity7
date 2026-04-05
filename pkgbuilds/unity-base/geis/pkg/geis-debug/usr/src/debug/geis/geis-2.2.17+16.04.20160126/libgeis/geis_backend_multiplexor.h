/**
 * @file geis_backend_multiplexor.h
 * @brief internal GEIS backend multiplexor interface
 *
 * Copyright 2010 Canonical Ltd.
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef GEIS_BACKEND_MULTIPLEXOR_H_
#define GEIS_BACKEND_MULTIPLEXOR_H_

#include "geis/geis.h"


/**
 * Multiplexes back end events into a single notification file descriptor.
 *
 * The GEIS API presents a single file descriptor to the application to watch
 * for activity notification.  A back end may be monitoring activity on more
 * than one file descriptor.  The multiplexor combines these requirements.
 *
 * Activities includes read-available, write-available, hangup-detected, and
 * error-detected.  Only the read- and write-available actvities may be
 * subscribed:  the other activities are always listened for and reported.
 */
typedef struct _GeisBackendMultiplexor *GeisBackendMultiplexor;

/**
 * Indicates the type of activiy(ies) that occurred on a multiplexed descriptor.
 *
 * This a bit map since more than oen activity can be requested or occur on a
 * file descriptor at the same time.
 *
 * @var GEIS_BE_MX_READ_AVAILABLE
 * Indicates data is available to be read on the file descriptor.
 *
 * @var GEIS_BE_MX_WRITE_AVAILABLE
 * Indicates data can be writtent ot the file descriptor without blocking.
 *
 * @var GEIS_BE_MX_HANGUP_DETECTED
 * Indicates a hangup was detected on the file descriptor.
 *
 * @var GEIS_BE_MX_ERROR_DETECTED
 * Indicates an error was detected on the file descriptor.
 */
typedef enum _GeisBackendMultiplexorActivity
{
  GEIS_BE_MX_READ_AVAILABLE  = 1 << 0,
  GEIS_BE_MX_WRITE_AVAILABLE = 1 << 1,
  GEIS_BE_MX_HANGUP_DETECTED = 1 << 2,
  GEIS_BE_MX_ERROR_DETECTED  = 1 << 3
} GeisBackendMultiplexorActivity;

/**
 * Handles events occurring on multiplexed file descriptors.
 *
 * Back ends must provide a callback with this signature to the multiplexor.
 */
typedef void (*GeisBackendFdEventCallback)(int                             fd,
                                     GeisBackendMultiplexorActivity  activity,
                                     void                           *context);

/**
 * Constructs a new back end multiplexor.
 */
GeisBackendMultiplexor geis_backend_multiplexor_new();

/**
 * A reasonable default value for the max_events_per_pump parameter to
 * geis_backend_multiplexor_new().
 */
#define GEIS_BE_MX_DEFAULT_EVENTS_PER_PUMP  16

/**
 * Destroys a back end multiplexor.
 *
 * @param[in] mx  The back end multiplexor to destroy.
 */
void geis_backend_multiplexor_delete(GeisBackendMultiplexor mx);

/**
 * Adds a file descriptor to a back end multiplexor.
 *
 * @param[in] mx        The back end multiplexor.
 * @param[in] fd        The file descriptor to add.
 * @param[in] activity  The actvitiy(ies) to monitor.
 * @param[in] callback  The function to call when a desired activity is detected.
 * @param[in] context   A context value to pass to the callback.
 *
 * The file descriptor will be monitored for one or more activities.  At least
 * one of GEIS_BE_MX_READ_AVAILABLE or GEIS_BE_MX_WRITE_AVAILABLE should be
 * passed, other activities are monitored automatically.
 */
void geis_backend_multiplexor_add_fd(GeisBackendMultiplexor          mx,
                                     int                             fd,
                                     GeisBackendMultiplexorActivity  activity,
                                     GeisBackendFdEventCallback      callback,
                                     void                           *context);

/**
 * Modifies the activities being monitored on a file descriptor.
 */
void geis_backend_multiplexor_modify_fd(GeisBackendMultiplexor          mx,
                                        int                             fd,
                                        GeisBackendMultiplexorActivity  activity);

/**
 * Removes a file descriptor from a back end multiplexor.
 *
 * @param[in] mx  The back end multiplexor.
 * @param[in] fd  The file descriptor to remove.
 */
void geis_backend_multiplexor_remove_fd(GeisBackendMultiplexor mx,
                                        int                    fd);

/**
 * Gets the file descriptor of the back end multiplexor.
 *
 * @param[in] mx  The back end multiplexor.
 */
int geis_backend_multiplexor_fd(GeisBackendMultiplexor mx);

/**
 * Gets the maximum number of fd events per pump.
 *
 * @param[in] mx                   The back end multiplexor.
 */
int geis_backend_multiplexor_max_events_per_pump(GeisBackendMultiplexor mx);

/**
 * Sets the maximum fd events per pump of the multiplexor.
 *
 * @param[in] mx                   The back end multiplexor.
 * @param[in] max_events_per_pump  The maximum number of fd events pumped
 *                                 per call of geis_backend_multiplexor_pump().
 *
 * This value is tunable to accommodate different back ends and
 * application/toolkit requirements.
 */
void geis_backend_multiplexor_set_max_events_per_pump(GeisBackendMultiplexor mx,
                                                      int max_events_per_pump);

/**
 * Exercises the back end multiplexor.
 *
 * @param[in] mx  The back end multiplexor.
 *
 * Dispatches any events on multiplexed file descriptors to their assciated
 * handlers.
 */
GeisStatus geis_backend_multiplexor_pump(GeisBackendMultiplexor mx);

#endif /* GEIS_BACKEND_MULTIPLEXOR_H_ */
