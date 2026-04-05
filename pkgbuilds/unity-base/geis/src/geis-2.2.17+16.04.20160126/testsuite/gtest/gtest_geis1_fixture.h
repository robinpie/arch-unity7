/**
 * @file gtest_geis_fixture.h
 * @brief A GTest fixture for testing the full stack through GEIS.
 */
/*
 *  Copyright 2012 Canonical Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License version 3, as published 
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranties of 
 *  MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along 
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GTEST_GEIS1_FIXTURE_H_
#define GTEST_GEIS1_FIXTURE_H_

#include "geis/geis.h"
#include <gtest/gtest.h>
#include <xorg/gtest/xorg-gtest.h>


/**
 * Fixture for testing expected attributes.
 */
class GTestGeis1Fixture
: public xorg::testing::Test
{
public:
  void
  SetUp();

  void
  TearDown();

  GeisInstance
  geis() { return geis_; }

  int
  geis_fd() const { return geis_fd_; }

private:
  GeisInstance  geis_;
  int geis_fd_;
};


#endif /* GTEST_GEIS1_FIXTURE_H_ */

