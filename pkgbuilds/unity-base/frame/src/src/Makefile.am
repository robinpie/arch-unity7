version_script = $(srcdir)/libframe.ver

lib_LTLIBRARIES = libframe.la

libframe_la_LDFLAGS = \
	-version-info @LIB_VERSION@ \
	-Wl,-z,defs -Wl,--as-needed \
	-Wl,--version-script=$(version_script)

libframe_la_LIBADD = \
	-lm \
	$(COVERAGE_LIBS) \
	$(XINPUT_LIBS) \
	$(CXX_LIBS)

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include/ \
	-I$(top_srcdir)/src \
	$(WARNING_CPPFLAGS) \
	-DBUILDING_FRAME

AM_CFLAGS = $(VISIBILITY_CFLAGS) $(COVERAGE_CFLAGS)

AM_CXXFLAGS = \
	$(CXX11_CXXFLAGS) \
	$(VISIBILITY_CXXFLAGS) \
	$(COVERAGE_CXXFLAGS) \
	$(XINPUT_CFLAGS)

libframeincludedir = $(includedir)/oif
libframeinclude_HEADERS = \
	$(top_builddir)/include/oif/frame.h \
	$(top_srcdir)/include/oif/frame_backend.h \
	$(top_srcdir)/include/oif/frame_internal.h

if HAVE_XINPUT
libframeinclude_HEADERS += \
	$(top_srcdir)/include/oif/frame_x11.h
endif

libframe_la_SOURCES = \
	axis.h \
	axis.cpp \
	device.h \
	device.cpp \
	event.h \
	event.cpp \
	frame.h \
	frame.cpp \
	handle.h \
	handle.cpp \
	property.h \
	touch.h \
	touch.cpp \
	typedefs.h \
	value.h \
	value.cpp \
	window.h \
	window.cpp

if HAVE_XINPUT
libframe_la_SOURCES += \
	x11/device_x11.h \
	x11/device_x11.cpp \
	x11/frame_x11.cpp \
	x11/handle_x11.h \
	x11/handle_x11.cpp \
	x11/window_x11.h \
	x11/window_x11.cpp
endif

EXTRA_DIST = $(version_script) 
