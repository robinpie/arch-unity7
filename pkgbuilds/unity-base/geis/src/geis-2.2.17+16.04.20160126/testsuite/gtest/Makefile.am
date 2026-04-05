#
# @file testsuite/gtest/Makefile.am 
# @brief automake recipe for the geis gtest testsuite harness directory
#

if HAVE_GTEST
  check_LIBRARIES = libgtest_geis.a
endif

nodist_libgtest_geis_a_SOURCES = \
	$(GTEST_SOURCE)/gtest-all.cc

libgtest_geis_a_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-DTEST_ROOT_DIR=\"$(abs_top_srcdir)/testsuite/\" \
	$(GTEST_CPPFLAGS)

if ENABLE_INTEGRATION_TESTS
libgtest_geis_a_SOURCES = \
	gtest_evemu_device.h   gtest_evemu_device.cpp \
	gtest_geis_fixture.h   gtest_geis_fixture.cpp \
	gtest_geis1_fixture.h   gtest_geis1_fixture.cpp 

nodist_libgtest_geis_a_SOURCES += \
	$(XORG_GTEST_SOURCE)/src/xorg-gtest-all.cpp

libgtest_geis_a_CPPFLAGS += \
	--std=c++0x \
	$(XORG_GTEST_CPPFLAGS) \
	$(EVEMU_CFLAGS) \
	$(XORG_GTEST_CFLAGS)
endif
