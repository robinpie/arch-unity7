#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
#
# Copyright 2010, 2011, 2012, 2013 Canonical, Ltd.
#
# This file is part of the geis library. This library is free software;
# you can redistribute it and/or modify it under the terms of the GNU Lesser
# General Public License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

AC_PREREQ(2.65)
AC_INIT([geis],
	[2.2.17],
	[stephen.webb@canonical.com],,
	[https://launchpad.net/geis])
AC_CONFIG_HEADERS([geis_config.h])
AC_CONFIG_AUX_DIR([config.aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 gnu -Wall color-tests dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_AR
AM_PATH_PYTHON([3.2])

LT_PREREQ([2.2.6b])
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES([DBUS], [dbus-1 >= 1.2.16], ,
		  AC_MSG_ERROR([D-Bus development libraries not found]))
PKG_CHECK_MODULES([GRAIL], [grail >= 3.0.8 frame >= 2.2], ,
		  AC_MSG_ERROR([Grail development libraries not found]))
PKG_CHECK_MODULES([XCB], [xcb >= 1.6], ,
		  AC_MSG_ERROR([XCB development libraries not found]))
PKG_CHECK_MODULES([XI2], [x11 xext xi >= 1.3], ,
		  AC_MSG_ERROR([XI2 development libraries not found]))
PKG_CHECK_MODULES([PYTHON], [python3 >= 3.2])

AX_ENABLE_XI2

# Check toolchain features
AC_MSG_CHECKING([whether hidden visibility is supported])
geis_hidden=no
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
  void __attribute__((visibility ("hidden"))) bar (void) {}])],
  [geis_hidden=yes])
AC_MSG_RESULT($geis_hidden)
if test x$geis_hidden = xyes; then
  AC_DEFINE([GEIS_DSO_PRIVATE],[1],[symbol visibility is supported])
fi
AC_DEFINE([GEIS_UNUSED],[__attribute__((unused))],[symbol is unused])

AC_SUBST(AM_CFLAGS,
         "-Wall -Wextra -Werror -Wno-error=pointer-compare -pedantic -std=iso9899:1999 -D_GNU_SOURCE=1")

# Check for TDD tools
PKG_CHECK_MODULES([CHECK], [check >= 0.9.8],
                  [have_check=yes],
                  AC_MSG_WARN([package 'check' not found: related unit testing disabled]))
AM_CONDITIONAL([HAVE_CHECK],[test "x$have_check" = xyes])

# Optionally build the integration test suite.
AC_ARG_ENABLE([integration-tests],
              [AS_HELP_STRING([--enable-integration-tests],
                  [Enable building integration test cases (default: auto)])],
              [enable_integration_tests=$enableval],
              [enable_integration_tests=auto])

# Check for Google C++ Testing Framework

have_xorg_gtest=no
AS_IF([test "x$enable_integration_tests" != xno],
      [CHECK_XORG_GTEST(
          [AC_SUBST(XORG_GTEST_LIBS, [-pthread])],
          [AS_IF([test "x$enable_integration_tests" = xyes],
                 [AC_MSG_ERROR(
                     [xorg-gtest is not available])],
                 [AC_MSG_WARN(
                     [xorg-gtest is not available, tests will not be built])])])])
AM_CONDITIONAL([HAVE_GTEST], [test "x$have_gtest" = xyes])
AM_CONDITIONAL([ENABLE_INTEGRATION_TESTS], [test "x$have_xorg_gtest" = xyes])

AC_CONFIG_FILES([Makefile
                 libgeis.pc
                 doc/Makefile
                 include/Makefile
                 libs/Makefile
                 libs/geis-util/Makefile
                 libs/geis-dbus/Makefile
                 libgeis/Makefile
                 libgeis/backend/Makefile
                 libgeis/backend/test_fixture/Makefile
                 libgeis/backend/dbus/Makefile
                 libgeis/backend/grail/Makefile
                 libgeis/server/Makefile
                 testsuite/Makefile
                 testsuite/gtest/Makefile
                 testsuite/geis-util/Makefile
                 testsuite/libgeis/Makefile
                 testsuite/geis2/Makefile
                 testsuite/geis1/Makefile
                 testsuite/geistest/Makefile
                 testsuite/x11_mocks/Makefile
                 examples/Makefile
                 python/Makefile
                 tools/Makefile
                 tools/geisview/Makefile
                 tools/geis-server/Makefile
                 data/Makefile])
AC_OUTPUT

bold_green=$(tput bold)$(tput setf 2)
bold_white=$(tput bold)$(tput setf 7)
reset=$(tput sgr0)

AC_MSG_NOTICE([])
AC_MSG_NOTICE([ ${bold_green}geis $VERSION${reset}])
AC_MSG_NOTICE([ ===============================])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([ prefix            : ${bold_white}${prefix}${reset}])
AC_MSG_NOTICE([ integration tests : ${bold_white}${have_xorg_gtest}${reset}])
AC_MSG_NOTICE([])

