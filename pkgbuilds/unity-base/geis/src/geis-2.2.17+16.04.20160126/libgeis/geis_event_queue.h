/**
 * @file geis_event_queue.h
 * @brief internal Geis event queue public interface
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
#ifndef GEIS_EVENT_QUEUE_H_
#define GEIS_EVENT_QUEUE_H_

#include <geis/geis.h>


/**
 * A container for event_queues.
 *
 * This is a simple FIFO container for opaque GeisEvent objetcs.
 *
 * This container does not asssume ownership of the contained GeisEvents.
 * Someone creates the events and pushes them into the queue, and someone pulls
 * the events off the queue and does something with them.
 *
 * The current implementation uses a pooled caching strategy for dynamic
 * allocations to minimize overhead due to high-turnover usage.
 */
typedef struct _GeisEventQueue *GeisEventQueue;


/**
 * Creates a new Geis Event queue.
 */
GeisEventQueue geis_event_queue_new();

/**
 * Destroys a Geis Event queue.
 *
 * @param[in] queue  The event queue.
 *
 * This function empties the queue and destroys any GeisEvents malingering
 * therein, then destroys the queue itself.
 */
void geis_event_queue_delete(GeisEventQueue queue);

/**
 * Pushes a new event onto the back of the event queue.
 *
 * @param[in] queue  The event queue.
 *
 * @retval GEIS_STATUS_SUCCESS        Normal successful completion.
 *
 * @retval GEIS_STATUS_UNKNOWN_ERROR  Something bad happened.
 */
GeisStatus geis_event_queue_enqueue(GeisEventQueue queue, GeisEvent event);

/**
 * Indicates if an event queue is empty.
 *
 * @param[in]  queue  The event queue.
 *
 * @returns GEIS_TRUE if the queue contains no events, GEIS_FALSE otherwise.
 */
GeisBoolean geis_event_queue_is_empty(GeisEventQueue queue);

/**
 * Pops the event off the front of the queue.
 *
 * @param[in] queue  The event queue.
 *
 * @returns the next GeisEvent or NULL of the queue is empty.
 */
GeisEvent geis_event_queue_dequeue(GeisEventQueue queue);

/**
 * Prototype for GEIS event matching predicate functions.
 */
typedef GeisBoolean (*GeisEventMatch)(GeisEvent event, void *context);

/**
 * Removes events from a queue of they match a condition.
 *
 * @param[in] queue     The event queue.
 * @param[in] matching  A unary predicate function to indicate the events to be
 *                      removed.
 * @param[in] context   An application-specific context value to be passed to
 *                      the matching function.
 */
void geis_event_queue_remove_if(GeisEventQueue  queue,
                                GeisEventMatch  matching,
                                void           *context);

#endif /* GEIS_EVENT_QUEUE_H_ */
