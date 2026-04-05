#
# @file testsuite/geis1/Makefile.am 
# @brief automake recipe for the geis v1.0 testsuite unit testing directory
#

test_targets =
if HAVE_CHECK
  test_targets += check_geis1_api
endif
if ENABLE_INTEGRATION_TESTS
  test_targets += gtest_geis1_instance gtest_geis1_api
endif

TESTS = $(test_targets)

check_PROGRAMS = $(test_targets)

check_geis1_api_SOURCES = \
	check_instance.c \
	check_gesture_types.c \
	check_gesture_attrs.c \
	check_subscription.c \
	check_geis1_api.c

check_geis1_api_CFLAGS = \
	-Wno-unused-but-set-variable \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	$(CHECK_CFLAGS)

check_geis1_api_LDADD = \
	$(top_builddir)/libgeis/libgeis.la \
	$(CHECK_LIBS)

gtest_geis1_api_SOURCES = \
        gtest_attrs.cpp \
        gtest_devices.cpp \
        gtest_subscription.cpp

nodist_gtest_geis1_api_SOURCES = \
	$(XORG_GTEST_SOURCE)/src/xorg-gtest_main.cpp

gtest_geis1_api_CPPFLAGS = \
        --std=c++0x \
        -I$(top_srcdir) \
        -I$(top_srcdir)/include \
        -I$(top_srcdir)/libgeis \
        -I$(top_srcdir)/testsuite/gtest \
        -DTEST_ROOT_DIR=\"$(abs_top_srcdir)/testsuite/\" \
        $(GTEST_CFLAGS) \
        $(EVEMU_CFLAGS) \
        $(XORG_GTEST_CPPFLAGS)

gtest_geis1_api_LDADD = \
        $(top_builddir)/testsuite/gtest/libgtest_geis.a \
        $(top_builddir)/libgeis/libgeis.la \
        $(XORG_GTEST_LIBS) \
        $(EVEMU_LIBS) \
        $(XI2_LIBS)

gtest_geis1_instance_SOURCES = gtest_instance.cpp
nodist_gtest_geis1_instance_SOURCES = $(nodist_gtest_geis1_api_SOURCES)
gtest_geis1_instance_CPPFLAGS = $(gtest_geis1_api_CPPFLAGS)
gtest_geis1_instance_LDADD = $(gtest_geis1_api_LDADD)

MOSTLYCLEANFILES = \
	geis1_api.log \
	geis1_api.xml

