/**
 * @file geis_backend.c
 * @brief internal GEIS back end base class implementation
 *
 * Copyright 2010-2013 Canonical Ltd.
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
#include "geis_backend.h"
#include "geis_backend_protected.h"

#include "geis_logging.h"
#include <stdlib.h>
#include <string.h>


typedef struct GeisBackendClass
{
  GeisString        name;
  GeisSize          size;
  GeisBackendVtable vtbl;
} *GeisBackendClass;


struct GeisBackend
{
  GeisBackendClass  be_class;
  char              be_data[1];
};


struct GeisBackendRegistry
{
  GeisBackendClass be;
  GeisSize         size;
  GeisSize         count;
};

static struct GeisBackendRegistry _be_registry = { NULL, 0, 0 };


/*
 * Gets the child part of the back end instance.
 */
static inline void *
_data_from_be(GeisBackend be)
{
  return (void *)be->be_data;
}


/*
 * Initializes a new back end class object.
 */
static void
_backend_class_init(GeisBackendClass  self,
                    GeisString        name,
                    GeisSize          size,
                    GeisBackendVtable vtbl)
{
  self->name = strdup(name);
  self->size = size;
  self->vtbl = vtbl;
}


/*
 * Registers back ends with the API.
 */
void
geis_register_backend(GeisString name, GeisSize size, GeisBackendVtable vtbl)
{
  GeisSize new_size = _be_registry.size + 1;
  GeisBackendClass new_be = realloc(_be_registry.be,
                                    new_size * sizeof(struct GeisBackendClass));
  if (!new_be)
  {
    geis_error("error reallocating back end registry");
    goto final_exit;
  }

  _backend_class_init(&new_be[_be_registry.size], name, size, vtbl);
  geis_debug("back end %zu registered as '%s'", _be_registry.count, name);

  _be_registry.be = new_be;
  ++_be_registry.size;
  ++_be_registry.count;

final_exit:
  return;
}


GeisBackendToken
geis_backend_create_token(GeisBackend be, GeisBackendTokenInitState init_state)
{
  return be->be_class->vtbl->create_token(_data_from_be(be), init_state);
}


/*
 * Creates a back end by name.
 */
GeisBackend
geis_backend_by_name(Geis geis, GeisString name)
{
  GeisBackend backend = NULL;
  GeisSize i;
  extern void geis_include_backend_test_fixture(void);
  extern void geis_include_dbus_backend(void);
  extern void geis_include_grail_backend(void);

  geis_debug("creating back end of class \"%s\"", name);
  /* temporary references to force symbol inclusion, replace with dlopen */
  geis_include_backend_test_fixture();
  geis_include_dbus_backend();
  geis_include_grail_backend();

  for (i = 0; i < _be_registry.count; ++i)
  {
    if (0 == strcmp(_be_registry.be[i].name, name))
    {
      size_t obj_size = sizeof(GeisBackend) + _be_registry.be[i].size;
      backend = malloc(obj_size);
      if (backend)
      {
        *(GeisBackendClass *)(backend) = &_be_registry.be[i];
        _be_registry.be[i].vtbl->construct(_data_from_be(backend), geis);
        if (geis_error_count(geis))
        {
          free(backend);
          backend = NULL;
        }
      }
      break;
    }
  }
  return backend;
}


/*
 * Destroys the back end.
 */
void
geis_backend_delete(GeisBackend be)
{
  if (be)
  {
    geis_debug("destroying back end %s", geis_backend_name(be));
    be->be_class->vtbl->finalize(_data_from_be(be));
    free(be);
  }
}


/*
 * Gets the name of the back end (RTTI).
 */
GeisString
geis_backend_name(GeisBackend be)
{
  return be->be_class->name;
}


GeisStatus
geis_backend_gesture_accept(GeisBackend   be,
                            GeisGroup     group,
                            GeisGestureId gesture_id)
{
  GeisStatus status = be->be_class->vtbl->accept_gesture(_data_from_be(be),
                                                         group,
                                                         gesture_id);
  return status;
}


GeisStatus
geis_backend_gesture_reject(GeisBackend   be,
                            GeisGroup     group,
                            GeisGestureId gesture_id)
{
  GeisStatus status = be->be_class->vtbl->reject_gesture(_data_from_be(be),
                                                         group,
                                                         gesture_id);
  return status;
}


GeisStatus
geis_backend_activate_device(GeisBackend  be,
                             GeisDevice   device)
{
  if (be->be_class->vtbl->activate_device)
    return  be->be_class->vtbl->activate_device(_data_from_be(be), device);
  return GEIS_STATUS_SUCCESS;
}


GeisStatus
geis_backend_deactivate_device(GeisBackend  be,
                               GeisDevice   device)
{
  if (be->be_class->vtbl->deactivate_device)
    return  be->be_class->vtbl->deactivate_device(_data_from_be(be), device);
  return GEIS_STATUS_SUCCESS;
}


/*
 * Gets a back end configuration value.
 */
GeisStatus
geis_backend_get_configuration(GeisBackend      be,
                               GeisSubscription subscription,
                               GeisString       configuration_item_name,
                               GeisPointer      configuration_item_value)
{
  return be->be_class->vtbl->get_configuration(be,
                                               subscription,
                                               configuration_item_name,
                                               configuration_item_value);
}


/*
 * Sets a back end configuration value.
 */
GeisStatus
geis_backend_set_configuration(GeisBackend      be,
                               GeisSubscription subscription,
                               GeisString       configuration_item_name,
                               GeisPointer      configuration_item_value)
{
  return be->be_class->vtbl->set_configuration(be,
                                               subscription,
                                               configuration_item_name,
                                               configuration_item_value);
}

