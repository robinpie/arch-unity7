dbus_servicesdir = $(DBUSSERVICEDIR)
service_in_files =  unity-scope-files-local.service.in
dbus_services_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in
	sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

#############################################################
# GSettings schemas
#############################################################
gsettings_SCHEMAS = com.canonical.Unity.FilesLens.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST = \
  com.canonical.Unity.FilesLens.gschema.xml.in.in \
  $(service_in_files)

CLEANFILES = \
  unity-scope-files-local.service \
  $(gsettings_SCHEMAS)

