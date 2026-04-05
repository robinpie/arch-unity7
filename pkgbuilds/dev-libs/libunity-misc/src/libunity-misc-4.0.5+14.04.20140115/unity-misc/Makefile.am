BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =

miscincludedir = $(includedir)/unity-misc/unity-misc

misc_headers = \
  gnome-bg-slideshow.h \
  na-marshal.h \
  na-tray-child.h \
  na-tray-manager.h \
  na-tray.h

miscinclude_HEADERS = \
  $(misc_headers)

lib_LTLIBRARIES = libunity-misc.la

libunity_misc_la_SOURCES = \
  gnome-bg-slideshow.c \
  gnome-bg-slideshow.h \
  fixedtip.c \
  fixedtip.h \
  na-marshal.c \
  na-marshal.h \
  na-tray-child.c \
  na-tray-child.h \
  na-tray-manager.c \
  na-tray-manager.h \
  na-tray.c \
  na-tray.h

libunity_misc_la_LIBADD = \
  $(UNITY_MISC_LIBS)

libunity_misc_la_LDFLAGS = \
  $(UNITY_MISC_LT_LDFLAGS)

libunity_misc_la_CPPFLAGS = \
	-I$(srcdir) \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"libunity-misc\" \
	-DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
  -DG_DISABLE_DEPRECATED \
	$(GCC_FLAGS) \
  $(UNITY_MISC_CFLAGS) \
  $(MAINTAINER_CFLAGS)

##
# Signal Marshallers
##
na-marshal.h: $(srcdir)/na-marshal.list
	glib-genmarshal --header \
		--prefix=_na_marshal $(srcdir)/na-marshal.list \
	 	> na-marshal.h

na-marshal.c: $(srcdir)/na-marshal.list
	glib-genmarshal --body \
		--prefix=_na_marshal $(srcdir)/na-marshal.list \
		> na-marshal.c

BUILT_SOURCES += \
	na-marshal.c \
	na-marshal.h

EXTRA_DIST += na-marshal.list


