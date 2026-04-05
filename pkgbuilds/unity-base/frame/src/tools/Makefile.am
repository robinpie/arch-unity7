AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	$(WARNING_CPPFLAGS)

AM_CFLAGS = $(C11_CFLAGS) $(COVERAGE_CFLAGS)

AM_CFLAGS += $(XINPUT_CFLAGS)

commondir = common

bin_PROGRAMS = frame-test-x11

frame_test_x11_SOURCES = frame-test-x11.c \
	common/device.c \
	common/device.h \
	common/frame.c \
	common/frame.h \
	common/touch.c \
	common/touch.h \
	frame-test-x11.txt

frame_test_x11_LDADD = \
	$(XINPUT_LIBS) \
	$(top_builddir)/src/libframe.la \
	$(COVERAGE_LIBS) \
	$(CXX_LIBS)

local_manpages = frame-test-x11.1

if HAVE_DOCTOOLS
dist_man_MANS = $(local_manpages)

%.1: %.txt
	$(AM_V_GEN)$(ASCIIDOC) -L -f manpage $<

CLEANFILES = $(dist_man_MANS)

endif

