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

#include "device.h"

#include "axis.h"

#include <oif/frame_backend.h>

#include <cstdio>

namespace oif {
namespace frame {

UFStatus UFDevice::GetAxisByIndex(unsigned int index, ::UFAxis* axis) const {
  if (index >= axes_.size())
    return UFStatusErrorInvalidAxis;

  auto it = axes_.cbegin();
  std::advance(it, index);

  *axis = it->second.get();

  return UFStatusSuccess;
}

UFStatus UFDevice::GetAxisByType(UFAxisType type, ::UFAxis* axis) const {
  auto it = axes_.find(type);
  if (it == axes_.end())
    return UFStatusErrorInvalidAxis;

  *axis = it->second.get();

  return UFStatusSuccess;
}

UFStatus UFDevice::AcceptTouch(UFWindowId window_id, UFTouchId touch_id)
{
    (void)window_id;
    (void)touch_id;
    return UFStatusSuccess;
}

UFStatus UFDevice::RejectTouch(UFWindowId window_id, UFTouchId touch_id)
{
    (void)window_id;
    (void)touch_id;
    return UFStatusSuccess;
}

} // namespace frame
} // namespace oif

extern "C" {

FRAME_PUBLIC
UFStatus frame_device_get_property_string_(UFDevice device,
                                           UFDeviceProperty property,
                                           char **value) {
  return static_cast<const oif::frame::UFDevice*>(device)->GetProperty(
      property,
      value);
}

FRAME_PUBLIC
UFStatus frame_device_get_property_int_(UFDevice device,
                                        UFDeviceProperty property, int *value) {
  return static_cast<const oif::frame::UFDevice*>(device)->GetProperty(
      property,
      value);
}

FRAME_PUBLIC
UFStatus frame_device_get_property_unsigned_int_(UFDevice device,
                                                 UFDeviceProperty property,
                                                 unsigned int *value) {

  if (property == UFDevicePropertyNumAxes) {
    *value = static_cast<const oif::frame::UFDevice*>(device)->axes_.size();
    return UFStatusSuccess;
  } else {
    return static_cast<const oif::frame::UFDevice*>(device)->GetProperty(
        property,
        value);
  }
}

#undef frame_device_get_property /* Override C11 generic selections macro */
FRAME_PUBLIC
UFStatus frame_device_get_property(UFDevice device, UFDeviceProperty property,
                                   void *value) {
  if (property == UFDevicePropertyNumAxes) {
    *reinterpret_cast<unsigned int *>(value) =
      static_cast<const oif::frame::UFDevice*>(device)->axes_.size();
    return UFStatusSuccess;
  } else {
    return static_cast<const oif::frame::UFDevice*>(device)->GetProperty(
        property,
        value);
  }
}

UFStatus frame_device_get_axis_by_index(UFDevice device, unsigned int index,
                                        UFAxis* axis) {
  return static_cast<const oif::frame::UFDevice*>(device)->GetAxisByIndex(
      index,
      axis);
}

UFStatus frame_device_get_axis_by_type(UFDevice device, UFAxisType type,
                                       UFAxis* axis) {
  return static_cast<const oif::frame::UFDevice*>(device)->GetAxisByType(
      type,
      axis);
}

unsigned int frame_device_get_num_axes(UFDevice device) {
  unsigned int num_axes;
  UFStatus status = frame_device_get_property(device, UFDevicePropertyNumAxes,
                                              &num_axes);
  if (status == UFStatusSuccess)
    return num_axes;

  fprintf(stderr, "Error: failed to get device number of axes\n");
  return 0;
}

float frame_device_get_window_resolution_x(UFDevice device) {
  float resolution;
  UFStatus status = frame_device_get_property(device,
                                              UFDevicePropertyWindowResolutionX,
                                              &resolution);
  if (status == UFStatusSuccess)
    return resolution;

  fprintf(stderr, "Error: failed to get device X resolution\n");
  return 0;
}

float frame_device_get_window_resolution_y(UFDevice device) {
  float resolution;
  UFStatus status = frame_device_get_property(device,
                                              UFDevicePropertyWindowResolutionY,
                                              &resolution);
  if (status == UFStatusSuccess)
    return resolution;

  fprintf(stderr, "Error: failed to get device Y resolution\n");
  return 0;
}

UFStatus frame_accept_touch(UFDevice device, UFWindowId window_id,
                            UFTouchId touch_id)
{
  return static_cast<oif::frame::UFDevice*>(device)->
      AcceptTouch(window_id, touch_id);
}

UFStatus frame_reject_touch(UFDevice device, UFWindowId window_id,
                            UFTouchId touch_id)
{
  return static_cast<oif::frame::UFDevice*>(device)->
      RejectTouch(window_id, touch_id);
}

UFBackendDevice frame_backend_device_new()
{
  return new UFBackendDevice_(new oif::frame::UFDevice);
}

UFDevice frame_backend_device_get_device(UFBackendDevice device)
{
  return device->shared_ptr.get();
}

void frame_backend_device_delete(UFBackendDevice device)
{
  delete device;
}

void frame_backend_device_set_name(UFBackendDevice device, const char *name)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyName,
                   new oif::frame::Value(name));
}

void frame_backend_device_set_direct(UFBackendDevice device, int direct)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyDirect,
                   new oif::frame::Value(direct));
}

void frame_backend_device_set_independent(UFBackendDevice device, int independent)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyIndependent,
                   new oif::frame::Value(independent));
}

void frame_backend_device_set_semi_mt(UFBackendDevice device, int semi_mt)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertySemiMT,
                   new oif::frame::Value(semi_mt));
}

void frame_backend_device_set_max_touches(UFBackendDevice device, unsigned int max_touches)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyMaxTouches,
                   new oif::frame::Value(max_touches));
}

void frame_backend_device_set_window_resolution(UFBackendDevice device, float x, float y)
{
  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyWindowResolutionX,
                   new oif::frame::Value(x));

  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    InsertProperty(UFDevicePropertyWindowResolutionY,
                   new oif::frame::Value(y));
}

void frame_backend_device_add_axis(UFBackendDevice device,
                           UFAxisType type,
                           float min, float max, float resolution)
{
  using oif::frame::UFAxis;

  UFAxis_* axis = new UFAxis(type, min, max, resolution);

  static_cast<oif::frame::UFDevice*>(device->shared_ptr.get())->
    axes_[type] = std::unique_ptr<UFAxis>(static_cast<UFAxis*>(axis));
}

} // extern "C"
