/**
 * @file geis_dbus_dispatcher.c
 * @brief Implementation of the GEIS DBus dispatcher.
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
#include "geis_dbus_dispatcher.h"

#include "geis_logging.h"
#include "geis_private.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct GeisDBusWatch *GeisDBusWatch;
typedef struct GeisDBusWatchBag *GeisDBusWatchBag;

/*
 * Connects a DBusWatch back to a DBusConnection.
 *
 * This is an intrusive linked list node.  See GeisDBusWatchBag.
 */
struct GeisDBusWatch 
{
  DBusConnection     *connection;
  DBusWatch          *watch;
  GeisDBusWatch       next;
};


/*
 * Maps file descriptors to watches and connections.
 *
 * A DBusWatch is assciated with a single file descriptor, but each file
 * descriptor may be associated with more than one DBusWatch. 
 *
 * Each DBusConnection has one or more DBusWatch.  The DBusWatches are passed
 * around without reference to the connection itself, but we often need the
 * connection when all we have is the watch.
 *
 * To make things more complex, the DBusServer does not have a connection
 * associated with its watches.
 *
 * This is a linked list with a free pool.
 */
struct GeisDBusWatchBag
{
  GeisDBusWatch front;
  GeisDBusWatch back;
  GeisDBusWatch pool;
};

static const int   _geis_dbus_watch_bag_initial_size = 4;


struct GeisDBusDispatcher
{
  Geis             geis;
  GeisDBusWatchBag watches;
};


/*
 * Creates a new empty collection of watches.
 *
 * The pool is primed with a few empty watches to save time later, on the
 * assumption that if you're creating a bag you're going to use it.
 */
static GeisDBusWatchBag 
_geis_dbus_watch_bag_new()
{
  GeisDBusWatchBag bag = calloc(1, sizeof(struct GeisDBusWatchBag));
  if (!bag)
  {
    geis_error("error allocating GeisDBusWatchBag");
    goto final_exit;
  }

  /* Prime the free pool. */
  for (int i = 0; i < _geis_dbus_watch_bag_initial_size; ++i)
  {
    GeisDBusWatch gdbw = calloc(1, sizeof(struct GeisDBusWatch));
    if (!gdbw)
    {
      geis_error("error allocating GeisDBusWatchBag");
      goto unwind_pool;
    }
    gdbw->next = bag->pool;
    bag->pool = gdbw;
  }
  goto final_exit;

unwind_pool:
final_exit:
  return bag;
}


/*
 * Destroys a collection of watches.
 *
 * @param[in] bag  A collection of %GeisDBusWatches.
 *
 * There should be no need to unref any of the contents of the bag, they can
 * just be freed without consequence.
 */
static void 
_geis_dbus_watch_bag_delete(GeisDBusWatchBag bag)
{
  /* Free the pool. */
  GeisDBusWatch gdbw = bag->pool;
  while (gdbw)
  {
    GeisDBusWatch next = gdbw->next;
    free(gdbw);
    gdbw = next;
  }

  /* Free the in-use watches. */
  gdbw = bag->front;
  while (gdbw)
  {
    GeisDBusWatch next = gdbw->next;
    free(gdbw);
    gdbw = next;
  }

  free(bag);
}


/*
 * Gets an allocated watch from the bag.
 *
 * @param[in] bag        A collection of %GeisDBusWatches.
 * @param[in] connection A DBusConnection.
 * @param[in] watch      A DBusWatch.
 *
 * A factory function to create a new watch in the collection and return a
 * pointer to it.
 */
static GeisDBusWatch
_geis_dbus_watch_bag_alloc_watch(GeisDBusWatchBag    bag,
                                 DBusConnection     *connection,
                                 DBusWatch          *watch)
{
  GeisDBusWatch gdbw = NULL;

  /* Either pull a free watch off the pool or allocate a new one. */
  if (bag->pool)
  {
    gdbw = bag->pool;
    bag->pool = bag->pool->next;
  }
  else
  {
    gdbw = calloc(1, sizeof(struct GeisDBusWatch));
    if (!gdbw)
    {
      geis_error("error allocating GeisDBusWatchBag");
      goto final_exit;
    }
  }

  /* Fill in the data bits. */
  gdbw->connection = connection;
  gdbw->watch = watch;
  gdbw->next = NULL;

  /* Add it to the in-use list. */
  if (!bag->front)
  {
    bag->front = gdbw;
  }
  if (bag->back)
  {
    bag->back->next = gdbw;
  }
  bag->back = gdbw;

final_exit:
  return gdbw;
}


/*
 * Removes a watch from a collection of such beasts.
 *
 * @param[in] bag      A collection of %GeisDBusWatches.
 * @param[in] watch    The watch to remove.
 */
static void
_geis_dbus_watch_bag_remove_watch(GeisDBusWatchBag    bag,
                                  DBusWatch          *watch)
{
  for (GeisDBusWatch gdbw = bag->front, prev = NULL; gdbw; gdbw = gdbw->next)
  {
    if (gdbw->watch == watch)
    {
      if (gdbw == bag->front)
      {
	bag->front = gdbw->next;
      }
      else
      {
	prev->next = gdbw->next;
      }
      if (gdbw == bag->back)
      {
	bag->back = prev;
      }

      gdbw->next = bag->pool;
      bag->pool = gdbw;

      break;
    }
    prev = gdbw;
  }
}


/*
 * Indicates if a file descriptor is already held in the watch bag.
 *
 * @param[in]  bag    A collection of %GeisDBusWatches.
 * @param[in]  fd     A file descriptor.
 * @param[out] flags  The DBus watch flags for any enabled watches found.
 *
 * @returns zero if the file descriptor is not in the bag, non-zero otherwise.
 */
static int
_geis_dbus_watch_bag_has_fd(GeisDBusWatchBag bag, int fd, unsigned int *flags)
{
  int has_fd = 0;
  for (GeisDBusWatch gdbw = bag->front; gdbw; gdbw = gdbw->next)
  {
    if (dbus_watch_get_unix_fd(gdbw->watch) == fd)
    {
      has_fd |= ~0;
      if (dbus_watch_get_enabled(gdbw->watch))
      {
	*flags |= dbus_watch_get_flags(gdbw->watch);
      }
    }
  }
  return has_fd;
}


/*
 * Finds a DBusWatch in the bag that matches the fd and current activity.
 *
 * @param[in] bag      A collection of %GeisDBusWatches.
 * @param[in] fd       The file descriptor on which an activity has been detected.
 * @param[in] activity The bitmask of currently detected activity on the fd.
 *
 * A DBusWatch will match if it has the same file descriptor and is watching for
 * (one of) the activity(ies) that has just occurred.
 *
 * Note that writers are implicitly looking for hangups or errors but the DBus
 * library goes into an infinite loop when a hangup has occurred no a write
 * watch, so defer that to a read watch.
 *
 * @returns a GeisDBusWatch or NULL if no matching watch was found.
 */
static GeisDBusWatch
_geis_dbus_watch_bag_find_fd_activity(GeisDBusWatchBag               bag,
                                      int                            fd,
                                      GeisBackendMultiplexorActivity activity)
{
  GeisDBusWatch gdbw = NULL;
  for (gdbw = bag->front; gdbw; gdbw = gdbw->next)
  {
    if (dbus_watch_get_unix_fd(gdbw->watch) == fd)
    {
      unsigned int flags = dbus_watch_get_flags(gdbw->watch);
      if ((activity & GEIS_BE_MX_READ_AVAILABLE && flags & DBUS_WATCH_READABLE)
       || (activity & GEIS_BE_MX_WRITE_AVAILABLE && flags & DBUS_WATCH_WRITABLE)
       || (activity & GEIS_BE_MX_HANGUP_DETECTED && flags & DBUS_WATCH_READABLE)
       || (activity & GEIS_BE_MX_ERROR_DETECTED))
      {
	break;
      }
    }
  }
  return gdbw;
}


/*
 * A callback function passed to the Geis multiplexor.
 *
 * @param[in] fd       The file descriptor on which an activity has been detected.
 * @param[in] activity The bitmask of currently detected activity on the fd.
 * @param[in] context  The %GeisDBusDispatcher passed through the multiplexor.
 *
 * This callback gets invoked whenever a requested activity is detected on a
 * regostered DBusWatch file descriptor.  It translates the GEIS Multiplexor
 * activity to DBus activity.
 */
static void
_geis_dbus_dispatcher_callback(int                             fd,
                               GeisBackendMultiplexorActivity  activity,
                               void                           *context)
{
  GeisDBusDispatcher dispatcher = (GeisDBusDispatcher)context;
  GeisDBusWatch gdb = _geis_dbus_watch_bag_find_fd_activity(dispatcher->watches,
                                                            fd,
                                                            activity);
  if (gdb)
  {
    /* Translate GEIS multiplexor activity to DBus watch flags. */
    unsigned int flags = 0;
    if (activity & GEIS_BE_MX_READ_AVAILABLE)  flags |= DBUS_WATCH_READABLE;
    if (activity & GEIS_BE_MX_WRITE_AVAILABLE) flags |= DBUS_WATCH_WRITABLE;
    if (activity & GEIS_BE_MX_HANGUP_DETECTED) flags |= DBUS_WATCH_HANGUP;
    if (activity & GEIS_BE_MX_ERROR_DETECTED)  flags |= DBUS_WATCH_ERROR;
    dbus_watch_handle(gdb->watch, flags);

    if (gdb->connection)
    {
      if (activity & GEIS_BE_MX_HANGUP_DETECTED)
      {
        dbus_connection_close(gdb->connection);
      }
      else
      {
        DBusDispatchStatus s;
        s = dbus_connection_get_dispatch_status(gdb->connection);
        while (DBUS_DISPATCH_DATA_REMAINS == s)
        {
          s = dbus_connection_dispatch(gdb->connection);
        }
      }
    }
  }
}


/*
 * Creates a new GEIS DBus dispatcher.
 */
GeisDBusDispatcher
geis_dbus_dispatcher_new(Geis geis)
{
  GeisDBusDispatcher dispatcher = calloc(1, sizeof(struct GeisDBusDispatcher));
  if (!dispatcher)
  {
    geis_error("error allocating GEIS DBus dispatcher.");
    goto final_exit;
  }

  dispatcher->geis = geis;
  dispatcher->watches = _geis_dbus_watch_bag_new();
  if (!dispatcher->watches)
  {
    geis_error("error creating GEIS DBus dispatcher watches.");
    goto unwind_dispatcher;
  }

  goto final_exit;

unwind_dispatcher:
  free(dispatcher);
final_exit:
  return dispatcher;
}


/*
 * Destroys an existing %GeisDBusDispatcher object.
 */
void
geis_dbus_dispatcher_delete(GeisDBusDispatcher dispatcher)
{
  for (GeisDBusWatch gdbw = dispatcher->watches->front;
       gdbw; gdbw = gdbw->next)
  {
    geis_dbus_dispatcher_unregister(dispatcher, gdbw->watch);
  }
  _geis_dbus_watch_bag_delete(dispatcher->watches);
  free(dispatcher);
 }


/*
 * Registers a new DBusWatch with a %GeisDBusDispatcher object.
 */
void
geis_dbus_dispatcher_register(GeisDBusDispatcher  dispatcher,
                              DBusConnection     *connection,
                              DBusWatch          *watch)
{
  int watch_fd = dbus_watch_get_unix_fd(watch);

  /* Calculate all the enabled flags on the fd for all watches. */
  unsigned int flags = 0;
  int has_fd = _geis_dbus_watch_bag_has_fd(dispatcher->watches, watch_fd, &flags);
  _geis_dbus_watch_bag_alloc_watch(dispatcher->watches, connection, watch);
  if (dbus_watch_get_enabled(watch))
  {
    flags |= dbus_watch_get_flags(watch);
  }

  /* Convert the watch flags to multiplexor activities. */
  GeisBackendMultiplexorActivity activity = 0;
  if (flags & DBUS_WATCH_READABLE) activity |= GEIS_BE_MX_READ_AVAILABLE;
  if (flags & DBUS_WATCH_WRITABLE) activity |= GEIS_BE_MX_WRITE_AVAILABLE;

  /* Set or adjust the multiplexor seubscription. */
  if (has_fd)
  {
    geis_remultiplex_fd(dispatcher->geis, watch_fd, activity);
  }
  else
  {
    geis_multiplex_fd(dispatcher->geis,
                      watch_fd,
                      activity,
                      _geis_dbus_dispatcher_callback,
                      dispatcher);
  }
}


/*
 * Unregisters a DBusWatch for events.
 */
void
geis_dbus_dispatcher_unregister(GeisDBusDispatcher  dispatcher,
                                DBusWatch          *watch)
{
  int watch_fd = dbus_watch_get_unix_fd(watch);
  unsigned int flags = 0;
  _geis_dbus_watch_bag_remove_watch(dispatcher->watches, watch);
  if (!_geis_dbus_watch_bag_has_fd(dispatcher->watches, watch_fd, &flags))
  {
    geis_demultiplex_fd(dispatcher->geis, watch_fd);
  }
}


/*
 * Marks a DBusWatch as active, maybe.
 */
void
geis_dbus_dispatcher_toggle_watch(GeisDBusDispatcher  dispatcher,
                                  DBusWatch          *watch)
{
  int watch_fd = dbus_watch_get_unix_fd(watch);

  /* Calculate all the enabled flags on the fd for all watches. */
  unsigned int flags = 0;
  _geis_dbus_watch_bag_has_fd(dispatcher->watches, watch_fd, &flags);
  if (dbus_watch_get_enabled(watch))
  {
    flags |= dbus_watch_get_flags(watch);
  }
  else
  {
    flags &= ~dbus_watch_get_flags(watch);
  }

  /* Convert the watch flags to multiplexor activities. */
  GeisBackendMultiplexorActivity activity = 0;
  if (flags & DBUS_WATCH_READABLE) activity |= GEIS_BE_MX_READ_AVAILABLE;
  if (flags & DBUS_WATCH_WRITABLE) activity |= GEIS_BE_MX_WRITE_AVAILABLE;

  /* Set or adjust the multiplexor seubscription. */
  geis_remultiplex_fd(dispatcher->geis, watch_fd, activity);

}


