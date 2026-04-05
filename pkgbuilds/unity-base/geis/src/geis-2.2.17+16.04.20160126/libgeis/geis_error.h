/**
 * @file geis_error.h
 * @brief internal GEIS error facilities
 *
 * Copyright 2010 Canonical Ltd.
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
#ifndef GEIS_ERROR_H_
#define GEIS_ERROR_H_

#include <geis/geis.h>


/*
 * A structure to hold the error stack
 */
typedef struct _GeisErrorStack
{
  GeisStatus *store;
  GeisSize    store_size;
  GeisSize    error_count;
} GeisErrorStack;


void geis_error_clear(Geis geis);

void geis_error_push(Geis geis, GeisStatus code);

#endif /* GEIS_ERROR_H_ */
