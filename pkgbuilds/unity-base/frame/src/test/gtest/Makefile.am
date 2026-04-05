include $(top_srcdir)/test/gtest/Makefile-xorg-gtest.am

check_LIBRARIES =

if HAVE_GTEST
  check_LIBRARIES += libgtest.a libgtest_main.a
endif

if ENABLE_INTEGRATION_TESTS
  check_LIBRARIES += libxorg-gtest.a libxorg-gtest_main.a
endif
