/**
 * @file geis_timer.c
 * @brief Implementation of the GEIS timer module.
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
#include "geis_config.h"
#include "geis_timer.h"

#include <errno.h>
#include "geis_logging.h"
#include "geis_private.h"
#include <stdlib.h>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>

static const int nanosecondsPerMillisecond = 1000000;
/*
 * The timer object.
 */
struct GeisTimer 
{
  int                fd;
  GeisTimerCallback  callback;
  void              *context;
};


/*
 * Bounces the multiplexor callback into the timer callback.
 */
void
_timer_trampoline(int                             fd GEIS_UNUSED,
                  GeisBackendMultiplexorActivity  ev GEIS_UNUSED,
                  void                           *ctx)
{
  GeisTimer timer = (GeisTimer)ctx;

  timer->callback(timer, timer->context);
}


/*
 * Creates a new timer object on a GEIS API instance.
 */
GeisTimer
geis_timer_new(Geis geis, GeisTimerCallback callback, void *context)
{
  GeisTimer timer = calloc(1, sizeof(struct GeisTimer));
  if (!timer)
  {
    geis_error("failed to allocate timer structure");
    goto final_exit;
  }

  timer->fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (timer->fd == -1)
  {
    geis_error("error %d creating timerfd: %s", errno, strerror(errno));
    goto unwind_timer;
  }

  timer->callback = callback;
  timer->context  = context;
  geis_multiplex_fd(geis, timer->fd, GEIS_BE_MX_READ_AVAILABLE,
                    _timer_trampoline, timer);
  goto final_exit;

unwind_timer:
  free(timer);
  timer = 0;
final_exit:
  return timer;
}


/*
 * Destroys an existing timer.
 */
void
geis_timer_delete(GeisTimer timer)
{
  close(timer->fd);
  free(timer);
}


/*
 * Cancels an outstanding timer.
 */
void
geis_timer_cancel(GeisTimer timer)
{
  struct itimerspec in = { { 0, 0 }, { 0, 0 } };
  struct itimerspec out;
  int status = timerfd_settime(timer->fd, 0, &in, &out);
  if (status != 0)
  {
    geis_error("error %d cancelling timerfd: %s", errno, strerror(errno));
  }
}


/*
 * Starts a timer object a-ticking.
 */
void
geis_timer_start(GeisTimer timer, GeisInteger msec)
{
  struct itimerspec in = { { 0, 0 }, { 0, msec * nanosecondsPerMillisecond } };
  struct itimerspec out;
  int status = timerfd_settime(timer->fd, 0, &in, &out);
  if (status != 0)
  {
    geis_error("error %d starting timerfd: %s", errno, strerror(errno));
  }
}


