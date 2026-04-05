#
# @file libs/geis-dbus/Makefile.am
# @brief automake recipe for the GEIS v2.0 DBus helper library
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
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

noinst_LTLIBRARIES = libgeis-dbus.la

dist_libgeis_dbus_la_SOURCES = \
	geis_dbus.h \
	geis_dbus_attr.h          geis_dbus_attr.c \
	geis_dbus_class.h         geis_dbus_class.c \
	geis_dbus_device.h        geis_dbus_device.c \
	geis_dbus_dispatcher.h    geis_dbus_dispatcher.c \
	geis_dbus_gesture_event.h geis_dbus_gesture_event.c \
	geis_dbus_region.h        geis_dbus_region.c \
	geis_dbus_subscription.h  geis_dbus_subscription.c 

libgeis_dbus_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/libs/geis-util \
	$(DBUS_CFLAGS)

libgeis_dbus_la_LIBADD = \
	$(DBUS_LIBS)


