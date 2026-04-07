NULL =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

DATADIR = $(datadir)

AM_CFLAGS = -w
AM_CPPFLAGS = $(COVERAGE_CFLAGS)
AM_LDFLAGS = $(COVERAGE_LDFLAGS)

pkglibexec_PROGRAMS = \
	unity-video-lens-daemon \
	$(NULL)

unity_video_lens_daemon_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-video-lens-daemon\" \
  $(SCOPE_DAEMON_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  $(NULL)

unity_video_lens_daemon_VALAFLAGS = \
  -C \
  --pkg dee-1.0 \
  --pkg unity \
  --pkg unity-extras \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --pkg glib-2.0 \
  --pkg gee-0.8 \
  --pkg zeitgeist-1.0 \
  --vapidir $(srcdir) \
  --vapidir $(top_srcdir)/vapi \
  --target-glib=2.26 \
  $(MAINTAINER_VALAFLAGS) \
  $(NULL)

unity_video_lens_daemon_LDADD = \
  $(SCOPE_DAEMON_LIBS) \
  $(NULL)

unity_video_lens_daemon_VALASOURCES = \
  blacklist-tracker.vala \
  daemon.vala \
  main.vala \
  locate.vala \
  scope.vala \
  config.vala \
  thumbnailer.vala \
  utils.vala \
  video-file.vala \
  $(NULL)

unity_video_lens_daemon_SOURCES = \
  $(unity_video_lens_daemon_VALASOURCES:.vala=.c) \
  $(NULL)

BUILT_SOURCES += \
  unity_video_lens_daemon.vala.stamp \
  $(NULL)

EXTRA_DIST += \
  unity_video_lens_daemon.vala.stamp \
  $(unity_video_lens_daemon_VALASOURCES) \
  $(NULL)

unity_video_lens_daemon.vala.stamp: $(unity_video_lens_daemon_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(unity_video_lens_daemon_VALAFLAGS) $^
	@touch $@

CLEANFILES += \
  *.stamp \
  $(unity_video_lens_daemon_VALASOURCES:.vala=.c) \
  $(NULL)
