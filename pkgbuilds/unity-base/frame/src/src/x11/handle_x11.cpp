/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2011-2012 Canonical Ltd.
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
 *
 ****************************************************************************/

#include "x11/handle_x11.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdexcept>

#include "oif/frame.h"
#include "event.h"
#include "value.h"
#include "x11/device_x11.h"

namespace oif {
namespace frame {

UFHandleX11::UFHandleX11(Display *display)
    : display_(display),
      xi2_opcode_(-1),
      devices_() {
  int event;
  int error;
  if (!XQueryExtension(display_, "XInputExtension", &xi2_opcode_, &event,
                       &error)) {
    fprintf(stderr, "Error: failed to query XInput extension\n");
    throw std::runtime_error("Failed to query XInput extension");
  }

  XIDeviceInfo* devices;
  int num_devices;
  devices = XIQueryDevice(display_, XIAllDevices, &num_devices);
  for (int i = 0; i < num_devices; ++i)
    AddDevice(devices[i], 0);
  
  XIFreeDeviceInfo(devices);
}

void UFHandleX11::AddDevice(const XIDeviceInfo& info, uint64_t time) {
  if (info.use != XISlavePointer)
    return;

  /* If the device is not attached to a master pointer, ignore */
  if (!info.attachment)
    return;

  bool is_touch = false;
  for (int i = 0; i < info.num_classes; i++) {
    if (info.classes[i]->type == XITouchClass) {
      is_touch = true;
      break;
    }
  }

  if (!is_touch)
    return;

  SharedUFDevice device(new UFDeviceX11(display_, info));
  devices_[info.deviceid] = device;

  const Value* value = new Value(device);
  UFEvent* event = new UFEvent(UFEventTypeDeviceAdded, value, time);

  EnqueueEvent(event);
}

UFStatus UFHandleX11::ProcessEvent(XGenericEventCookie* xcookie) {
  if (xcookie->extension != xi2_opcode_)
    return UFStatusSuccess;

  switch (xcookie->evtype) {
    case XI_HierarchyChanged: {
      const XIHierarchyEvent* hierarchy_event =
        reinterpret_cast<const XIHierarchyEvent*>(xcookie->data);
      HandleHierarchyEvent(hierarchy_event);
      break;
    }

    case XI_TouchBegin: {
    case XI_TouchUpdate:
    case XI_TouchEnd:
      const XIDeviceEvent* device_event =
        reinterpret_cast<const XIDeviceEvent*>(xcookie->data);
      HandleDeviceEvent(device_event);
      break;
    }

    case XI_TouchOwnership: {
      const XITouchOwnershipEvent* ownership_event =
        reinterpret_cast<const XITouchOwnershipEvent*>(xcookie->data);
      HandleOwnershipEvent(ownership_event);
      break;
    }
  }

  return UFStatusSuccess;
}

void UFHandleX11::HandleHierarchyEvent(const XIHierarchyEvent* event) {
  for (int i = 0; i < event->num_info; ++i) {
    if (event->info[i].flags == XISlaveAdded ||
        event->info[i].flags == XISlaveAttached) {
      int num_devices;
      XIDeviceInfo* devices = XIQueryDevice(display_,
                                                  event->info[i].deviceid,
                                                  &num_devices);

      if (num_devices == 1) {
        AddDevice(devices[0], event->time);
        XIFreeDeviceInfo(devices);
      }
    } else if (event->info[i].flags == XISlaveRemoved ||
               event->info[i].flags == XISlaveDetached) {
      if (devices_.find(event->info[i].deviceid) == devices_.end())
        return;

      SharedUFDevice& device = devices_[event->info[i].deviceid];
      const Value* value = new Value(device);
      UFEvent* frame_event = new UFEvent(UFEventTypeDeviceRemoved, value,
                                         event->time);

      EnqueueEvent(frame_event);

      UFDeviceX11* device_x11 = static_cast<UFDeviceX11*>(device.get());
      device_x11->ReleaseFrames();

      devices_.erase(event->info[i].deviceid);
    }
  }
}

void UFHandleX11::HandleDeviceEvent(const XIDeviceEvent* event) {
  if (devices_.find(event->sourceid) == devices_.end())
    return;

  UFDevice* device = devices_[event->sourceid].get();
  UFDeviceX11* device_x11 = static_cast<UFDeviceX11*>(device);

  SharedUFFrame frame;
  if (device_x11->HandleDeviceEvent(event, &frame)) {
    const Value* value = new Value(frame);
    UFEvent* frame_event = new UFEvent(UFEventTypeFrame, value, event->time);
    EnqueueEvent(frame_event);
  }
}

void UFHandleX11::HandleOwnershipEvent(const XITouchOwnershipEvent* event) {
  if (devices_.find(event->sourceid) == devices_.end())
    return;

  UFDevice* device = devices_[event->sourceid].get();
  UFDeviceX11* device_x11 = static_cast<UFDeviceX11*>(device);

  SharedUFFrame frame;
  if (device_x11->HandleOwnershipEvent(event, &frame)) {
    const Value* value = new Value(frame);
    UFEvent* frame_event = new UFEvent(UFEventTypeFrame, value, event->time);
    EnqueueEvent(frame_event);
  }
}

UFHandleX11::~UFHandleX11() {
  for (auto& pair : devices_) {
    UFDeviceX11* device = static_cast<UFDeviceX11*>(pair.second.get());
    device->ReleaseFrames();
  }
  devices_.clear();
}

} // namespace frame
} // namespace oif
