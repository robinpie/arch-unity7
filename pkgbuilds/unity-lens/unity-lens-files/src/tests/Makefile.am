check_PROGRAMS = test-url-checker

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = \
  -w \
  $(SCOPE_DAEMON_CFLAGS) \
  $(NULL)

AM_VALAFLAGS = \
  --pkg gio-2.0 \
  $(srcdir)/assertions.vapi \
  $(NULL)

test_libs = \
  $(SCOPE_DAEMON_LIBS) \
  $(NULL)

test_url_checker_LDADD = $(test_libs)

test_url_checker_SOURCES = \
  test-url-checker.vala \
  $(top_srcdir)/src/url-checker.vala \
  $(NULL)

CLEANFILES = *.stamp

