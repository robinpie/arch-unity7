test_targets = check-integration

TESTS = $(test_targets)
check_PROGRAMS = $(test_targets)

x11dir = x11

check_integration_SOURCES = \
	events.cpp \
	events.h \
	x11/device.cpp \
	x11/dynamic_type_checking.cpp \
	x11/fixture.cpp \
	x11/fixture.h \
	x11/ids.cpp \
	x11/recording.cpp

check_integration_CPPFLAGS = \
	--std=c++0x \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I../include \
	-DTEST_ROOT_DIR=\"$(abs_top_srcdir)/test/integration/\"\
	$(WARNING_CPPFLAGS) \
	$(GTEST_CPPFLAGS) \
	$(XORG_GTEST_CPPFLAGS)

check_integration_CFLAGS = \
	$(C11_CFLAGS) \
	$(COVERAGE_CFLAGS) \
	$(XORG_GTEST_CFLAGS)

check_integration_CXXFLAGS = \
	$(CXX11_CXXFLAGS) \
	$(COVERAGE_CXXFLAGS) \
	$(GTEST_CXXFLAGS) \
	$(XORG_GTEST_CXXFLAGS)

# check_integration_LDFLAGS = $(GTEST_LDFLAGS)

#
# Link against the (non-distributed) static lib to pick up the
# internal symbols.
#
check_integration_LDADD = \
	$(top_builddir)/src/.libs/libframe.a \
	$(top_builddir)/test/gtest/libgtest.a \
	$(top_builddir)/test/gtest/libxorg-gtest.a \
	$(top_builddir)/test/gtest/libxorg-gtest_main.a \
	$(GTEST_LIBS) \
	$(XORG_GTEST_LIBS) \
	$(XINPUT_LIBS) \
	$(EVEMU_LIBS) \
	$(COVERAGE_LIBS) \
	$(CXX_LIBS)

EXTRA_DIST = recordings
