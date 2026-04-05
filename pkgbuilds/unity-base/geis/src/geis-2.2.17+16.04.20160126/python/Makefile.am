#
# @file python/Makefile.am 
# @brief automake recipe for the libgeis python bindings
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

dist_bin_SCRIPTS = pygeis

dist_noinst_SCRIPTS = run_pygeis

nobase_python_PYTHON = \
	geis/__init__.py \
	geis/geis_v2.py

pyexec_LTLIBRARIES = _geis_bindings.la

_geis_bindings_la_SOURCES = \
	_geis_bindings/_geis_bindings.c

_geis_bindings_la_CFLAGS = \
	$(AM_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	$(PYTHON_CFLAGS)

_geis_bindings_la_LDFLAGS = \
	-module -avoid-version \
	-export-symbols-regex PyInit__geis_bindings \
	$(top_builddir)/libgeis/libgeis.la \
	$(PYTHON_LIBS)

