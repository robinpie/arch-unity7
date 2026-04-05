#
# @file libgeis/server/Makefile.am
# @brief automake recipe for the GEIS v2.0 DBus Server
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

noinst_LTLIBRARIES = libgeis-dbus-server.la

libgeis_dbus_server_la_SOURCES = \
	geis_dbus_announcer.h    geis_dbus_announcer.c \
	geis_dbus_client_proxy.h geis_dbus_client_proxy.c \
	geis_dbus_proxy_box.h    geis_dbus_proxy_box.c \
	geis_dbus_server.h       geis_dbus_server.c 

libgeis_dbus_server_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libs/geis-dbus \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/libs/geis-util \
	$(DBUS_CFLAGS) 

libgeis_dbus_server_la_LIBADD = \
	$(DBUS_LIBS) 


