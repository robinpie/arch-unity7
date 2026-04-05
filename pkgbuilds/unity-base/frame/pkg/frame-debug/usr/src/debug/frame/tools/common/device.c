/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "common/device.h"

#include <stdio.h>

void get_axis_info(UFAxis axis, UFAxisType *type, const char **name, float *min,
                   float *max, float *res) {
  *type = frame_axis_get_type(axis);

  switch (*type) {
    case UFAxisTypeX:
      *name = "X";
      break;

    case UFAxisTypeY:
      *name = "Y";
      break;

    case UFAxisTypeTouchMajor:
      *name = "Touch major";
      break;

    case UFAxisTypeTouchMinor:
      *name = "Touch minor";
      break;

    case UFAxisTypeWidthMajor:
      *name = "Width major";
      break;

    case UFAxisTypeWidthMinor:
      *name = "Width minor";
      break;

    case UFAxisTypeOrientation:
      *name = "Orientation";
      break;

    case UFAxisTypeTool:
      *name = "Tool";
      break;

    case UFAxisTypeBlobId:
      *name = "Blob ID";
      break;

    case UFAxisTypeTrackingId:
      *name = "Tracking ID";
      break;

    case UFAxisTypePressure:
      *name = "Pressure";
      break;

    case UFAxisTypeDistance:
      *name = "Distance";
      break;

    default:
      *name = "Unknown";
      break;
  }

  *min = frame_axis_get_minimum(axis);
  *max = frame_axis_get_maximum(axis);
  *res = frame_axis_get_resolution(axis);
}

void print_device_added(UFHandle handle, UFEvent event) {
  UFDevice device;
  char *string = NULL;
  UFStatus status;
  int num_axes = 0;
  int integer;
  int i;

  status = frame_event_get_property(event, UFEventPropertyDevice, &device);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Error: failed to get device from event\n");
    return;
  }
    
  printf("Device added:\n");

  printf("  Time: %ju ms\n", frame_event_get_time(event));

  status = frame_device_get_property(device, UFDevicePropertyName, &string);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get name from device\n");
  else
    printf("  Name: %s\n", string);

  status = frame_device_get_property(device, UFDevicePropertyDirect, &integer);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get direct property from device\n");
  else
    printf("  Direct: %s\n", integer ? "yes" : "no");

  status = frame_device_get_property(device, UFDevicePropertyIndependent,
                                     &integer);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get independent property from device\n");
  else
    printf("  Independent: %s\n", integer ? "yes" : "no");

  status = frame_device_get_property(device, UFDevicePropertySemiMT, &integer);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get semi-MT property from device\n");
  else
    printf("  Semi-MT: %s\n", integer ? "yes" : "no");

  status = frame_device_get_property(device, UFDevicePropertyMaxTouches,
                                     &integer);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get max touches from device\n");
  else
    printf("  Maximum touches: %d\n", integer);

  num_axes = frame_device_get_num_axes(device);
  printf("  Number of axes: %d\n", num_axes);

  for (i = 0; i < num_axes; ++i) {
    UFAxis axis;
    UFAxisType type;
    const char *name;
    float min;
    float max;
    float res;

    printf("  Axis %d:\n", i);

    status = frame_device_get_axis_by_index(device, i, &axis);
    if (status != UFStatusSuccess) {
      fprintf(stderr, "Error: failed to get axis %d from device\n", i);
      continue;
    }

    get_axis_info(axis, &type, &name, &min, &max, &res);
    printf("    Type: %s\n", name);
    printf("    Minimum: %f\n", min);
    printf("    Maximum: %f\n", max);
    printf("    Resolution: %f\n", res);
  }

  printf("\n");
}

void print_device_removed(UFHandle handle, UFEvent event) {
  UFDevice device;
  char *string = NULL;
  UFStatus status;

  status = frame_event_get_property(event, UFEventPropertyDevice, &device);
  if (status != UFStatusSuccess) {
    fprintf(stderr, "Error: failed to get device from event\n");
    return;
  }
    
  printf("Device removed:\n");

  printf("  Time: %ju ms\n", frame_event_get_time(event));

  status = frame_device_get_property(device, UFDevicePropertyName, &string);
  if (status != UFStatusSuccess)
    fprintf(stderr, "Error: failed to get name from device\n");
  else
    printf("  Name: %s\n", string);
}
