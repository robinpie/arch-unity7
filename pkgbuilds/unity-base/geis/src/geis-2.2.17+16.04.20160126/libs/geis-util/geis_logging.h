/**
 * @file geis_logging.h
 * @brief internal GEIS debug/logging facilities
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
#ifndef GEIS_LOGGING_H_
#define GEIS_LOGGING_H_

/**
 * @defgroup geis_logging GEIS Logging Facilities
 * @ingroup geis_util
 *
 * This facility allows the run-time delivery of debugging and error emssages
 * from the library.  By default, no debug or error diagnostics are emitted by
 * the library.  However, if the environment variable GEIS_DEBUG is set to an
 * appropriate value, these diagnostic messages will be emitted to stderr.
 *
 * The following GEIS_DEBUG values are supported.
 *
 * GEIS_DEBUG=1  error messages only are emitted
 * GEIS_DEBUG=2  error and warning messages are emitted
 * GEIS_DEBUG=3  error, warning, and debug messages are emitted
 *
 * @{
 */

#define _GEIS_LOG_LEVEL_ERROR    1
#define _GEIS_LOG_LEVEL_WARNING  2
#define _GEIS_LOG_LEVEL_DEBUG    3

/**
 * Emits a message at the DEBUG logging level.
 * @param[in] fmt printf-style format string
 */
#define geis_debug(...) _geis_message(_GEIS_LOG_LEVEL_DEBUG, \
                                      __extension__ __FUNCTION__, __LINE__, \
                                      __VA_ARGS__)
#define geis_warning(...) _geis_message(_GEIS_LOG_LEVEL_WARNING, \
                                        __extension__ __FUNCTION__, __LINE__, \
                                        __VA_ARGS__)
#define geis_error(...) _geis_message(_GEIS_LOG_LEVEL_ERROR, \
                                      __extension__ __FUNCTION__, __LINE__, \
                                      __VA_ARGS__)
int _geis_message(int level,
                  const char *function, int line,
                  const char *format, ...) __attribute__((format(printf, 4, 5)));

/* @} */

#endif /* GEIS_LOGGING_H_ */
