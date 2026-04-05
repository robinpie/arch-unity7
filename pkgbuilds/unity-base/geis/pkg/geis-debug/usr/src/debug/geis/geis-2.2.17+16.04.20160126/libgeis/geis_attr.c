/**
 * @file geis_attr.c
 * @brief internal GeisAttr facilities
 *
 * Copyright 2010, 2011 Canonical Ltd.
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include "geis_attr.h"

#include "geis_logging.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct _GeisAttr
{
  GeisString   attr_name;
  GeisAttrType attr_type;
  union
  {
    GeisBoolean  b;
    GeisFloat    f;
    GeisInteger  i;
    GeisString   s;
    void        *p;
  } attr_value;
  void (*attr_destructor)(void* p);
};


struct _GeisAttrBag
{
  GeisAttr *attr_store;
  GeisSize  attr_store_size;
  GeisSize  attr_count;
};

static const float attr_bag_growth_constant = 1.5f;


GeisAttrBag
geis_attr_bag_new(GeisSize size_hint)
{
  GeisAttrBag bag = calloc(1, sizeof(struct _GeisAttrBag));
  if (!bag)
  {
    geis_error("failed to allocate attr bag");
  }
  else
  {
    GeisSize min_size = size_hint > 2 ? size_hint : 2;
    bag->attr_count = 0;
    bag->attr_store_size = min_size;
    bag->attr_store = calloc(bag->attr_store_size, sizeof(GeisAttr));
    if (!bag->attr_store)
    {
      geis_error("failed to allocate attr bag store");
      free(bag);
      bag = NULL;
    }
  }

  return bag;
}


void
geis_attr_bag_delete(GeisAttrBag bag)
{
  GeisSize i;
  for (i = 0; i < bag->attr_count; ++i)
  {
    geis_attr_delete(bag->attr_store[i]);
  }
  free(bag->attr_store);
  free(bag);
}


GeisSize
geis_attr_bag_count(GeisAttrBag bag)
{
  return bag->attr_count;
}


GeisStatus
geis_attr_bag_insert(GeisAttrBag bag, GeisAttr attr)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (bag->attr_count >= bag->attr_store_size)
  {
    GeisSize new_store_size = ceilf(bag->attr_store_size
                                  * attr_bag_growth_constant);
    GeisAttr *new_store = realloc(bag->attr_store,
        new_store_size * sizeof(struct _GeisAttr));
    if (!new_store)
    {
      geis_error("failed to reallocate attr bag");
      goto error_exit;
    }
    bag->attr_store = new_store;
    bag->attr_store_size = new_store_size;
  }
  bag->attr_store[bag->attr_count++] = attr;
  status = GEIS_STATUS_SUCCESS;

error_exit:
  return status;
}


GeisStatus
geis_attr_bag_replace(GeisAttrBag bag, GeisAttr attr)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisSize i;
  for (i = 0; i < bag->attr_count; ++i)
  {
    if (0 == strcmp(bag->attr_store[i]->attr_name, geis_attr_name(attr)))
    {
      geis_attr_delete(bag->attr_store[i]);
      bag->attr_store[i] = attr;
      status = GEIS_STATUS_SUCCESS;
      break;
    }
  }
  return status;
}


GeisAttr
geis_attr_bag_attr(GeisAttrBag bag, GeisSize index)
{
  GeisAttr attr = NULL;
  if (index >= bag->attr_count)
  {
    geis_error("index out of range");
  }
  else
  {
    return bag->attr_store[index];
  }
  return attr;
}


GeisAttr
geis_attr_bag_find(GeisAttrBag bag, GeisString attr_name)
{
  GeisAttr attr = NULL;
  GeisSize i;
  for (i = 0; i < bag->attr_count; ++i)
  {
    if (0 == strcmp(bag->attr_store[i]->attr_name, attr_name))
    {
      attr = bag->attr_store[i];
      break;
    }
  }
  return attr;
}


GeisAttr
geis_attr_new(GeisString attr_name, GeisAttrType attr_type, void* attr_value)
{
  GeisAttr attr = calloc(1, sizeof(struct _GeisAttr));
  if (!attr)
  {
    geis_error("failed to allocate attr");
  }
  else
  {
    attr->attr_name = strdup(attr_name);
    attr->attr_type = attr_type;
    switch (attr_type)
    {
      case GEIS_ATTR_TYPE_BOOLEAN:
	attr->attr_value.b = *(GeisBoolean*)attr_value;
	break;

      case GEIS_ATTR_TYPE_FLOAT:
	attr->attr_value.f = *(GeisFloat*)attr_value;
	break;

      case GEIS_ATTR_TYPE_INTEGER:
	attr->attr_value.i = *(GeisInteger*)attr_value;
	break;

      case GEIS_ATTR_TYPE_STRING:
	attr->attr_value.s = (GeisString)strdup(attr_value);
	break;

      default:
	attr->attr_value.p = attr_value;
    }
  }
  return attr;
}


void
geis_attr_delete(GeisAttr attr)
{
  if (attr->attr_type == GEIS_ATTR_TYPE_POINTER && attr->attr_destructor)
  {
    attr->attr_destructor(attr->attr_value.p);
  }
  else if (attr->attr_type == GEIS_ATTR_TYPE_STRING)
  {
    free((char *)attr->attr_value.s);
  }
  free((void *)attr->attr_name);
  free(attr);
}


GeisString
geis_attr_name(GeisAttr attr)
{
  return attr->attr_name;
}


void
geis_attr_set_destructor(GeisAttr attr, GeisAttrDestructor destructor)
{
  if (attr->attr_type == GEIS_ATTR_TYPE_POINTER)
  {
    attr->attr_destructor = destructor;
  }
}


GeisAttrType
geis_attr_type(GeisAttr attr)
{
  return attr->attr_type;
}


void *
geis_attr_value(GeisAttr attr)
{
  if (attr->attr_type == GEIS_ATTR_TYPE_POINTER
   || attr->attr_type == GEIS_ATTR_TYPE_STRING)
    return attr->attr_value.p;
  else
    return &attr->attr_value;
}


GeisBoolean
geis_attr_value_to_boolean(GeisAttr attr)
{
  GeisBoolean b_value = GEIS_FALSE;
  switch (attr->attr_type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      b_value = attr->attr_value.b;
      break;

    case GEIS_ATTR_TYPE_FLOAT:
      b_value = attr->attr_value.f != 0.0f;
      break;

    case GEIS_ATTR_TYPE_INTEGER:
      b_value = attr->attr_value.i != 0;
      break;

    case GEIS_ATTR_TYPE_STRING:
      b_value = (0 == strlen(attr->attr_value.s)
              || 0 != strcmp(attr->attr_value.s, "false"));
      break;

    default:
      break;
  }
  return b_value;
}


GeisFloat
geis_attr_value_to_float(GeisAttr attr)
{
  GeisFloat f_value = 0.0f;
  switch (attr->attr_type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      f_value = attr->attr_value.b ? 1.0f : 0.0f;
      break;

    case GEIS_ATTR_TYPE_FLOAT:
      f_value = attr->attr_value.f;
      break;

    case GEIS_ATTR_TYPE_INTEGER:
      f_value = attr->attr_value.i;
      break;

    case GEIS_ATTR_TYPE_STRING:
      sscanf(attr->attr_value.s, "%f", &f_value);
      break;

    default:
      break;
  }
  return f_value;
}


GeisInteger
geis_attr_value_to_integer(GeisAttr attr)
{
  GeisInteger i_value = 0;
  switch (attr->attr_type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      i_value = attr->attr_value.b ? 1 : 0;
      break;

    case GEIS_ATTR_TYPE_FLOAT:
      i_value = attr->attr_value.f;
      break;

    case GEIS_ATTR_TYPE_INTEGER:
      i_value = attr->attr_value.i;
      break;

    case GEIS_ATTR_TYPE_STRING:
      sscanf(attr->attr_value.s, "%d", &i_value);
      break;

    default:
      break;
  }
  return i_value;
}


GeisPointer
geis_attr_value_to_pointer(GeisAttr attr)
{
  GeisPointer p_value = NULL;
  switch (attr->attr_type)
  {
    case GEIS_ATTR_TYPE_POINTER:
      p_value = attr->attr_value.p;
      break;

    default:
      break;
  }
  return p_value;
}


GeisString
geis_attr_value_to_string(GeisAttr attr)
{
  GeisString s_value = "";
  static char buf[32];
  switch (attr->attr_type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      sprintf(buf, "%s", attr->attr_value.b ? "true" : "false"); /* i18n */
      s_value = buf;
      break;

    case GEIS_ATTR_TYPE_FLOAT:
      sprintf(buf, "%f", attr->attr_value.f);
      s_value = buf;
      break;

    case GEIS_ATTR_TYPE_INTEGER:
      sprintf(buf, "%d", attr->attr_value.i);
      s_value = buf;
      break;

    case GEIS_ATTR_TYPE_POINTER:
      sprintf(buf, "%p", attr->attr_value.p);
      s_value = buf;
      break;

    case GEIS_ATTR_TYPE_STRING:
      s_value = attr->attr_value.s;
      break;

    default:
      sprintf(buf, "%p", attr->attr_value.p);
      s_value = buf;
      break;
  }
  return s_value;
}


GeisBoolean
geis_attr_compare(GeisAttr lhs, GeisAttr rhs, GeisFilterOperation op)
{
  GeisBoolean result = GEIS_FALSE;

  if (lhs->attr_type != rhs->attr_type)
    return result;

  switch (lhs->attr_type)
  {
    case GEIS_ATTR_TYPE_BOOLEAN:
      switch (op)
      {
        case GEIS_FILTER_OP_EQ:
          result = lhs->attr_value.b == rhs->attr_value.b;
          break;
        case GEIS_FILTER_OP_NE:
        case GEIS_FILTER_OP_GT:
        case GEIS_FILTER_OP_LT:
          result = lhs->attr_value.b != rhs->attr_value.b;
          break;
        case GEIS_FILTER_OP_GE:
        case GEIS_FILTER_OP_LE:
          result = GEIS_TRUE;
          break;
      }
      break;

    case GEIS_ATTR_TYPE_FLOAT:
      switch (op)
      {
        case GEIS_FILTER_OP_EQ:
          result = lhs->attr_value.f == rhs->attr_value.f;
          break;
        case GEIS_FILTER_OP_NE:
          result = lhs->attr_value.f != rhs->attr_value.f;
          break;
        case GEIS_FILTER_OP_GT:
          result = lhs->attr_value.f > rhs->attr_value.f;
          break;
        case GEIS_FILTER_OP_GE:
          result = lhs->attr_value.f >= rhs->attr_value.f;
          break;
        case GEIS_FILTER_OP_LT:
          result = lhs->attr_value.f < rhs->attr_value.f;
          break;
        case GEIS_FILTER_OP_LE:
          result = lhs->attr_value.f <= rhs->attr_value.f;
          break;
      }
      break;

    case GEIS_ATTR_TYPE_INTEGER:
      switch (op)
      {
        case GEIS_FILTER_OP_EQ:
          result = lhs->attr_value.i == rhs->attr_value.i;
          break;
        case GEIS_FILTER_OP_NE:
          result = lhs->attr_value.i != rhs->attr_value.i;
          break;
        case GEIS_FILTER_OP_GT:
          result = lhs->attr_value.i > rhs->attr_value.i;
          break;
        case GEIS_FILTER_OP_GE:
          result = lhs->attr_value.i >= rhs->attr_value.i;
          break;
        case GEIS_FILTER_OP_LT:
          result = lhs->attr_value.i < rhs->attr_value.i;
          break;
        case GEIS_FILTER_OP_LE:
          result = lhs->attr_value.i <= rhs->attr_value.i;
          break;
      }
      break;

    case GEIS_ATTR_TYPE_POINTER:
      switch (op)
      {
        case GEIS_FILTER_OP_EQ:
          result = lhs->attr_value.p == rhs->attr_value.p;
          break;
        case GEIS_FILTER_OP_NE:
          result = lhs->attr_value.p != rhs->attr_value.p;
          break;
        case GEIS_FILTER_OP_GT:
          result = lhs->attr_value.p > rhs->attr_value.p;
          break;
        case GEIS_FILTER_OP_GE:
          result = lhs->attr_value.p >= rhs->attr_value.p;
          break;
        case GEIS_FILTER_OP_LT:
          result = lhs->attr_value.p < rhs->attr_value.p;
          break;
        case GEIS_FILTER_OP_LE:
          result = lhs->attr_value.p <= rhs->attr_value.p;
          break;
      }
      break;

    case GEIS_ATTR_TYPE_STRING:
      switch (op)
      {
        case GEIS_FILTER_OP_EQ:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) == 0;
          break;
        case GEIS_FILTER_OP_NE:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) != 0;
          break;
        case GEIS_FILTER_OP_GT:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) > 0;
          break;
        case GEIS_FILTER_OP_GE:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) >= 0;
          break;
        case GEIS_FILTER_OP_LT:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) < 0;
          break;
        case GEIS_FILTER_OP_LE:
          result = strcmp(lhs->attr_value.s, rhs->attr_value.s) <= 0;
          break;
      }
      break;

    default:
      /* no comparisons are supported on indeterminate types */
      break;
  }

  return result;
}

