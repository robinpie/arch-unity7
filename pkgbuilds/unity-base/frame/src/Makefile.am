SUBDIRS = doc src test

# tools are all X11 based
if HAVE_XINPUT
SUBDIRS += tools 
endif

ACLOCAL_AMFLAGS = -I m4 --install

AM_MAKEFLAGS = --no-print-directory
AM_DISTCHECK_CONFIGURE_FLAGS = --disable-integration-tests


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = frame.pc

if HAVE_XINPUT
pkgconfig_DATA +=  frame-x11.pc
endif

if HAVE_GCOV
  .PHONY: clean-gcda
  clean-gcda:
	@echo Removing old coverage results
	$(AM_V_at)-find -name '*.gcda' -print | xargs -r rm
	$(AM_V_at)-rm -f coverage-src.info coverage-test.info coverage.info

  .PHONY: coverage-html generate-coverage-html coverage-html-clean
  coverage-html: clean-gcda
	@echo Running test suite
	$(AM_V_at)-$(SUDO) $(MAKE) -C test $(AM_MAKEFLAGS) -k check
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) generate-coverage-html

  coverage-src.info:
	$(AM_V_GEN)
	$(LCOV_verbose) --directory $(top_builddir)/src --base-directory src --capture --output-file coverage-src.info --no-checksum --compat-libtool --ignore-errors source

  coverage-test.info:
	$(AM_V_GEN)
	$(LCOV_verbose) --directory $(top_builddir)/test --base-directory test --capture --output-file coverage-test.info --no-checksum --compat-libtool --ignore-errors source

  coverage.info: coverage-src.info coverage-test.info
	$(AM_V_GEN)
	$(LCOV_verbose) --add-tracefile coverage-src.info --add-tracefile coverage-test.info --output-file coverage.info

  generate-coverage-html: coverage.info
	@echo "  GEN    coveragereport"
	$(GENHTML_verbose) --prefix $(top_builddir) --output-directory coveragereport --title "Code Coverage" --legend --show-details --demangle-cpp coverage.info

  clean-coverage-html: clean-gcda
	@echo Removing coverage HTML report
	$(AM_V_at)-$(LCOV_verbose) --directory $(top_builddir) -z
	$(AM_V_at)-rm -rf coverage.info coveragereport

  .PHONY: coverage-xml generate-coverage-xml coverage-xml-clean
  coverage-xml: clean-gcda
	@echo Running test suite
	$(AM_V_at)-$(SUDO) $(MAKE) -C test $(AM_MAKEFLAGS) -k check
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) generate-coverage-xml

  generate-coverage-xml:
	@echo "  GEN    coverage.xml"
	$(AM_V_at)$(GCOVR_verbose) -x -r $(top_builddir) -o $(top_builddir)/coverage.xml --object-directory=$(abs_builddir)/.libs

  clean-coverage-xml: clean-gcda
	@echo Removing coverage XML report
	$(AM_V_at)-rm -rf $(top_builddir)/coverage.xml
endif # HAVE_GCOV

.PHONY: INSTALL
INSTALL:
	$(INSTALL_CMD)

doc-%:
	$(MAKE) -C doc $@

dist-hook: INSTALL

EXTRA_DIST = COPYING.GPL3
