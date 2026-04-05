/**
 * @file geis_gesture_flick.h
 * @brief higher-level "flick" gesture recognizer
 *
 * Copyright 2011 Canonical Ltd.
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
#ifndef GEIS_GESTURE_FLICK_H_
#define GEIS_GESTURE_FLICK_H_

#include "geis/geis.h"

typedef struct GeisGestureFlick *GeisGestureFlick;


GeisGestureFlick geis_gesture_flick_new(Geis geis);

void geis_gesture_flick_delete(GeisGestureFlick flick);

#endif /* GEIS_GESTURE_FLICK_H_ */
