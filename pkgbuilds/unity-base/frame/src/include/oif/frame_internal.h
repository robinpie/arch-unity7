/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2012 Canonical Ltd.
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

#ifndef FRAME_OIF_FRAME_INTERNAL_H_
#define FRAME_OIF_FRAME_INTERNAL_H_

#ifdef HAS_C_GENERIC_SELECTIONS

#include <stdint.h>

FRAME_PUBLIC
UFStatus frame_device_get_property_string_(UFDevice device,
                                           UFDeviceProperty property,
                                           char **value);
FRAME_PUBLIC
UFStatus frame_device_get_property_int_(UFDevice device,
                                        UFDeviceProperty property, int *value);
FRAME_PUBLIC
UFStatus frame_device_get_property_unsigned_int_(UFDevice device,
                                                 UFDeviceProperty property,
                                                 unsigned int *value);

#define frame_device_get_property(device, property, value) \
    _Generic((value), \
             char **: frame_device_get_property_string_, \
             int *: frame_device_get_property_int_, \
             unsigned int *: frame_device_get_property_unsigned_int_) ((device), \
                                                                       (property), \
                                                                       (value))

FRAME_PUBLIC
UFStatus frame_event_get_property_type_(UFEvent event, UFEventProperty property,
                                        UFEventType *value);
FRAME_PUBLIC
UFStatus frame_event_get_property_device_(UFEvent event,
                                          UFEventProperty property,
                                          UFDevice *value);
FRAME_PUBLIC
UFStatus frame_event_get_property_frame_(UFEvent event,
                                         UFEventProperty property,
                                         UFFrame *value);
FRAME_PUBLIC
UFStatus frame_event_get_property_uint64_(UFEvent event,
                                          UFEventProperty property,
                                          uint64_t *value);

#define frame_event_get_property(event, property, value) \
    _Generic((value), \
             UFEventType *: frame_event_get_property_type_, \
             UFDevice *: frame_event_get_property_device_, \
             UFFrame *: frame_event_get_property_frame_, \
             uint64_t *: frame_event_get_property_uint64_) ((event), \
                                                            (property), (value))

FRAME_PUBLIC
UFStatus frame_frame_get_property_device_(UFFrame frame,
                                          UFFrameProperty property,
                                          UFDevice *value);
FRAME_PUBLIC
UFStatus frame_frame_get_property_uint64_(UFFrame frame,
                                          UFFrameProperty property,
                                          uint64_t *value);
FRAME_PUBLIC
UFStatus frame_frame_get_property_unsigned_int_(UFFrame frame,
                                                UFFrameProperty property,
                                                unsigned int *value);

#define frame_frame_get_property(frame, property, value) \
    _Generic((value), \
             UFDevice *: frame_frame_get_property_device_, \
             uint64_t *: frame_frame_get_property_window_id_, \
             unsigned int *: frame_frame_get_property_unsigned_int_) ( \
                 (frame), \
                 (property), \
                 (value))

FRAME_PUBLIC
UFStatus frame_touch_get_property_uint64_(UFTouch touch,
                                          UFTouchProperty property,
                                          uint64_t *value);
FRAME_PUBLIC
UFStatus frame_touch_get_property_state_(UFTouch touch,
                                         UFTouchProperty property,
                                         UFTouchState *value);
FRAME_PUBLIC
UFStatus frame_touch_get_property_float_(UFTouch touch,
                                         UFTouchProperty property,
                                         float *value);
FRAME_PUBLIC
UFStatus frame_touch_get_property_int_(UFTouch touch, UFTouchProperty property,
                                       int *value);

#define frame_touch_get_property(touch, property, value) \
    _Generic((value), \
             uint64_t *: frame_touch_get_property_uint64_, \
             UFTouchState *: frame_touch_get_property_state_, \
             float *: frame_touch_get_property_float_, \
             int *: frame_touch_get_property_int_) ((touch), (property), \
                                                    (value))

#endif // HAS_C_GENERIC_SELECTIONS

#endif // FRAME_OIF_FRAME_INTERNAL_H_
