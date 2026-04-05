/**
 * geis-server.c Test driver for the GEIS server.
 *
 * Copyright 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU General Public License as published by the
 * Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/.
 */
#include <errno.h>
#include <geis/geis.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>


int
main()
{
  GeisStatus status;

  Geis geis = geis_new(GEIS_INIT_SERVICE_PROVIDER,
                       GEIS_INIT_XCB_BACKEND,
                       NULL);
  if (!geis)
  {
    fprintf(stderr, "error creating geis instance.\n");
    goto final_exit;
  }

  int geis_fd = -1;
  status = geis_get_configuration(geis, GEIS_CONFIGURATION_FD, &geis_fd);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error obtaining geis fd.\n");
    goto unwind_geis;
  }

  while (1)
  {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    FD_SET(geis_fd, &read_fds);

    int sstat = select(geis_fd + 1, &read_fds, NULL, NULL, NULL);
    if (sstat < 0)
    {
      fprintf(stderr, "error %d in select(): %s\n", errno, strerror(errno));
      break;
    }

    if (FD_ISSET(geis_fd, &read_fds))
    {
      status = geis_dispatch_events(geis);

      GeisEvent  event_out;
      status = geis_next_event(geis, &event_out);
      while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
      {
	geis_event_delete(event_out);
	status = geis_next_event(geis, &event_out);
      }
    }

    if (FD_ISSET(0, &read_fds))
    {
      break;
    }
  }

unwind_geis:
  geis_delete(geis);
final_exit:
  return 0;
}
