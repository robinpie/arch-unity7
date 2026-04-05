/**
 * geistest.c Demo code for programming against the geis interface.
 *
 * Copyright 2010 Canonical Ltd.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"

#include <errno.h>
#include <geis/geis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <xcb/xcb.h>

GeisSize g_smw = 0;

const char* s_gestures[] = {
  GEIS_GESTURE_TYPE_DRAG2, GEIS_GESTURE_TYPE_DRAG3,
  GEIS_GESTURE_TYPE_PINCH2, GEIS_GESTURE_TYPE_PINCH3,
  GEIS_GESTURE_TYPE_ROTATE2, GEIS_GESTURE_TYPE_ROTATE3,
  GEIS_GESTURE_TYPE_TAP2, GEIS_GESTURE_TYPE_TAP3, GEIS_GESTURE_TYPE_TAP4,
  GEIS_GESTURE_TYPE_TOUCH3, GEIS_GESTURE_TYPE_TOUCH4,
  GEIS_GESTURE_TYPE_FLICK2, GEIS_GESTURE_TYPE_FLICK3, GEIS_GESTURE_TYPE_FLICK4,
  NULL
};

GeisSize           g_device_count = 0;
GeisInputDeviceId *g_devices = GEIS_ALL_INPUT_DEVICES;


static void
print_attr(GeisGestureAttr *attr)
{
  fprintf(stdout, "\tattr \"%s\" = ", attr->name);
  switch (attr->type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      fprintf(stdout, "%s\n", attr->boolean_val ? "true" : "false");
      break;
    case GEIS_ATTR_TYPE_FLOAT:
      fprintf(stdout, "%f\n", attr->float_val);
      break;
    case GEIS_ATTR_TYPE_INTEGER:
      fprintf(stdout, "%d\n", attr->integer_val);
      break;
    case GEIS_ATTR_TYPE_STRING:
      fprintf(stdout, "\"%s\"\n", attr->string_val);
      break;
    default:
      fprintf(stdout, "<unknown>\n");
      break;
  }
}


static void
input_device_added(void              *cookie GEIS_UNUSED,
                   GeisInputDeviceId  device_id,
                   void              *attrs)
{
  GeisGestureAttr *a;
  fprintf(stdout, "Device %d added\n", device_id);
  for (a = attrs; a->name; ++a)
  {
    print_attr(a);
  }
}


static void
input_device_changed(void              *cookie GEIS_UNUSED,
                     GeisInputDeviceId  device_id GEIS_UNUSED,
                     void              *attrs GEIS_UNUSED)
{
}


static void
input_device_removed(void              *cookie GEIS_UNUSED,
                     GeisInputDeviceId  device_id,
                     void              *attrs)
{
  GeisGestureAttr *a;
  fprintf(stdout, "Device %d removed\n", device_id);
  for (a = attrs; a->name; ++a)
  {
    print_attr(a);
  }
}


static void
gesture_added(void            *cookie GEIS_UNUSED,
	      GeisGestureType  gesture_type,
	      GeisGestureId    gesture_id GEIS_UNUSED,
	      GeisSize         attr_count,
	      GeisGestureAttr *attrs)
{
  GeisSize i = 0;
  fprintf(stdout, "Gesture type %d added\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_removed(void              *cookie GEIS_UNUSED,
                GeisGestureType    gesture_type,
                GeisGestureId      gesture_id GEIS_UNUSED,
                GeisSize           attr_count,
                GeisGestureAttr   *attrs)
{
  GeisSize i = 0;
  fprintf(stdout, "Gesture type %d removed\n", gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_start(void              *cookie GEIS_UNUSED,
              GeisGestureType    gesture_type,
              GeisGestureId      gesture_id,
              GeisSize           attr_count,
              GeisGestureAttr   *attrs)
{
  GeisSize i = 0;
  fprintf(stdout, "Gesture id %d type %d started\n", gesture_id, gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_update(void              *cookie GEIS_UNUSED,
               GeisGestureType    gesture_type,
               GeisGestureId      gesture_id,
               GeisSize           attr_count,
               GeisGestureAttr   *attrs)
{
  GeisSize i = 0;
  fprintf(stdout, "Gesture id %d type %d updated\n", gesture_id, gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}

static void
gesture_finish(void              *cookie GEIS_UNUSED,
               GeisGestureType    gesture_type,
               GeisGestureId      gesture_id,
               GeisSize           attr_count,
               GeisGestureAttr   *attrs)
{
  GeisSize i = 0;
  fprintf(stdout, "Gesture id %d type %d finished\n", gesture_id, gesture_type);
  for (i = 0; i < attr_count; ++i)
    print_attr(&attrs[i]);
}


GeisInputFuncs input_funcs = {
  input_device_added,
  input_device_changed,
  input_device_removed
};

GeisGestureFuncs gesture_funcs = {
  gesture_added,
  gesture_removed,
  gesture_start,
  gesture_update,
  gesture_finish
};


static void
_add_device_id(GeisInteger id)
{
  GeisInputDeviceId *d = realloc(g_devices, g_device_count + 2);
  if (!d)
  {
    fprintf(stderr, "error allocating device list.\n");
    exit(1);
  }
  g_devices = d;
  g_devices[g_device_count] = id;
  g_devices[g_device_count+1]   = 0;
  ++g_device_count;
}


int
parse_opts(int argc, char* argv[], uint32_t *window_id)
{
  int opt;

  while ((opt = getopt(argc, argv, "w:d:")) != -1)
  {
    switch (opt)
    {
      case 'w':
	*window_id = strtol(optarg, NULL, 0);
	break;

      case 'd':
	{
	  GeisInteger id = strtol(optarg, NULL, 0);
	  if (0 == id)
	  {
	    fprintf(stderr, "invalid device id '%s'\n'", optarg);
	    return 0;
	  }
	  _add_device_id(id);
	}
	break;

      default:
	return 0;
    }
  }

  return 1;
}


static GeisInstance
subscribe_window(uint32_t window_id)
{
  GeisStatus status = GEIS_UNKNOWN_ERROR;
  GeisXcbWinInfo xcb_win_info = {
    .display_name  = NULL,
    .screenp       = NULL,
    .window_id     = window_id
  };
  GeisWinInfo win_info = {
    GEIS_XCB_FULL_WINDOW,
    &xcb_win_info
  };
  GeisInstance instance;

  status = geis_init(&win_info, &instance);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error in geis_init\n");
    return NULL;
  }

  status = geis_input_devices(instance, &input_funcs, NULL);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error subscribing to input devices\n");
    return NULL;
  }

  status = geis_subscribe(instance,
                          g_devices,
                          s_gestures,
                          &gesture_funcs,
                          NULL);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error subscribing to gestures\n");
    return NULL;
  }

  return instance;
}


static void
subscribe_all_windows(GeisInstance **instance_table)
{
  int instance_table_size = 0;

  xcb_connection_t *xcb = xcb_connect(NULL, NULL);
  if (!xcb) {
    fprintf(stderr, "error connecting to X server\n");
    exit(1);
  }

  const xcb_setup_t *setup = xcb_get_setup(xcb);
  if (!setup)
  {
    fprintf(stderr, "error getting xcb setup\n");
    exit(1);
  }

  xcb_screen_iterator_t screen = xcb_setup_roots_iterator(setup);
  while (screen.rem)
  {
    ++instance_table_size;
    xcb_screen_next(&screen);
  }

  *instance_table = calloc(instance_table_size, sizeof(GeisInstance));

  screen = xcb_setup_roots_iterator(setup);
  for (int i = 0; screen.rem; ++i, xcb_screen_next(&screen))
  {
    GeisInstance instance = subscribe_window(screen.data->root);
    if (!instance)
    {
      fprintf(stderr, "error subscribing window 0x%08x\n",
              (unsigned)screen.data->root);
      exit(1);
    }
    *instance_table[i] = instance;
  }

  xcb_disconnect(xcb);
}

int
main(int argc, char* argv[])
{
  int result = -1;
  uint32_t      window_id = 0;
  int           fd = -1;
  GeisStatus    status = GEIS_UNKNOWN_ERROR;
  GeisInstance *instance_table = NULL;
  size_t        instance_table_size = 0;

  if (!parse_opts(argc, argv, &window_id))
  {
    fprintf(stderr, "usage: %s windowid\n", argv[0]);
    return -1;
  }

  if (window_id != 0)
  {
    instance_table_size = 1;
    instance_table = calloc(instance_table_size, sizeof(GeisInstance));
    instance_table[0] = subscribe_window(window_id);
    if (!instance_table[0])
    {
      fprintf(stderr, "can not continue, exiting....\n");
      goto error_exit;
    }
  }
  else
  {
    subscribe_all_windows(&instance_table);
  }

  status = geis_configuration_supported(instance_table[0], GEIS_CONFIG_UNIX_FD);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "GEIS does not support Unix fd\n");
    goto fail_exit;
  }

  status = geis_configuration_get_value(instance_table[0], GEIS_CONFIG_UNIX_FD, &fd);
  if (status != GEIS_STATUS_SUCCESS)
  {
    fprintf(stderr, "error retrieving GEIS fd\n");
    goto fail_exit;
  }

  while (1)
  {
    size_t i;
    int fd;
    int max_fd = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    for (i = 0; i < instance_table_size; ++i)
    {
      status = geis_configuration_get_value(instance_table[i],
                                            GEIS_CONFIG_UNIX_FD, &fd);
      max_fd = (fd > max_fd ? fd : max_fd);
      FD_SET(fd, &read_fds);
    }
    int sstat = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
    if (sstat < 0)
    {
      fprintf(stderr, "error %d in select(): %s\n", errno, strerror(errno));
      break;
    }

    for (i = 0; i < instance_table_size; ++i)
    {
      status = geis_configuration_get_value(instance_table[i],
                                            GEIS_CONFIG_UNIX_FD, &fd);
      if (FD_ISSET(fd, &read_fds))
      {
	geis_event_dispatch(instance_table[i]);
      }
    }

    if (FD_ISSET(0, &read_fds))
    {
      break;
    }
  }

fail_exit:
  {
    size_t i;
    for (i = 0; i < instance_table_size; ++i)
    {
      geis_finish(instance_table[i]);
    }
  }

error_exit:
  if (instance_table_size > 0)
  {
    free(instance_table);
  }
  return result;
}

