AC_INIT(unity-lens-music, 6.9.1, https://launchpad.net/unity-lens-music)
AC_COPYRIGHT([Copyright 2011-2015 Canonical])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

#####################################################
# Silent build rules
#####################################################
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PREREQ(2.59)

AC_CONFIG_HEADERS([config.h])

#####################################################
# Init the other things we depend on
#####################################################
AM_MAINTAINER_MODE
AM_PROG_VALAC([0.18.0])
AS_IF([test -z "$VALAC"], [AC_MSG_ERROR(["No valac compiler found."])])
AC_PROG_CC
AM_PROG_CC_C_O
AC_HEADER_STDC

LT_INIT
AC_CONFIG_MACRO_DIR([m4])

#############################################
# Gettext
#############################################
GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/po/LINGUAS'])
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [gettext domain])
AM_GLIB_GNU_GETTEXT

# AM_GNOME_GETTEXT above substs $DATADIRNAME
# this is the directory where the *.{mo,gmo} files are installed
localedir='${prefix}/${DATADIRNAME}/locale'
AC_SUBST(localedir)

IT_PROG_INTLTOOL([0.40.0])

AC_DEFINE_UNQUOTED(LOCALE_DIR, "${PREFIX}/${DATADIRNAME}/locale",[Locale directory])
AC_DEFINE_UNQUOTED(DATADIR, "${PREFIX}/${DATADIRNAME}",[Data directory])
AC_DEFINE_UNQUOTED(PREFIXDIR, "${PREFIX}",[Prefix directory])

######################################################
# intltool rule for generating translated .lens file
######################################################
INTLTOOL_SCOPE_RULE='%.scope: %.scope.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< [$]@'
AC_SUBST(INTLTOOL_SCOPE_RULE)

###########################
# gcov coverage reporting
###########################
m4_include([m4/gcov.m4])
AC_TDD_GCOV
AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AM_CONDITIONAL([HAVE_LCOV], [test "x$ac_cv_check_lcov" = xyes])
AM_CONDITIONAL([HAVE_GCOVR], [test "x$ac_cv_check_gcovr" = xyes])
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

#####################################################
# Check for module and library dependancies
#####################################################
GLIB_REQUIRED=2.32
PKG_CHECK_MODULES(SCOPE_DAEMON,
                  glib-2.0 >= $GLIB_REQUIRED
                  gobject-2.0 >= $GLIB_REQUIRED
                  gio-2.0 >= $GLIB_REQUIRED
                  gio-unix-2.0 >= $GLIB_REQUIRED
                  dee-1.0 >= 1.2.5
                  sqlite3 >= 3.7.7
                  gee-0.8
                  json-glib-1.0
                  libnotify
                  unity >= 7.1.0
                  unity-extras >= 7.0.0
                  tdb >= 1.2.6)

PKG_CHECK_MODULES(PREVIEW_PLAYER,
                  glib-2.0 >= $GLIB_REQUIRED
                  gobject-2.0 >= $GLIB_REQUIRED
                  gio-2.0 >= $GLIB_REQUIRED
                  gstreamer-1.0
                  gstreamer-pbutils-1.0
                  )

AC_SUBST(SCOPE_DAEMON_CFLAGS)
AC_SUBST(SCOPE_DAEMON_LIBS)
AC_SUBST(PREVIEW_PLAYER_CFLAGS)
AC_SUBST(PREVIEW_PLAYER_LIBS)

#####################################################
# local install for distcheck and stand-alone running
#####################################################
with_localinstall="no"
AC_ARG_ENABLE(localinstall,
              AS_HELP_STRING([--enable-localinstall],
                             [Install all of the files locally instead of in system directories (for distcheck)]),
              with_localinstall=$enableval,
              with_localinstall=no)

AM_CONDITIONAL([HAVE_LOCALINSTALL], [test "x$with_localinstall" = "xyes"])

#####################################################
# Expand variables needed for config.vala
#####################################################
AS_AC_EXPAND(PREFIX, $prefix)
AC_SUBST(PREFIX)

AS_AC_EXPAND(DATADIR, $datarootdir)
AC_SUBST(DATADIR)

AS_AC_EXPAND(LIBEXECDIR, $libexecdir)
AC_SUBST(LIBEXECDIR)

#####################################################
# Look for dbus service dir
#####################################################
if test "x$with_localinstall" = "xyes"; then
	DBUSSERVICEDIR="${datadir}/dbus-1/services/"
else
	DBUSSERVICEDIR=`$PKG_CONFIG --variable=session_bus_services_dir dbus-1`
fi
AC_SUBST(DBUSSERVICEDIR)

#####################################################
# Look for correct Scopes dir
#####################################################
if test "x$with_localinstall" = "xyes"; then
	SCOPESDIR="${datadir}/unity/scopes"
else
	SCOPESDIR=`$PKG_CONFIG --variable=scopesdir unity`
fi
AC_SUBST(SCOPESDIR)

#####################################################
# Create the Makefiles
#####################################################
AC_CONFIG_FILES([
  Makefile
  rhythmbox.scope.in
  data/Makefile
  src/Makefile
  src/config.vala
  po/Makefile.in
  tests/Makefile
  tests/unit/Makefile
])
AC_OUTPUT

#####################################################
# Output the results
#####################################################
AC_MSG_NOTICE([

  Unity Music Lens Daemon $VERSION
  -------------------------------

  Prefix          : ${prefix}

  Local install   : ${with_localinstall}

  Extra CFlags    : ${CPPFLAGS} $MAINTAINER_CFLAGS
  Extra ValaFlags : ${CPPFLAGS} $MAINTAINER_VALAFLAGS

  Scopes Directory: ${SCOPESDIR}

  Testing
    Coverage reporting : ${use_gcov}

])
