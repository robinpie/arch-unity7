/**
 * @file geis_grail_backend.h
 * @brief The GEIS native grail back end
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
#ifndef GEIS_BACKEND_GRAIL_BACKEND_H_
#define GEIS_BACKEND_GRAIL_BACKEND_H_

#include "geis/geis.h"


/** The opaque Grail Back End type. */
typedef struct GeisGrailBackend *GeisGrailBackend;

/**
 * Activates a GEIS subscription on the back end.
 */
GeisStatus
geis_grail_backend_activate_subscription(GeisGrailBackend gbe,
                                         GeisSubscription subscription);

/**
 * Deactivates a GEIS subscription on the back end.
 */
GeisStatus
geis_grail_backend_deactivate_subscription(GeisGrailBackend gbe,
                                           GeisSubscription subscription);

/**
 * Frees the memory allocated for the GEIS subscription private data
 */
void
geis_grail_backend_free_subscription_pdata(GeisGrailBackend gbe,
                                           GeisSubscription subscription);

#endif /* GEIS_BACKEND_GRAIL_BACKEND_H_ */
