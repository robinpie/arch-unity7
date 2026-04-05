
bin_PROGRAMS = geistest

geistest_SOURCES = \
	geistest.c

geistest_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgeis \
	$(XCB_CFLAGS)

geistest_LDADD = \
	$(top_builddir)/libgeis/libgeis.la \
	$(XCB_LIBS)

