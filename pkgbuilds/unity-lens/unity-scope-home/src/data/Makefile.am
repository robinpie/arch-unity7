EXTRA_DIST =

#
# Install the home.scope
#
scope_in_files = \
	home.scope.in \
	master-scopes/applications.scope.in \
	master-scopes/files.scope.in \
	master-scopes/music.scope.in \
	master-scopes/video.scope.in \
	master-scopes/photos.scope.in \
	master-scopes/more_suggestions.scope.in \
	master-scopes/books.scope.in \
	master-scopes/boxes.scope.in \
	master-scopes/code.scope.in \
	master-scopes/graphics.scope.in \
	master-scopes/help.scope.in \
	master-scopes/info.scope.in \
	master-scopes/news.scope.in \
	master-scopes/notes.scope.in \
	master-scopes/reference.scope.in \
	master-scopes/web.scope.in \
	master-scopes/weather.scope.in \
	master-scopes/recipes.scope.in \
	master-scopes/calendar.scope.in \
	master-scopes/searchin.scope.in \
	$(NULL)

scopedir = $(SCOPESDIR)
scope_DATA = $(scope_in_files:.scope.in=.scope)

icondir = $(datadir)/unity/themes

@INTLTOOL_SCOPE_RULE@

dbus_servicesdir = $(DBUSSERVICEDIR)
service_in_files =  \
  unity-scope-home.service.in \
  $(NULL)

dbus_services_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in
	$(AM_V_GEN)sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

CLEANFILES = \
  unity-scope-home.service \
  home.scope \
  $(NULL)

EXTRA_DIST = \
  $(scope_in_files) \
  $(service_in_files)
