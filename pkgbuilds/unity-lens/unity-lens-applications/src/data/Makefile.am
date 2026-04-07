#############################################################
# Unity scope files
#############################################################
@INTLTOOL_SCOPE_RULE@

%.scope.in: %.scope.in.in
	sed -e "s|\@prefix\@|$(prefix)|" -e "s|\@libdir\@|$(libdir)|" $< > $@

scope_in_in_files = applications.scope.in.in scopes.scope.in.in commands.scope.in.in

scopedir = $(datadir)/unity/scopes
scope_DATA = commands.scope
appscopedir = $(scopedir)/applications
appscope_DATA = applications.scope scopes.scope

icondir = $(datadir)/unity/themes
icon_DATA = applications.png

#############################################################
# DBus service files
#############################################################
dbus_servicesdir = $(DBUSSERVICEDIR)
service_in_files =  unity-scope-applications.service.in
dbus_services_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in
	sed -e "s|\@bindir\@|$(bindir)|" $< > $@

#############################################################
# XDG Menu files
#############################################################
@INTLTOOL_DIRECTORY_RULE@

directorydir = $(datadir)/desktop-directories
directory_in_files =                 \
  X-Unity-All-Applications.directory
directory_DATA = $(directory_in_files:.directory.in=.directory)

menudir = $(sysconfdir)/xdg/menus
menu_in_files = \
  unity-lens-applications.menu.in
menu_DATA = $(menu_in_files:.menu.in=.menu)

#############################################################
# GSettings schemas
#############################################################
gsettings_SCHEMAS = com.canonical.Unity.AppsLens.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST = \
  applications.png \
  $(scope_in_in_files) \
  com.canonical.Unity.AppsLens.gschema.xml.in.in \
  $(service_in_files) \
  $(directory_in_files) \
  $(menu_in_files)

CLEANFILES = \
  $(scope_DATA) \
  $(scope_in_in_files:.scope.in.in=.scope.in) \
  unity-scope-applications.service \
  $(gsettings_SCHEMAS)

