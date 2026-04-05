#
# @file libgeis/Makefile.am 
# @brief automake recipe for the GEIS v2.0 implementation
#
# Copyright 2010-2012 Canonical, Ltd.
#
# This file is part of the geis library. This library is free software;
# you can redistribute it and/or modify it under the terms of version 3 of
# the GNU Lesser General Public License as published by the Free Software
# Foundation.
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
version_script = $(srcdir)/libgeis.ver

SUBDIRS = backend server

lib_LTLIBRARIES = libgeis.la

nobase_include_HEADERS = geis/geisimpl.h

libgeis_la_SOURCES = \
	geis_atomic.h \
	geis_attr.h                 geis_attr.c \
	geis_backend.h              geis_backend.c \
	geis_backend_protected.h    geis_private.h \
	geis_backend_multiplexor.h  geis_backend_multiplexor.c \
	geis_backend_token.h        geis_backend_token.c \
	geis_class.h                geis_class.c \
	geis_device.h               geis_device.c \
	geis_error.h                geis_error.c \
	geis_event.h                geis_event.c \
	geis_event_queue.h          geis_event_queue.c \
	geis_filter.h               geis_filter.c \
	geis_filterable.h           geis_filterable.c \
	geis_filter_term.h          geis_filter_term.c \
	geis_frame.h                geis_frame.c \
	geis_gesture_flick.h        geis_gesture_flick.c \
	geis_group.h                geis_group.c \
	geis_region.h               geis_region.c \
	geis_subscription.h         geis_subscription.c \
	geis_test_api.h \
	geis_timer.h                geis_timer.c \
	geis_touch.h                geis_touch.c \
	geis_v1.c                   geis.c

libgeis_la_CPPFLAGS = \
	-fvisibility=hidden \
	-DGEIS_BUILDING_DSO=1 \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(srcdir)/backend \
	-I$(top_srcdir)/libs/geis-dbus \
	-I$(top_srcdir)/libs/geis-util \
	$(DBUS_CFLAGS) \
	$(GRAIL_CFLAGS) 

libgeis_la_LDFLAGS = \
	-Wl,-z,defs -Wl,--as-needed \
	-version-info 4:0:3 \
	-Wl,--version-script=$(version_script) \
	-lm

libgeis_la_LIBADD = \
	${builddir}/backend/test_fixture/libgeis-test-fixture.la \
	${builddir}/backend/dbus/libgeis-dbus-backend.la \
	${builddir}/backend/grail/libgeis-grail-backend.la \
	${builddir}/server/libgeis-dbus-server.la \
	$(top_builddir)/libs/geis-util/libgeis-util.la \
	$(top_builddir)/libs/geis-dbus/libgeis-dbus.la 

EXTRA_DIST = $(version_script)

