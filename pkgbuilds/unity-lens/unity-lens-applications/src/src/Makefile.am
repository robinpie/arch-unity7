NULL =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

DATADIR = $(datadir)

unitylibdir = $(libdir)/unity
unitylib_LTLIBRARIES = unity-scope-applications.la unity-scope-runningapps.la

AM_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-applications-daemon\" \
  -DGMENU_I_KNOW_THIS_IS_UNSTABLE \
  $(LENS_DAEMON_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  $(COVERAGE_CFLAGS) \
  -g

if !ENABLE_C_WARNINGS
  AM_CPPFLAGS += -w
endif

AM_CXXFLAGS = --std=c++11

# Note that we require glib 2.26 because libzeitgeist
# expects us to use g_ptr_array_unref instead of g_ptr_array_free.
AM_VALAFLAGS = \
  -C \
  --vapidir=$(top_srcdir)/vapi \
  --pkg posix \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --pkg dee-1.0 \
  --pkg zeitgeist-1.0 \
  --pkg unity \
  --pkg unity-protocol \
  --pkg gee-0.8 \
  --pkg libgnome-menu-3.0 \
  --pkg unity-package-search \
  --pkg unity-ratings-db \
  --target-glib=2.26 \
  $(MAINTAINER_VALAFLAGS)

unity_package_search_libs = -lxapian -lstdc++

unity_ratings_db_libs = -ldb

unity_scope_applications_la_LIBADD = \
  $(LENS_DAEMON_LIBS) \
  $(unity_package_search_libs) \
  $(unity_ratings_db_libs) \
  $(NULL)

unity_scope_runningapps_la_LIBADD = \
$(LENS_DAEMON_LIBS) \
$(NULL)

unity_scope_applications_la_LDFLAGS = \
  -module -shared -avoid-version \
  -R $(PROTOCOLPRIVATELIBDIR) $(COVERAGE_LDFLAGS)

unity_scope_runningapps_la_LDFLAGS = \
-module -shared -avoid-version \
-R $(PROTOCOLPRIVATELIBDIR) $(COVERAGE_LDFLAGS)

unity_scope_applications_la_VALASOURCES = \
  app-watcher.vala \
  config.vala \
  daemon.vala \
  launcher-client.vala \
  main.vala \
  commands-scope.vala \
  scopes-scope.vala \
  schemas.vala \
  utils.vala \
  aptd-client.vala \
  purchase-info-helper.vala \
  software-center-data-provider.vala \
  software-center-data-cache.vala \
  software-center-app-details.vala \
  software-center-utils.vala \
  xapian-utils.vala \
  $(NULL)

unity_scope_runningapps_la_VALASOURCES = \
  running-apps-scope.vala \
  running-apps-search.vala \
  winstack-client.vala \
  windows-stack.vala \
  schemas.vala \
  config.vala \
  $(NULL)

unity_scope_applications_la_SOURCES = \
  $(unity_scope_applications_la_VALASOURCES:.vala=.c) \
  unity-package-search.cc \
  unity-package-search.h \
  unity-ratings-db.c \
  unity-ratings-db.h \
  $(NULL)

unity_scope_runningapps_la_SOURCES = \
  $(unity_scope_runningapps_la_VALASOURCES:.vala=.c) \
  $(NULL)

BUILT_SOURCES += \
  unity-scope-applications.vala.stamp \
  unity-scope-runningapps.vala.stamp \
  $(NULL)

EXTRA_DIST += \
  unity-scope-applications.vala.stamp \
  unity-scope-runningapps.vala.stamp \
  $(unity_scope_applications_la_VALASOURCES) \
  $(unity_scope_runningapps_la_VALASOURCES) \
  unity-package-search.cc \
  unity-package-search.h \
  unity-ratings-db.c \
  unity-ratings-db.h \
  $(NULL)

unity-scope-applications.vala.stamp: $(unity_scope_applications_la_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(AM_VALAFLAGS) $^
	@touch $@

unity-scope-runningapps.vala.stamp: $(unity_scope_runningapps_la_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(AM_VALAFLAGS) $^
	@touch $@

CLEANFILES += \
  *.stamp \
  *.vapi \
  $(unity_scope_applications_la_VALASOURCES:.vala=.c) \
  $(unity_scope_runningapps_la_VALASOURCES:.vala=.c) \
  unity-package-search.h.gch \
  $(NULL)
