/**
 * @file geis_test_api.h
 * @brief internal interface of the GEIS test API
 *
 * This header exposes the GEIS internal test API for use in the GEIS test
 * suite.
 */

/*
 * Copyright 2012 Canonical Ltd.
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
#ifndef GEIS_TEST_API_H_
#define GEIS_TEST_API_H_

/**
 * Back end to use in unit testing.
 */
#define GEIS_INIT_MOCK_BACKEND    "com.canonical.oif.backend.mock"

/**
 * Causes device messages to be discarded from back end.
 *
 * Argument is of tyep GeisBoolean.  GEIS_TRUE means device messages will be
 * discarded, GEIS_FALSE means messages will not be discarded.
 *
 * This cofiguration option is intended to be used with various device-related
 * integration and unit tests.
 */
#define GEIS_CONFIG_DISCARD_DEVICE_MESSAGES "com.canonical.oif.discard.device"

#endif /* GEIS_TEST_API_H_ */
