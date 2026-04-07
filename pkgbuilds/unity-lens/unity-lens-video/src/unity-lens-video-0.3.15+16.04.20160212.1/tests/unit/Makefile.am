include $(top_srcdir)/Makefile.decl

check_PROGRAMS = \
	test-locate \
	test-utils \
        test-ubuntu-video-search \
	$(NULL)

TEST_PROGS += $(check_PROGRAMS)

AM_VALAFLAGS = \
	  --pkg dee-1.0 \
	  --pkg unity \
	  --pkg unity-extras \
	  --pkg gio-2.0 \
	  --pkg gio-unix-2.0 \
	  --pkg json-glib-1.0 \
	  --pkg glib-2.0 \
	  --pkg gee-0.8 \
	  --pkg zeitgeist-1.0 \
	  --pkg libsoup-gnome-2.4 \
	  --pkg libsoup-2.4 \
	  --vapidir $(srcdir) \
	  --vapidir $(top_srcdir)/vapi \
	  --target-glib=2.26 \
	  $(MAINTAINER_VALAFLAGS) \
	  $(NULL)

LDADD = $(SCOPE_DAEMON_LIBS) \
	$(test_libs)

AM_CPPFLAGS = \
  $(SCOPE_DAEMON_CFLAGS) \
  -I$(srcdir) \
  -I$(top_srcdir)/src \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  $(COVERAGE_CFLAGS) \
  $(NULL)

AM_LDFLAGS = $(COVERAGE_LDFLAGS)

if !ENABLE_C_WARNINGS
  AM_CPPFLAGS += -w
endif

test_locate_VALASOURCES = \
	test-locate.vala \
	thumbnailer-mock.vala \
	config-tests.vala \
	$(top_srcdir)/src/locate.vala \
	$(top_srcdir)/src/utils.vala \
	$(top_srcdir)/src/video-file.vala \
	$(NULL)

test_utils_VALASOURCES = \
	test-utils.vala \
	config-tests.vala \
	$(top_srcdir)/src/utils.vala \
	$(NULL)

test_ubuntu_video_search_VALASOURCES = \
	test-ubuntu-video-search.vala \
	config-tests.vala \
	$(top_srcdir)/src/remote-scope-globals.vala \
	$(top_srcdir)/src/ubuntu-video-search.vala \
	$(top_srcdir)/src/video-file.vala \
	$(top_srcdir)/src/remote-scope.vala \
	$(top_srcdir)/src/utils.vala \
	$(NULL)

nodist_test_locate_SOURCES = $(test_locate_VALASOURCES:.vala=.c)

nodist_test_utils_SOURCES = $(test_utils_VALASOURCES:.vala=.c)

nodist_test_ubuntu_video_search_SOURCES = $(test_ubuntu_video_search_VALASOURCES:.vala=.c)

CLEANFILES = *.stamp \
	  *.c \
	  $(NULL)

EXTRA_DIST = \
	$(test_locate_VALASOURCES) \
	$(test_utils_VALASOURCES) \
	$(test_ubuntu_video_search_VALASOURCES) \
	data/videosearch_input1.txt \
	data/videosearch_input2.txt \
	data/videosearch_details1.txt \
        data/video1.avi \
        data/video2.avi \
        data/video3.avi \
        data/video1.mpg \
        data/video2.mpg \
        data/video3.mpg \
	$(NULL)

BUILT_SOURCES = \
	test-locate.vala.stamp \
	test-utils.vala.stamp \
	test-ubuntu-video-search.vala.stamp \
	$(NULL)

test-locate.vala.stamp: $(test_locate_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-utils.vala.stamp: $(test_utils_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-ubuntu-video-search.vala.stamp: $(test_ubuntu_video_search_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

# START HEADLESS TESTS
if ENABLE_HEADLESS_TESTS
test-headless:
	$(XVFB) make test-nonrecursive; \
	sleep 1;
endif
# END HEADLESS TESTS
