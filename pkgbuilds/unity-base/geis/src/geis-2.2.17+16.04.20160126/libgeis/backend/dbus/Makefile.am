#
# @file libgeis/backend/xcb/Makefile.am
# @brief automake recipe for the GEIS v2.0 XCB back end
#
# Copyright 2011 Canonical, Ltd.
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/
#

noinst_LTLIBRARIES = libgeis-dbus-backend.la

libgeis_dbus_backend_la_SOURCES = \
	geis_dbus_backend.c \
	geis_dbus_client.h   geis_dbus_client.c \
	geis_dbus_locator.h  geis_dbus_locator.c

libgeis_dbus_backend_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libs/geis-dbus \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/libs/geis-util \
	$(DBUS_CFLAGS)

