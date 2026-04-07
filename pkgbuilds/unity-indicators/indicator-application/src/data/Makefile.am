
SUBDIRS = upstart

upstart_jobsdir = $(datadir)/upstart/sessions/
upstart_jobs_DATA = indicator-application.conf

%.conf: %.conf.in
	$(AM_V_GEN) sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

xdg_autostartdir = /etc/xdg/autostart
xdg_autostart_DATA = indicator-application.desktop

%.desktop: %.desktop.in
	$(AM_V_GEN) sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

overridedir = $(pkgdatadir)
override_DATA = ordering-override.keyfile

%.service: %.service.in
	$(AM_V_GEN) sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

systemddir = $(SYSTEMD_USERDIR)
systemd_DATA = indicator-application.service

upstartsystemdoverridedir = $(datadir)/upstart/systemd-session/upstart
upstartsystemdoverride_DATA = indicator-application.override

EXTRA_DIST = \
	indicator-application.desktop.in \
	indicator-application.conf.in \
	indicator-application.service.in \
	indicator-application.override \
	ordering-override.keyfile

CLEANFILES = \
	indicator-application.conf \
	indicator-application.desktop \
	indicator-application.service

