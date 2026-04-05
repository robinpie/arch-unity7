INCLUDES = \
        -I$(top_srcdir)/src -I$(top_builddir)/src \
        $(ZEITGEIST_CFLAGS)

AM_CFLAGS = \
  -Wall  \
  -g

zeitgeist_libs = $(top_builddir)/src/libzeitgeist-1.0.la $(ZEITGEIST_LIBS)

noinst_PROGRAMS = \
  find-events \
  monitor-events \
  search-events

EXTRA_DIST = \
  find-events-alt.vala \
  insert-event.vala


find_events_SOURCES = find-events.c
find_events_LDADD   = $(zeitgeist_libs)

monitor_events_SOURCES = monitor-events.c
monitor_events_LDADD   = $(zeitgeist_libs)

search_events_SOURCES = search-events.c
search_events_LDADD   = $(zeitgeist_libs)
