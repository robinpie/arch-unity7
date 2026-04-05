/**
 * @file libgeis/geis_region.c
 * @brief implementation of the GEIS v2.0 API Input Device module
 *
 * Copyright 2010, 2010 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_config.h"
#include "geis_device.h"

#include "geis_atomic.h"
#include "geis_attr.h"
#include "geis_logging.h"
#include <stdlib.h>
#include <string.h>


struct _GeisDevice
{
  GeisRefCount ref_count;
  GeisAttrBag  attr_bag;
};

struct _GeisDeviceBag
{
  GeisDevice *device_store;
  GeisSize    device_store_size;
  GeisSize    device_count;
};

static const int device_bag_growth_constant = 2;


GeisDeviceBag
geis_device_bag_new()
{
  GeisDeviceBag bag = calloc(1, sizeof(struct _GeisDeviceBag));
  if (!bag)
  {
    geis_error("error allocating device bag");
    goto final_exit;
  }

  bag->device_store = calloc(1, sizeof(struct _GeisDevice));
  if (!bag->device_store)
  {
    geis_error("error allocating device bag store");
    goto unwind_bag;
  }

  bag->device_store_size = 1;
  bag->device_count = 0;
  goto final_exit;

unwind_bag:
  free(bag);
  bag = NULL;
final_exit:
  return bag;
}


void
geis_device_bag_delete(GeisDeviceBag bag)
{
  GeisSize i;
  for (i = bag->device_count; i > 0; --i)
  {
    geis_device_unref(bag->device_store[i-1]);
  }
  free(bag->device_store);
  free(bag);
}


GeisSize
geis_device_bag_count(GeisDeviceBag bag)
{
  return bag->device_count;
}


GeisDevice
geis_device_bag_device(GeisDeviceBag bag, GeisSize index)
{
  GeisDevice device = NULL;
  if (index >= bag->device_count)
  {
    geis_warning("device bag index out of range");
  }
  else
  {
    device = bag->device_store[index];
  }
  return device;
}


GeisStatus
geis_device_bag_insert(GeisDeviceBag bag, GeisDevice device)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  for (GeisSize i = 0; i < bag->device_count; ++i)
  {
    if (bag->device_store[i] == device)
    {
      geis_device_ref(device);
      goto final_exit;
    }
  }

  if (bag->device_count >= bag->device_store_size)
  {
    GeisSize new_store_size = bag->device_store_size * device_bag_growth_constant;
    GeisDevice *new_store = realloc(bag->device_store,
             new_store_size * sizeof(struct _GeisDevice));
    if (!new_store)
    {
      geis_error("failed to reallocate device bag");
      goto final_exit;
    }
    bag->device_store = new_store;
    bag->device_store_size = new_store_size;
  }
  bag->device_store[bag->device_count++] = geis_device_ref(device);
  status = GEIS_STATUS_SUCCESS;

final_exit:
  return status;
}


GeisStatus
geis_device_bag_remove(GeisDeviceBag bag, GeisDevice device)
{
  GeisSize i;
  GeisStatus status = GEIS_STATUS_SUCCESS;
  for (i = 0; i < bag->device_count; ++i)
  {
    if (bag->device_store[i] == device)
    {
      GeisSize j;
      geis_device_unref(bag->device_store[i]);
      --bag->device_count;
      for (j = i; j < bag->device_count; ++j)
      {
	bag->device_store[j] = bag->device_store[j+1];
      }
      break;
    }
  }
  return status;
}


/*
 * Creates a new, empty device.
 */
GeisDevice
geis_device_new(GeisString name, GeisInteger id)
{
  GeisAttr attr;
  GeisDevice device = calloc(1, sizeof(struct _GeisDevice));
  if (!device)
  {
    geis_error("error allocating input device");
    goto final_exit;
  }

  device->attr_bag = geis_attr_bag_new(4);
  if (!device->attr_bag)
  {
    geis_debug("error allocating attr bag");
    goto unwind_device;
  }

  attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_NAME,
                       GEIS_ATTR_TYPE_STRING,
                       (void *)name);
  if (!attr)
  {
    geis_debug("error allocating device name attr");
    goto unwind_attrs;
  }
  geis_attr_bag_insert(device->attr_bag, attr);

  attr = geis_attr_new(GEIS_DEVICE_ATTRIBUTE_ID,
                       GEIS_ATTR_TYPE_INTEGER,
                       &id);
  if (!attr)
  {
    geis_debug("error allocating device id attr");
    goto unwind_attrs;
  }
  geis_attr_bag_insert(device->attr_bag, attr);

  geis_device_ref(device);
  goto final_exit;

unwind_attrs:
  geis_attr_bag_delete(device->attr_bag);
unwind_device:
  free(device);
  device = NULL;
final_exit:
  return device;
}


/*
 * Destroys a device.
 */
static void
_device_delete(GeisDevice device)
{
  geis_attr_bag_delete(device->attr_bag);
  free(device);
}


/*
 * Adds a reference count to a device.
 */
GeisDevice
geis_device_ref(GeisDevice device)
{
  geis_atomic_ref(&device->ref_count);
  return device;
}


/*
 * Removes a reference count from a device.
 */
void
geis_device_unref(GeisDevice device)
{
  if (0 ==  geis_atomic_unref(&device->ref_count))
  {
    _device_delete(device);
  }
}


/*
 * Gets the name of the input device.
 */
GeisString
geis_device_name(GeisDevice device)
{
  GeisString device_name = NULL;
  GeisAttr name_attr = geis_attr_bag_find(device->attr_bag,
                                          GEIS_DEVICE_ATTRIBUTE_NAME);
  if (name_attr)
  {
    device_name = geis_attr_value_to_string(name_attr);
  }
  return device_name;
}


/*
 * Gets the system identifier of the iput device.
 */
GeisInteger
geis_device_id(GeisDevice device)
{
  GeisInteger device_id = -1;
  GeisAttr attr = geis_attr_bag_find(device->attr_bag, GEIS_DEVICE_ATTRIBUTE_ID);
  if (attr)
  {
    device_id = geis_attr_value_to_integer(attr);
  }
  return device_id;
}


/*
 * Gets the number of attributes of the device.
 */
GeisSize
geis_device_attr_count(GeisDevice device)
{
  return geis_attr_bag_count(device->attr_bag);
}


/*
 * Inserts an attr into a device.
 */
void
geis_device_add_attr(GeisDevice device, GeisAttr attr)
{
  geis_attr_bag_insert(device->attr_bag, attr);
}


/*
 * Gets the indicated attribute of the device.
 */
GeisAttr
geis_device_attr(GeisDevice device, GeisSize index)
{
  return geis_attr_bag_attr(device->attr_bag, index);
}

/*
 * Gets a named attr.
 */
GeisAttr
geis_device_attr_by_name(GeisDevice device, GeisString attr_name)
{
  return geis_attr_bag_find(device->attr_bag, attr_name);
}
