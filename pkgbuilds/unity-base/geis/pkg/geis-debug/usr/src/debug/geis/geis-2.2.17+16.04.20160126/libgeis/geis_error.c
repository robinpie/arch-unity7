/**
 * @file libgeis/geis_error.c
 * @brief implementation of the GEIS v2.0 API error reporting module
 *
 * Copyright 2010 Canonical Ltd.
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
#include "geis_error.h"

#include "geis_logging.h"
#include "geis_private.h"
#include <stdlib.h>


static GeisErrorStack g_error_stack = { 0, 0, 0 };


static void
geis_error_stack_clear(GeisErrorStack *error_stack)
{
  error_stack->error_count = 0;
}


static void
geis_error_stack_push(GeisErrorStack *error_stack, GeisStatus code)
{
  if (error_stack->error_count < error_stack->store_size)
  {
    error_stack->store[error_stack->error_count++] = code;
  }
  else
  {
    error_stack->store_size += 2;
    error_stack->store = realloc(error_stack->store,
                                 error_stack->store_size * sizeof(GeisErrorStack
));
    if (!error_stack->store)
    {
      geis_error("error_stack realloc failed for size %zu",
                 error_stack->store_size);
      return;
    }
    error_stack->store[error_stack->error_count++] = code;
  }
}


static GeisStatus
geis_error_stack_get(GeisErrorStack *error_stack, GeisSize index)
{
  if (index < error_stack->error_count)
  {
    return error_stack->store[index];
  }
  return GEIS_STATUS_BAD_ARGUMENT;
}


void
geis_error_clear(Geis geis)
{
  if (geis)
  {
    geis_error_stack_clear(geis_error_stack(geis));
  }
  else
  {
    geis_error_stack_clear(&g_error_stack);
  }
}


void
geis_error_push(Geis geis, GeisStatus code)
{
  if (geis)
  {
    geis_error_stack_push(geis_error_stack(geis), code);
  }
  else
  {
    geis_error_stack_push(&g_error_stack, code);
  }
}


GeisSize
geis_error_count(Geis geis)
{
  if (geis)
  {
    return geis_error_stack(geis)->error_count;
  }
  else
  {
    return g_error_stack.error_count;
  }
}


GeisStatus
geis_error_code(Geis geis, GeisSize index)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  if (geis)
  {
    status = geis_error_stack_get(geis_error_stack(geis), index);
  }
  else
  {
    status = geis_error_stack_get(&g_error_stack, index);
  }
  return status;
}


GeisString
geis_error_message(Geis geis, GeisSize index GEIS_UNUSED)
{
  GeisString message = "";
  if (geis)
  {
  }
  else
  {
  }
  return message;
}


