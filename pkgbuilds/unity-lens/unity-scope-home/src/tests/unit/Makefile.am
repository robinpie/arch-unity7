include $(top_srcdir)/Makefile.decl

check_PROGRAMS = \
	test-home-scope \
	$(NULL)

TEST_PROGS += $(check_PROGRAMS)

run_server:
	pkill fake-sss-server 2>&1 > /dev/null || true
	$(top_srcdir)/tests/fake-server/fake-sss-server.py --search $(top_builddir)/tests/unit/search.dump --feedback $(top_builddir)/tests/unit/feedback.dump --requests 2 --port 8888 $(top_srcdir)/tests/unit/data/search_results1.txt  &

AM_VALAFLAGS = \
	  --pkg dee-1.0 \
	  --pkg unity \
	  --pkg unity-protocol \
	  --pkg gio-2.0 \
	  --pkg gio-unix-2.0 \
	  --pkg glib-2.0 \
	  --pkg json-glib-1.0 \
	  --pkg gee-0.8 \
	  --pkg unity-extras \
	  --vapidir $(srcdir) \
	  --vapidir $(top_srcdir)/vapi \
	  --target-glib=2.26 \
	  --pkg libuuid \
	  --pkg posix \
	  $(MAINTAINER_VALAFLAGS) \
	  $(NULL)

AM_LDFLAGS = -rpath $(PROTOCOLPRIVATELIBDIR) $(COVERAGE_LDFLAGS)

LDADD = $(HOME_SCOPE_LIBS) \
	$(test_libs)

AM_CPPFLAGS = \
  -DDATADIR=\"$(DATADIR)\" \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DG_LOG_DOMAIN=\"unity-scope-home\" \
  $(HOME_SCOPE_CFLAGS) \
  $(MAINTAINER_CFLAGS) \
  -I$(srcdir) \
  $(COVERAGE_CFLAGS) \
  $(NULL)

if !ENABLE_C_WARNINGS
  AM_CPPFLAGS += -w
endif

MAINSOURCES = \
	../../src/scope.vala \
	../../src/scope-manager.vala \
	../../src/keyword-search.vala \
	../../src/scope-registry.vala \
	../../src/remote-scope-registry.vala \
	../../src/meta-scope-registry.vala \
	../../src/search-util.vala \
	../../src/filter-state.vala \
	../../src/scope-definitions-parser.vala \
	../../src/search-query-state.vala \
	../../src/smart-scopes-interface.vala \
	../../src/smart-scopes-id-map.vala \
	../../src/smart-scopes-parse.vala \
	../../src/smart-scopes-default-parser.vala \
	../../src/smart-scopes-more-suggestions-parser.vala \
	../../src/smart-scopes-remote-scopes-parser.vala \
	../../src/smart-scopes-metrics.vala \
	../../src/client-scopes-info.vala \
	../../src/platform-info.vala \
	../../src/category-manager.vala \
	../../src/markup-cleaner.vala \
	../../src/master-scopes.vala \
	../../src/smart-scopes-preview-parser.vala \
	$(NULL)

TEST_SOURCES = \
	config-tests.vala \
	test-utils.vala \
	test-home-scope.vala \
	$(NULL)

test_home_scope_VALASOURCES = $(TEST_SOURCES) $(MAINSOURCES)

# the patsubst rule below and objects override are needed to force dependency on *.c files
# generated in current test dir rather than on those from ../../src
nodist_test_home_scope_SOURCES = $(TEST_SOURCES:.vala=.c) $(patsubst ../../src/%,%,$(MAINSOURCES:.vala=.c))
nodist_test_home_scope_OBJECTS = $(nodist_test_home_scope_SOURCES:.c=.o)


CLEANFILES = *.stamp \
	  *.c \
	  $(NULL)

EXTRA_DIST = \
	$(test_home_scope_VALASOURCES) \
	$(NULL)

BUILT_SOURCES = \
	test-home-scope.vala.stamp \
	$(NULL)

test-home-scope.vala.stamp: $(test_home_scope_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@


# START HEADLESS TESTS
if ENABLE_HEADLESS_TESTS
test-headless:
	$(XVFB) make test-nonrecursive; \
	sleep 1;
endif
# END HEADLESS TESTS
