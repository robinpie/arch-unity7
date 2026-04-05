SUBDIRS =	\
  src \
  bindings \
  examples \
  tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = zeitgeist-1.0.pc

zeitgeistdocdir = $(datadir)/doc/libzeitgeist-0.3.18

zeitgeistdoc_DATA = \
	README      \
	COPYING     \
	AUTHORS     \
	INSTALL     \
	MAINTAINERS

EXTRA_DIST = \
	autogen.sh \
	zeitgeist-1.0.pc.in	 \
	COPYING.GPL \
	tools/onto2c.py \
	$(zeitgeistdoc_DATA)

DISTCLEANFILES = 

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

check-headless:
	cd tests; \
	make check-headless
