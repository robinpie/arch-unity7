#
# @file testsuite/libgeis/Makefile.am 
# @brief automake recipe for the GEIS v2.0 internals unit test suite
#
#
# Copyright 2010, 2011 Canonical, Ltd.
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

if HAVE_CHECK
  test_targets = check_geis2_internals
else
  test_targets = 
endif

compile_tests = check-c-compile check-cxx-compile
test_cppflags = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis

TESTS = $(test_targets) $(compile_tests)

check_PROGRAMS = $(test_targets) $(compile_tests)

check_c_compile_SOURCES = check-c-compile.c
check_c_compile_CPPFLAGS = $(test_cppflags)
check_c_compile_LDADD = $(top_builddir)/libgeis/libgeis.la

check_cxx_compile_SOURCES = check-cxx-compile.cpp
check_cxx_compile_CPPFLAGS = $(test_cppflags)
check_cxx_compile_LDADD = $(top_builddir)/libgeis/libgeis.la

check_geis2_internals_SOURCES = \
	check_attr.c \
	check_backend_multiplexor.c \
	check_backend_token.c \
	check_class.c \
	check_device.c \
	check_error_reporting.c \
	check_event_queue.c \
	check_filter.c \
	check_filter_term.c \
	check_geis_private.c \
	check_region.c \
	check_subscription.c \
	check_select_devices.c \
	check_timer.c \
	check_geis2_internals.c

check_geis2_internals_CPPFLAGS = \
	$(test_cppflags) \
	$(CHECK_CFLAGS)

# 
# Link against the (non-distributed) static libgeis to pick up the
# internal symbols.
#
check_geis2_internals_LDADD = \
	$(top_builddir)/libgeis/.libs/libgeis.a \
	$(DBUS_LIBS) \
	$(GRAIL_LIBS) \
	$(X11XCB_LIBS) \
	$(XCB_LIBS) \
	$(XI2_LIBS) \
	$(CHECK_LIBS) \
	-lm -ldl -lpthread

MOSTLYCLEANFILES = \
	geis2_internals.log \
	geis2_internals.xml

