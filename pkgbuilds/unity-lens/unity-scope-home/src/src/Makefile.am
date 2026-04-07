NULL =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

DATADIR = $(datadir)

pkglibexec_PROGRAMS = \
	unity-scope-home \
	$(NULL)

unity_scope_home_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DPKGDATADIR=\"$(PKGDATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-scope-home\" \
  $(HOME_SCOPE_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  $(COVERAGE_CFLAGS) \
  $(NULL)

if !ENABLE_C_WARNINGS
  unity_scope_home_CPPFLAGS += -w
endif

unity_scope_home_VALAFLAGS = \
  -C \
  --pkg dee-1.0 \
  --pkg gee-0.8 \
  --pkg unity \
  --pkg unity-extras \
  --pkg unity-protocol \
  --pkg gio-2.0 \
  --pkg gio-unix-2.0 \
  --pkg glib-2.0 \
  --pkg json-glib-1.0 \
  --pkg libuuid \
  --vapidir $(srcdir) \
  --vapidir $(top_srcdir)/vapi \
  --target-glib=2.26 \
  $(MAINTAINER_VALAFLAGS) \
  $(NULL)

unity_scope_home_LDFLAGS = -rpath $(PROTOCOLPRIVATELIBDIR) $(COVERAGE_LDFLAGS)

unity_scope_home_LDADD = \
  $(HOME_SCOPE_LIBS) \
  $(NULL)

unity_scope_home_VALASOURCES = \
  category-manager.vala \
  config.vala \
  keyword-search.vala \
  main.vala \
  scope.vala \
  scope-manager.vala \
  scope-registry.vala \
  client-scopes-info.vala \
  markup-cleaner.vala \
  master-scopes.vala \
  platform-info.vala \
  remote-scope-registry.vala \
  meta-scope-registry.vala \
  search-util.vala \
  filter-state.vala \
  scope-definitions-parser.vala \
  search-query-state.vala \
  smart-scopes-interface.vala \
  smart-scopes-id-map.vala \
  smart-scopes-parse.vala \
  smart-scopes-metrics.vala \
  smart-scopes-default-parser.vala \
  smart-scopes-more-suggestions-parser.vala \
  smart-scopes-remote-scopes-parser.vala \
  smart-scopes-preview-parser.vala \
  $(NULL)

unity_scope_home_SOURCES = \
  $(unity_scope_home_VALASOURCES:.vala=.c) \
  $(NULL)

BUILT_SOURCES += \
  unity_scope_home.vala.stamp \
  $(NULL)

EXTRA_DIST += \
  $(unity_scope_home_VALASOURCES) \
  $(NULL)

unity_scope_home.vala.stamp: $(unity_scope_home_VALASOURCES)
	$(AM_V_GEN) $(VALAC) $(unity_scope_home_VALAFLAGS) $^
	@touch $@

CLEANFILES += \
  *.stamp \
  $(unity_scope_home_VALASOURCES:.vala=.c) \
  $(NULL)
