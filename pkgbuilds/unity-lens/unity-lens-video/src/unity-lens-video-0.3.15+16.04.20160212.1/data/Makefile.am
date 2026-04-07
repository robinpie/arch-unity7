dbus_servicesdir = $(DBUSSERVICEDIR)
service_in_files =  \
  unity-scope-video-local.service.in \
  $(NULL)

dbus_services_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in
	$(AM_V_GEN)sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

scope_in_files = local.scope.in
scopedir = $(SCOPESDIR)/video
scope_DATA = $(scope_in_files:.scope.in=.scope)

@INTLTOOL_SCOPE_RULE@

EXTRA_DIST = \
  $(service_in_files) \
  $(scope_in_files) \
  $(NULL)

CLEANFILES = \
  unity-lens-video.service \
  local.scope \
  $(NULL)
