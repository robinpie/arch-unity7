NULL =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

DATADIR = $(datadir)
LIBEXECDIR = $(libexecdir)

# we're using Vala, C warnings are useless
AM_CFLAGS = -w
AM_CPPFLAGS = $(COVERAGE_CFLAGS)
AM_LDFLAGS = $(COVERAGE_LDFLAGS)

pkglibexec_PROGRAMS = \
  unity-music-daemon \
  music-preview-player

unity_music_daemon_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-music-daemon\" \
  $(SCOPE_DAEMON_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  -g

unity_music_daemon_VALAFLAGS = \
  -C \
  --pkg dee-1.0 \
  --pkg sqlite3 \
  --pkg unity \
  --pkg unity-extras \
  --pkg gee-0.8 \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --pkg glib-2.0 \
  --vapidir $(srcdir) \
  --pkg tdb \
  --target-glib=2.32 \
  $(MAINTAINER_VALAFLAGS)

unity_music_daemon_LDADD = \
  $(SCOPE_DAEMON_LIBS) \
  $(NULL)

unity_music_daemon_VALASOURCES = \
  album.vala \
  categories.vala \
  config.vala \
  daemon.vala \
  filter-parser.vala \
  filter-parser-decade.vala \
  filter-parser-genre.vala \
  genre.vala \
  main.vala \
  rhythmbox-scope.vala \
  rhythmbox-collection.vala \
  simple-scope.vala \
  track.vala \
  $(NULL)

unity_music_daemon_SOURCES = \
  $(unity_music_daemon_VALASOURCES:.vala=.c) \
  $(NULL)

music_preview_player_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  $(MAINTAINER_CFLAGS) \
  $(PREVIEW_PLAYER_CFLAGS) \
  -I$(srcdir) \
  -g

music_preview_player_LDADD = \
  $(PREVIEW_PLAYER_LIBS)

music_preview_player_VALASOURCES = \
  music-preview-player.vala \
  player-service.vala \
  dash-proxy.vala \
  player.vala

music_preview_player_SOURCES = \
  $(music_preview_player_VALASOURCES:.vala=.c) \
  $(NULL)

# We pass --enable-experimental to quiet warnings about using Gst-1.0 bindings
music_preview_player_VALAFLAGS = \
  -C \
  --pkg gstreamer-1.0 \
  --pkg gstreamer-pbutils-1.0 \
  --pkg gio-2.0 \
  --enable-experimental \
  $(MAINTAINER_VALAFLAGS)

BUILT_SOURCES += \
  unity_music_daemon.vala.stamp \
  music_preview_player.vala.stamp

EXTRA_DIST += \
  unity_music_daemon.vala.stamp \
  music_preview_player.vala.stamp \
  $(unity_music_daemon_VALASOURCES) \
  $(music_preview_player_VALASOURCES) \
  tdb.vapi \
  tdb.deps \
  $(NULL)

unity_music_daemon.vala.stamp: $(unity_music_daemon_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(unity_music_daemon_VALAFLAGS) $^
	touch unity_music_daemon.vala.stamp

music_preview_player.vala.stamp: $(music_preview_player_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(music_preview_player_VALAFLAGS) $^
	touch music_preview_player.vala.stamp

CLEANFILES += \
  *.stamp \
  $(unity_music_daemon_VALASOURCES:.vala=.c) \
  $(music_preview_player_VALASOURCES:.vala=.c) \
  $(NULL)
