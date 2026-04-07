include $(top_srcdir)/Makefile.decl

check_PROGRAMS = \
	test-software-center-data-cache \
	test-purchase-info-helper \
	test-software-center-utils \
	test-software-center-details \
	test-xapian-utils \
	test-utils

TEST_PROGS += $(check_PROGRAMS)

AM_VALAFLAGS = \
	--vapidir $(top_srcdir)/vapi \
	--pkg glib-2.0 \
	--pkg posix \
	--pkg dee-1.0 \
	--pkg gio-2.0 \
	--pkg gee-0.8 \
	--pkg gio-unix-2.0 \
	--pkg unity-package-search \
	--pkg unity \
	--target-glib=2.26 \
	$(MAINTAINER_VALAFLAGS) \
	$(NULL)

UNITY_PACKAGE_SEARCH_LIBS = -lxapian -lstdc++

LDADD = $(LENS_DAEMON_LIBS) \
	$(test_libs) \
	$(UNITY_PACKAGE_SEARCH_LIBS)

LDFLAGS += -rpath $(PROTOCOLPRIVATELIBDIR)

AM_CPPFLAGS = \
  $(LENS_DAEMON_CFLAGS) \
  -I$(srcdir) \
  -I$(top_srcdir)/src \
  -DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
  -DGMENU_I_KNOW_THIS_IS_UNSTABLE \
  $(COVERAGE_CFLAGS) \
  $(NULL)

if !ENABLE_C_WARNINGS
  AM_CPPFLAGS += -w
endif

AM_LDFLAGS = $(COVERAGE_LDFLAGS)

test_software_center_data_cache_VALASOURCES = \
	test-software-center-data-cache.vala \
	$(top_srcdir)/src/software-center-data-cache.vala \
	$(top_srcdir)/src/software-center-app-details.vala \
	software-center-data-provider-mock.vala \
	$(NULL)

nodist_test_software_center_data_cache_SOURCES = $(test_software_center_data_cache_VALASOURCES:.vala=.c)

test_purchase_info_helper_VALASOURCES = \
	test-purchase-info-helper.vala \
	$(top_srcdir)/src/purchase-info-helper.vala \
	$(top_srcdir)/src/software-center-app-details.vala \
	$(top_srcdir)/src/utils.vala \
	$(NULL)

nodist_test_purchase_info_helper_SOURCES = $(test_purchase_info_helper_VALASOURCES:.vala=.c)

test_purchase_info_helper_LDADD = \
	$(LDADD) \
	unity-package-search.o \
	unity-ratings-db.o \
	-ldb

test_software_center_utils_VALASOURCES = \
	test-software-center-utils.vala \
	$(top_srcdir)/src/software-center-utils.vala \
	$(NULL)

nodist_test_software_center_utils_SOURCES = $(test_software_center_utils_VALASOURCES:.vala=.c)

test_xapian_utils_VALASOURCES = \
	test-xapian-utils.vala \
	$(top_srcdir)/src/xapian-utils.vala \
	$(top_srcdir)/src/utils.vala \
	$(NULL)

nodist_test_xapian_utils_SOURCES = $(test_xapian_utils_VALASOURCES:.vala=.c)


# Also includes source files that don't have proper tests
# so they show up in coverage reports.
test_utils_VALASOURCES = \
	test-utils.vala \
	$(top_srcdir)/src/utils.vala \
	$(top_srcdir)/src/app-watcher.vala \
	$(top_srcdir)/src/aptd-client.vala \
	$(top_srcdir)/src/schemas.vala \
	$(top_srcdir)/src/xapian-utils.vala \
	$(NULL)

nodist_test_utils_SOURCES = $(test_utils_VALASOURCES:.vala=.c)

test_software_center_details_VALASOURCES = \
	test-software-center-app-details.vala \
	$(top_srcdir)/src/software-center-app-details.vala \
	$(NULL)

nodist_test_software_center_details_SOURCES = $(test_software_center_details_VALASOURCES:.vala=.c)

unity-package-search.o : $(top_srcdir)/src/unity-package-search.cc $(top_srcdir)/src/unity-package-search.h
	$(AM_V_GEN)$(CXX) --std=c++11 -g $(COVERAGE_CFLAGS) $(UNITY_PACKAGE_SEARCH_LIBS) -DGMENU_I_KNOW_THIS_IS_UNSTABLE `pkg-config --cflags --libs glib-2.0 libgnome-menu-3.0 unity gee-0.8 libcolumbus` -c $(top_srcdir)/src/unity-package-search.cc
unity-ratings-db.o : $(top_srcdir)/src/unity-ratings-db.c $(top_srcdir)/src/unity-ratings-db.h
	$(AM_V_CC)$(CC) -g $(COVERAGE_CFLAGS) $(unity_ratings_db_libs) `pkg-config --cflags --libs glib-2.0` -c $(top_srcdir)/src/unity-ratings-db.c

CLEANFILES = *.stamp \
	  $(test_software_center_data_cache_VALASOURCES:.vala=.c) \
	  $(test_purchase_info_helper_VALASOURCES:.vala=.c) \
	  $(test_software_center_utils_VALASOURCES:.vala=.c) \
	  $(test_xapian_utils_VALASOURCES:.vala=.c) \
	  $(test_utils_VALASOURCES:.vala=.c) \
	  $(test_software_center_details_VALASOURCES:.vala=.c) \
	  $(NULL)

BUILT_SOURCES = \
	  test-software-center-data-cache.vala.stamp \
	  test-purchase-info-helper.vala.stamp \
	  test-software-center-utils.vala.stamp \
	  test-software-center-details.vala.stamp \
	  test-xapian-utils.vala.stamp \
	  test-utils.vala.stamp \
	  $(NULL)

test-software-center-data-cache.vala.stamp: $(test_software_center_data_cache_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-purchase-info-helper.vala.stamp: $(test_purchase_info_helper_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-software-center-utils.vala.stamp: $(test_software_center_utils_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-software-center-details.vala.stamp: $(test_software_center_details_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-xapian-utils.vala.stamp: $(test_xapian_utils_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

test-utils.vala.stamp: $(test_utils_VALASOURCES)
	$(AM_V_GEN)$(VALAC) -C $(AM_VALAFLAGS) $(VALAFLAGS) $^
	@touch $@

# START HEADLESS TESTS
if ENABLE_HEADLESS_TESTS
DISPLAY = :27
LOG_PATH = headless-logs
test-headless:
	set -e; \
	export DISPLAY=$(DISPLAY); \
	rm -rf $(LOG_PATH); \
	mkdir $(LOG_PATH); \
	$(XVFB) $(DISPLAY) -screen 0 1024x768x8 > $(LOG_PATH)/Xvfb.out 2>&1 & \
	sleep 1; \
	\
	$(DBUS_LAUNCH) > $(LOG_PATH)/sessionbus.sh; \
	source $(LOG_PATH)/sessionbus.sh; \
	sleep 1; \
	\
	make test; \
	sleep 1; \
	\
	kill `grep DBUS_SESSION_BUS_PID $(LOG_PATH)/sessionbus.sh | grep -oE '[0-9]+'`; \
	pkill Xvfb;
endif
# END HEADLESS TESTS
