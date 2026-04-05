/**
 * @file geis_timer.h
 * @brief Internal interface for the GEIS timer module.
 */
/* Copyright (C) 2011-2012 Canonical Ltd
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
#ifndef GEIS_TIMER_H_
#define GEIS_TIMER_H_

#include <geis/geis.h>

/**
 * @defgroup geis_timer Integral GEIS timers
 * @{
 */

/**
 * An opaque timer object.
 */
typedef struct GeisTimer *GeisTimer;

/**
 * The type of the callback to be invoked on timer expiry.
 */
typedef void (*GeisTimerCallback)(GeisTimer timer, void *context);

/**
 * Creates a new timer object on a GEIS API instance.
 *
 * @param[in] geis     A GEIS API instance.
 * @param[in] callback A callback to be executed on timer expiry.
 * @param[in] context  User context to be passed to the callback.
 */
GeisTimer geis_timer_new(Geis geis, GeisTimerCallback callback, void *context);

/**
 * Destroys an existing timer.
 * @param timer A timer object.
 */
void geis_timer_delete(GeisTimer timer);

/**
 * Cancels an outstanding timer.
 * @param timer A timer object.
 */
void geis_timer_cancel(GeisTimer timer);

/**
 * Starts a timer object a-ticking.
 * @param timer A timer object.
 * @param msec  The number of milliseconds until the timer expires.
 */
void geis_timer_start(GeisTimer timer, GeisInteger msec);

/* @} */

#endif /* GEIS_TIMER_H_ */
