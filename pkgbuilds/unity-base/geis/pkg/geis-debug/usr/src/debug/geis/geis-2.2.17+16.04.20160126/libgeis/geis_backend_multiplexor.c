/**
 * @file geis_backend_multiplexor.c
 * @brief internal GEIS backend multiplexor implementation
 *
 * Copyright 2010, 2012 Canonical Ltd.
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
#include "geis_config.h"
#include "geis_backend_multiplexor.h"

#include <errno.h>
#include <fcntl.h>
#include "geis_logging.h"
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>


typedef struct CallbackInfo *CallbackInfo;

struct CallbackInfo
{
  int                             fd;
  GeisBackendMultiplexorActivity  activity;
  GeisBackendFdEventCallback      callback;
  void                           *context;
  CallbackInfo                    next;
};

typedef struct CallbackInfoBag
{
  CallbackInfo front;
  CallbackInfo back;
  CallbackInfo pool;
} *CallbackInfoBag;


struct _GeisBackendMultiplexor
{
  int             mx_fd;
  int             mx_max_events_per_pump;
  CallbackInfoBag mx_callback_infos;
};


/*
 * Creates a new container for callback info.
 */
static CallbackInfoBag
_callback_info_bag_new()
{
  CallbackInfoBag cbib = calloc(1, sizeof(struct CallbackInfoBag));
  if (!cbib)
  {
    geis_error("error allocating Callback Info bag.");
  }
  return cbib;
}


/*
 * Destroys a callback info container.
 */
static void
_callback_info_bag_delete(CallbackInfoBag cbib)
{
  /* Drain the pool. */
  CallbackInfo cbi = cbib->pool;
  while (cbi)
  {
    CallbackInfo next = cbi->next;
    free(cbi);
    cbi = next;
  }

  /* Dump the bag contents. */
  cbi = cbib->front;
  while (cbi)
  {
    CallbackInfo next = cbi->next;
    free(cbi);
    cbi = next;
  }

  free(cbib);
}


/*
 * Allocates a CallbackInfo.
 */
static CallbackInfo
_callback_info_bag_alloc(CallbackInfoBag                 cbib,
                         int                             fd,
                         GeisBackendMultiplexorActivity  activity,
                         GeisBackendFdEventCallback      callback,
                         void                           *context)
{
  CallbackInfo callback_info = NULL;

  /* Either pull a free cbi from the pool or allocate a new one. */
  if (cbib->pool)
  {
    callback_info = cbib->pool;
    cbib->pool = callback_info->next;
  }
  else
  {
    callback_info = calloc(1, sizeof(struct CallbackInfo));
    if (!callback_info)
    {
      geis_error("error allocating CallbackInfoBag");
      goto final_exit;
    }
  }

  /* Copy the stuff in. */
  callback_info->fd       = fd;
  callback_info->activity = activity;
  callback_info->callback = callback;
  callback_info->context  = context;

  /* Add it to the in-use list. */
  if (!cbib->front)
  {
    cbib->front = callback_info;
  }
  if (cbib->back)
  {
    cbib->back->next = callback_info;
  }
  cbib->back = callback_info;

final_exit:
  return callback_info;
}


/*
 * Finds a CallbackInfo by file descriptor.
 */
CallbackInfo
_callback_info_bag_find_by_fd(CallbackInfoBag cbib, int fd)
{
  CallbackInfo callback_info = NULL;
  for (callback_info = cbib->front;
       callback_info;
       callback_info = callback_info->next)
  {
    if (callback_info->fd == fd)
    {
      break;
    }
  }
  return callback_info;
}


/*
 * Deallocates a CallbackInfo.
 */
static void
_callback_info_bag_release(CallbackInfoBag cbib, int fd)
{
  for (CallbackInfo callback_info = cbib->front, prev = NULL;
       callback_info;
       prev = callback_info, callback_info = callback_info->next)
  {
    if (callback_info->fd == fd)
    {
      if (callback_info == cbib->front)
      {
	cbib->front = callback_info->next;
      }
      else
      {
	prev->next = callback_info->next;
      }
      if (callback_info == cbib->back)
      {
	cbib->back = prev;
      }

      callback_info->next = cbib->pool;
      cbib->pool = callback_info;

      break;
    }
  }
}




/**
 * Creates a new backend multiplexor.
 */
GeisBackendMultiplexor
geis_backend_multiplexor_new()
{
  GeisBackendMultiplexor mx = calloc(1, sizeof(struct _GeisBackendMultiplexor));
  if (!mx)
  {
    geis_error("failed to allocate backend multiplexor");
  }
  else
  {
    mx->mx_fd = epoll_create(5);
    if (mx->mx_fd < 0)
    {
      geis_error("error %d creating backend multiplexor: %s",
                 errno, strerror(errno));
      goto unwind_mx;
    }
    if (fcntl(mx->mx_fd, F_SETFD, FD_CLOEXEC) < 0)
    {
      geis_error("error %d setting close-on-exec flag: %s",
                 errno, strerror(errno));
    }

    mx->mx_max_events_per_pump = GEIS_BE_MX_DEFAULT_EVENTS_PER_PUMP;

    mx->mx_callback_infos = _callback_info_bag_new();
    if (!mx->mx_callback_infos)
    {
      geis_error("failed to allocate backend multiplexor callback_infos");
      goto unwind_epoll;
    }
  }
  goto final_exit;

unwind_epoll:
  close(mx->mx_fd);
unwind_mx:
  free(mx);
  mx = NULL;
final_exit:
  return mx;
}


/**
 * Destroys an backend multiplexor.
 */
void
geis_backend_multiplexor_delete(GeisBackendMultiplexor mx)
{
  _callback_info_bag_delete(mx->mx_callback_infos);
  close(mx->mx_fd);
  free(mx);
}


static uint32_t
_epoll_events_from_activity(GeisBackendMultiplexorActivity activity)
{
  uint32_t events = 0;
  if (activity & GEIS_BE_MX_READ_AVAILABLE) events |= EPOLLIN;
  if (activity & GEIS_BE_MX_WRITE_AVAILABLE) events |= EPOLLOUT;
  return events;
}


/*
 * Adds a file descriptor to an backend multiplexor.
 */
void
geis_backend_multiplexor_add_fd(GeisBackendMultiplexor          mx,
                                int                             fd,
                                GeisBackendMultiplexorActivity  activity,
                                GeisBackendFdEventCallback      callback,
                                void                           *context)
{
  CallbackInfo callback_info = _callback_info_bag_alloc(mx->mx_callback_infos,
                                                        fd,
                                                        activity,
                                                        callback,
                                                        context);

  struct epoll_event ev;
  ev.events = _epoll_events_from_activity(activity);
  ev.data.ptr = callback_info;
   
  int status = epoll_ctl(mx->mx_fd, EPOLL_CTL_ADD, fd, &ev);
  if (status < 0)
  {
    geis_error("error %d multiplexing fd %d: %s",
               errno, fd, strerror(errno));
  }
}


/*
 * Modifies the activities being monitored on a file descriptor.
 */
void
geis_backend_multiplexor_modify_fd(GeisBackendMultiplexor          mx,
                                   int                             fd,
                                   GeisBackendMultiplexorActivity  activity)
{
  int status;
  struct epoll_event ev;
  CallbackInfo callback_info;

  callback_info = _callback_info_bag_find_by_fd(mx->mx_callback_infos, fd);
  callback_info->activity = activity;

  ev.events = _epoll_events_from_activity(activity);
  ev.data.ptr = callback_info;
  status = epoll_ctl(mx->mx_fd, EPOLL_CTL_MOD, fd, &ev);
  if (status < 0)
  {
    geis_error("error %d remultiplexing fd %d: %s",
               errno, fd, strerror(errno));
  }
}


/**
 * Removes a file descriptor from a backend multiplexor.
 *
 * @todo free callback_info
 */
void
geis_backend_multiplexor_remove_fd(GeisBackendMultiplexor mx, int fd)
{
  _callback_info_bag_release(mx->mx_callback_infos, fd);
  int status = epoll_ctl(mx->mx_fd, EPOLL_CTL_DEL, fd, NULL);
  if (status < 0)
  {
    geis_error("error %d demultiplexing fd %d: %s",
               errno, fd, strerror(errno));
  }
}


/**
 * Gets the single file descriptor of the backend multiplexor itself.
 */
int
geis_backend_multiplexor_fd(GeisBackendMultiplexor mx)
{
  return mx->mx_fd;
}


/**
 * gets the maximum number of fd events per pump.
 */
int
geis_backend_multiplexor_max_events_per_pump(GeisBackendMultiplexor mx)
{
  return mx->mx_max_events_per_pump;
}


/**
 * Sets the maximum number of fd events processed per pump.
 */
void
geis_backend_multiplexor_set_max_events_per_pump(GeisBackendMultiplexor mx,
                                                 int max_events_per_pump)
{
  mx->mx_max_events_per_pump = max_events_per_pump;
}


/**
 * Dispatches events on the multiplexed file descriptors.
 */
GeisStatus
geis_backend_multiplexor_pump(GeisBackendMultiplexor mx)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  int processed_event_count = 0;
  int available_event_count = 1;
  struct epoll_event events[4];

  while (available_event_count > 0
      && processed_event_count < mx->mx_max_events_per_pump)
  {
    available_event_count = epoll_wait(mx->mx_fd, events, 4, 0);
    if (available_event_count < 0)
    {
      geis_error("error %d in epoll_wait: %s", errno, strerror(errno));
      goto error_exit;
    }

    for (int i = 0; i < available_event_count; ++i)
    {
      GeisBackendMultiplexorActivity flags = 0;
      if (events[i].events & EPOLLIN)  flags |= GEIS_BE_MX_READ_AVAILABLE;
      if (events[i].events & EPOLLOUT) flags |= GEIS_BE_MX_WRITE_AVAILABLE;
      if (events[i].events & EPOLLHUP) flags |= GEIS_BE_MX_HANGUP_DETECTED;
      if (events[i].events & EPOLLERR) flags |= GEIS_BE_MX_ERROR_DETECTED;

      CallbackInfo callback_info = (CallbackInfo)events[i].data.ptr;
      geis_debug("activity 0x%x on fd %d callback_info=%p", events[i].events, callback_info->fd, (void *)callback_info);
      callback_info->callback(callback_info->fd, flags, callback_info->context);
      ++processed_event_count;
    }
  }
  if (available_event_count)
    status = GEIS_STATUS_CONTINUE;
  else
    status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}

