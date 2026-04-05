/**
 * @file geis_event_queue.c
 * @brief internal Geis event queue implementation
 */
/* Copyright 2010, 2012 Canonical Ltd.
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
#include "geis_event_queue.h"

#include "geis_logging.h"
#include <stdlib.h>


typedef struct _GeisEventQueueNode *GeisEventQueueNode;

/**
 * A node in a GEIS event queue.  The event queue is a singly-linked list.
 */
struct _GeisEventQueueNode
{
  GeisEventQueueNode eq_next;
  GeisEvent          eq_event;
};

/**
 * A GEIS event queue.  The event queue is a singly linked list with a pool of
 * free nodes.
 */
struct _GeisEventQueue
{
  GeisEventQueueNode eq_front;
  GeisEventQueueNode eq_back;
  GeisEventQueueNode eq_pool;
};


/*
 * Creates a new Geis Event queue.
 */
GeisEventQueue
geis_event_queue_new()
{
  GeisEventQueue queue = calloc(1, sizeof(struct _GeisEventQueue));
  if (!queue)
  {
    geis_error("can not allocate event queue");
  }
  return queue;
}


/*
 * Destroys a Geis Event queue.
 */
void
geis_event_queue_delete(GeisEventQueue queue)
{
  GeisEventQueueNode node = queue->eq_pool;
  while (node)
  {
    GeisEventQueueNode eq_next = node->eq_next;
    free(node);
    node = eq_next;
  }

  node = queue->eq_front;
  while (node)
  {
    GeisEventQueueNode eq_next = node->eq_next;
    geis_event_delete(node->eq_event);
    free(node);
    node = eq_next;
  }

  free(queue);
}


/*
 * Pushes a new event onto the back of the event queue.
 */
GeisStatus
geis_event_queue_enqueue(GeisEventQueue queue, GeisEvent event)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisEventQueueNode node;

  if (queue->eq_pool)
  {
    node = queue->eq_pool;
    queue->eq_pool = node->eq_next;
    node->eq_next = NULL;
  }
  else
  {
    node = calloc(1, sizeof(struct _GeisEventQueueNode));
    if (!node)
    {
      geis_error("can not allocate event queue node");
      goto error_exit;
    }
  }

  node->eq_event = event;
  if (!queue->eq_front)
  {
    queue->eq_front = node;
  }
  if (queue->eq_back)
  {
    queue->eq_back->eq_next = node;
  }
  queue->eq_back = node;
  status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}


/*
 * Indicates if the event queue is empty.
 */
GeisBoolean
geis_event_queue_is_empty(GeisEventQueue queue)
{
  return queue->eq_front == NULL;
}


/*
 * Pushes a node into the front of the free pool for a queue.
 */
static void
_geis_event_queue_add_node_to_pool(GeisEventQueue     queue,
                                   GeisEventQueueNode node)
{
  node->eq_next = queue->eq_pool;
  queue->eq_pool = node;
}


/*
 * Pops the event off the front of the event queue.
 */
GeisEvent
geis_event_queue_dequeue(GeisEventQueue queue)
{
  GeisEvent event = NULL;
  if (queue->eq_front)
  {
    event = queue->eq_front->eq_event;

    GeisEventQueueNode node = queue->eq_front;
    queue->eq_front = node->eq_next;
    _geis_event_queue_add_node_to_pool(queue, node);
    if (!queue->eq_front)
      queue->eq_back = NULL;
  }
  return event;
}


/*
 * Removes events from a queue of they match a condition.
 */
void
geis_event_queue_remove_if(GeisEventQueue  queue,
                           GeisEventMatch  matching,
                           void           *context)
{
  GeisEventQueueNode prev = NULL;
  GeisEventQueueNode node = queue->eq_front;
  while (node)
  {
    if (matching(node->eq_event, context))
    {
      GeisEventQueueNode matching_node = node;
      node = node->eq_next;

      if (matching_node == queue->eq_front)
      {
        queue->eq_front = matching_node->eq_next;
      }
      else
      {
        prev->eq_next = matching_node->eq_next;
      }
      if (matching_node == queue->eq_back)
      {
        queue->eq_back = prev;
      }
      geis_event_delete(matching_node->eq_event);
      _geis_event_queue_add_node_to_pool(queue, matching_node);
    }
    else
    {
      prev = node;
      node = node->eq_next;
    }
  }
}

