test_targets =

if HAVE_XINPUT
test_targets += check-c-compile check-cxx-compile
endif

if HAVE_GTEST
test_targets += check-regular
endif

TESTS = $(test_targets)
check_PROGRAMS = $(test_targets) check-static-type-checking

AM_CFLAGS = \
	$(COVERAGE_CFLAGS)

AM_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	-iquote$(srcdir)

check_c_compile_SOURCES = check-c-compile.c
check_c_compile_LDADD = \
	$(top_builddir)/src/libframe.la \
	$(COVERAGE_LIBS)

check_cxx_compile_SOURCES = check-cxx-compile.cpp
check_cxx_compile_LDADD = \
	$(top_builddir)/src/libframe.la \
	$(COVERAGE_LIBS)

check-static-type-checking$(EXEEXT):
	@echo "Checking whether static type checking works..."
	$(AM_V_at)if $(CC) -I../../include -L../../.libs check-static-type-checking.c -o check_static_type_checking; then \
		echo "FAIL: Static type checking source compiled without error"; \
		exit -1; \
	else \
		echo "SUCCESS: Static type checking source failed to compile"; \
		exit 0; \
	fi

### check-regular

check_regular_SOURCES = \
	backend.cpp

if HAVE_XINPUT
check_regular_SOURCES += \
	accept-ended-touch.cpp \
    x11-accept-touch.cpp \
	frame-x11-fixture.cpp \
	frame-x11-fixture.h
endif

#
# Link against the (non-distributed) static lib to pick up the
# internal symbols.
#
check_regular_LDADD = \
	$(top_builddir)/src/.libs/libframe.a \
	$(top_builddir)/test/gtest/libgtest.a \
	$(top_builddir)/test/gtest/libgtest_main.a \
	$(COVERAGE_LIBS) \
	$(GTEST_LIBS)

if HAVE_XINPUT
check_regular_LDADD += \
	$(top_builddir)/test/x11_mocks/libx11_mocks.a
endif

check_regular_CPPFLAGS = \
	--std=c++0x \
	-I$(top_srcdir)/test/x11_mocks \
	$(AM_CPPFLAGS) \
	$(WARNING_CPPFLAGS) \
	$(GTEST_CPPFLAGS)
