SUBDIRS = unity-misc doc examples

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = unity-misc.pc

CLEANFILES = unity-misc.pc
DISTCLEANFILES = unity-misc.pc
EXTRA_DIST = \
  unity-misc.pc.in \
  COPYING.GPL \
  COPYING.LGPL-2

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc
