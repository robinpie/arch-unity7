check_PROGRAMS = test-rhythmbox-parser

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = \
  -w \
  $(SCOPE_DAEMON_CFLAGS) \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -I$(top_srcdir)/src \
  $(COVERAGE_CFLAGS) \
  $(NULL)

AM_LDFLAGS = $(COVERAGE_LDFLAGS)

AM_VALAFLAGS = \
  -C			\
  --pkg dee-1.0 \
  --pkg gee-0.8 \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --pkg glib-2.0 \
  --pkg json-glib-1.0 \
  --vapidir $(top_srcdir)/src \
  --pkg tdb \
  --pkg unity \
  $(srcdir)/assertions.vapi \
  $(MAINTAINER_VALAFLAGS)
  $(NULL)

test_libs = \
  $(SCOPE_DAEMON_LIBS) \
  $(NULL)

test_rhythmbox_parser_LDADD = $(test_libs)

# Files starting from track.vala are not used in this test,
# but they are listed here so they show up in coverage
# measurements. When new tests are written for these features,
# move these files to them.
#
# All main scope vala files are missing because they would
# not compile and really need their own test binaries.

test_rhythmbox_parser_SOURCES = \
  test-rhythmbox-parser.vala \
  $(top_srcdir)/src/album.vala \
  $(top_srcdir)/src/categories.vala \
  $(top_srcdir)/src/config.vala \
  $(top_srcdir)/src/filter-parser.vala \
  $(top_srcdir)/src/filter-parser-decade.vala \
  $(top_srcdir)/src/filter-parser-genre.vala \
  $(top_srcdir)/src/genre.vala \
  $(top_srcdir)/src/rhythmbox-collection.vala \
  $(top_srcdir)/src/track.vala \
  $(top_srcdir)/src/dash-proxy.vala \
  $(NULL)

EXTRA_DIST = assertions.vapi
CLEANFILES = *.stamp

