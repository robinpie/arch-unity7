#
# @file testsuite/geis2/Makefile.am 
# @brief automake recipe for the geis v2.0 testsuite testing directory
#

test_targets = 

##### check_geis2_api - Tests using Check C testing framework #####

if HAVE_CHECK
  test_targets += check_geis2_api
endif

check_geis2_api_SOURCES = \
	check_attr.c \
	check_class.c \
	check_config.c \
	check_device.c \
	check_error_codes.c \
	check_event.c \
	check_filter.c \
	check_frame.c \
	check_geis_new.c \
	check_general_types.c \
	check_region.c \
	check_subscription.c \
	check_version_macro.c \
	check_geis2_api.c

check_geis2_api_CPPFLAGS = \
	-Wno-unused -Wno-unused-function -Wno-unused-but-set-variable \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	$(CHECK_CFLAGS)

check_geis2_api_LDADD = \
	$(top_builddir)/libgeis/libgeis.la \
	$(CHECK_LIBS) \
	-lpthread

#### gtest_geis2_grail_backend - Test for the GRAIL backend of Geis v2 API #####
####                             using gtest                               #####

if HAVE_GTEST
  test_targets += gtest_geis2_grail_backend
endif

gtest_geis2_grail_backend_SOURCES = \
	gtest_grail_backend.h gtest_grail_backend.cpp \
	gtest_gbe_accept_ended_grail_gesture.cpp \
	gtest_gbe_configure_new_devices.cpp \
	gtest_gbe_deactivate_sub.cpp \
	gtest_gbe_direct_touch_coords.cpp \
	gtest_gbe_construction_finished.cpp \
	gtest_gbe_outdated_events.cpp \
	gtest_gbe_null_filter.cpp

gtest_geis2_grail_backend_CPPFLAGS = \
	--std=c++0x \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/testsuite/gtest \
	-I$(top_srcdir)/testsuite/x11_mocks \
	-DTEST_ROOT_DIR=\"$(abs_top_srcdir)/testsuite/\" \
	$(GTEST_CFLAGS)

gtest_geis2_grail_backend_LDFLAGS = \
	-export-dynamic

gtest_geis2_grail_backend_LDADD = \
	$(top_builddir)/testsuite/gtest/libgtest_geis.a \
	$(top_builddir)/testsuite/x11_mocks/libx11_mocks.a \
	$(top_builddir)/libgeis/libgeis.la \
	$(GTEST_LIBS)

#### gtest_geis2_api - Integration tests using xorg-gest and evemu #####

if ENABLE_INTEGRATION_TESTS
  test_targets += gtest_geis2_api
endif

gtest_geis2_api_SOURCES = \
	gtest_attrs.cpp \
	gtest_config.cpp \
	gtest_devices.cpp \
	gtest_subscriptions.cpp

nodist_gtest_geis2_api_SOURCES = \
	$(XORG_GTEST_SOURCE)/src/xorg-gtest_main.cpp

gtest_geis2_api_CPPFLAGS = \
	--std=c++0x \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	-I$(top_srcdir)/testsuite/gtest \
	-DTEST_ROOT_DIR=\"$(abs_top_srcdir)/testsuite/\" \
	$(GTEST_CFLAGS) \
	$(EVEMU_CFLAGS) \
	$(XORG_GTEST_CPPFLAGS)

gtest_geis2_api_LDADD = \
	$(top_builddir)/testsuite/gtest/libgtest_geis.a \
	$(top_builddir)/libgeis/libgeis.la \
	$(XORG_GTEST_LIBS) \
	$(EVEMU_LIBS) \
	$(XI2_LIBS)

MOSTLYCLEANFILES = \
	geis2_api.log \
	geis2_api.xml

TESTS = $(test_targets)
check_PROGRAMS = $(test_targets)
