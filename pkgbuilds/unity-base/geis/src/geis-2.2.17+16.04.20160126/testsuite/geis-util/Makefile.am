# @file testsuite/geis-util/Makefile.am 
# @brief automake recipe for the GEIS internal utility library test suite

# Copyright 2011 Canonical, Ltd.
#
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License version 3, as published 
# by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranties of 
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along 
# with this program.  If not, see <http://www.gnu.org/licenses/>.

if HAVE_CHECK
  test_targets = check_geis_util
else
  test_targets = 
endif

TESTS = $(test_targets)

check_PROGRAMS = $(test_targets)

check_geis_util_SOURCES = \
	check_geis_bag.c \
	check_geis_util.c

check_geis_util_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/libs/geis-util \
	$(CHECK_CFLAGS)

check_geis_util_LDADD = \
	$(top_builddir)/libs/geis-util/libgeis-util.la \
	$(CHECK_LIBS) \
	-lm -lpthread

MOSTLYCLEANFILES = \
	geis_util.log \
	geis_util.xml

