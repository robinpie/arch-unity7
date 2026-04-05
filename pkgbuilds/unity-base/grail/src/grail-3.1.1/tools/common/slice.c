/*****************************************************************************
 *
 * frame - Touch Frame Library
 *
 * Copyright (C) 2010-2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of version 3 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "common/slice.h"

#include <stdio.h>

void print_slice(UGHandle handle, UGSlice slice, uint64_t time) {
  const UGGestureTypeMask recognized = grail_slice_get_recognized(slice);
  const UGTransform *transform = grail_slice_get_transform(slice);
  const UGTransform *cumulative_transform =
      grail_slice_get_cumulative_transform(slice);

  printf("Gesture slice:\n");
  printf("  Time: %ju\n", time);
  printf("  ID: %u\n", grail_slice_get_id(slice));

  switch (grail_slice_get_state(slice)) {
    case UGGestureStateBegin:
      printf("  State: Begin\n");
      break;

    case UGGestureStateUpdate:
      printf("  State: Update\n");
      break;

    case UGGestureStateEnd:
      printf("  State: End\n");
      break;
  }

  printf("  Subscription: %p\n", grail_slice_get_subscription(slice));

  printf("  Recognized Gestures:");
  if (recognized & UGGestureTypeDrag)
    printf(" Drag");
  if (recognized & UGGestureTypePinch)
    printf(" Pinch");
  if (recognized & UGGestureTypeRotate)
    printf(" Rotate");
  if (recognized & UGGestureTypeTap)
    printf(" Tap");
  if (recognized & UGGestureTypeTouch)
    printf(" Touch");
  printf("\n");

  printf("  Construction Finished: %s\n",
         (grail_slice_get_construction_finished(slice) ? "Yes" : "No"));

  printf("  Number Of Touches: %u\n", grail_slice_get_num_touches(slice));
  printf("  Original Center: (%f, %f)\n",
         grail_slice_get_original_center_x(slice),
         grail_slice_get_original_center_y(slice));
  printf("  Original Radius: %f\n", grail_slice_get_original_radius(slice));
  printf("  Instantaneous Center Of Rotation: (%f, %f)\n",
         grail_slice_get_center_of_rotation_x(slice),
         grail_slice_get_center_of_rotation_y(slice));

  printf("  Instantaneous Transform:\n");
  printf("    [ %22f, %22f, %22f ]\n", (*transform)[0][0], (*transform)[0][1],
         (*transform)[0][2]);
  printf("    [ %22f, %22f, %22f ]\n", (*transform)[1][0], (*transform)[1][1],
         (*transform)[1][2]);
  printf("    [ %22f, %22f, %22f ]\n", (*transform)[2][0], (*transform)[2][1],
         (*transform)[2][2]);

  printf("  Cumulative Transform:\n");
  printf("    [ %22f, %22f, %22f ]\n", (*cumulative_transform)[0][0],
         (*cumulative_transform)[0][1], (*cumulative_transform)[0][2]);
  printf("    [ %22f, %22f, %22f ]\n", (*cumulative_transform)[1][0],
         (*cumulative_transform)[1][1], (*cumulative_transform)[1][2]);
  printf("    [ %22f, %22f, %22f ]\n", (*cumulative_transform)[2][0],
         (*cumulative_transform)[2][1], (*cumulative_transform)[2][2]);

  printf("\n");
}
