NULL =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

DATADIR = $(datadir)

pkglibexec_PROGRAMS = \
  unity-files-daemon

AM_CFLAGS = -w

unity_files_daemon_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-files-daemon\" \
  $(SCOPE_DAEMON_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  -g

unity_files_daemon_VALAFLAGS = \
  -C \
  --pkg dee-1.0 \
  --pkg zeitgeist-2.0 \
  --pkg unity \
  --pkg gee-0.8 \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --target-glib=2.26 \
  $(MAINTAINER_VALAFLAGS)

unity_files_daemon_LDADD = \
  $(SCOPE_DAEMON_LIBS) \
  $(NULL)

unity_files_daemon_VALASOURCES = \
  blacklist-tracker.vala \
  config.vala \
  daemon.vala \
  folder.vala \
  locate.vala \
  main.vala \
  schemas.vala \
  url-checker.vala \
  utils.vala \
  $(NULL)

unity_files_daemon_SOURCES = \
  $(unity_files_daemon_VALASOURCES:.vala=.c) \
  $(NULL)

BUILT_SOURCES += unity_files_daemon.vala.stamp
EXTRA_DIST += \
  unity_files_daemon.vala.stamp \
  $(unity_files_daemon_VALASOURCES) \
  $(NULL)

# Major hack alert: https://bugzilla.gnome.org/show_bug.cgi?id=621292
# Introduced because of our requirements for the GLIB_2_22 define. See above.
unity_files_daemon.vala.stamp: $(unity_files_daemon_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(unity_files_daemon_VALAFLAGS) $^
	@touch $@

CLEANFILES += \
  *.stamp \
  *.vapi \
  $(unity_files_daemon_VALASOURCES:.vala=.c) \
  $(NULL)

