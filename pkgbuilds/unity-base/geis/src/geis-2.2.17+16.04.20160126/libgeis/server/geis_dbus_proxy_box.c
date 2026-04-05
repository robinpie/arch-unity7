/**
 * @file geis_dbus_proxy_box.c
 * @brief Implementation of a storage facility for GEIS DBus server client
 * proxies.
 */

/*
 * Copyright 2011 Canonical Ltd.
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
#include "geis_config.h"
#include "geis_dbus_proxy_box.h"

#include <assert.h>
#include "geis_logging.h"


typedef struct GeisDBusProxyBoxNode *GeisDBusProxyBoxNode;

struct GeisDBusProxyBoxNode 
{
  GeisDBusClientProxy  proxy;
  GeisDBusProxyBoxNode next;
};


/**
 * The box is represented using a circular linked list with a free pool.
 */
struct GeisDBusProxyBox 
{
  GeisDBusProxyBoxNode head;
  GeisDBusProxyBoxNode avail;
};


/*
 * Constructs a %GeisDBusProxyBox.
 */
GeisDBusProxyBox
geis_dbus_proxy_box_new()
{
  GeisDBusProxyBox box = calloc(1, sizeof(struct GeisDBusProxyBox));
  if (!box)
  {
    geis_error("error allocating GeisDBusProxyBox");
  }

  return box;
}


/*
 * Destroys a %GeisDBusProxyBox.
 *
 * Assumes all contained client proxies have already been freed, otherwise there
 * will be resource leaks because this container does not know how to free them.
 */
void
geis_dbus_proxy_box_delete(GeisDBusProxyBox box)
{
  GeisDBusProxyBoxNode p = box->avail;
  while (p)
  {
    box->avail = p->next;
    free(p);
    p = box->avail;
  }
  free(box);
}


/*
 * Inserts a %GeisDBusClientProxy in to a %GeisDBusProxyBox.
 */
void
geis_dbus_proxy_box_insert(GeisDBusProxyBox    box,
                           GeisDBusClientProxy proxy)
{
  if (box->avail == NULL)
  {
    box->avail = calloc(1, sizeof(struct GeisDBusProxyBoxNode));
    if (!box->avail)
    {
      geis_error("error allocating proxy box node");
      return;
    }
  }

  GeisDBusProxyBoxNode p = box->avail;
  p->proxy = proxy;
  box->avail = p->next;

  if (box->head == NULL)
  {
    box->head = p;
  }
  else
  {
    p->next = box->head->next;
  }
  box->head->next = p;
}


/*
 * Removes a %GeisDBusClientProxy from a %GeisDBusProxyBox.
 */
void
geis_dbus_proxy_box_remove(GeisDBusProxyBox    box,
                           GeisDBusClientProxy proxy)
{
  assert(box->head != NULL);

  GeisDBusProxyBoxNode ptr = box->head;
  GeisDBusProxyBoxNode p = box->head->next;
  do
  {
    if (p->proxy == proxy)
    {
      if (ptr == p)
      {
        box->head = NULL;
      }
      else
      {
        ptr->next = p->next;
        if (box->head == p)
          box->head = ptr;
      }
      p->next = box->avail;
      box->avail = p;
      break;
    }
    ptr = ptr->next;
    p = p->next;
  } while (ptr != box->head);
}


/*
 * Gets an iterator initialized to the first client proxy in a box.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_begin(GeisDBusProxyBox box)
{
  if (box->head)
    return box->head->next;
  return box->head;
}


/*
 * Gets an iterator initialized to one-past-the-end of a client proxy box.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_end(GeisDBusProxyBox box GEIS_UNUSED)
{
  return NULL;
}


/*
 * Gets the current client proxy from a client proxy box iterator.
 */
GeisDBusClientProxy
geis_dbus_proxy_box_iter_value(GeisDBusProxyBoxIterator iter)
{
  return iter->proxy;
}


/*
 * Advances a client proxy box iterator.
 */
GeisDBusProxyBoxIterator
geis_dbus_proxy_box_iter_next(GeisDBusProxyBox         box,
                              GeisDBusProxyBoxIterator iter)
{
  if (iter == box->head)
     return geis_dbus_proxy_box_end(box);
  else
    return iter->next;
}



