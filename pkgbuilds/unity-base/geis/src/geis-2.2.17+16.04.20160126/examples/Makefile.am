#
# @file examples/Makefile.am 
# @brief automake recipe for the GEIS examples
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

noinst_PROGRAMS = geis2

geis2_SOURCES = geis2.c

geis2_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/libgeis

geis2_LDADD = $(top_builddir)/libgeis/libgeis.la

