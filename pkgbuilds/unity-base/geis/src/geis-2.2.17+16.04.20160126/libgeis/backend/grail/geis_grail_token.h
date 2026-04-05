/**
 * @file geis_grail_token.h
 * @brief GEIS filter token for the grail-based back end
 */
/*
 * Copyright 2011-2012 Canonical Ltd.
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
 */
#ifndef GEIS_BACKEND_GRAIL_TOKEN_H_
#define GEIS_BACKEND_GRAIL_TOKEN_H_

#include "geis_backend_protected.h"

/** The Grail Back End token type */
typedef struct GeisGrailToken *GeisGrailToken;


GeisBackendToken
geis_grail_token_new(GeisBackend be, GeisBackendTokenInitState init_state);

void             
geis_grail_token_delete(GeisBackendToken token);

/**
 * Callback for adding a filter term for a gesture class.
 */
GeisStatus
geis_grail_token_add_class_term(GeisBackendToken     gbtoken,
                                void                *context,
                                GeisString           name,
                                GeisFilterOperation  op,
                                void                *value);

/**
 * Callback for adding a filter term for a device.
 */
GeisStatus
geis_grail_token_add_device_term(GeisBackendToken     gbtoken,
                                 void                *context,
                                 GeisString           name,
                                 GeisFilterOperation  op,
                                 void                *value);

/**
 * Callback for adding a filter term for a feature.
 */
GeisStatus
geis_grail_token_add_feature_term(GeisBackendToken     gbtoken,
                                  void                *context,
                                  GeisString           name,
                                  GeisFilterOperation  op,
                                  void                *value);

/**
 * Callback for adding a filter term for a region.
 */
GeisStatus
geis_grail_token_add_region_term(GeisBackendToken     gbtoken,
                                 void                *context,
                                 GeisString           name,
                                 GeisFilterOperation  op,
                                 void                *value);

#endif /* GEIS_BACKEND_GRAIL_TOKEN_H_ */
