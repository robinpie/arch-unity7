noinst_PROGRAMS = \
  systray

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/unity-misc \
  $(GCC_FLAGS) \
	$(UNITY_MISC_CFLAGS) \
	$(MAINTAINER_CFLAGS)

systray_SOURCES = systray.c
systray_LDADD = $(top_builddir)/unity-misc/libunity-misc.la $(UNITY_MISC_LIBS)
