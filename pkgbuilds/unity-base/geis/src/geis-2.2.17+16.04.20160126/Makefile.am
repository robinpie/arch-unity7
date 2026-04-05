#
# @file Makefile.am 
# @brief top-level makefile for the libgeis project
#
# Copyright 2010, 2011, 2012 Canonical, Ltd.
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


ACLOCAL_AMFLAGS = -I m4 --install
AM_MAKEFLAGS = --no-print-directory
AM_DISTCHECK_CONFIGURE_FLAGS = --disable-integration-tests

SUBDIRS = include libs libgeis testsuite python tools examples doc data

doc-%:
	$(MAKE) -C doc $@

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libgeis.pc

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	COPYING.GPL \
	INSTALL \
	NEWS \
	README

