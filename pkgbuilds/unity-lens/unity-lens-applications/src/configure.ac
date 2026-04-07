AC_INIT(unity-lens-applications, 7.1.0, https://launchpad.net/unity-lens-applications)
AC_COPYRIGHT([Copyright 2010-2012 Canonical])
AM_INIT_AUTOMAKE([1.11])

LT_INIT

#############################################
# Silent build rules
#############################################
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PREREQ(2.59)

AC_CONFIG_HEADERS([config.h])

#############################################
# Init the other things we depend on
#############################################
AM_MAINTAINER_MODE([enable])
AM_PROG_VALAC([0.18.0])
AS_IF([test -z "$VALAC"], [AC_MSG_ERROR(["No valac compiler found."])])
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_LANG([C++])

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

#############################################
# Check for module and library dependancies
#############################################
GLIB_REQUIRED=2.27
PKG_CHECK_MODULES(LENS_DAEMON,
                  glib-2.0 >= $GLIB_REQUIRED
                  gobject-2.0 >= $GLIB_REQUIRED
                  gio-2.0 >= $GLIB_REQUIRED
                  gio-unix-2.0 >= $GLIB_REQUIRED
                  gee-0.8
                  dee-1.0 >= 0.5.16
                  zeitgeist-1.0 >= 0.3.8
                  libcolumbus >= 1.0.0
                  unity >= 7.1.1
                  unity-protocol-private
                  libgnome-menu-3.0 >= 3.6.0)

AC_SUBST(LENS_DAEMON_CFLAGS)
AC_SUBST(LENS_DAEMON_LIBS)

AC_CHECK_HEADER(db.h, [],
                AC_MSG_FAILURE([Unable to find db.h. Please install the package libdb-dev]))

AC_CHECK_HEADER(xapian.h, [],
                AC_MSG_FAILURE([Unable to find xapian.h. Please install the package libxapian-dev]))


####################################################################
# Headless tests
####################################################################
AC_ARG_ENABLE([headless-tests],
              AS_HELP_STRING([--enable-headless-tests=@<:@no/yes@:>@],[enable headless test suite (requires Xvfb) @<:@default=no@:>@]),,
              [enable_headless_tests=no])

AM_CONDITIONAL([ENABLE_HEADLESS_TESTS],[test "x$enable_headless_tests" != "xno"])

if test "x$enable_headless_tests" = "xyes"; then
  AC_PATH_PROG([XVFB],[Xvfb])
  AC_PATH_PROG([DBUS_LAUNCH],[dbus-launch])
fi

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


#############################################
# local install for distcheck and stand-alone running
#############################################
with_localinstall="no"
AC_ARG_ENABLE(localinstall,
              AS_HELP_STRING([--enable-localinstall],
                             [Install all of the files locally instead of in system directories (for distcheck)]),
              with_localinstall=$enableval,
              with_localinstall=no)

AM_CONDITIONAL([HAVE_LOCALINSTALL], [test "x$with_localinstall" = "xyes"])

#############################################
# Expand variables needed for config.vala
#############################################
AS_AC_EXPAND(PREFIX, $prefix)
AC_SUBST(PREFIX)

AS_AC_EXPAND(DATADIR, $datarootdir)
AC_SUBST(DATADIR)

#####################################################
# Look for protocol-private lib dir
#####################################################
PROTOCOLPRIVATELIBDIR="`$PKG_CONFIG --variable=libdir unity-protocol-private`/libunity"
AC_SUBST(PROTOCOLPRIVATELIBDIR)

#############################################
# look for dbus service dir
#############################################
if test "x$with_localinstall" = "xyes"; then
        DBUSSERVICEDIR="${datadir}/dbus-1/services/"
else
        DBUSSERVICEDIR=`$PKG_CONFIG --variable=session_bus_services_dir dbus-1`
fi
AC_SUBST(DBUSSERVICEDIR)

#############################################
# GSettings macros
#############################################

GLIB_GSETTINGS

#############################################
# Create the Makefiles
#############################################
AC_CONFIG_FILES([
  Makefile
  data/com.canonical.Unity.AppsLens.gschema.xml.in
  data/Makefile
  data/X-Unity-All-Applications.directory
  data/unity-lens-applications.menu
  src/Makefile
  src/config.vala
  po/Makefile.in
  tests/unit/Makefile
])
AC_OUTPUT

dnl Output the results
AC_MSG_NOTICE([

  Unity Applications Lens Daemon $VERSION
  ------------------------------------

  Prefix         : ${prefix}
  Sysconfdir     : ${sysconfdir}

  Local install  : ${with_localinstall}

  Extra CFlags   : ${CPPFLAGS} $MAINTAINER_CFLAGS
  Extra ValaFlags: ${CPPFLAGS} $MAINTAINER_VALAFLAGS

  Testing
    Headless tests     : ${enable_headless_tests}
    Coverage reporting : ${use_gcov}
])
