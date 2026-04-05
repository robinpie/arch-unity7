NULL = 

# create a .h file containing the introspection data in a variable for gdbus
%-xml.h : dbus/%.xml
	name=`basename $< | sed -e 's/[-\.]/_/g' -e 's/.xml/_xml/g'`; \
	echo "static const gchar $$name[] = " > $@; \
	cat $< | tr \" \' | sed 's/^/\"/g' | sed 's/$$/\"/g' >> $@; \
	echo ";" >> $@;

zeitgeist-marshal.h : marshal.list
	$(GLIB_GENMARSHAL) $< --prefix=_zeitgeist_cclosure_marshal --header > $@

zeitgeist-marshal.c : marshal.list zeitgeist-marshal.h
	$(GLIB_GENMARSHAL) $< --prefix=_zeitgeist_cclosure_marshal --body > $@

zeitgeist-enumtypes.h: $(enum_headers) zeitgeist-enumtypes.h.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	  cd $(srcdir) && glib-mkenums --template $@.template $(enum_headers) ) > \
	    $@.tmp && mv $@.tmp $@

zeitgeist-enumtypes.c: $(enum_headers) zeitgeist-enumtypes.c.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	  cd $(srcdir) && glib-mkenums --template $@.template $(enum_headers) ) > \
	    $@.tmp && mv $@.tmp $@

enum_headers =  \
	zeitgeist-enums.h  \
	$(NULL)

BUILT_SOURCES = \
  zeitgeist-enumtypes.h \
  zeitgeist-enumtypes.c \
  zeitgeist-marshal.h \
  zeitgeist-marshal.c \
  org.gnome.zeitgeist.Blacklist-xml.h \
  org.gnome.zeitgeist.DataSourceRegistry-xml.h \
  org.gnome.zeitgeist.Index-xml.h \
  org.gnome.zeitgeist.Log-xml.h \
  org.gnome.zeitgeist.Monitor-xml.h \
  $(NULL)

AM_CPPFLAGS = \
	$(ZEITGEIST_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-DZEITGEIST_COMPILATION \
	-DG_LOG_DOMAIN=\"LibZeitgeist\" \
	$(MAINTAINER_CFLAGS) \
	$(NULL)

lib_LTLIBRARIES = libzeitgeist-1.0.la

libzeitgeist_1_0_la_SOURCES = \
  zeitgeist-data-source.h \
  zeitgeist-data-source.c \
  zeitgeist-data-source-registry.h \
  zeitgeist-data-source-registry.c \
  zeitgeist-enums.h \
  zeitgeist-enumtypes.h \
  zeitgeist-enumtypes.c \
  zeitgeist-event.c   \
  zeitgeist-event.h   \
  zeitgeist-index.c   \
  zeitgeist-index.h   \
  zeitgeist-log.c   \
  zeitgeist-log.h   \
  zeitgeist-marshal.c \
  zeitgeist-marshal.h \
  zeitgeist-mimetypes.c   \
  zeitgeist-mimetypes.h   \
  zeitgeist-monitor.c   \
  zeitgeist-monitor.h   \
  zeitgeist-ontology-interpretations.h \
  zeitgeist-ontology-manifestations.h \
  zeitgeist-result-set.h \
  zeitgeist-simple-result-set.c \
  zeitgeist-simple-result-set.h \
  zeitgeist-result-set.c \
  zeitgeist-subject.c   \
  zeitgeist-subject.h   \
  zeitgeist-symbols.c   \
  zeitgeist-symbols.h   \
  zeitgeist-timerange.c   \
  zeitgeist-timerange.h   \
  zeitgeist-timestamp.c   \
  zeitgeist-timestamp.h   \
  zeitgeist.h \
  org.gnome.zeitgeist.Monitor-xml.h \
  $(NULL)

libzeitgeist_1_0_la_LIBADD = \
	$(ZEITGEIST_LIBS) \
	$(NULL)

libzeitgeist_1_0_la_LDFLAGS = \
	$(ZEITGEIST_LT_LDFLAGS)

devel_headersdir = $(includedir)/zeitgeist-1.0

devel_headers = \
  zeitgeist-data-source.h \
  zeitgeist-data-source-registry.h \
  zeitgeist-enums.h \
  zeitgeist-enumtypes.h \
  zeitgeist-event.h \
  zeitgeist-index.h \
  zeitgeist-log.h \
  zeitgeist-mimetypes.h \
  zeitgeist-monitor.h \
  zeitgeist-ontology-interpretations.h \
  zeitgeist-ontology-manifestations.h \
  zeitgeist-result-set.h \
  zeitgeist-subject.h \
  zeitgeist-symbols.h \
  zeitgeist-timerange.h \
  zeitgeist-timestamp.h \
  zeitgeist.h

devel_headers_HEADERS = \
  $(devel_headers)

EXTRA_DIST = \
  marshal.list \
  zeitgeist-enumtypes.c.template \
  zeitgeist-enumtypes.h.template \
  dbus/org.gnome.zeitgeist.Blacklist.xml \
  dbus/org.gnome.zeitgeist.DataSourceRegistry.xml \
  dbus/org.gnome.zeitgeist.Index.xml \
  dbus/org.gnome.zeitgeist.Log.xml \
  dbus/org.gnome.zeitgeist.Monitor.xml \
  $(NULL)

DISTCLEANFILES = \
  $(BUILT_SOURCES)
  $(NULL)
