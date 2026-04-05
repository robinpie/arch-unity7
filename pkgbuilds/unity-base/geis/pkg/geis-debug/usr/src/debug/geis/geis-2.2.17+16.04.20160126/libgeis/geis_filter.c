/**
 * @file libgeis/geis_filter.c
 * @brief implementation of the GEIS v2.0 API filter module
 *
 * Copyright 2010, 2011 Canonical Ltd.
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
#include "geis_filter.h"

#include "geis_atomic.h"
#include "geis_attr.h"
#include "geis_error.h"
#include "geis_filter_term.h"
#include "geis_logging.h"
#include "geis_private.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


struct _GeisFilter
{
  GeisRefCount      refcount;
  GeisString        name;
  Geis              geis;
  GeisSize          oid;
  GeisBackendToken  backend_token;
  GeisFilterTermBag terms;
};

struct _GeisFilterBag
{
  GeisFilter *filter_store;
  GeisSize    filter_store_size;
  GeisSize    filter_count;
};

static const int filter_bag_growth_constant = 2;

static GeisSize s_filter_oid = 0;


/*
 * Creates a new filter bag,
 */
GeisFilterBag
geis_filter_bag_new()
{
  GeisFilterBag bag = calloc(1, sizeof(struct _GeisFilterBag));
  if (!bag)
  {
    geis_error("failed to allocate filter bag");
    goto final_exit;
  }

  bag->filter_store_size = 3;
  bag->filter_count = 0;
  bag->filter_store = calloc(bag->filter_store_size, sizeof(GeisFilter));
  if (!bag->filter_store)
  {
    geis_error("failed to allocate filter bag store");
    goto unwind_bag;
  }
  goto final_exit;

unwind_bag:
  free(bag);
final_exit:
  return bag;
}


/*
 * Destroys a filter bag.
 */
void
geis_filter_bag_delete(GeisFilterBag bag)
{
  GeisSize i;
  for (i = bag->filter_count; i > 0; --i)
  {
    geis_filter_delete(bag->filter_store[i-1]);
  }
  free(bag->filter_store);
  free(bag);
}


/*
 * Gets the number of filters in the bag.
 */
GeisSize
geis_filter_bag_count(GeisFilterBag bag)
{
  return bag->filter_count;
}


/*
 * Gets an indicated filter from a bag.
 */
GeisFilter
geis_filter_bag_filter(GeisFilterBag bag, GeisSize index)
{
  GeisFilter filter = NULL;
  if (index < bag->filter_count)
  {
    filter = bag->filter_store[index];
  }
  return filter;
}


/*
 * Gets an indicated filter from a bag.
 */
GeisFilter
geis_filter_bag_filter_by_name(GeisFilterBag bag, GeisString name)
{
  GeisFilter filter = NULL;
  GeisSize i;
  for (i = 0; i < bag->filter_count; ++i)
  {
    if (0 == strcmp(bag->filter_store[i]->name, name))
    {
      filter = bag->filter_store[i];
      break;
    }
  }
  return filter;
}
      

/*
 * Gets an iterator initialized to the first filter contained in the bag.
 */
GeisFilterIterator
geis_filter_bag_begin(GeisFilterBag bag)
{
  if (bag->filter_count > 0)
    return (GeisFilterIterator)bag->filter_store;
  return geis_filter_bag_end(bag);
}


/*
 * * Gets an iterator initialized to one-past-the-end of the filter bag.
 */
GeisFilterIterator
geis_filter_bag_end(GeisFilterBag bag GEIS_UNUSED)
{
  return NULL;
}


/*
 * Advances the iterator to the next filter in the bag.
 */
GeisFilterIterator
geis_filter_iterator_next(GeisFilterBag bag, GeisFilterIterator iter)
{
  GeisFilterIterator new_iter = iter + 1;
  if ((GeisSize)(new_iter - bag->filter_store) < bag->filter_count)
    return new_iter;
  return geis_filter_bag_end(bag);
}


/*
 * Inserts a filter in the bag.
 */
GeisStatus
geis_filter_bag_insert(GeisFilterBag bag, GeisFilter filter)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->filter_count >= bag->filter_store_size)
  {
    GeisSize new_store_size = bag->filter_store_size * filter_bag_growth_constant;
    GeisFilter *new_store = realloc(bag->filter_store,
             new_store_size * sizeof(struct _GeisFilter));
    if (!new_store)
    {
      geis_error("failed to reallocate filter bag");
      goto error_exit;
    }
    bag->filter_store = new_store;
    bag->filter_store_size = new_store_size;
  }
  bag->filter_store[bag->filter_count++] = geis_filter_ref(filter);
  status = GEIS_STATUS_SUCCESS;
  goto final_exit;

error_exit:
  geis_filter_unref(filter);
final_exit:
  return status;
}


/*
 * Remoes a filter from the bag.
 */
GeisStatus
geis_filter_bag_remove(GeisFilterBag bag, GeisFilter filter)
{
  GeisSize i;
  GeisStatus status = GEIS_STATUS_SUCCESS;
  for (i = 0; i < bag->filter_count; ++i)
  {
    if (bag->filter_store[i] == filter)
    {
      GeisSize j;
      geis_filter_delete(bag->filter_store[i]);
      --bag->filter_count;
      for (j = i; j < bag->filter_count; ++j)
      {
	bag->filter_store[j] = bag->filter_store[j+1];
      }
      break;
    }
  }
  return status;
} 


/*
 * Creates a new, empty filter object.
 */
static GeisFilter
_filter_new_empty(GeisString name)
{
  GeisFilter filter = calloc(1, sizeof(struct _GeisFilter));
  if (!filter)
  {
    geis_error("error allocating filter");
    goto final_exit;
  }

  if (name)
  {
    filter->name = strdup(name);
  }
  else
  {
    filter->name = strdup("");
  }
  if (!filter->name)
  {
    geis_error("error allocating filter name");
    goto unwind_filter;
  }
  filter->oid = s_filter_oid++;
  goto final_exit;

unwind_filter:
  free(filter);
  filter = NULL;
final_exit:
  return filter;
}


/*
 * Creates a GEIS v2.0 filter object.
 */
GeisFilter
geis_filter_new(Geis geis, GeisString name)
{
  GeisFilter filter = _filter_new_empty(name);
  if (!filter)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    goto final_exit;
  }

  filter->terms = geis_filter_term_bag_new(0);
  if (!filter->terms)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating filter terms");
    goto unwind_filter;
  }

  filter->backend_token = geis_backend_token_new(geis,
                                                 GEIS_BACKEND_TOKEN_INIT_ALL);
  if (!filter->backend_token)
  {
    geis_error_push(geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating filter token");
    goto unwind_term_bag;
  }

  filter->geis = geis_ref(geis);
  geis_filter_ref(filter);
  goto final_exit;

unwind_term_bag:
  geis_filter_term_bag_delete(filter->terms);
unwind_filter:
  free((char *)filter->name);
  free(filter);
  filter = NULL;
final_exit:
  return filter;
}


/*
 * Creates a new filter by copying an existing filter.
 */
GeisFilter
geis_filter_clone(GeisFilter original, GeisString name)
{
  GeisFilter filter = _filter_new_empty(name);
  if (!filter)
  {
    geis_error_push(original->geis, GEIS_STATUS_UNKNOWN_ERROR);
    goto final_exit;
  }

  filter->terms = geis_filter_term_bag_clone(original->terms);
  if (!filter->terms)
  {
    geis_error_push(original->geis, GEIS_STATUS_UNKNOWN_ERROR);
    geis_error("error allocating filter terms");
    goto unwind_filter;
  }

  filter->geis = geis_ref(original->geis);
  filter->backend_token = geis_backend_token_clone(original->backend_token);
  geis_filter_ref(filter);
  goto final_exit;

unwind_filter:
  free((char *)filter->name);
  free(filter);
  filter = NULL;
final_exit:
  return filter;
}


/*
 * Destroys a GEIS v2.0 filter object.
 */
GeisStatus
geis_filter_delete(GeisFilter filter)
{
  GeisStatus status = GEIS_STATUS_SUCCESS;

  if (filter && (geis_filter_unref(filter) == 0))
  {
    geis_unref(filter->geis);
    geis_backend_token_delete(filter->backend_token);
    geis_filter_term_bag_delete(filter->terms);
    free((char *)filter->name);
    free(filter);
  }
  return status;
}


/*
 * Gets the name given to the filter when it was created.
 */
GeisString
geis_filter_name(GeisFilter filter)
{
  return filter->name;
}


/*
 * Indicates if the facility is valid.
 */
static GeisBoolean
_facility_is_valid(GeisFilterFacility facility)
{
  return facility == GEIS_FILTER_DEVICE
    ||   facility == GEIS_FILTER_CLASS
    ||   facility == GEIS_FILTER_REGION
    ||   facility == GEIS_FILTER_SPECIAL;
}


/*
 * Indicates if the operation is valid.
 */
static GeisBoolean
_operation_is_valid(GeisFilterOperation op)
{
  return op == GEIS_FILTER_OP_EQ
    ||   op == GEIS_FILTER_OP_NE
    ||   op == GEIS_FILTER_OP_GT
    ||   op == GEIS_FILTER_OP_GE
    ||   op == GEIS_FILTER_OP_LT
    ||   op == GEIS_FILTER_OP_LE;
}


/*
 * Gets the attr description by name.
 * @todo implement this function
 */
static GeisAttrType
_get_attr_type_for_facility(Geis               geis,
                            GeisFilterFacility facility,
                            GeisString         attr_name)
{
  GeisAttrType type = GEIS_ATTR_TYPE_UNKNOWN;
  switch (facility)
  {
    case GEIS_FILTER_DEVICE:
      type = geis_get_device_attr_type(geis, attr_name);
      break;
    case GEIS_FILTER_CLASS:
      type = geis_get_class_attr_type(geis, attr_name);
      break;
    case GEIS_FILTER_REGION:
      type = geis_get_region_attr_type(geis, attr_name);
      break;
    case GEIS_FILTER_SPECIAL:
      type = geis_get_special_attr_type(geis, attr_name);
      break;
    default:
      break;
  }
  return type;
}


GeisStatus
geis_filter_add_term_internal(GeisFilter filter, GeisFilterTerm term)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisAttr attr = geis_filter_term_attr(term);

  geis_filterable_attribute_foreach(filter->geis,
                                    geis_filter_term_facility(term),
                                    filter->backend_token,
                                    geis_attr_name(attr),
                                    geis_filter_term_operation(term),
                                    geis_attr_value(attr));
  geis_filter_term_bag_insert(filter->terms, term);

  return status;
}


/*
 * Adds zero or more terms to a filter.
 */
GeisStatus
geis_filter_add_term(GeisFilter         filter,
                     GeisFilterFacility facility,
                                        ...)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  va_list    varargs;
  GeisString attr_name;

  if (!_facility_is_valid(facility))
  {
    status = GEIS_STATUS_BAD_ARGUMENT;
    geis_error_push(filter->geis, status);
    geis_error("invalid filter facility");
    goto final_exit;
  }

  va_start(varargs, facility);
  for (attr_name = va_arg(varargs, GeisString);
       attr_name;
       attr_name = va_arg(varargs, GeisString))
  {
    GeisAttrType attr_type = _get_attr_type_for_facility(filter->geis,
                                                         facility,
                                                         attr_name);
    if (attr_type == GEIS_ATTR_TYPE_UNKNOWN)
    {
      status = GEIS_STATUS_BAD_ARGUMENT;
      geis_error_push(filter->geis, status);
      geis_error("invalid attr name \"%s\" for facility", attr_name);
      goto final_exit;
    }

    GeisFilterOperation op = va_arg(varargs, GeisFilterOperation);
    if (!_operation_is_valid(op))
    {
      status = GEIS_STATUS_BAD_ARGUMENT;
      geis_error_push(filter->geis, status);
      geis_error("invalid filter operation");
      goto final_exit;
    }

    switch (attr_type)
    {
      case GEIS_ATTR_TYPE_BOOLEAN:
	{
	  GeisBoolean value = va_arg(varargs, GeisBoolean);
	  geis_filterable_attribute_foreach(filter->geis,
	                                    facility,
	                                    filter->backend_token,
	                                    attr_name,
	                                    op,
	                                    &value);
          GeisAttr attr = geis_attr_new(attr_name, attr_type, &value);
          GeisFilterTerm term = geis_filter_term_new(facility, op, attr);
          geis_filter_term_bag_insert(filter->terms, term);
	}
	break;

      case GEIS_ATTR_TYPE_FLOAT:
	{
	  GeisFloat value = va_arg(varargs, double);
	  geis_filterable_attribute_foreach(filter->geis,
	                                    facility,
	                                    filter->backend_token,
	                                    attr_name,
	                                    op,
	                                    &value);
          GeisAttr attr = geis_attr_new(attr_name, attr_type, &value);
          GeisFilterTerm term = geis_filter_term_new(facility, op, attr);
          geis_filter_term_bag_insert(filter->terms, term);
	}
	break;

      case GEIS_ATTR_TYPE_INTEGER:
	{
	  GeisInteger value = va_arg(varargs, GeisInteger);
	  geis_filterable_attribute_foreach(filter->geis,
	                                    facility,
	                                    filter->backend_token,
	                                    attr_name,
	                                    op,
	                                    &value);
          GeisAttr attr = geis_attr_new(attr_name, attr_type, &value);
          GeisFilterTerm term = geis_filter_term_new(facility, op, attr);
          geis_filter_term_bag_insert(filter->terms, term);
	}
	break;

      case GEIS_ATTR_TYPE_POINTER:
	{
	  GeisPointer value = va_arg(varargs, GeisPointer);
	  geis_filterable_attribute_foreach(filter->geis,
	                                    facility,
	                                    filter->backend_token,
	                                    attr_name,
	                                    op,
	                                    value);
          GeisAttr attr = geis_attr_new(attr_name, attr_type, value);
          GeisFilterTerm term = geis_filter_term_new(facility, op, attr);
          geis_filter_term_bag_insert(filter->terms, term);
	}
	break;

      case GEIS_ATTR_TYPE_STRING:
	{
	  GeisString value = va_arg(varargs, GeisString);
	  geis_filterable_attribute_foreach(filter->geis,
	                                    facility,
	                                    filter->backend_token,
	                                    attr_name,
	                                    op,
	                                    (void *)value);
          GeisAttr attr = geis_attr_new(attr_name, attr_type, (void *)value);
          GeisFilterTerm term = geis_filter_term_new(facility, op, attr);
          geis_filter_term_bag_insert(filter->terms, term);
	}
	break;

      default:
	status = GEIS_STATUS_BAD_ARGUMENT;
	geis_error_push(filter->geis, status);
	geis_error("invalid filter argument");
	goto final_exit;
	break;
    }
  }
  va_end(varargs);
  status = GEIS_STATUS_SUCCESS;

final_exit:
  return status;
}


/*
 * Atomically increments filter refcount.
 */
GeisFilter
geis_filter_ref(GeisFilter filter)
{
  geis_atomic_ref(&filter->refcount);
  return filter;
}


/*
 * Atomically decrements and returns filter refcount.
 */
GeisSize
geis_filter_unref(GeisFilter filter)
{
  return geis_atomic_unref(&filter->refcount);
}


/*
 * Gets the number of terms in the filter.
 */
GeisSize
geis_filter_term_count(GeisFilter filter)
{
  return geis_filter_term_bag_count(filter->terms);
}


/*
 * Gets the indicated term in the filter.
 */
GeisFilterTerm
geis_filter_term(GeisFilter filter, GeisSize index)
{
  return geis_filter_term_bag_term(filter->terms, index);
}

/*
 * Gets the back end token from the filter.
 */
GeisBackendToken
geis_filter_token(GeisFilter filter)
{
  return filter->backend_token;
}


/*
 * Performs the filter function on a Geis event.
 */
GeisBoolean
geis_filter_pass_event(GeisFilter filter, GeisEvent event)
{
  GeisBoolean pass = GEIS_TRUE;
  for (GeisSize i = 0; i < geis_filter_term_bag_count(filter->terms); ++i)
  {
    GeisFilterTerm term = geis_filter_term_bag_term(filter->terms, i);
    pass &= geis_filter_term_match_event(term, event);
  }
  return pass;
}


