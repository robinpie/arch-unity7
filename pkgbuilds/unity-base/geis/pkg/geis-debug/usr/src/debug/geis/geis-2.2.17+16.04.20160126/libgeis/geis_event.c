/**
 * @file geis_event.c
 * @brief GeisEvent module implementation
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

#include "geis/geis.h"
#include "geis_attr.h"
#include "geis_logging.h"
#include <stdlib.h>


struct _GeisEvent
{
  GeisEventType ev_type;
  GeisAttrBag   ev_attr_bag;
};


/*
 * Creates a new event.
 */
GeisEvent
geis_event_new(GeisEventType ev_type)
{
  GeisEvent event = calloc(1, sizeof(struct _GeisEvent));
  if (!event)
  {
    geis_error("unable to allocate GeisEvent");
    goto final_exit;
  }

  event->ev_type = ev_type;
  event->ev_attr_bag =  geis_attr_bag_new(4);
  if (!event->ev_attr_bag)
  {
    geis_error("unable to allocate GeisEvent attribute bag");
    goto unwind_event;
  }
  goto final_exit;

unwind_event:
  free(event);
final_exit:
  return event;
}


/*
 * Destroys an event.
 */
void
geis_event_delete(GeisEvent event)
{
  geis_attr_bag_delete(event->ev_attr_bag);
  free(event);
}


/*
 * Gets the type of the event.
 */
GeisEventType
geis_event_type(GeisEvent event)
{
  return event->ev_type;
}


/*
 * Sets the type of the event.
 */
void
geis_event_override_type(GeisEvent event, GeisEventType ev_type)
{
  event->ev_type = ev_type;
}


/*
 * Gets how many attrs are in the event.
 */
GeisSize
geis_event_attr_count(GeisEvent event)
{
  return geis_attr_bag_count(event->ev_attr_bag);
}


/*
 * Gets an indicated attr.
 */
GeisAttr
geis_event_attr(GeisEvent event, GeisSize index)
{
  return geis_attr_bag_attr(event->ev_attr_bag, index);
}


/*
 * Gets a named attr.
 */
GeisAttr
geis_event_attr_by_name(GeisEvent event, GeisString attr_name)
{
  return geis_attr_bag_find(event->ev_attr_bag, attr_name);
}


/*
 * Adds an attr.
 */
GeisStatus
geis_event_add_attr(GeisEvent event, GeisAttr attr)
{
  return geis_attr_bag_insert(event->ev_attr_bag, attr);
}


