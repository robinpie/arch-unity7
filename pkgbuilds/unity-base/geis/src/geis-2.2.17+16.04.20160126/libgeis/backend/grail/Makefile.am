#
# @file libgeis/backend/grail/Makefile.am
# @brief automake recipe for the GEIS v2.0 grail back end
#
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

noinst_LTLIBRARIES = libgeis-grail-backend.la

libgeis_grail_backend_la_SOURCES = \
	geis_grail_token.h          geis_grail_token.c \
	geis_grail_backend.h        geis_grail_backend.c  \
	geis_grail_window_grab.h    geis_grail_window_grab.c \
	geis_grail_xsync.h          geis_grail_xsync.c \
	geis_ugsubscription_store.h geis_ugsubscription_store.c

libgeis_grail_backend_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/libs/geis-util \
	$(GRAIL_CFLAGS)

libgeis_grail_backend_la_LIBADD = \
	$(GRAIL_LIBS) \
	$(XI2_LIBS)
