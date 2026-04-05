/**
 * @file check_gesture_types.c
 * @brief Unit tests for GEIS v1 gesture types
 *
 * Copyright 2011 Canonical Ltd.
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
#include <check.h>

#include <geis/geis.h>


START_TEST(gesture_types)
{
  GeisInteger geis_type;
  GeisString  geis_type_name;

  geis_type = GEIS_GESTURE_PRIMITIVE_DRAG;
  geis_type = GEIS_GESTURE_PRIMITIVE_PINCH;
  geis_type = GEIS_GESTURE_PRIMITIVE_ROTATE;
  geis_type = GEIS_GESTURE_PRIMITIVE_TAP;
  geis_type = GEIS_GESTURE_PRIMITIVE_TOUCH;

  geis_type_name = GEIS_GESTURE_TYPE_DRAG1;
  geis_type_name = GEIS_GESTURE_TYPE_DRAG2;
  geis_type_name = GEIS_GESTURE_TYPE_DRAG3;
  geis_type_name = GEIS_GESTURE_TYPE_DRAG4;
  geis_type_name = GEIS_GESTURE_TYPE_DRAG5;
  geis_type_name = GEIS_GESTURE_TYPE_PINCH1;
  geis_type_name = GEIS_GESTURE_TYPE_PINCH2;
  geis_type_name = GEIS_GESTURE_TYPE_PINCH3;
  geis_type_name = GEIS_GESTURE_TYPE_PINCH4;
  geis_type_name = GEIS_GESTURE_TYPE_PINCH5;
  geis_type_name = GEIS_GESTURE_TYPE_ROTATE1;
  geis_type_name = GEIS_GESTURE_TYPE_ROTATE2;
  geis_type_name = GEIS_GESTURE_TYPE_ROTATE3;
  geis_type_name = GEIS_GESTURE_TYPE_ROTATE4;
  geis_type_name = GEIS_GESTURE_TYPE_ROTATE5;
  geis_type_name = GEIS_GESTURE_TYPE_TAP1;
  geis_type_name = GEIS_GESTURE_TYPE_TAP2;
  geis_type_name = GEIS_GESTURE_TYPE_TAP3;
  geis_type_name = GEIS_GESTURE_TYPE_TAP4;
  geis_type_name = GEIS_GESTURE_TYPE_TAP5;
  geis_type_name = GEIS_GESTURE_TYPE_TOUCH1;
  geis_type_name = GEIS_GESTURE_TYPE_TOUCH2;
  geis_type_name = GEIS_GESTURE_TYPE_TOUCH3;
  geis_type_name = GEIS_GESTURE_TYPE_TOUCH4;
  geis_type_name = GEIS_GESTURE_TYPE_TOUCH5;
}
END_TEST

Suite *
geis1_gesture_types_new()
{
  Suite *s = suite_create("geis1_gesture_types");
  TCase *test;

  test = tcase_create("gesture_types");
  tcase_add_test(test, gesture_types);
  suite_add_tcase(s, test);

  return s;
}

