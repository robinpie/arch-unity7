#
# @file tools/geisview/Makefile.am 
# @brief automake recipe for the geisview tool
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

geisviewdir = $(pythondir)/geisview
shareddir = $(datadir)/geisview
xdgdir = $(datadir)/applications

dist_bin_SCRIPTS = geisview
dist_shared_DATA = \
	geisview.ui \
	filter_definition.ui \
	filter_list.ui

geisview_PYTHON = \
	__init__.py \
	classview.py \
	defaults.py \
	deviceview.py \
	filter_definition.py \
	filter_list.py

defaults.py: defaults.py.in Makefile
	$(AM_V_GEN)$(SED) \
	  -e 's,[@]shareddir[@],$(shareddir),g' \
	  $< >$@

dist_xdg_DATA = geisview.desktop

dist_man_MANS = geisview.1

BUILT_SOURCES = defaults.py
EXTRA_DIST = defaults.py.in
CLEANFILES = $(BUILT_SOURCES)

