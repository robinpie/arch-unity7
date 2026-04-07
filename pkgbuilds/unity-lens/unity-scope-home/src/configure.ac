AC_INIT(unity-scope-home, 6.8.3, https://launchpad.net/unity-scope-home)
AC_COPYRIGHT([Copyright 2012 Canonical])

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
# intltool rule for generating translated .scope file
######################################################
INTLTOOL_SCOPE_RULE='%.scope: %.scope.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< [$]@'
AC_SUBST(INTLTOOL_SCOPE_RULE)

#####################################################
# Check for module and library dependancies
#####################################################
GLIB_REQUIRED=2.27
LIBUNITY_REQURED=7.1.3
PKG_CHECK_MODULES(HOME_SCOPE,
                  glib-2.0 >= $GLIB_REQUIRED
                  gobject-2.0 >= $GLIB_REQUIRED
                  gio-2.0 >= $GLIB_REQUIRED
                  gio-unix-2.0 >= $GLIB_REQUIRED
                  dee-1.0 >= 1.0.7
                  gee-0.8
                  json-glib-1.0
                  uuid >= 2.20.0
                  unity >= $LIBUNITY_REQURED
                  unity-protocol-private >= $LIBUNITY_REQURED
                  unity-extras >= $LIBUNITY_REQURED
                  )

AC_SUBST(HOME_SCOPE_CFLAGS)
AC_SUBST(HOME_SCOPE_LIBS)

####################################################################
# C compiler warnings
####################################################################
AC_ARG_ENABLE([c-warnings],
              AC_HELP_STRING([--enable-c-warnings=@<:@no/yes@:>@], [show warnings from the C compiler @<:@default=no@:>@]),,
              [enable_c_warnings=no])

if test "x$enable_c_warnings" = "xyes"; then
	AC_DEFINE(ENABLE_C_WARNINGS, 1, [show warnings from the C compiler])
fi

AM_CONDITIONAL(ENABLE_C_WARNINGS, test "$enable_c_warnings" = "yes")

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

####################################################################
# Headless tests
####################################################################
AC_ARG_ENABLE([headless-tests],
              AS_HELP_STRING([--enable-headless-tests=@<:@no/yes@:>@],[enable headless test suite (requires Xvfb) @<:@default=no@:>@]),,
              [enable_headless_tests=yes])

AM_CONDITIONAL([ENABLE_HEADLESS_TESTS],[test "x$enable_headless_tests" != "xno"])

if test "x$enable_headless_tests" = "xyes"; then
  AC_PATH_PROG([XVFB],[xvfb-run])
fi

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

#####################################################
# Look for protocol-private lib dir
#####################################################
PROTOCOLPRIVATELIBDIR="`$PKG_CONFIG --variable=libdir unity-protocol-private`/libunity"
AC_SUBST(PROTOCOLPRIVATELIBDIR)

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

#############################################
# GSettings macros
#############################################

GLIB_GSETTINGS

#####################################################
# Create the Makefiles
#####################################################
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  data/Makefile
  tests/unit/Makefile
  src/config.vala
  tests/unit/config-tests.vala
  data/home.scope.in
  data/master-scopes/applications.scope.in
  data/master-scopes/files.scope.in
  data/master-scopes/music.scope.in
  data/master-scopes/video.scope.in
  data/master-scopes/photos.scope.in
  data/master-scopes/more_suggestions.scope.in
  data/master-scopes/books.scope.in
  data/master-scopes/boxes.scope.in
  data/master-scopes/code.scope.in
  data/master-scopes/graphics.scope.in
  data/master-scopes/help.scope.in
  data/master-scopes/info.scope.in
  data/master-scopes/news.scope.in
  data/master-scopes/notes.scope.in
  data/master-scopes/reference.scope.in
  data/master-scopes/weather.scope.in
  data/master-scopes/web.scope.in
  data/master-scopes/recipes.scope.in
  data/master-scopes/calendar.scope.in
  data/master-scopes/searchin.scope.in
  po/Makefile.in
])
AC_OUTPUT

#####################################################
# Output the results
#####################################################
AC_MSG_NOTICE([

  Unity Home Scope Daemon $VERSION
  ----------------------------------

  Prefix          : ${prefix}

  Local install   : ${with_localinstall}

  Extra CFlags    : ${CPPFLAGS} $MAINTAINER_CFLAGS
  Extra ValaFlags : ${CPPFLAGS} $MAINTAINER_VALAFLAGS

  Scopes Directory: ${SCOPESDIR}

  Testing
    Headless tests     : ${enable_headless_tests}
    Coverage reporting : ${use_gcov}
])
